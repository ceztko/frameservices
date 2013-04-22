using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using Interop.FrameServices;
using System.ComponentModel;
using System.Drawing.Drawing2D;

namespace NET.FrameServices
{
    public class PaintSurface : UserControl
    {
        private VideoPlayer _Player;
        private Bitmap _bitmap;
        private Matrix _scaleMatrix;
        private BufferedGraphicsContext _currentContext;
        private BufferedGraphics _buffer;
        bool _BlankOnStop;

        public PaintSurface()
        {
            // No double buffering set on the control. We provide our own double
            // (actually triple) buffering
            this.SetStyle(ControlStyles.UserPaint | ControlStyles.Opaque
                | ControlStyles.AllPaintingInWmPaint, true);

            _currentContext = BufferedGraphicsManager.Current;

            _scaleMatrix = new Matrix();

            allocateBuffer();
            renderBackground();

            _Player = null;
            _bitmap = null;
            _BlankOnStop = true;
        }

        protected override void Dispose(bool disposing)
        {
            if (_Player != null)
                _Player.Stop();

            base.Dispose(disposing);
        }

        protected override void OnResize(EventArgs args)
        {
            if (this.DisplayRectangle.Width == 0 || this.DisplayRectangle.Height == 0)
            {
                base.OnResize(args);
                return;
            }

            allocateBuffer();

            if (_BlankOnStop)
            {
                _buffer.Graphics.ResetTransform();
                renderBackground();
            }
            else
            {
                if (_bitmap != null)
                {
                    setSourceScale();
                    _buffer.Graphics.DrawImage(_bitmap, 0, 0);
                }
            }

            this.Invalidate();

            base.OnResize(args);
        }

        protected override void OnPaint(PaintEventArgs args)
        {
            _buffer.Render();

            base.OnPaint(args);
        }

        private void allocateBuffer()
        {
            _buffer = _currentContext.Allocate(this.CreateGraphics(), this.DisplayRectangle);

            _buffer.Graphics.CompositingMode = CompositingMode.SourceCopy;
            _buffer.Graphics.CompositingQuality = CompositingQuality.HighSpeed;
            _buffer.Graphics.InterpolationMode = InterpolationMode.NearestNeighbor;
            _buffer.Graphics.SmoothingMode = SmoothingMode.HighSpeed;

            _buffer.Graphics.Transform = _scaleMatrix;
        }

        private void renderBackground()
        {
            _buffer.Graphics.Clear(this.BackColor);
        }

        private void setSourceScale()
        {
            _buffer.Graphics.ResetTransform();
            _buffer.Graphics.ScaleTransform(this.Width / (float)_bitmap.Width,
                this.Height / (float)_bitmap.Height);
        }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public VideoPlayer Player
        {
            get { return _Player; }
            set
            {
                if (_Player != null)
                {
                    _Player.PlayerStarted -= player_PlayerStarted;
                    _Player.FrameReady -= player_NewFrameDecoded;
                    _Player.PlayerStopped -= player_PlayerStopped;
                }

                if (value != null)
                {
                    if (_bitmap != null)
                        _bitmap.Dispose();

                    _bitmap = value.Bitmap;

                    if (!_BlankOnStop)
                    {
                        setSourceScale();
                        _buffer.Graphics.DrawImage(_bitmap, 0, 0);
                        this.Invalidate();
                    }

                    value.PlayerStarted += player_PlayerStarted;
                    value.FrameReady += player_NewFrameDecoded;
                    value.PlayerStopped += player_PlayerStopped;
                }
                else
                {
                    _buffer.Graphics.ResetTransform();
                    renderBackground();
                    this.Invalidate();
                }

                _Player = value;
            }
        }

        [DefaultValue(true)]
        public bool BlankOnStop
        {
            get { return _BlankOnStop; }
            set
            {
                if (_BlankOnStop == value)
                    return;

                if (_BlankOnStop)
                {
                    _buffer.Graphics.ResetTransform();
                    renderBackground();
                    this.Invalidate();
                }
                else
                {
                    if (_bitmap != null)
                    {
                        setSourceScale();
                        _buffer.Graphics.DrawImage(_bitmap, 0, 0);
                        this.Invalidate();
                    }
                }

                _BlankOnStop = value;
            }
        }

        void player_PlayerStarted()
        {
            if (_BlankOnStop)
            {
                setSourceScale();
                _buffer.Graphics.DrawImage(_bitmap, 0, 0);
                this.Invalidate();
            }
        }

        void player_NewFrameDecoded()
        {
            _buffer.Graphics.DrawImage(_bitmap, 0, 0);
            this.Invalidate();
        }

        void player_PlayerStopped()
        {
            if (_BlankOnStop)
            {
                _buffer.Graphics.ResetTransform();
                renderBackground();
                this.Invalidate();
            }
        }
    }
}
