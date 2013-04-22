#pragma once

#include <atlstr.h>
#include <string>

#include <DSDecoder.h>

using namespace std;
using namespace System;
using namespace System::Drawing;

using namespace FrameServices;
using namespace FrameServices::DirectShow;
using namespace System::Runtime::InteropServices;

namespace Interop { namespace FrameServices { namespace DirectShow
{
    public ref class DSDecoder : public FrameDecoder
	{
	private:
        ::DSDecoder *_decoder;
        void init(String ^filename)
        {
			CString c_str(filename);
			CT2A utf8_str(c_str, CP_UTF8);
            _decoder = new ::DSDecoder(string(utf8_str));
        }
	public:
		DSDecoder(String ^filename)
		{
            init(filename);
		}
        DSDecoder(String ^filename, [Out] IntPtr% handle)
		{
            init(filename);
            handle = IntPtr(_decoder->GetFrameHandle()->Data[0]);
		}
        ~DSDecoder()
        {
            if (this->IsDisposed)
                return;

            delete _decoder;
        }
        virtual operator ::FrameDecoder *() override sealed
        {
            return _decoder;
        }
        virtual operator ::FrameProvider *() override sealed
        {
            return _decoder;
        }
	};
} } }
