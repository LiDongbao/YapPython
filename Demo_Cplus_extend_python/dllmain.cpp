// dllmain.cpp

#include "stdafx.h"
#include "YapPythonImp.h"

extern "C" {
	_declspec(dllexport) IYapPython* GetYapPython() {
		return (new YapPython());
	};
};

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
};
