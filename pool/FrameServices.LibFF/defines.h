#pragma once

#ifdef WIN32

#ifdef LIBAV_EXPORTS
#define LIBAV_API __declspec(dllexport)
#else
#define LIBAV_API __declspec(dllimport)
#endif // LIBAV_EXPORTS

#else

#define LIBAV_API

#endif // WIN32
