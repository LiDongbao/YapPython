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
T * GetReturnData(T * input_data, IYapPython &python, const wchar_t* module_name, const wchar_t* method_name,
	size_t dimension_count, size_t input_size[], size_t output_size[])
{
	auto return_list = python.Process(module_name, method_name,
		data_type_id<T>::type, input_data, dimension_count, input_size, output_size);
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
size_t input_size[4] = { 10,10,50,50 };
size_t output_size[4] = { 0 };
IYapPython *python = GetPython();

// bool, complex<float>, complex<double>
typedef boost::mpl::list<int, unsigned int, short, unsigned short, double, float, char, unsigned char> test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(test_2D_python, T, test_types)
{
	auto in_data = CreateData<T>(2, input_size);
	auto out_data = GetReturnData<T>(in_data, *python, L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py", L"test2d", 2, input_size, output_size);

	for (size_t i = 0; i < GetTotalSize(2, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_3D_python, T, test_types)
{
	auto in_data = CreateData<T>(3, input_size);
	auto out_data = GetReturnData<T>(in_data, *python, L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py", L"test3d", 3, input_size, output_size);

	for (size_t i = 0; i < GetTotalSize(3, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_4D_python, T, test_types)
{
	auto in_data = CreateData<T>(4, input_size);
	auto out_data = GetReturnData<T>(in_data, *python, L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py", L"test4d", 4, input_size, output_size);

	for (size_t i = 0; i < GetTotalSize(4, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
}

BOOST_AUTO_TEST_CASE(test_stop)
{
	system("pause");
}

