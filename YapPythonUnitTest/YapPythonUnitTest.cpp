// YapPythonUnitTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#define BOOST_TEST_MODULE MyUnitTest

#include <boost\test\unit_test.hpp>
#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include "..\Demo_Cplus_extend_python\YapPython.h"
#include <complex>
#include <windows.h>
#include <boost/python/str.hpp>
#include <comutil.h>
#include <atlconv.h>
#include <iostream>


using namespace std;

class Check2D {

protected:
	template<typename T>
	void Check2DNoResizeEqual(IYapPython& python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight)
	{
		size_t out_width = 0, out_height = 0;
		T * input_data = new T[dwidth*dheight];
		T * output_data = reinterpret_cast<T*>(create2DOutputData(input_data, python, module_name,
			method_name, dwidth, dheight, out_width, out_height));
		BOOST_CHECK(dwidth == out_width);
		BOOST_CHECK(dheight == out_height);
		for (int i = 0; i < dwidth*dheight; ++i)
			BOOST_CHECK(input_data[i] == output_data[i]);
		delete input_data, output_data;
		return;
	};

	template<typename T>
	void Check2DNoResizeFloat(IYapPython& python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight)
	{
		size_t out_width = 0, out_height = 0;
		T * input_data = new T[dwidth*dheight];
		T * output_data = reinterpret_cast<T*>(create2DOutputData(input_data, python, module_name,
			method_name, dwidth, dheight, out_width, out_height));
		BOOST_CHECK(dwidth == out_width);
		BOOST_CHECK(dheight == out_height);
		for (int i = 0; i < dwidth*dheight; ++i)
			BOOST_CHECK((input_data[i] - output_data[i]) < 1e-7 && (input_data[i] - output_data[i]) > -1e-7);
		delete input_data, output_data;
		return;
	};

	template<typename T>
	void Check2DNoResizeComplex(IYapPython& python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight)
	{
		size_t out_width = 0, out_height = 0;
		T * input_data = new T[dwidth*dheight];
		T * output_data = reinterpret_cast<T*>(create2DOutputData(input_data, python, module_name,
			method_name, dwidth, dheight, out_width, out_height));
		BOOST_CHECK(dwidth == out_width);
		BOOST_CHECK(dheight == out_height);
		// complex<float>* input = reinterpret_cast<complex<float>*>(input_data);
		// complex<float>* output = reinterpret_cast<complex<float>*>(output_data);
		for (int i = 0; i < dwidth*dheight; ++i)
		{
			BOOST_CHECK(abs(input_data[i].real() - output_data[i].real()) < 1e-7);
			BOOST_CHECK(abs(input_data[i].imag() - output_data[i].imag()) < 1e-7);
		}
		delete []input_data;
		delete []output_data;

		return;
	};

	template<typename T>
	void* create2DOutputData(T * input_data, IYapPython &python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight, size_t& out_width, size_t & out_height)
	{
		T *p = input_data;
		for (size_t i = 0; i < dwidth * dheight; ++i)
		{
			*(p++) = T(static_cast<float>(i));
		}
		return python.Process(module_name, method_name, data_type_id<T>::type, input_data, dwidth, dheight, out_width, out_height);
	}

	template<typename T>
	void Check2DResize(IYapPython& python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight)
	{
		size_t out_width = 0, out_height = 0;
		T * input_data = new T[dwidth*dheight];
		T * output_data = reinterpret_cast<T*>(create2DOutputData(input_data, python, module_name,
			method_name, dwidth, dheight, out_width, out_height));
		BOOST_CHECK(dwidth != out_width || dheight != out_height);
		BOOST_CHECK(output_data != nullptr);
		delete input_data, output_data;
		return;
	};

public:
	void Check2DNoResizeAll(IYapPython& python, const wchar_t* module_name,
		const wchar_t* method_name, size_t dwidth, size_t dheight)
	{
		Check2DNoResizeEqual<char>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeEqual<unsigned char>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeEqual<short>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeEqual<unsigned short>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeFloat<float>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeFloat<double>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeEqual<int>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeEqual<unsigned int>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeComplex<std::complex<float>>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeComplex<std::complex<double>>(python, module_name, method_name, dwidth, dheight);
		Check2DNoResizeEqual<bool>(python, module_name, method_name, dwidth, dheight);
	};

	void Check2DResizeAll(IYapPython& python, const wchar_t* module_name,
		const wchar_t* method_name, size_t dwidth, size_t dheight)
	{
		Check2DResize<char>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<unsigned char>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<short>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<unsigned short>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<float>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<double>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<int>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<unsigned int>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<std::complex<float>>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<std::complex<double>>(python, module_name, method_name, dwidth, dheight);
		Check2DResize<bool>(python, module_name, method_name, dwidth, dheight);
	};
};

class Check3D {
protected:
	template<typename T>
	void* create3DOutputData(T * input_data, IYapPython &python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight, size_t dslice, size_t &out_width, size_t &out_height, size_t &out_slice)
	{
		T *p = input_data;
		for (size_t i = 0; i < dwidth*dheight*dslice; ++i)
		{
			*(p++) = T(static_cast<float>(i));
		}
		return python.Process(module_name, method_name, data_type_id<T>::type,
			input_data, dwidth, dheight, dslice,
			out_width, out_height, out_slice);
	}

	template<typename T>
	void Check3DNoResizeEqual(IYapPython& python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight, size_t dslice)
	{
		size_t out_width = 0, out_height = 0, out_slice = 0;
		T * input_data = new T[dwidth*dheight*dslice];
		T * output_data = reinterpret_cast<T*>(create3DOutputData(input_data, python, module_name,
			method_name, dwidth, dheight, dslice, out_width, out_height, out_slice));
		BOOST_CHECK(dwidth == out_width);
		BOOST_CHECK(dheight == out_height);
		BOOST_CHECK(dslice == out_slice);
		for (int i = 0; i < dwidth*dheight*dslice; ++i)
			BOOST_CHECK(input_data[i] == output_data[i]);
		delete input_data, output_data;
		return;
	};

	template<typename T>
	void Check3DNoResizeFloat(IYapPython& python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight, size_t dslice)
	{
		size_t out_width = 0, out_height = 0, out_slice = 0;
		T * input_data = new T[dwidth*dheight*dslice];
		T * output_data = reinterpret_cast<T*>(create3DOutputData(input_data, python, module_name,
			method_name, dwidth, dheight, dslice, out_width, out_height, out_slice));
		BOOST_CHECK(dwidth == out_width);
		BOOST_CHECK(dheight == out_height);
		BOOST_CHECK(dslice == out_slice);
		for (int i = 0; i < dwidth*dheight*dslice; ++i)
			BOOST_CHECK(abs(input_data[i] - output_data[i]) < 1e-7);
		delete input_data, output_data;
		return;
	};

	template<typename T>
	void Check3DNoResizeComplex(IYapPython& python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight, size_t dslice)
	{
		size_t out_width = 0, out_height = 0, out_slice = 0;
		T * input_data = new T[dwidth*dheight*dslice];
		T * output_data = reinterpret_cast<T*>(create3DOutputData(input_data, python, module_name,
			method_name, dwidth, dheight, dslice, out_width, out_height, out_slice));
		BOOST_CHECK(dwidth == out_width);
		BOOST_CHECK(dheight == out_height);
		BOOST_CHECK(dslice == out_slice);
		for (int i = 0; i < dwidth*dheight; ++i)
		{
			BOOST_CHECK(abs(input_data[i].real() - output_data[i].real()) < 1e-7);
			BOOST_CHECK(abs(input_data[i].imag() - output_data[i].imag()) < 1e-7);
		}
		delete[]input_data;
		delete[]output_data;
	};

	template<typename T>
	void Check3DResize(IYapPython& python, const wchar_t* module_name, const wchar_t* method_name,
		size_t dwidth, size_t dheight, size_t dslice)
	{
		size_t out_width = 0, out_height = 0, out_slice = 0;
		T * input_data = new T[dwidth*dheight*dslice];
		T * output_data = reinterpret_cast<T*>(create3DOutputData(input_data, python, module_name,
			method_name, dwidth, dheight, dslice, out_width, out_height, out_slice));

		BOOST_CHECK((dwidth != out_width) || (dheight != out_height) || (dslice != out_slice));
		BOOST_CHECK(output_data != nullptr);
		delete[]input_data;
		delete[]output_data;
	};

public:

	void Check3DNoResizeAll(IYapPython& python, const wchar_t* module_name,
		const wchar_t* method_name, size_t dwidth, size_t dheight, size_t dslice)
	{
		Check3DNoResizeEqual<char>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeEqual<unsigned char>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeEqual<short>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeEqual<unsigned short>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeFloat<float>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeFloat<double>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeEqual<int>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeEqual<unsigned int>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeComplex<std::complex<float>>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeComplex<std::complex<double>>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DNoResizeEqual<bool>(python, module_name, method_name, dwidth, dheight, dslice);
	};

	void Check3DResizeAll(IYapPython& python, const wchar_t* module_name,
		const wchar_t* method_name, size_t dwidth, size_t dheight, size_t dslice)
	{
		Check3DResize<char>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<unsigned char>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<short>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<unsigned short>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<float>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<double>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<int>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<unsigned int>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<std::complex<float>>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<std::complex<double>>(python, module_name, method_name, dwidth, dheight, dslice);
		Check3DResize<bool>(python, module_name, method_name, dwidth, dheight, dslice);
	};

};

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(YapPythonTest, 2)

BOOST_AUTO_TEST_CASE(YapPythonTest)
{
	auto _module = ::LoadLibrary(L"C:\\Users\\James\\Documents\\Visual Studio 2015\\Projects\\Demo_Cplus_extend_python\\x64\\Debug\\YapPython_demo.dll");
	if (!_module)
		return;
	auto python_add = (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	if (python_add == nullptr)
		return;

	IYapPython* python = python_add();

	size_t dheight = 64, dwidth = 64, dslice = 64;
	const wchar_t * module_name = L"D:\\Projects\\YAP\\PluginSDK\\BasicRecon\\Python\\Py2C.py",
		*method_name = L"ShowImage", *method_name3d = L"ShowImage3d",
		*class_name = L"Py4C", *class_function = L"ShowImage3D";

	// check 2d
	Check2D check2d;
	check2d.Check2DNoResizeAll(*python, module_name, method_name, dwidth, dheight);
	check2d.Check2DResizeAll(*python, module_name, method_name, dwidth, dheight);

	// check 3d
	Check3D check3d;
	check3d.Check3DNoResizeAll(*python, module_name, method_name3d, dwidth, dheight, dslice);
	check3d.Check3DResizeAll(*python, module_name, method_name3d, dwidth, dheight, dslice);

	// system("pause");
};


//////////////////////////////////////////////////////////////////////////
template<typename MYTYPE>
void buidData(MYTYPE* data, size_t image_size)
{

	if (std::is_same<char, MYTYPE>::value)
	{
		for (int i = 0; i < image_size; ++i)
			data[i] = unsigned char(int(i % 127));
	}
	else if (std::is_same<unsigned char, MYTYPE>::value)
	{
		for (int i = 0; i < image_size; ++i)
			data[i] = unsigned char(int(i % 255));
	}
	else if (std::is_same<bool, MYTYPE>::value)
	{
		for (int i = 0; i < image_size; ++i)
			data[i] = MYTYPE(i % 2);
	}
	for (int i = 0; i < image_size; ++i)
		data[i] = MYTYPE(i);
}

template<typename MYTYPE>
void Test(IYapPython* python, const wchar_t * module_name,
	const wchar_t * method_name, size_t dimension_size, size_t input_size[])
{
	size_t output_size[4] = { 0 };
	size_t input_image_size = 1;
	for (size_t i = 0; i < dimension_size; ++i)
	{
		input_image_size *= input_size[i];
	}
	//	std::vector<MYTYPE> input_data(input_image_size);
	MYTYPE* input_data = new  MYTYPE[input_image_size];
	buidData(input_data, input_image_size);
	cout << "\tinput  data:  " << input_data[0] << " ~ " << input_data[input_image_size - 1] << endl;

	MYTYPE* output_data = reinterpret_cast<MYTYPE*>(python->Process(module_name, method_name,
		data_type_id<MYTYPE>::type, input_data, 2, input_size, output_size));
	size_t output_image_size = 1;
	for (size_t i = 0; i < dimension_size; ++i)
	{
		output_image_size *= output_size[i];
	}
	cout << "\toutput data:  " << *(output_data) << " ~ " << *(output_data + output_image_size - 1) << endl;

	delete[]output_data;
}
/* Test result:
2D: Pass: bool, double, float, int, unsigned int, short, unsigned short, unsigned char, char, complex<float>, complex<double>. Failed: None type.
3D: Pass: bool, double, float, int, unsigned int, short, unsigned short, unsigned char, char, complex<float>, complex<double>. Failed: None type.
4D: Pass: bool, double, float, int, unsigned int, short, unsigned short, unsigned char, char, complex<float>, complex<double>. Failed: None type.
*/
void AllDimensionAllTypeTest()
{
	using namespace std;
	auto _module = ::LoadLibrary(L"..\\x64\\Debug\\YapPythonDll.dll");
	if (!_module)
	{
		cout << "Error loading YapPythonDll.dll.\n";
		return ;
	}

	auto get_yap_python_func = (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	if (get_yap_python_func == nullptr)
	{
		cout << "Cannot find GetYapPython() in YapPythonDLL.dll.\n";
		return ;
	}
	IYapPython* python = get_yap_python_func();
	const wchar_t* function_2d = L"test2d";
	size_t input_size[4] = { 32, 32, 10, 4 };
	cout << "\t\t== test2d ==" << endl;
	cout << "unsigned int	" << endl; Test<unsigned int	>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 2, input_size);
	cout << "char			" << endl; Test<char			>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 2, input_size);
	cout << "unsigned char	" << endl; Test<unsigned char	>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 2, input_size);
	cout << "int			" << endl; Test<int				>(python, L"..\\PythonScripts\\Py2C.py", function_2d, 2, input_size);

	const wchar_t* function_3d = L"test3d";
	cout << "\t\t== test3d ==" << endl;
	cout << "float			" << endl; Test<float			>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 3, input_size);
	cout << "double			" << endl; Test<double			>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 3, input_size);
	cout << "short			" << endl; Test<short			>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 3, input_size);
	cout << "unsigned short	" << endl; Test<unsigned short	>(python, L"..\\PythonScripts\\Py2C.py", function_3d, 3, input_size);

	const wchar_t * function_4d = L"test4d";
	cout << "\t\t== test4d ==" << endl;
	cout << "bool			" << endl; Test<bool			>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 4, input_size);
	cout << "complex<float>	" << endl; Test<complex<float>	>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 4, input_size);
	cout << "complex<double>" << endl; Test<complex<double>	>(python, L"..\\PythonScripts\\Py2C.py", function_4d, 4, input_size);
}
