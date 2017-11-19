// YapPythonTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include <windows.h>

#ifndef OUT
#define OUT
#endif

using namespace std;

template<typename MYTYPE>
void Test2D(
	IYapPython* python, 
	const wchar_t * module_name, 
	const wchar_t * method_name, 
	size_t dwidth, 
	size_t dheight,
	OUT size_t& out_width,
	OUT size_t& out_height)
{
	size_t image_size2d = dheight * dwidth;
	MYTYPE* data = new MYTYPE[image_size2d], *p = data;//  *out_data = new MYTYPE[image_size2d];
	for (int i = 0; i < image_size2d; ++i)
	{
		*(p++) = MYTYPE(i); //int(i/image_size2d * 127)
	}

	MYTYPE* output_data = reinterpret_cast<MYTYPE*>(
		python->Process2D(module_name, method_name,
			data_type_id<MYTYPE>::type, data, dwidth, dheight, out_width, out_height));

	delete[]data;
	delete[]output_data;
}

// Pass: bool, complex<float>, complex<doulbe>, double, float, int, unsigned int, short, unsigned short, char.
// Error: unsigned char(初始化数据出错，并不是程序有问题)
// 3D: not yet

int main()
{
	auto _module = ::LoadLibrary(L"..\\x64\\Debug\\YapPythonDll.dll");
	if (!_module)
	{
		std::cout << "Error loading YapPythonDll.dll.\n";
		return 0;
	}

	auto get_yap_python_func = (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	if (get_yap_python_func == nullptr)
	{
		std::cout << "Cannot find GetYapPython() in YapPythonDLL.dll.\n";
		return 0;
	}
	
	IYapPython* python = get_yap_python_func();

	size_t out_width = 0, out_height = 0, out_slice = 0;

	const wchar_t * class_name = L"Py4C";
	const wchar_t * myfunction = L"ShowImage3D";
	const wchar_t * complex_method_name = L"ShowComplexImage";
	const wchar_t * char_method_name = L"ShowCharImage";

	Test2D<unsigned int>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 128, 256, out_width, out_height);

	// Test2D<unsigned int>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<complex<float>>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<complex<double>>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<char>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<unsigned char>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<int>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<unsigned int>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<float>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<double>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<short>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	// Test2D<unsigned short>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);

	system("pause");

	return 0;
};

