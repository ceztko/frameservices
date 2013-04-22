#pragma once

#include <atlstr.h>
#include <string>

#include <FFDecoder.h>

using namespace std;
using namespace System;
using namespace System::Drawing;

using namespace FrameServices;
using namespace FrameServices::LibFF;
using namespace System::Runtime::InteropServices;

namespace Interop { namespace FrameServices { namespace LibFF
{
    public ref class FFDecoder : public FrameDecoder
	{
	private:
        ::FFDecoder *_decoder;
        void init(String ^filename)
        {
			CString c_str(filename);
			CT2A utf8_str(c_str, CP_UTF8);
            _decoder = new ::FFDecoder(string(utf8_str.m_psz));
        }
	public:
		FFDecoder(String ^filename)
		{
            init(filename);
		}
        FFDecoder(String ^filename, [Out] IntPtr% handle)
		{
            init(filename);
            handle = IntPtr(_decoder->GetFrameHandle()->Data[0]);
		}
        ~FFDecoder()
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
