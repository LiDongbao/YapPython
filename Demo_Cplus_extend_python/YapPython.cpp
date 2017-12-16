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

//#pragma warning(pop)

using namespace std;
namespace boostpy = boost::python;

std::string ToMbs(const wchar_t * wcs)
{
	assert(wcslen(wcs) < 500 && "The string cannot contain more than 500 characters.");

	static char buffer[1024];

#pragma warning(once:4996)
	wcstombs(buffer, wcs, 1024);//wcstombs_s(wcs,buffer,1024);

	return std::string(buffer);
}

class YapPythonImpl : public IYapPython
{

public:
	YapPythonImpl();
	~YapPythonImpl();

	virtual void* Process(const wchar_t* module, const wchar_t* method, int data_type,
		void * data, size_t dim_count, size_t input_size[], size_t output_size[])
	{
		switch (data_type)
		{
		case DataTypeInt:
			return DoProcess(module, method, reinterpret_cast<int*>(data), dim_count, input_size, output_size);
		case DataTypeUnsignedInt:
			return DoProcess(module, method, reinterpret_cast<unsigned int*>(data), dim_count, input_size, output_size);
		case DataTypeChar:
			return DoProcess(module, method, reinterpret_cast<char*>(data), dim_count, input_size, output_size);
		case DataTypeUnsignedChar:
			return DoProcess(module, method, reinterpret_cast<unsigned char*>(data), dim_count, input_size, output_size);
		case DataTypeShort:
			return DoProcess(module, method, reinterpret_cast<short*>(data), dim_count, input_size, output_size);
		case DataTypeUnsignedShort:
			return DoProcess(module, method, reinterpret_cast<unsigned short*>(data), dim_count, input_size, output_size);
		case DataTypeFloat:
			return DoProcess(module, method, reinterpret_cast<float*>(data), dim_count, input_size, output_size);
		case DataTypeDouble:
			return DoProcess(module, method, reinterpret_cast<double*>(data), dim_count, input_size, output_size);
		case DataTypeComplexFloat:
			return DoProcess(module, method, reinterpret_cast<std::complex<float>*>(data), dim_count, input_size, output_size);
		case DataTypeComplexDouble:
			return DoProcess(module, method, reinterpret_cast<std::complex<double>*>(data), dim_count, input_size, output_size);
		case DataTypeBool:
			return DoProcess(module, method, reinterpret_cast<bool*>(data), dim_count, input_size, output_size);
		default:
			return nullptr;
		}
	}

protected:
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
		T * data, size_t dim_count, size_t input_size[], size_t output_size[])
	{
		if (!Py_IsInitialized())
			return nullptr;

		try
		{
			auto main_module = boostpy::import("__main__");
			auto main_namespace = main_module.attr("__dict__");
			boostpy::object simple = boostpy::exec_file(ToMbs(module).c_str(), main_namespace, main_namespace);
			boostpy::object func = main_namespace[ToMbs(method).c_str()];

			// convert T* to python list
			boostpy::list return_list;
			auto in_data_list = CArray2Pylist(data, dim_count, input_size);

			switch (dim_count)
			{
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

			// convert vector to T* out_data
			boostpy::list out_data_list = boostpy::extract<boostpy::list>(return_list[0]);
			if (PyList_Check(out_data_list.ptr()) && PyList_Size(return_list.ptr()) == dim_count + 1)
			{
				for (size_t i = 0; i < dim_count; ++i)
				{
					output_size[i] = boostpy::extract<size_t>(return_list[i + 1]);
				}

				T* output_data = new T[GetTotalSize(dim_count, output_size)];
				Pylist2CArray(out_data_list, output_data, dim_count, output_size);

				return output_data;
			}
			else
			{
				return nullptr;
			}
		}
		catch (...)
		{
			if (PyErr_Occurred())
			{
				PyErr_Print();
				PyErr_Clear();
			}
		}
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

		return DoCArray2Pylist(p, dimension_count, reversed_size);
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
		DoPylist2CArray(li, p, dimension_count, size);
	}

	template<typename T>
	void YapPythonImpl::DoPylist2CArray(const boostpy::list& li, T*& out_data, size_t dim_count, size_t size[])
	{
		assert(out_data != nullptr);

		if (dim_count == 1)
		{
			for (size_t i = 0; i < size[0]; ++i)
			{
				*out_data++ = boostpy::extract<T>(li[i]);
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
