// YapPythonTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include <windows.h>
#include "..\ReadFolderAllFiles\NiiReader.h"

#ifndef OUT
#define OUT
#endif

using namespace std;

template<typename MYTYPE>
void buidData(size_t image_size, MYTYPE* data)
{
	for (int i = 0; i < image_size; ++i)
	{
		data[i] = MYTYPE(i);
	}
}

template<> void buidData(size_t image_size, char * data)
{
	for (int i = 0; i < image_size; ++i)
	{
		data[i] = char(int(i%127));
	}
}

template<> void buidData(size_t image_size, unsigned char * data)
{
	for (int i = 0; i < image_size; ++i)
	{
		data[i] = char(int(i % 127));
	}
}

template<typename MYTYPE>
void Test(IYapPython* python, const wchar_t * module_name, 
	const wchar_t * method_name, size_t dwidth, size_t dheight, size_t dslice = 1)
{
	size_t out_width = 0, out_height = 0, out_slice = 0;
	size_t image_size = dheight * dwidth * dslice;
	MYTYPE* data = new MYTYPE[image_size];
	buidData(image_size, data);
	
	MYTYPE* output_data = reinterpret_cast<MYTYPE*>(
		python->Process2D(module_name, method_name,
			data_type_id<MYTYPE>::type, data, dwidth, dheight, out_width, out_height));

	delete[]data;
	delete[]output_data;
}

void LoadDll()
{
	auto _module = ::LoadLibrary(L"..\\x64\\Debug\\YapPythonDll.dll");
	if (!_module)
	{
		std::cout << "Error loading YapPythonDll.dll.\n";
		return ;
	}

	auto get_yap_python_func = (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	if (get_yap_python_func == nullptr)
	{
		std::cout << "Cannot find GetYapPython() in YapPythonDLL.dll.\n";
		return ;
	}

	IYapPython* python = get_yap_python_func();

	auto _module1 = ::LoadLibrary(L"..\\x64\\Debug\\ReadFolderAllFiles.dll");
	if (!_module1)
		return ;
	auto get_func = (INiiReader*(*)())::GetProcAddress(_module1, "GetNiiData");
	if (!get_func)
		return ;
	auto nii_reader = get_func();

	auto data = nii_reader->ReadFile(L"D:\\test_data\\003_t2_tse_sag.nii");

	auto dimensions = nii_reader->GetDimensions();
	size_t width = dimensions[0];
	size_t height = dimensions[1];
	size_t slice = dimensions[2];
	size_t out_width = 0, out_height = 0, out_slice = 0;
	auto out_data_2d = python->Process2D(L"..\\PythonScripts\\Py2C.py", L"ShowImage2d",
		DataTypeUnsignedShort, data, width, height, out_width, out_height);
	auto out_data_3d = python->Process3D(L"..\\PythonScripts\\Py2C.py", L"ShowImage3d",
		DataTypeUnsignedShort, data, width, height, slice, out_width, out_height, out_slice);
	cout << out_width << "\t" << out_height << "\t" << out_slice << endl;
	// Feed("output",out_data.get());
	delete data;
	return ;
}

// Pass: bool, complex<float>, complex<doulbe>, double, float, int, unsigned int, short, unsigned short, unsigned char.
// Error: char(in python )
// 3D: not yet

int main()
{
	LoadDll();

	// Test<unsigned int>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<complex<float>>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<complex<double>>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<char>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128); // Error
	// Test<unsigned char>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<int>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<unsigned int>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<float>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<double>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<short>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);
	// Test<unsigned short>(python, L"..\\PythonScripts\\Py2C.py", L"ShowImage", 256, 128);

	system("pause");

	return 0;
};

