using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Interop.FrameServices;
using System.Timers;
using AutoTimer = System.Timers.Timer;
using System.Threading;

namespace NET.FrameServices
{
    internal class TimedPlayer : VideoPlayer
    {
        private AutoTimer _timer;

        public TimedPlayer(FrameProvider provider)
            : this(provider, provider.Properties.TimeBase) { }

        public TimedPlayer(FrameProvider provider, Rational timebase)
            : base(provider)
        {
            _timer = new AutoTimer(1000 / timebase.Numerator / timebase.Denominator);
        }

        protected override void StartLoop(ILoopContext loopContext)
        {
            ManualResetEvent stopped = new ManualResetEvent(false);
            object decodingLock = new object();

            _timer.Elapsed += new ElapsedEventHandler(
            delegate(object sender, ElapsedEventArgs e)
            {
                lock (decodingLock)
                {
                    if (stopped.WaitOne(0))
                        return;

                    if (loopContext.Stopping)
                    {
                        _timer.Enabled = false;
                        stopped.Set();
                        return;
                    }

                    Decode();
                }
            });

            _timer.Enabled = true;
            stopped.WaitOne();
        }
    }
}
