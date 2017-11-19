// YapPythonTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include <windows.h>

using namespace std;

template<typename MYTYPE>
void Test2D(IYapPython* python, const wchar_t * module_name, 
	const wchar_t * method_name, 
	size_t dwidth, size_t dheight, 
	size_t out_width, size_t out_height)
{
	size_t image_size2d = dheight * dwidth;
	MYTYPE* data = new MYTYPE[image_size2d], *p = data;
	for (int i = 0; i < image_size2d; ++i)
	{
		*(p++) = MYTYPE(i);
	}

	MYTYPE* output_data = reinterpret_cast<MYTYPE*>(
		python->Process2D(module_name, method_name,
			data_type_id<MYTYPE>::type, data, dwidth, dheight, out_width, out_height));

	std::cout << "INPUT Data range: [" << *(data) << "~" << *(data + image_size2d - 1) << "]" << std::endl;
	std::cout << dwidth << "\t" << dheight << std::endl;
	std::cout << "OUTPUT Data range: [" << *(output_data) << "~" << *(output_data + out_width * out_height - 1) << "]" << std::endl;
	std::cout << out_width << "\t" << out_height << std::endl;
	delete[] data;
	delete[] output_data;
}

template<>  void Test2D<complex<float>>(IYapPython* python, const wchar_t * module_name,
	const wchar_t * method_name,
	size_t dwidth, size_t dheight,
	size_t out_width, size_t out_height)
{
	size_t image_size2d = dheight * dwidth;
	complex<float>* data = new complex<float>[image_size2d];
	for (int i = 0; i < image_size2d; ++i)
	{
		data[i].real(i);
		data[i].imag(i);
	}

	complex<float>* output_data = reinterpret_cast<complex<float>*>(
		python->Process2D(module_name, method_name,
			data_type_id<complex<float>>::type, data, dwidth, dheight, out_width, out_height));

	std::cout << "INPUT Data range: [" << *(data) << "~" << *(data + image_size2d - 1) << "]" << std::endl;
	std::cout << dwidth << "\t" << dheight << std::endl;
	std::cout << "OUTPUT Data range: [" << *(output_data) << "~" << *(output_data + out_width * out_height - 1) << "]" << std::endl;
	std::cout << out_width << "\t" << out_height << std::endl;
	delete[] data;
	delete[] output_data;
}

template<>  void Test2D<complex<double>>(IYapPython* python, const wchar_t * module_name,
	const wchar_t * method_name,
	size_t dwidth, size_t dheight,
	size_t out_width, size_t out_height)
{
	size_t image_size2d = dheight * dwidth;
	complex<double>* data = new complex<double>[image_size2d];
	for (int i = 0; i < image_size2d; ++i)
	{
		data[i].real(i);
		data[i].imag(i);
	}

	complex<double>* output_data = reinterpret_cast<complex<double>*>(
		python->Process2D(module_name, method_name,
			data_type_id<complex<double>>::type, data, dwidth, dheight, out_width, out_height));

	std::cout << "INPUT Data range: [" << *(data) << "~" << *(data + image_size2d - 1) << "]" << std::endl;
	std::cout << dwidth << "\t" << dheight << std::endl;
	std::cout << "OUTPUT Data range: [" << *(output_data) << "~" << *(output_data + out_width * out_height - 1) << "]" << std::endl;
	std::cout << out_width << "\t" << out_height << std::endl;
	delete[] data;
	delete[] output_data;
}

template<>  void Test2D<char>(IYapPython* python, const wchar_t * module_name,
	const wchar_t * method_name,
	size_t dwidth, size_t dheight,
	size_t out_width, size_t out_height)
{
	size_t image_size2d = dheight * dwidth;
	char * data = new char[image_size2d];
	for (int i = 0; i < image_size2d; ++i)
	{
		data[i] = unsigned(i%127);
	}

	char* output_data = reinterpret_cast<char*>(
		python->Process2D(module_name, method_name,
			data_type_id<char>::type, data, dwidth, dheight, out_width, out_height));

	std::cout << "INPUT Data range: [" << *(data) << "~" << *(data + image_size2d - 1) << "]" << std::endl;
	std::cout << dwidth << "\t" << dheight << std::endl;
	std::cout << "OUTPUT Data range: [" << *(output_data) << "~" << *(output_data + out_width * out_height - 1) << "]" << std::endl;
	std::cout << out_width << "\t" << out_height << std::endl;
	delete[] data;
	delete[] output_data;
}

template<>  void Test2D<unsigned char>(IYapPython* python, const wchar_t * module_name,
	const wchar_t * method_name,
	size_t dwidth, size_t dheight,
	size_t out_width, size_t out_height)
{
	size_t image_size2d = dheight * dwidth;
	char * data = new char[image_size2d];
	for (int i = 0; i < image_size2d; ++i)
	{
		data[i] = unsigned(i % 127);
	}

	unsigned char* output_data = reinterpret_cast<unsigned char*>(
		python->Process2D(module_name, method_name,
			data_type_id<unsigned char>::type, data, dwidth, dheight, out_width, out_height));

	std::cout << "INPUT Data range: [" << *(data) << "~" << *(data + image_size2d - 1) << "]" << std::endl;
	std::cout << dwidth << "\t" << dheight << std::endl;
	std::cout << "OUTPUT Data range: [" << *(output_data) << "~" << *(output_data + out_width * out_height - 1) << "]" << std::endl;
	std::cout << out_width << "\t" << out_height << std::endl;
	delete[] data;
	delete[] output_data;
}

// Pass: bool, complex<float>, complex<doulbe>, double, float, int, unsigned int, short, unsigned short, char.
// Error: unsigned char(初始化数据出错，并不是程序有问题)
// 3D: not yet

int main()
{
	auto _module = ::LoadLibrary(L"..\\x64\\Debug\\YapPythonDll.dll");
	if (!_module)
		return 0;

	auto get_python_func= (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	if (get_python_func == nullptr)
		return 0;
	
	IYapPython* python = get_python_func();

	size_t out_width = 0, out_height = 0, out_slice = 0;
	
	const wchar_t * class_name = L"Py4C";
	const wchar_t * myfunction = L"ShowImage3D";
	const wchar_t * complex_method_name = L"ShowComplexImage";
	const wchar_t * char_method_name = L"ShowCharImage";

	Test2D<unsigned int>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	//Test2D<complex<float>>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
	//Test2D<complex<double>>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128, out_width, out_height);
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

