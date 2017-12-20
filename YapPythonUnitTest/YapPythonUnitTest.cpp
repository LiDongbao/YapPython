// YapPythonUnitTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#define BOOST_TEST_MODULE MyUnitTest

#include <boost\test\unit_test.hpp>
#include <boost\test\data\test_case.hpp>
#include <boost\test\data\monomorphic.hpp>
#include <boost\mpl\list.hpp>

#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include <windows.h>
#include <time.h>

namespace bstdata = boost::unit_test::data;

////////////// prepare function
size_t GetTotalSize(size_t dimension_count, size_t size[])
{
	size_t total_size = 1;
	for (size_t i = 0; i < dimension_count; ++i)
	{
		total_size *= size[i];
	}
	return total_size;
}

template<typename T>
T * CreateData(size_t dimension_count, size_t input_size[])
{
	//generate data
	size_t totalsize = GetTotalSize(dimension_count, input_size);
	T * input_data = new T[totalsize];
	T * p = input_data;
	std::srand((unsigned)time(NULL)); //用当前系统时间设置种子

	if (std::is_same<char, T>::value)
	{
		for (int i = 0; i < totalsize; ++i)
			input_data[i] = char(int(rand() % 127));
	}
	else if (std::is_same<unsigned char, T>::value)
	{
		for (int i = 0; i < totalsize; ++i)
			input_data[i] = unsigned char(int(rand() % 255));
	}
	else if (std::is_same<bool, T>::value)
	{
		for (int i = 0; i < totalsize; ++i)
			input_data[i] = bool(rand() % 2);
	}
	else
	{
		for (int i = 0; i < totalsize; ++i)
			input_data[i] = T(rand() % 1000);
	}

	return input_data;
}

template<typename T> 
T * GetReturnData(IYapPython &python, const wchar_t* module_name, const wchar_t* method_name, size_t input_dimensions, 
	T * input_data, size_t &output_dimensions, size_t input_size[], size_t output_size[])
{
	void* return_list = python.Process(module_name, method_name,
		data_type_id<T>::type, input_dimensions, input_data, output_dimensions, input_size, output_size);
	assert(return_list != nullptr);
	return reinterpret_cast<T*>(return_list);
}

IYapPython * GetPython()
{
	auto _module = ::LoadLibrary(L"D:\\Projects\\Demo_Cplus_extend_python\\x64\\Debug\\YapPythonDll.dll");
	assert(_module != nullptr);
	auto get_yap_python_func = (IYapPython*(*)())::GetProcAddress(_module, "GetYapPython");
	assert(get_yap_python_func != nullptr);

	return get_yap_python_func();
}

////////////// prepare data
size_t input_size[4] = { 10,11,12,13 };
size_t output_size[4] = { 0 };
size_t output_dimensions = 0;
IYapPython *python = GetPython();

// bool, complex<float>, complex<double>
typedef boost::mpl::list<int, unsigned int, short, unsigned short, double, float, char, unsigned char, bool> test_types;


BOOST_AUTO_TEST_CASE_TEMPLATE(test_1D_python, T, test_types)
{
	size_t input_dims = 1;
	auto in_data = CreateData<T>(input_dims, input_size);
	auto out_data = GetReturnData<T>(*python, L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py",
		L"test1d", input_dims, in_data, output_dimensions, input_size, output_size);

	for (size_t i = 0; i < GetTotalSize(input_dims, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
	for (size_t i = 0; i < input_dims; ++i)
	{
		BOOST_TEST(input_size[i] == output_size[i]);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_2D_python, T, test_types)
{
	size_t input_dims = 2;
	auto in_data = CreateData<T>(input_dims, input_size);
	auto out_data = GetReturnData<T>(*python, L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py",
		L"test2d", input_dims, in_data, output_dimensions, input_size, output_size);

	for (size_t i = 0; i < GetTotalSize(input_dims, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
	for (size_t i = 0; i < input_dims; ++i)
	{
		BOOST_TEST(input_size[i] == output_size[i]);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_3D_python, T, test_types)
{
	size_t input_dims = 3;
	auto in_data = CreateData<T>(input_dims, input_size);
	auto out_data = GetReturnData<T>(*python, L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py", 
		L"test3d", input_dims, in_data, output_dimensions, input_size, output_size);

	for (size_t i = 0; i < GetTotalSize(input_dims, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
	for (size_t i = 0; i < input_dims; ++i)
	{
		BOOST_TEST(input_size[i] == output_size[i]);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_4D_python, T, test_types)
{
	size_t input_dims = 4;
	auto in_data = CreateData<T>(input_dims, input_size);
	auto out_data = GetReturnData<T>(*python, L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py", 
		L"test4d", input_dims, in_data, output_dimensions, input_size, output_size);

	for (size_t i = 0; i < GetTotalSize(input_dims, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
	for (size_t i = 0; i < input_dims; ++i)
	{
		BOOST_TEST(input_size[i] == output_size[i]);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_3D_2D_python, T, test_types)
{
	size_t input_dims = 3;
	auto in_data = CreateData<T>(input_dims, input_size);
	auto out_data = GetReturnData<T>(*python, L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py",
		L"test3d2d", input_dims, in_data, output_dimensions, input_size, output_size);
	// get 1st slice, so the output_data == input_data[0:output_size]
	for (size_t i = 0; i < GetTotalSize(output_dimensions, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
	BOOST_TEST(output_dimensions == 2);
}


BOOST_AUTO_TEST_CASE(test_stop)
{
	system("pause");
}

