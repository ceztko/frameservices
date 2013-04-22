#pragma once

#include <atlstr.h>
#include <string>

#include <FrameFilter.h>

#include "FrameProcessorW.h"

using namespace std;
using namespace System;
using namespace System::Drawing;
using namespace FrameServices;
using namespace System::Runtime::InteropServices;

namespace Interop { namespace FrameServices
{
    public ref class FrameFilter : FrameProcessor
    {
	private:
        ::FrameFilter *_processor;
        void init(String ^filename, FrameProvider ^sourceProvider)
        {
			CString cstr_filename(filename);
			CT2A u8str_filename(cstr_filename, CP_UTF8);
            _processor = new ::FrameFilter(u8str_filename.m_psz, sourceProvider);
        }
    public:
        FrameFilter(String ^filename, FrameProvider ^sourceProvider)
            : FrameProcessor(sourceProvider)
		{
            init(filename, sourceProvider);
		}
        FrameFilter(String ^filename, FrameProvider ^sourceProvider, [Out] IntPtr% handle)
            : FrameProcessor(sourceProvider)
		{
            init(filename, sourceProvider);
            handle = IntPtr(_processor->GetFrameHandle()->Data[0]);
		}
        ~FrameFilter()
        {
            if (this->IsDisposed)
                return;

            delete _processor;
        }
        virtual operator ::FrameProcessor *() override sealed
        {
            return _processor;
        }
        virtual operator ::FrameProvider *() override sealed
        {
            return _processor;
        }
    };
} }
