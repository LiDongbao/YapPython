// YapPythonUnitTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#pragma once
#define BOOST_TEST_MODULE MyUnitTest

#include <boost\test\unit_test.hpp>
#include <boost\test\data\test_case.hpp>
#include <boost\test\data\monomorphic.hpp>
#include <boost\mpl\list.hpp>

#include "..\Demo_Cplus_extend_python\IYapPython.h"
#include <windows.h>
#include <time.h>


namespace bstdata = boost::unit_test::data;

struct  PerpareData
{
	PerpareData() : output_dimensions{0}
	{
		for (size_t i = 0; i < 10; ++i)
		{
			input_size[i] = 10 + i;
			output_size[i] = 0;
		}
		python = GetPython();
		script_path = L"D:\\Projects\\Demo_Cplus_extend_python\\PythonScripts\\Py2C.py";
	};
	~PerpareData()
	{
	};

	size_t GetTotalSize(size_t dimension_count, size_t size[])
	{
		size_t total_size = 1;
		for (size_t i = 0; i < dimension_count; ++i)
		{
			total_size *= size[i];
		}
		return total_size;
	}

#pragma warning(push)
#pragma warning(disable:4800)
	template<typename T>
	T * CreateData(size_t dimension_count, size_t input_size[])
	{
		//generate data
		size_t totalsize = GetTotalSize(dimension_count, input_size);
		T * input_data = new T[totalsize];
		T * p = input_data;
		std::srand((unsigned)time(NULL));

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
#pragma warning(pop)

	template<typename T>
	T * GetReturnData(IYapPython &python, const wchar_t* module_name, const wchar_t* method_name, size_t input_dimensions,
		T * input_data, size_t &output_dimensions, size_t input_size[], size_t output_size[], bool is_need_ref_data = 0)
	{
		void* return_list = python.Process(module_name, method_name,
			data_type_id<T>::type, input_dimensions, input_data, output_dimensions, input_size, output_size, is_need_ref_data);
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

	size_t input_size[10];
	size_t output_size[10];
	size_t output_dimensions;
	IYapPython *python;
	wchar_t * script_path;
};

BOOST_FIXTURE_TEST_SUITE(S,PerpareData)


typedef boost::mpl::list<int, unsigned int, short, unsigned short, double, float, char, unsigned char, bool> test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(test_1D_python, T, test_types)
{
	size_t input_dims = 1;
	auto in_data = CreateData<T>(input_dims, input_size);
	auto out_data = GetReturnData<T>(*python, script_path,
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
	auto out_data = GetReturnData<T>(*python, script_path,
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
	auto out_data = GetReturnData<T>(*python, script_path, 
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
	auto out_data = GetReturnData<T>(*python, script_path, 
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
	auto out_data = GetReturnData<T>(*python, script_path,
		L"test3d2d", input_dims, in_data, output_dimensions, input_size, output_size);
	// get 1st slice, so the output_data == input_data[0:output_size]
	for (size_t i = 0; i < GetTotalSize(output_dimensions, input_size); ++i)
	{
		BOOST_TEST(in_data[i] == out_data[i]);
	}
	BOOST_TEST(output_dimensions == 2);
}

BOOST_AUTO_TEST_CASE(test_ref_data_python)
{
	size_t input_dims = 3;
	auto in_data = CreateData< int >(input_dims, input_size);
	auto ref_data = reinterpret_cast<void*>(CreateData< int >(input_dims, input_size));
	auto out_data = GetReturnData< int >(*python, script_path,
		L"test3d", input_dims, in_data, output_dimensions, input_size, output_size, true);
	BOOST_TEST(out_data == (int*)nullptr);

	python->SetRefData(ref_data, DataTypeInt, input_dims, input_size);
	auto out_data1 = GetReturnData< int >(*python, script_path,
		L"test3d2image", input_dims, in_data, output_dimensions, input_size, output_size, true);
	BOOST_TEST(out_data1 != (int*)nullptr);

	python->DeleteRefData();
	auto out_data2 = GetReturnData< int >(*python, script_path,
		L"test3d2image", input_dims, in_data, output_dimensions, input_size, output_size, true);
	BOOST_TEST(out_data2 == (int*)nullptr);

	auto out_data3 = GetReturnData< int >(*python, script_path,
		L"test3d", input_dims, in_data, output_dimensions, input_size, output_size, false);
	BOOST_TEST(out_data3 != (int*)nullptr);

}


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_CASE(test_stop)
{
	system("pause");
}
