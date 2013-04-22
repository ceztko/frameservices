// dllmain.cpp : Defines the entry point for the DLL application.

extern "C" {
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
}

#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// Register all formats and codecs
		av_register_all();

		// Register all filters
		avfilter_register_all();

        break;
	case DLL_THREAD_ATTACH:
        break;
	case DLL_THREAD_DETACH:
        break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

