// YapPythonTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include "..\ReadFolderAllFiles\INiiReader.h"
#include <windows.h>
#include <vector>

#ifndef OUT
#define OUT
#endif

using namespace std;

INiiReader * LoadNiiDll()
{
	auto _module = ::LoadLibrary(L"D:\\Projects\\Demo_Cplus_extend_python\\x64\\Debug\\ReadNiiFile.dll");
	if (!_module)
	{
		std::cout << "Error loading ReadFolderAllFiles.dll.\n";
		return nullptr;
	}
	auto get_func = (INiiReader*(*)())::GetProcAddress(_module, "GetNiiData");
	if (!get_func)
	{
		::FreeLibrary(_module);
		std::cout << "Cannot find GetYapPython() in ReadFolderAllFiles.dll.\n";
		return nullptr;
	}
	std::cout << "sucess load ReadFolderAllFiles.dll" << std::endl;
	return get_func();
}

IYapPython* LoadPythonDll()
{
	auto _module = ::LoadLibrary(L"D:\\Projects\\Demo_Cplus_extend_python\\x64\\Debug\\YapPythonDll.dll");
	if (!_module)
	{
		std::cout << "Error loading YapPythonDll.dll.\n";
		return nullptr;
	}

	auto get_yap_python_func = (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	if (get_yap_python_func == nullptr)
	{
		::FreeLibrary(_module);
		std::cout << "Cannot find GetYapPython() in YapPythonDLL.dll.\n";
		return nullptr;
	}
	std::cout << "sucess load YapPythonDll.dll" << std::endl;
	return get_yap_python_func();
}

void YapPythonTest()
{
	auto nii_reader = LoadNiiDll();
	auto python = LoadPythonDll();
	auto t1ce_data = nii_reader->ReadFile(L"D:\\test_data\\MICCAI_BraTS17_Data_Training_for_TGuo_2017modified\\Brats17TrainingData\\test\\Brats17_TCIA_321_1\\Brats17_TCIA_321_1_flair.nii");
	auto data_type = nii_reader->GetDataType();
	auto roi_data = nii_reader->ReadFile(L"D:\\test_data\\MICCAI_BraTS17_Data_Training_for_TGuo_2017modified\\Brats17TrainingData\\test\\Brats17_TCIA_321_1\\Brats17_TCIA_321_1_seg.nii");
	auto roi_type = nii_reader->GetDataType();

	auto dimensions = reinterpret_cast<size_t*>(nii_reader->GetDimensions());
	
	size_t input_size[4] = { dimensions[0], dimensions[1], dimensions[2], dimensions[3] };
	size_t output_dimensions = 0;
	size_t output_size[10] = { 0 };

	// auto out_data_2d = python->Process(L"..\\PythonScripts\\Py2C.py", L"ShowImage2d",
	// DataTypeUnsignedShort, 2, data, output_dimensions, input_size, output_size);
	// 
	// auto out_data_3d = python->Process(L"..\\PythonScripts\\Py2C.py", L"ShowImage3d",
	// DataTypeUnsignedShort, 3, data, output_dimensions, input_size, output_size);
	python->SetRefData(roi_data, roi_type, 3, input_size); //DataTypeUnsignedShort
	auto out_data = python->Process(L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\demo_test.py", L"test_radiomics",
		data_type, DataTypeFloat, 3, t1ce_data, output_dimensions, input_size, output_size,true);
	python->DeleteRefData();
	std::cout << "output data dimension: " << output_dimensions << std::endl;
}

int main()
{
	YapPythonTest();
	system("pause");
	return 0;
};

