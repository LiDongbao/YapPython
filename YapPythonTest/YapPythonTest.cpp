// YapPythonTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include <windows.h>

#define MYTYPE unsigned int
// Pass: bool, complex<float>, complex<doulbe>, double, float, int, unsigned int, short, unsigned short.
// Error: char, unsigned char(有传入Python，但数据错误)
// 3D: not yet

int main()
{
	auto _module = ::LoadLibrary(L"C:\\Users\\James\\Documents\\Visual Studio 2015\\Projects\\Demo_Cplus_extend_python\\x64\\Debug\\YapPython_demo.dll");
	if (!_module)
		return 0;
	using namespace std;

	auto python_add = (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	if (python_add == nullptr)
		return 0;
	
	IYapPython* python = python_add();

	size_t dheight = 256, dwidth = 128, dslice = 256;
	size_t out_width = 0, out_height = 0, out_slice = 0;
	size_t image_size2d = dheight * dwidth;
	const wchar_t * module_name = L"D:\\Projects\\YAP\\PluginSDK\\BasicRecon\\Python\\Py2C.py", //D:\\Projects\\YAP\\PluginSDK\\BasicRecon\\Python\\ 
		*class_name = L"Py4C", *myfunction = L"ShowImage3D", *method_name = L"ShowImage", *complex_method_name = L"ShowComplexImage",
		*char_method_name = L"ShowCharImage";
	MYTYPE* data = new MYTYPE[image_size2d], *p = data;//  *out_data = new MYTYPE[image_size2d];
	for (int i = 0; i < image_size2d; ++i)
	{
		*(p++) = MYTYPE(i); //int(i/image_size2d * 127)
	}

	MYTYPE* output_data = reinterpret_cast<MYTYPE*>(
		python->Process2D(module_name, method_name, 
			data_type_id<MYTYPE>::type, data, dwidth, dheight, out_width, out_height));

	std::cout << "INPUT Data range: [" << *(data) << "~" << *(data + image_size2d - 1) << "]" << std::endl;
	std::cout << dwidth << "\t" << dheight << std::endl;
	std::cout << "OUTPUT Data range: [" << *(output_data) << "~" << *(output_data + out_width*out_height - 1) << "]" << std::endl;
	std::cout << out_width << "\t" << out_height << std::endl;

	delete data, output_data;
	system("pause");
	return 0;
};

