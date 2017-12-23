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
namespace bpy = boost::python;

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
		size_t input_dimensions, void * data, OUT size_t& output_dimensions, size_t input_size[], OUT size_t output_size[], bool is_need_ref_data) override
	{
		switch (data_type)
		{
		case DataTypeInt:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<int*>(data),output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeUnsignedInt:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned int*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeChar:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<char*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeUnsignedChar:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned char*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeShort:
			return DoProcess(module, method, input_dimensions, reinterpret_cast<short*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeUnsignedShort:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<unsigned short*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeFloat:				   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<float*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeDouble:			   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<double*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeComplexFloat:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<std::complex<float>*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeComplexDouble:		   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<std::complex<double>*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		case DataTypeBool:				   
			return DoProcess(module, method, input_dimensions, reinterpret_cast<bool*>(data), output_dimensions, input_size, output_size, is_need_ref_data);
		default:
			return nullptr;
		}
	}

	virtual void SetRefData(void * ref_data, int32_t data_type, size_t dimension_count, size_t size[]) override 
	{

		if (ref_data == nullptr)
		{
			return;
		}
		_ref_data = ref_data;
		switch (data_type)
		{
		case DataTypeInt:
			_ref_data_list = CArray2Pylist(reinterpret_cast<int*>(ref_data), dimension_count, size);
			break;
		case DataTypeUnsignedInt:
			_ref_data_list = CArray2Pylist(reinterpret_cast<unsigned int*>(ref_data), dimension_count, size);
			break;
		case DataTypeChar:
			_ref_data_list = CArray2Pylist(reinterpret_cast<char*>(ref_data), dimension_count, size);
			break;
		case DataTypeUnsignedChar:
			_ref_data_list = CArray2Pylist(reinterpret_cast<unsigned char*>(ref_data), dimension_count, size);
			break;
		case DataTypeShort:
			_ref_data_list = CArray2Pylist(reinterpret_cast<short*>(ref_data), dimension_count, size);
			break;
		case DataTypeUnsignedShort:
			_ref_data_list = CArray2Pylist(reinterpret_cast<unsigned short*>(ref_data), dimension_count, size);
			break;
		case DataTypeFloat:
			_ref_data_list = CArray2Pylist(reinterpret_cast<float*>(ref_data), dimension_count, size);
			break;
		case DataTypeDouble:
			_ref_data_list = CArray2Pylist(reinterpret_cast<double*>(ref_data), dimension_count, size);
			break;
		case DataTypeComplexFloat:
			_ref_data_list = CArray2Pylist(reinterpret_cast<complex<float>*>(ref_data), dimension_count, size);
			break;
		case DataTypeComplexDouble:
			_ref_data_list = CArray2Pylist(reinterpret_cast<complex<double>*>(ref_data), dimension_count, size);
			break;
		case DataTypeBool:
			_ref_data_list = CArray2Pylist(reinterpret_cast<bool*>(ref_data), dimension_count, size);
			break;
		default:
			break;
		}
	};

	virtual void DeleteRefData() override
	{
		_ref_data = nullptr;
	}
protected:
	
	template<typename T>
	void pylist2Array(const bpy::object& iterable, T *& out_data, size_t size)
	{
		auto tv = std::vector<T>(bpy::stl_input_iterator<T>(iterable),
			bpy::stl_input_iterator<T>());
		T * source_data = tv.data();
		memcpy(reinterpret_cast<void*>(out_data), reinterpret_cast<void*>(source_data), (size_t)(size * sizeof(T)));
		out_data += size;
	}
	template<>
	void pylist2Array(const bpy::object& iterable, bool *& out_data, size_t size)
	{
		for (size_t i = 0; i < size; ++i)
		{
			*(out_data++) = bpy::extract<bool>(iterable[i]);
		}
	}
	

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
		size_t input_dimensions,T * data, OUT size_t &output_dimensions, 
		size_t input_size[], OUT size_t output_size[],bool is_need_ref_data)
	{
		if (is_need_ref_data && _ref_data == nullptr)
		{
			return nullptr;
		}
		if (!Py_IsInitialized())
			return nullptr;

		try
		{
			bpy::object main_module = bpy::import("__main__");
			bpy::object main_namespace = main_module.attr("__dict__");
			bpy::object simple = bpy::exec_file(ToMbs(module).c_str(), main_namespace, main_namespace);
			bpy::object func = main_namespace[ToMbs(method).c_str()];

			// convert T* to python list
			auto in_data_list = CArray2Pylist(data, input_dimensions, input_size);

			bpy::list return_list;
			switch (input_dimensions)
			{
			case 1:
				return_list = is_need_ref_data ? (bpy::extract<bpy::list>(func(in_data_list, _ref_data_list, input_size[0]))) 
					: (bpy::extract<bpy::list>(func(in_data_list, input_size[0])));
				break;
			case 2:
				return_list = is_need_ref_data ? (bpy::extract<bpy::list>(func(in_data_list, _ref_data_list, input_size[0], input_size[1])))
					: (bpy::extract<bpy::list>(func(in_data_list, input_size[0], input_size[1])));
				break;
			case 3:
				return_list = is_need_ref_data ? (bpy::extract<bpy::list>(func(in_data_list, _ref_data_list, input_size[0],input_size[1],input_size[2])))
					: (bpy::extract<bpy::list>(func(in_data_list, input_size[0], input_size[1], input_size[2])));
				break;
			case 4:
				return_list = is_need_ref_data ? (bpy::extract<bpy::list>(func(in_data_list, _ref_data_list, input_size[0], input_size[1], input_size[2],input_size[3])))
					: (bpy::extract<bpy::list>(func(in_data_list, input_size[0], input_size[1], input_size[2], input_size[3])));
				break;
			default:
				return nullptr;
			}

			size_t output_dims = bpy::extract<int>(return_list[0]);
			if (PyList_Size(return_list.ptr()) == output_dims + 2)
			{
				bpy::list out_data_list = bpy::extract<bpy::list>(return_list[1]);
				if (!PyList_Check(out_data_list.ptr()))
					return nullptr;
				for (size_t i = 0; i < output_dims; ++i)
				{
					output_size[i] = bpy::extract<size_t>(return_list[i + 2]);
				}
				T* output_data = new T[GetTotalSize(output_dims, output_size)];
				Pylist2CArray(out_data_list, output_data, output_dims, output_size);
				output_dimensions = output_dims;
				return output_data;
			}
			else
			{
				// �޷������ݣ��ú������صķ���ֵoutput_data��ָ��Ҳ��ȻΪnullptr��
				// ���߷������ݸ�ʽ����
				return nullptr;
			}
		}
		catch (bpy::error_already_set const&)
		{
			PyErr_Print();
			PyErr_Clear();
		}
		output_dimensions = 0;
		return nullptr;
	};

	template<typename T>
	bpy::list CArray2Pylist(const T* data, size_t dimension_count, size_t size[])
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
	bpy::list DoCArray2Pylist(const T*& data, size_t dimension_count, size_t size[])
	{
		bpy::list result;
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
	void Pylist2CArray(const bpy::list& li, T* out_data, size_t dimension_count, size_t size[])
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
	void DoPylist2CArray(const bpy::list& li, T*& out_data, size_t dim_count, size_t size[])
	{
		assert(out_data != nullptr);

		if (dim_count == 1)
		{
			pylist2Array(li, out_data, size[0]);
			/*for (size_t i = 0; i < size[0]; ++i)
			{
				*(out_data++) = bpy::extract<T>(li[i]);
			}*/
		}
		else
		{
			for (size_t i = 0; i < size[0]; ++i)
			{
				auto sub_list = bpy::extract<bpy::list>(li[i]);
				DoPylist2CArray(sub_list, out_data, dim_count - 1, size + 1);
			}

		}
	}

private:
	//
	void* _ref_data;
	bpy::list _ref_data_list;
};

YapPythonImpl::YapPythonImpl() : 
	_ref_data{ nullptr }
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
