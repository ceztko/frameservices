using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Interop.FrameServices;
using System.Threading;
using System.Drawing;

namespace NET.FrameServices
{
    public abstract class VideoPlayer
    {
        #region Fields

        public event Action PlayerStarted;
        public event Action FrameReady;
        public event Action PlayerStopped;

        private object _playingLock;
        private LoopContext _loopContext;
        private ManualResetEvent _stopped;
        private FrameProvider _Provider;
        private bool _AutoRewind;
        private bool _rewindRequested;

        #endregion // Fields

        #region Static methods

        public static VideoPlayer New(FrameProvider provider)
        {
            return New(provider, false);
        }

        public static VideoPlayer New(FrameProvider provider, bool fastestPossible)
        {
            if (fastestPossible)
                return new UntimedPlayer(provider);
            else
                return new TimedPlayer(provider);
        }

        public static VideoPlayer New(FrameProvider provider, Rational timebase)
        {
            return new TimedPlayer(provider, timebase);
        }

        #endregion // Static methods

        #region Constructors

        protected VideoPlayer(FrameProvider provider)
        {
            _Provider = provider;
            _playingLock = new object();
            _loopContext = new LoopContext();
            _stopped = new ManualResetEvent(true);
            _AutoRewind = true;
            _rewindRequested = false;
        }

        ~VideoPlayer()
        {
            Close();
        }

        #endregion // Constructors

        #region Inquiry

        public void Play()
        {
            Thread playerThread = new Thread(delegate()
            {
                if (!Monitor.TryEnter(_playingLock))
                    return;

                try
                {
                    if (_rewindRequested && _Provider.LineDecoder != null)
                    {
                        _Provider.LineDecoder.Rewind();
                        _Provider.RestoreLine();
                        _rewindRequested = false;
                    }

                    OnPlayerStarted();

                    _stopped.Reset();

                    Thread thread = new Thread(delegate()
                    {
                        StartLoop(_loopContext);
                    });
                    thread.Start();
                    thread.Join();

                    OnPlayerStopped();
                    _stopped.Set();

                    // Set a new loop context *after* the player is stopped so it won't be used
                    // by spurious calls to Stop()
                    _loopContext = new LoopContext();
                }
                finally
                {
                    Monitor.Exit(_playingLock);
                }
            });
            playerThread.Start();
        }

        public void Pause()
        {
            if (_stopped.WaitOne(0))
                return;

            _loopContext.SetStopping();
            _stopped.WaitOne();
        }

        public void Stop()
        {
            if (_stopped.WaitOne(0))
                return;

            if (_AutoRewind)
                _rewindRequested = true;

            _loopContext.SetStopping();
            _stopped.WaitOne();
        }

        public void OneShot(bool requestFrame)
        {
            Thread oneShotThread = new Thread(delegate()
            {
                if (!Monitor.TryEnter(_playingLock))
                    return;

                try
                {
                    OnPlayerStarted();

                    if (requestFrame)
                        _Provider.RequestNextFrame();

                    OnFrameReady();
                }
                finally
                {
                    OnPlayerStopped();
                    Monitor.Exit(_playingLock);
                }
            });
            oneShotThread.Start();
        }

        public void Close()
        {
            Stop();
        }

        #endregion // Inquiry

        #region Private methods

        protected void Decode()
        {
            OnFrameReady();

            bool frameAvailable = _Provider.RequestNextFrame();

            if (!frameAvailable)
            {
                if (_AutoRewind)
                    _rewindRequested = true;

                _loopContext.SetStopping();
            }
        }

        protected abstract void StartLoop(ILoopContext loopContext);

        #endregion // Private methods

        #region Properties

        public bool Playing
        {
            get
            {
                if (!Monitor.TryEnter(_playingLock))
                    return true;

                Monitor.Exit(_playingLock);

                return false;
            }
        }

        public FrameProvider Provider
        {
            get { return _Provider; }
        }

        public Bitmap Bitmap
        {
            get
            {
                VideoProps props = _Provider.Properties;
                return new Bitmap(props.Width, props.Height, _Provider.LineSize,
                    props.PixelFormat, _Provider.FrameHandle);
            }
        }

        public bool AutoRewind
        {
            get { return _AutoRewind; }
            set { _AutoRewind = value; }
        }

        #endregion // Properties

        #region Events

        protected virtual void OnPlayerStarted()
        {
            Action handler = PlayerStarted;
            if (handler != null)
                handler();
        }

        protected virtual void OnFrameReady()
        {
            Action handler = FrameReady;
            if (handler != null)
                handler();
        }

        protected virtual void OnPlayerStopped()
        {
            Action handler = PlayerStopped;
            if (handler != null)
                handler();
        }

        #endregion // Events

        #region Support

        protected class LoopContext : ILoopContext
        {
            private bool _Stopping;

            public LoopContext()
            {
                _Stopping = false;
            }

            public bool Stopping
            {
                get { return _Stopping; }
            }

            public void SetStopping()
            {
                _Stopping = true;
            }
        }

        protected interface ILoopContext
        {
            bool Stopping
            {
                get;
            }
        }

        #endregion // Support
    }
}
