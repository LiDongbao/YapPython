#include "stdafx.h"
#include "YapPython.h"
#include "IYapPython.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

//#pragma warning(push)
#pragma warning(disable:4244)

#include <stdlib.h>
#include <complex>
#include <vector>
#include <list>

#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/import.hpp>
#include <boost/python/stl_iterator.hpp>
#include <vcruntime.h>
#include <iostream>

//#pragma warning(pop)

using namespace std;
namespace boostpy = boost::python;

std::string ToMbs(const wchar_t * wcs)
{
	assert(wcslen(wcs) < 500 && "The string cannot contain more than 500 characters.");

	static char buffer[1024];
	size_t size;

	wcstombs_s(&size, buffer, (size_t)1024, wcs, (size_t)1024);
	return std::string(buffer);
}

class YapPythonImpl : public IYapPython
{

public:
	YapPythonImpl();
	~YapPythonImpl();

	virtual void* Process(const wchar_t* module, const wchar_t* method, int data_type,
		size_t input_dimensions, void * data,  size_t& output_dimensions, size_t input_size[], size_t output_size[]) override
	{
		switch (data_type)
		{
		case DataTypeInt:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<int*>(data),output_dimensions, input_size, output_size);
		case DataTypeUnsignedInt:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned int*>(data), output_dimensions, input_size, output_size);
		case DataTypeChar:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<char*>(data), output_dimensions, input_size, output_size);
		case DataTypeUnsignedChar:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned char*>(data), output_dimensions, input_size, output_size);
		case DataTypeShort:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<short*>(data), output_dimensions, input_size, output_size);
		case DataTypeUnsignedShort:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned short*>(data), output_dimensions, input_size, output_size);
		case DataTypeFloat:				   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<float*>(data), output_dimensions, input_size, output_size);
		case DataTypeDouble:			   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<double*>(data), output_dimensions, input_size, output_size);
		case DataTypeComplexFloat:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<std::complex<float>*>(data), output_dimensions, input_size, output_size);
		case DataTypeComplexDouble:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<std::complex<double>*>(data), output_dimensions, input_size, output_size);
		case DataTypeBool:				   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<bool*>(data), output_dimensions, input_size, output_size);
		default:
			return nullptr;
		}
	}

	virtual void* Process(const wchar_t* module, const wchar_t* method, int data_type,
		size_t input_dimensions, void * data1, void * data2, size_t&output_dimensions, size_t input_size[], size_t output_size[]) override
	{
		switch (data_type)
		{
		case DataTypeInt:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<int*>(data1), reinterpret_cast<int*>(data2), output_dimensions, input_size, output_size);
		case DataTypeUnsignedInt:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned int*>(data1), reinterpret_cast<unsigned int*>(data2), output_dimensions, input_size, output_size);
		case DataTypeChar:				   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<char*>(data1), reinterpret_cast<char*>(data2), output_dimensions, input_size, output_size);
		case DataTypeUnsignedChar:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned char*>(data1), reinterpret_cast<unsigned char*>(data2), output_dimensions, input_size, output_size);
		case DataTypeShort:				   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<short*>(data1), reinterpret_cast<short*>(data2), output_dimensions, input_size, output_size);
		case DataTypeUnsignedShort:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned short*>(data1), reinterpret_cast<unsigned short*>(data2), output_dimensions, input_size, output_size);
		case DataTypeFloat:				   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<float*>(data1), reinterpret_cast<float*>(data2), output_dimensions, input_size, output_size);
		case DataTypeDouble:			   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<double*>(data1), reinterpret_cast<double*>(data2), output_dimensions, input_size, output_size);
		case DataTypeComplexFloat:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<std::complex<float>*>(data1), reinterpret_cast<std::complex<float>*>(data2), output_dimensions, input_size, output_size);
		case DataTypeComplexDouble:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<std::complex<double>*>(data1), reinterpret_cast<std::complex<double>*>(data2), output_dimensions, input_size, output_size);
		case DataTypeBool:				   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<bool*>(data1), reinterpret_cast<bool*>(data2), output_dimensions, input_size, output_size);
		default:
			return nullptr;
		}
	}

protected:
	/*
	template<typename T>
	void pylist2Array(const boostpy::object& iterable, T *& out_data, size_t size[])
	{
		T * source_data = std::vector<T>(boostpy::stl_input_iterator<T>(iterable),
			boostpy::stl_input_iterator<T>()).data();
		memcpy(out_data, source_data, (size_t)(size[0] * sizeof(T)));
		// out_data = out_data + (size_t)(size[0] * sizeof(T));
	}
	template<>
	void pylist2Array(const boostpy::object& iterable, bool *& out_data, size_t size[])
	{
		for (size_t i = 0; i < size_t(boostpy::len(iterable)); ++i)
		{
			*(out_data++) = boostpy::extract<bool>(iterable[i]);
		}
	}
	*/

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
	void* DoProcess(const wchar_t* module, const wchar_t* method,
		size_t input_dimensions,T * data, size_t &output_dimensions, size_t input_size[], size_t output_size[])
	{
		if (!Py_IsInitialized())
			return nullptr;

		try
		{
			boostpy::object main_module = boostpy::import("__main__");
			boostpy::object main_namespace = main_module.attr("__dict__");
			boostpy::object simple = boostpy::exec_file(ToMbs(module).c_str(), main_namespace, main_namespace);
			boostpy::object func = main_namespace[ToMbs(method).c_str()];

			// convert T* to python list
			auto in_data_list = CArray2Pylist(data, input_dimensions, input_size);

			boostpy::list return_list;
			switch (input_dimensions)
			{
			case 1:
				return_list = boostpy::extract<boostpy::list>(func(in_data_list, input_size[0]));
				break;
			case 2:
				return_list = boostpy::extract<boostpy::list>(func(in_data_list, input_size[0], input_size[1]));
				break;
			case 3:
				return_list = boostpy::extract<boostpy::list>(func(in_data_list, input_size[0], input_size[1], input_size[2]));
				break;
			case 4:
				return_list = boostpy::extract<boostpy::list>(func(in_data_list, input_size[0], input_size[1], input_size[2], input_size[3]));
				break;
			default:
				return nullptr;
			}

			size_t output_dims = boostpy::extract<int>(return_list[0]);
			if (PyList_Size(return_list.ptr()) == output_dims + 2)
			{
				boostpy::list out_data_list = boostpy::extract<boostpy::list>(return_list[1]);
				if (!PyList_Check(out_data_list.ptr()))
					return nullptr;
				for (size_t i = 0; i < output_dims; ++i)
				{
					output_size[i] = boostpy::extract<size_t>(return_list[i + 2]);
				}
				T* output_data = new T[GetTotalSize(output_dims, output_size)];
				Pylist2CArray(out_data_list, output_data, output_dims, output_size);
				output_dimensions = output_dims;
				return output_data;
			}
			else
			{
				// 无返回数据，该函数返回的返回值output_data的指针也自然为nullptr。
				// 或者返回数据格式出错！
				return nullptr;
			}
		}
		catch (boostpy::error_already_set const&)
		{
			PyErr_Print();
			PyErr_Clear();
		}
		output_dimensions = 0;
		return nullptr;
	};

	template<typename T1,typename T2>
	void* DoProcess(const wchar_t* module, const wchar_t* method,
		size_t input_dimensions,T1 * data1, T2* data2,size_t &output_dimensions, size_t input_size[], size_t output_size[])
	{
		if (!Py_IsInitialized())
			return nullptr;

		try
		{
			boostpy::object main_module = boostpy::import("__main__");
			boostpy::object main_namespace = main_module.attr("__dict__");
			boostpy::object simple = boostpy::exec_file(ToMbs(module).c_str(), main_namespace, main_namespace);
			boostpy::object func = main_namespace[ToMbs(method).c_str()];

			// convert T* to python list
			boostpy::list return_list;
			auto in_data_list1 = CArray2Pylist(data1, input_dimensions, input_size);
			auto in_data_list2 = CArray2Pylist(data2, input_dimensions, input_size);

			switch (input_dimensions)
			{
			case 1:
				return_list = boostpy::extract<boostpy::list>(func(in_data_list1, in_data_list2, input_size[0]));
				break;
			case 2:
				return_list = boostpy::extract<boostpy::list>(func(in_data_list1, in_data_list2, input_size[0], input_size[1]));
				break;
			case 3:
				return_list = boostpy::extract<boostpy::list>(func(in_data_list1, in_data_list2, input_size[0], input_size[1], input_size[2]));
				break;
			case 4:
				return_list = boostpy::extract<boostpy::list>(func(in_data_list1, in_data_list2, input_size[0], input_size[1], input_size[2], input_size[3]));
				break;
			default:
				return nullptr;
			}

			// 添加输出数据的维度判断，根据维度提示，进行数据读取；
			// 返回的数据格式为 : [output_dimensions，数据list，第一维大小，第二维大小, ... ]
			// 如果output_dimension为0，则表示无数据返回，要求返回的列表必须是：[0] 否则不予处理
			// 若 output_dimensions = 1, 则返回列表为：[1, [width个数据点], width]
			// 若 output_dimensions = 2, 则返回列表为：[2, [[width个数据点],[[width个数据点]], ...,[width个数据点]], width, height]，数据内部为height个[width个数据点] list
			// 依次类推...
			size_t output_dims = boostpy::extract<int>(return_list[0]);
			if (PyList_Size(return_list.ptr()) == output_dims + 2)
			{
				boostpy::list out_data_list = boostpy::extract<boostpy::list>(return_list[1]);
				if (!PyList_Check(out_data_list.ptr()))
					return nullptr;
				for (size_t i = 0; i < output_dims; ++i)
				{
					output_size[i] = boostpy::extract<size_t>(return_list[i + 2]);
				}
				T1* output_data = new T1[GetTotalSize(output_dims, output_size)];
				Pylist2CArray(out_data_list, output_data, output_dims, output_size);
				output_dimensions = output_dims;
				return output_data;
			}
			else
			{
				// 无返回数据，该函数返回的返回值output_data的指针也自然为nullptr。
				// 或者返回数据格式出错！
				return nullptr;
			}
		}
		catch (boostpy::error_already_set const&)
		{
			PyErr_Print();
			PyErr_Clear();
		}
		output_dimensions = 0;
		return nullptr;
	};

	template<typename T>
	boostpy::list YapPythonImpl::CArray2Pylist(const T* data, size_t dimension_count, size_t size[])
	{
		const T* p = data;
		vector<size_t> reversed_size(dimension_count);
		for (size_t i = 0; i < dimension_count; ++i)
		{
			reversed_size[i] = size[dimension_count - i - 1];
		}

		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}

	/*convert 1D array to python 2D list*/
	template<typename T>
	boostpy::list YapPythonImpl::DoCArray2Pylist(const T*& data, size_t dimension_count, size_t size[])
	{
		boostpy::list result;
		if (dimension_count == 1)
		{
			for (size_t i = 0; i < size[0]; ++i)
			{
				result.append(*data++);
			}
		}
		else
		{
			for (size_t i = 0; i < size[0]; ++i)
			{
				result.append(DoCArray2Pylist(data, dimension_count - 1, size + 1));
			}
		}
		return result;
	};

	template<typename T>
	void YapPythonImpl::Pylist2CArray(const boostpy::list& li, T* out_data, size_t dimension_count, size_t size[])
	{
		T* p = out_data;
		size_t * reversed_size = new size_t[4];
		// vector<size_t> reversed_size(dimension_count);
		for (size_t i = 0; i < dimension_count; ++i)
		{
			reversed_size[i] = size[dimension_count - i - 1];
		}

		DoPylist2CArray(li, p, dimension_count, reversed_size);
	}

	/*
	use function:[ pylist2Vector ] to convert pylist to vector, then memory allocate to c++ array.
	*/
	template<typename T>
	void YapPythonImpl::DoPylist2CArray(const boostpy::list& li, T*& out_data, size_t dim_count, size_t size[])
	{
		assert(out_data != nullptr);

		if (dim_count == 1)
		{
			for (size_t i = 0; i < size[0]; ++i)
			{
				*(out_data++) = boostpy::extract<T>(li[i]);
			}
		}
		else
		{
			for (size_t i = 0; i < size[0]; ++i)
			{
				auto sub_list = boostpy::extract<boostpy::list>(li[i]);
				DoPylist2CArray(sub_list, out_data, dim_count - 1, size + 1);
			}

		}
	}
};

YapPythonImpl::YapPythonImpl()
{
	Py_Initialize();
	if (!Py_IsInitialized())
		throw PyErr_Occurred;
}

YapPythonImpl::~YapPythonImpl()
{
	if (Py_IsInitialized()) 
	{
		Py_Finalize(); 
	}
};

IYapPython * PythonFactory::GetPython()
{
	return new YapPythonImpl();
}
