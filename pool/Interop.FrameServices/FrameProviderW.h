#pragma once

#include <atlstr.h>
#include <string>

#include <FrameProvider.h>

#include "LineStatusW.h"
#include "Conversion.h"
#include "RationalW.h"
#include "VideoPropsW.h"

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace FrameServices;
using namespace System::Drawing;

#pragma make_public(::FrameProvider)

namespace Interop { namespace FrameServices
{
    public enum class ImageType
    {
        IMAGE_TYPE_JPEG = 0,
        IMAGE_TYPE_PNG,
        IMAGE_TYPE_PPM
    };

    ref class FrameDecoder;
    ref class FrameProcessor;

    public ref class FrameProvider abstract
    {
    private:
        bool _IsDisposed;
        LinkedList<FrameProcessor ^> ^_processors;
    internal:
        void AddProcessor(FrameProcessor ^processor)
        {
            _processors->AddLast(processor);
        }
        void RemoveProcessor(FrameProcessor ^processor)
        {
            _processors->Remove(processor);
        }
    protected:
        FrameProvider()
        {
            _IsDisposed = false;
            _processors = gcnew LinkedList<FrameProcessor ^>();
        }
        void Assert()
        {
            if (_IsDisposed)
                throw gcnew ObjectDisposedException(this->GetType()->ToString());
        }
        property bool DebugSaveFrame
        {
            bool get() { return false; }
            void set(bool value)
            {
                // WARNING: install VS90SP1-KB976656-x86.exe in VS2008 SP1
                String ^filename = Path::Combine(
                    Environment::GetFolderPath(Environment::SpecialFolder::MyPictures),
                        GetHashCode().ToString() + "-" + this->Pts + ".ppm");
                this->SaveImage(filename, ImageType::IMAGE_TYPE_PPM);
            }
        }
    public:
        ~FrameProvider()
        {
            if (_IsDisposed)
                return;

            _IsDisposed = true;
        }
        !FrameProvider()
        {
            Close();
        }
        void Close();
        void CloseLine();
        void RestoreLine()
        {
            Assert();
            ::FrameProvider *wrapped = this;
            wrapped->RestoreLine();
        }
        bool RequestNextFrame()
        {
            Assert();
            ::FrameProvider *wrapped = this;
            return wrapped->RequestNextFrame();
        }
        void SaveImage(String ^filename)
        {
            Assert();
            ::FrameProvider *wrapped = this;

            CString cstr_filename(filename);
			CT2A ustr_filename(cstr_filename, CP_UTF8);
            wrapped->SaveImage(string(ustr_filename));
        }
        void SaveImage(String ^filename, ImageType imageType)
        {
            Assert();
            ::FrameProvider *wrapped = this;

            CString cstr_filename(filename);
			CT2A ustr_filename(cstr_filename, CP_UTF8);
            wrapped->SaveImage(string(ustr_filename), (::ImageType)imageType);
        }
        property VideoProps Properties
        {
            VideoProps get()
            {
                Assert();
                ::FrameProvider *wrapped = this;
                return VideoProps(wrapped->GetVideoProps());
            }
        }
        property LineStatus Status
        {
            LineStatus get()
            {
                Assert();
                ::FrameProvider *wrapped = this;
                return LineStatus(wrapped->GetLineStatus());
            }
        }
        FrameProvider ^ GetExpectedProvider(VideoProps properties);
        property IntPtr FrameHandle
        {
            IntPtr get()
            {
                Assert();
                ::FrameProvider *wrapped = this;
                return IntPtr(wrapped->GetFrameHandle()->Data[0]);
            }
        }
        property int64_t Pts
        {
            int64_t get()
            {
                Assert();
                ::FrameProvider *wrapped = this;
                return wrapped->GetFrameHandle()->Pts;
            }
        }
        property int LineSize
        {
            int get()
            {
                Assert();
                ::FrameProvider *wrapped = this;
                return wrapped->GetFrameHandle()->LineSize[0];
            }
        }
        property bool IsDisposed
        {
            bool get()
            {
                return _IsDisposed;
            }
        }
        virtual property FrameDecoder ^ LineDecoder
        {
            FrameDecoder ^ get() abstract;
        }
        virtual operator ::FrameProvider *() abstract;
    private:
        bool isSourceCompatible(VideoProps %source, VideoProps %expected)
        {
            if (source.Width != expected.Width
                    || source.Height != expected.Height)
                return false;

            if (expected.PixelFormat != Imaging::PixelFormat::DontCare
                    && source.PixelFormat != expected.PixelFormat)
                return false;

            return true;
        }
    };
} }
