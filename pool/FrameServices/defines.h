#pragma once

#ifdef WIN32

#ifdef FRAMESERVICES_EXPORTS
#define FRAMESERVICES_API __declspec(dllexport)
#else
#define FRAMESERVICES_API __declspec(dllimport)
#endif // FRAEMSERVICES_EXPORTS

#define snprintf sprintf_s

#else

#define FRAMESERVICES_API

#endif // WIN32
