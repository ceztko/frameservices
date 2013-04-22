using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Interop.FrameServices;
using System.Threading;

namespace NET.FrameServices
{
    internal class UntimedPlayer : VideoPlayer
    {
        public UntimedPlayer(FrameProvider provider)
            : base(provider) { }

        protected override void StartLoop(ILoopContext loopContext)
        {
            while (true)
            {
                if (loopContext.Stopping)
                    break;

                Decode();
            }
        }
    }
}
