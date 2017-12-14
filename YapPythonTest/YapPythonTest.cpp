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

template<> void buidData(size_t image_size, complex<float> * data)
{
	for (int i = 0; i < image_size; ++i)
	{
		data[i].real(float(i));
		data[i].imag(float(i));
	}
}

template<> void buidData(size_t image_size, complex<double> * data)
{
	for (int i = 0; i < image_size; ++i)
	{
		data[i].real(double(i));
		data[i].imag(double(i));
	}
}

template<typename MYTYPE>
void Test2D(IYapPython* python, const wchar_t * module_name, 
	const wchar_t * method_name, size_t dwidth, size_t dheight)
{
	size_t out_width = 0, out_height = 0;
	size_t image_size = dheight * dwidth;
	MYTYPE* data = new MYTYPE[image_size];
	buidData(image_size, data);
	cout << "\tinput  data:  " << *(data) << " ~ " << *(data + dwidth*dheight - 1) << endl;

	MYTYPE* output_data = reinterpret_cast<MYTYPE*>(
		python->Process2D(module_name, method_name,
			data_type_id<MYTYPE>::type, data, dwidth, dheight, out_width, out_height));
	cout << "\toutput data:  " << *(output_data) << " ~ " << *(output_data + out_width*out_height - 1) << endl;
	delete[]data;
	delete[]output_data;
}

template<typename MYTYPE>
void Test3D(IYapPython* python, const wchar_t * module_name,
	const wchar_t * method_name, size_t dwidth, size_t dheight, size_t dslice)
{
	size_t out_width = 0, out_height = 0, out_slice = 0;
	size_t image_size = dheight * dwidth * dslice;
	MYTYPE* data = new MYTYPE[image_size];
	buidData(image_size, data);
	cout << "\tinput  data:  " << *(data) << " ~ " << *(data + dwidth*dheight*dslice - 1) << endl;
	MYTYPE* output_data = reinterpret_cast<MYTYPE*>(
		python->Process3D(module_name, method_name,
			data_type_id<MYTYPE>::type, data, dwidth, 
			dheight, dslice, out_width, out_height, out_slice));
	cout << "\toutput data:  " << *(output_data) << " ~ " << *(output_data + out_width*out_height*out_slice - 1) << endl;
	delete[]data;
	delete[]output_data;
}

template<typename MYTYPE>
void Test4D(IYapPython* python, const wchar_t * module_name,
	const wchar_t * method_name, size_t dwidth, size_t dheight, size_t dslice, size_t dtime)
{
	size_t out_width = 0, out_height = 0, out_slice = 0, out_time=0;
	size_t image_size = dheight * dwidth * dslice * dtime;
	MYTYPE* data = new MYTYPE[image_size];
	buidData(image_size, data);
	cout << "\tinput  data:  " << *(data) << " ~ " << *(data + dwidth*dheight*dslice*dtime - 1) << endl;
	MYTYPE* output_data = reinterpret_cast<MYTYPE*>(
		python->Process4D(module_name, method_name,
			data_type_id<MYTYPE>::type, data, dwidth,
			dheight, dslice, dtime, out_width, out_height, out_slice, out_time));
	cout << "\toutput data:  " << *(output_data) << " ~ " << *(output_data + out_width*out_height*out_slice*out_time - 1) << endl;
	delete[]data;
	delete[]output_data;
}

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

	auto dimensions = nii_reader->GetDimensions();
	size_t width = dimensions[0];
	size_t height = dimensions[1];
	size_t slice = dimensions[2];
	size_t time = dimensions[3];
	size_t out_width = 0, out_height = 0, out_slice = 0, out_time = 0;

	auto out_data_2d = python->Process2D(L"..\\PythonScripts\\Py2C.py", L"ShowImage2d",
	DataTypeUnsignedShort, data, width, height, out_width, out_height);
	auto out_data_3d = python->Process3D(L"..\\PythonScripts\\Py2C.py", L"ShowImage3d",
	DataTypeUnsignedShort, data, width, height, slice, out_width, out_height, out_slice);
	auto out_data_4d = python->Process4D(L"..\\PythonScripts\\Py2C.py", L"ShowImage4d",
	DataTypeUnsignedShort, data, width, height, slice, time,
	out_width, out_height, out_slice, out_time);

	return;
}

/* Test result:
2D: Pass: bool, double, float, int, unsigned int, short, unsigned short, unsigned char, char, complex<float>, complex<double>. Failed: None type.
3D: Pass: bool, double, float, int, unsigned int, short, unsigned short, unsigned char, char, complex<float>, complex<double>. Failed: None type.
4D: Pass: bool, double, float, int, unsigned int, short, unsigned short, unsigned char, char, complex<float>, complex<double>. Failed: None type.
*/
void AllDimensionAllTypeTest()
{
	IYapPython* python = LoadPythonDll();
	const wchar_t* function_2d = L"test2d";
	cout << "\t\t== test2d ==" << endl;
	cout << "unsigned int	" << endl; Test2D<unsigned int	>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "char			" << endl; Test2D<char			>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "unsigned char	" << endl; Test2D<unsigned char	>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "int			" << endl; Test2D<int			>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "unsigned int	" << endl; Test2D<unsigned int	>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "float			" << endl; Test2D<float			>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "double			" << endl; Test2D<double		>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "short			" << endl; Test2D<short			>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "unsigned short	" << endl; Test2D<unsigned short>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "bool			" << endl; Test2D<bool			>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "complex<float> " << endl; Test2D<complex<float>>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);
	cout << "complex<double>" << endl; Test2D<complex<double>>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 128, 64);

	const wchar_t* function_3d = L"test3d";
	cout << "\t\t== test3d ==" << endl;
	cout << "unsigned int	" << endl; Test3D<unsigned int	>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "char			" << endl; Test3D<char			>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "unsigned char	" << endl; Test3D<unsigned char	>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "int			" << endl; Test3D<int			>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "unsigned int	" << endl; Test3D<unsigned int	>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "float			" << endl; Test3D<float			>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "double			" << endl; Test3D<double		>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "short			" << endl; Test3D<short			>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "unsigned short	" << endl; Test3D<unsigned short>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "bool			" << endl; Test3D<bool			>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "complex<float>	" << endl; Test3D<complex<float>>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);
	cout << "complex<double>" << endl; Test3D<complex<double>>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 64, 64, 10);

	const wchar_t * function_4d = L"test4d";
	cout << "\t\t== test4d ==" << endl;
	cout << "unsigned int	" << endl; Test4D<unsigned int	>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "char			" << endl; Test4D<char			>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "unsigned char	" << endl; Test4D<unsigned char	>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "int			" << endl; Test4D<int			>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "unsigned int	" << endl; Test4D<unsigned int	>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "float			" << endl; Test4D<float			>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "double			" << endl; Test4D<double		>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "short			" << endl; Test4D<short			>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "unsigned short	" << endl; Test4D<unsigned short>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "bool			" << endl; Test4D<bool			>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "complex<float>	" << endl; Test4D<complex<float>>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
	cout << "complex<double>" << endl; Test4D<complex<double>>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 32, 32, 10, 4);
}

int main()
{
	YapPythonTest();
	AllDimensionAllTypeTest();
	system("pause");
	return 0;
};

