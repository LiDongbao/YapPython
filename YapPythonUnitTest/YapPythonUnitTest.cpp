// YapPythonUnitTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#define BOOST_TEST_MODULE MyUnitTest

#include <boost\test\unit_test.hpp>
#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include "..\Demo_Cplus_extend_python\YapPythonImp.h"
#include <complex>
#include <windows.h>
#include <boost/python/str.hpp>
#include <comutil.h>
#include <atlconv.h>


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
		delete input_data, output_data;
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
		return python.Process2D(module_name, method_name, data_type_id<T>::type, input_data, dwidth, dheight, out_width, out_height);
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
		return python.Process3D(module_name, method_name, data_type_id<T>::type, 
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
		delete input_data, output_data;
		return;
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
		delete input_data, output_data;
		return;
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

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(my_test1, 1)
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(YapPythonTest, 2)

BOOST_AUTO_TEST_CASE(my_test1)
{
	BOOST_TEST(2 == 1);
};

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
