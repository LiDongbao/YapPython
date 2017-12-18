// YapPythonTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include <windows.h>
#include "..\ReadFolderAllFiles\NiiReader.h"
#include <vector>

#ifndef OUT
#define OUT
#endif

using namespace std;

INiiReader * LoadNiiDll()
{
	auto _module1 = ::LoadLibrary(L"..\\x64\\Debug\\ReadFolderAllFiles.dll");
	if (!_module1)
	{
		std::cout << "Error loading YapPythonDll.dll.\n";
		return nullptr;
	}
	auto get_func = (INiiReader*(*)())::GetProcAddress(_module1, "GetNiiData");
	if (!get_func)
	{
		std::cout << "Cannot find GetYapPython() in YapPythonDLL.dll.\n";
		return nullptr;
	}
	return get_func();
}

IYapPython* LoadPythonDll()
{
	auto _module = ::LoadLibrary(L"..\\x64\\Debug\\YapPythonDll.dll");
	if (!_module)
	{
		std::cout << "Error loading YapPythonDll.dll.\n";
		return nullptr;
	}

	auto get_yap_python_func = (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	if (get_yap_python_func == nullptr)
	{
		std::cout << "Cannot find GetYapPython() in YapPythonDLL.dll.\n";
		return nullptr;
	}

	return get_yap_python_func();
}

void YapPythonTest()
{
	auto python = LoadPythonDll();
	auto nii_reader = LoadNiiDll();
	auto data = nii_reader->ReadFile(L"D:\\test_data\\003_t2_tse_sag.nii");

	auto dimensions = reinterpret_cast<size_t*>(nii_reader->GetDimensions());

	size_t input_size[4] = { dimensions[0], dimensions[1], dimensions[2], dimensions[3] };
	size_t output_size[4] = { 0 };

	// auto out_data_2d = python->Process(L"..\\PythonScripts\\Py2C.py", L"ShowImage2d",
	// DataTypeUnsignedShort, data, 2, input_size, output_size);
	// 
	// auto out_data_3d = python->Process(L"..\\PythonScripts\\Py2C.py", L"ShowImage3d",
	// DataTypeUnsignedShort, data, 3, input_size, output_size);

	auto out_data_4d = python->Process(L"..\\PythonScripts\\Py2C.py", L"ShowImage4d",
		DataTypeUnsignedShort, data, 4, input_size, output_size);
}

int main()
{
	YapPythonTest();
	system("pause");
	return 0;
};

