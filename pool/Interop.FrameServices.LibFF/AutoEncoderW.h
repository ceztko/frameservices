#pragma once

#include <atlstr.h>
#include <string>

#include <AutoEncoder.h>

using namespace std;
using namespace System;
using namespace System::Drawing;
using namespace FrameServices;
using namespace FrameServices::LibFF;
using namespace System::Runtime::InteropServices;

namespace Interop { namespace FrameServices { namespace LibFF
{
    public ref class AutoEncoder : FrameProcessor
    {
	private:
        ::AutoEncoder *_autoEncoder;
        void init(String ^filename, FrameProvider ^sourceProvider, Rational timeBase,
            bool rawvideo)
        {
			CString cstr_filename(filename);
			CT2A u8str_filename(cstr_filename, CP_UTF8);
            _autoEncoder = new ::AutoEncoder(u8str_filename.m_psz, sourceProvider,
                timeBase, rawvideo);
        }
    public:
        AutoEncoder(String ^filename, FrameProvider ^sourceProvider)
            : FrameProcessor(sourceProvider)
		{
            VideoProps source = sourceProvider->Properties;
            init(filename, sourceProvider, source.TimeBase, false);
		}
        AutoEncoder(String ^filename, FrameProvider ^sourceProvider, Rational timeBase)
            : FrameProcessor(sourceProvider)
		{
            init(filename, sourceProvider, timeBase, false);
		}
        AutoEncoder(String ^filename, FrameProvider ^sourceProvider, bool rawvideo)
            : FrameProcessor(sourceProvider)
		{
            VideoProps source = sourceProvider->Properties;
            init(filename, sourceProvider, source.TimeBase, rawvideo);
		}
        AutoEncoder(String ^filename, FrameProvider ^sourceProvider, Rational timeBase,
            bool rawvideo)
            : FrameProcessor(sourceProvider)
		{
            init(filename, sourceProvider, timeBase, rawvideo);
		}
        ~AutoEncoder()
        {
            if (this->IsDisposed)
                return;

            delete _autoEncoder;
        }
        property bool Enabled
        {
            bool get()
            {
                return _autoEncoder->IsEnabled();
            }
            void set(bool value)
            {
                _autoEncoder->SetEnabled(value);
            }
        }
        virtual operator ::FrameProcessor *() override sealed
        {
            return _autoEncoder;
        }
        virtual operator ::FrameProvider *() override sealed
        {
            return _autoEncoder;
        }
    };
} } }
