#include "stdafx.h"
#include "YapPythonImp.h"
#include "IYapPython.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#pragma warning(disable:4996)

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

using namespace std;
namespace boostpy = boost::python;

std::string ToMbs(const wchar_t * wcs)
{
	assert(wcslen(wcs) < 500 && "The string cannot contain more than 500 characters.");

	static char buffer[1024];
	wcstombs(buffer, wcs, 1024);//wcstombs_s(wcs,buffer,1024);

	return std::string(buffer);
}

class YapPython : public IYapPython
{

public:
	YapPython();
	~YapPython();

	virtual void* Process2D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
		void * data, size_t width, size_t height, size_t& out_width, size_t& out_height) override;

	virtual void* Process3D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
		void * data, size_t width, size_t height, size_t slice,
		size_t& out_width, size_t& out_height, size_t& out_slice) override;

protected:
	template<typename T>
	void* DoProcess2D(const wchar_t* module_name, const wchar_t* method_name,
		T* data, size_t width, size_t height,
		size_t& out_width, size_t& out_height);

	template<typename T>
	void* DoProcess3D(const wchar_t* module_name, const wchar_t* method_name,
		T* data, size_t width, size_t height, size_t slice,
		size_t& out_width, size_t& out_height, size_t& out_slice);

	template< typename T>
	std::vector<T> Pylist2Vector(const boostpy::object& iterable);

	template< typename T>
	std::list<T> Pylist2list(const boostpy::object& iterable);

	template<typename T>
	boostpy::list Vector2Pylist(const std::vector<T>& v);

	template<typename T>
	boostpy::list List2Pylist(const std::list<T>& v);

	template<typename T>
	boostpy::list CArray2Pylist(const T* data, size_t length);

	template<typename T>
	void Pylist2CArray(const boostpy::list& li, T* out_data, size_t length);
};

YapPython::YapPython()
{
	Py_Initialize();
	if (!Py_IsInitialized())
		throw PyErr_Occurred;
}

YapPython::~YapPython()
{
	if (Py_IsInitialized()) 
	{
		Py_Finalize(); 
	}
};

void* YapPython::Process2D(const wchar_t* module, const wchar_t* method, int data_type, 
	void * data, size_t width, size_t height, size_t & out_width, size_t & out_height)
{
	switch (data_type)
	{
	case DataTypeInt:
		return DoProcess2D(module, method, reinterpret_cast<int*>(data), width, height, out_width, out_height);
	case DataTypeUnsignedInt:
		return DoProcess2D(module, method, reinterpret_cast<unsigned int*>(data),width, height, out_width, out_height);
	case DataTypeChar:
		return DoProcess2D(module, method, reinterpret_cast<char*>(data), width, height, out_width, out_height);
	case DataTypeUnsignedChar:
		return DoProcess2D(module, method, reinterpret_cast<unsigned char*>(data), width, height, out_width, out_height);
	case DataTypeShort:
		return DoProcess2D(module, method, reinterpret_cast<short*>(data), width, height, out_width, out_height);
	case DataTypeUnsignedShort:
		return DoProcess2D(module, method, reinterpret_cast<unsigned short*>(data), width, height, out_width, out_height);
	case DataTypeFloat:
		return DoProcess2D(module, method, reinterpret_cast<float*>(data),  width, height, out_width, out_height);
	case DataTypeDouble:
		return DoProcess2D(module, method, reinterpret_cast<double*>(data), width, height, out_width, out_height);
	case DataTypeComplexFloat:
		return DoProcess2D(module, method, reinterpret_cast<std::complex<float>*>(data), width, height, out_width, out_height);
	case DataTypeComplexDouble:
		return DoProcess2D(module, method, reinterpret_cast<std::complex<double>*>(data), width, height, out_width, out_height);
	case DataTypeBool:
		return DoProcess2D(module, method, reinterpret_cast<bool*>(data), width, height, out_width, out_height);
	default:
		return nullptr;
	}
}

void* YapPython::Process3D(const wchar_t* module, const wchar_t* method, int data_type,
	void * data, size_t width, size_t height, size_t slice, 
	size_t& out_width, size_t& out_height, size_t& out_slice)
{
	switch (data_type)
	{
	case DataTypeInt:
		return DoProcess3D(module, method, 
			reinterpret_cast<int*>(data), width, height, slice, out_width, out_height, out_slice);
	case DataTypeUnsignedInt:
		return DoProcess3D(module, method, 
			reinterpret_cast<unsigned int*>(data), width, height, slice, out_width, out_height, out_slice);
	case DataTypeChar:
		return DoProcess3D(module, method, 
			reinterpret_cast<char*>(data), width, height,slice, out_width, out_height, out_slice);
	case DataTypeUnsignedChar:
		return DoProcess3D(module, method, 
			reinterpret_cast<unsigned char*>(data), width, height,slice, out_width, out_height, out_slice);
	case DataTypeShort:
		return DoProcess3D(module, method, 
			reinterpret_cast<short*>(data), width, height,slice, out_width, out_height, out_slice);
	case DataTypeUnsignedShort:
		return DoProcess3D(module, method,
			reinterpret_cast<unsigned short*>(data), width, height,slice, out_width, out_height, out_slice);
	case DataTypeFloat:
		return DoProcess3D(module, method,
			reinterpret_cast<float*>(data), width, height,slice, out_width, out_height, out_slice);
	case DataTypeDouble:
		return DoProcess3D(module, method,
			reinterpret_cast<double*>(data), width, height,slice, out_width, out_height, out_slice);
	case DataTypeComplexFloat:
		return DoProcess3D(module, method,
			reinterpret_cast<std::complex<float>*>(data), width, height,slice, out_width, out_height, out_slice);
	case DataTypeComplexDouble:
		return DoProcess3D(module, method, 
			reinterpret_cast<std::complex<double>*>(data), width, height,slice, out_width, out_height, out_slice);
	case DataTypeBool:
		return DoProcess3D(module, method, 
			reinterpret_cast<bool*>(data), width, height,slice, out_width, out_height, out_slice);
	default:
		return nullptr;
	}
}

template< typename T>
std::vector<T>
YapPython::Pylist2Vector(const boostpy::object& iterable)
{
	return std::vector<T>(boostpy::stl_input_iterator<T>(iterable),
		boostpy::stl_input_iterator<T>());
}

template<> std::vector<bool> YapPython::Pylist2Vector(const boostpy::object& iterable)
{
	std::vector<bool> vector_bool(len(iterable));
	try
	{
		for (size_t i = 0; i < size_t(len(iterable)); ++i)
		{
			vector_bool[i] = boostpy::extract<bool>(iterable[i]);
		}
	}
	catch (...)
	{
		if (PyErr_Occurred())
			PyErr_Print();
		PyErr_Clear();
	}
	return vector_bool;
}

template< typename T>
std::list<T>
YapPython::Pylist2list(const boostpy::object& iterable)
{
	return std::list<T>(boostpy::stl_input_iterator<T>(iterable),
		boostpy::stl_input_iterator<T>());
}

template<typename T>
boostpy::list YapPython::Vector2Pylist(const std::vector<T>& v)
{
	return CArray2Pylist(v.data(), v.size());
}

template<typename T>
boostpy::list YapPython::List2Pylist(const std::list<T>& v)
{
	boostpy::list li;
	for (auto element : v)
		li.append(element);

	return li;
}

template<typename T>
boostpy::list YapPython::CArray2Pylist(const T* data, size_t length)
{
	boostpy::list li;
	for (auto p = data; p < data + length; ++p)
	{
		li.append(*p);
	}

	return li;
}

template<typename T>
void YapPython::Pylist2CArray(const boostpy::list& li, T* out_data, size_t length)
{
	if (PyList_Size(li.ptr()) != length)
		throw PyErr_NewException("Python return data list size != return list marked size", li.ptr(), NULL);

	T* p = out_data;
	for (size_t i = 0; i < length; ++i)
	{
		*(p++) = boostpy::extract<T>(li[i]);
	}
}

template <>
void YapPython::Pylist2CArray(const boostpy::list& li, complex<double>* out_data, size_t length)
{
	Py_complex s;
	for (size_t i = 0; i < length; ++i)
	{
		PyArg_Parse(PyList_GET_ITEM(li.ptr(), i), "D", &s);
		out_data[i].real(s.real);
		out_data[i].imag(s.imag);
	}
}

template <>
void YapPython::Pylist2CArray(const boostpy::list& li, complex<float>* out_data, size_t length)
{
	Py_complex s;
	for (size_t i = 0; i < length; ++i)
	{
		PyArg_Parse(PyList_GET_ITEM(li.ptr(), i), "D", &s);
		out_data[i].real(float(s.real));
		out_data[i].imag(float(s.imag));
	}
}

template<typename T>
void* YapPython::DoProcess2D(const wchar_t* module_name, const wchar_t* method_name,
	T* data, size_t width, size_t height, 
	size_t& out_width, size_t& out_height)
{
	if (!Py_IsInitialized())
		return nullptr;

	boostpy::object main_module;
	boostpy::object main_namespace;
	try
	{
		main_module = boostpy::import("__main__");
		main_namespace = main_module.attr("__dict__");
		boostpy::object simple = boostpy::exec_file(ToMbs(module_name).c_str(), main_namespace, main_namespace); // "D:\\demoPython\\example.py"
		boostpy::object method = main_namespace[ToMbs(method_name).c_str()];

		// convert T* to python list
		boostpy::list pylist = CArray2Pylist(data, width * height);

		boostpy::list retList = boostpy::extract<boostpy::list>(method(pylist, width, height));

		// convert vector to T* out_data: [[..., data, ...], width, height];
		boostpy::list datalist = boostpy::extract<boostpy::list>(retList[0]);
		if (PyList_Check(datalist.ptr()) && PyList_Size(retList.ptr())==3)
		{
			out_width = boostpy::extract<size_t>(retList[1]);
			out_height = boostpy::extract<size_t>(retList[2]);
			T* output_data = new T[out_width * out_height];
			Pylist2CArray(datalist, output_data, out_width * out_height);
			return reinterpret_cast<void*>(output_data);
		}
		else
		{
			// PyErr_SetString(datalist.ptr(), "Return value[0] is not a data list. Check python script return value!");
			throw PyErr_NewException("Return value[0] is not a data list. Check python script return value!", 
				datalist.ptr(), main_namespace.ptr());
		}
	}
	catch (...)
	{
		if (PyErr_Occurred())
			PyErr_Print();
		PyErr_Clear();
	}
	return nullptr;
};

template<typename T>
void* YapPython::DoProcess3D(const wchar_t* module_name, const wchar_t * method_name,
	T * data, size_t width, size_t height, size_t slice, 
	size_t& out_width, size_t& out_height, size_t& out_slice)
{
	if (!Py_IsInitialized())
		return nullptr;

	boostpy::object main_module;
	boostpy::object main_namespace;
	try
	{
		main_module = boostpy::import("__main__");
		main_namespace = main_module.attr("__dict__");
		boostpy::object simple = boostpy::exec_file(ToMbs(module_name).c_str(), main_namespace, main_namespace); // "D:\\demoPython\\example.py"
		boostpy::object method = main_namespace[ToMbs(method_name).c_str()];

		// convert T* to python list
		auto pylist = CArray2Pylist(data, width * height * slice);

		boostpy::list retList = boostpy::extract<boostpy::list>(method(pylist, width, height, slice));

		// convert vector to T* out_data
		boostpy::list datalist = boostpy::extract<boostpy::list>(retList[0]);
		if (PyList_Check(datalist.ptr()) && PyList_Size(retList.ptr()) == 4)
		{
			out_width = boostpy::extract<size_t>(retList[1]);
			out_height = boostpy::extract<size_t>(retList[2]);
			out_slice = boostpy::extract<size_t>(retList[3]);

			T* output_data = new T[out_width * out_height * out_slice];
			Pylist2CArray(datalist, output_data, out_width * out_height * out_slice);

			return reinterpret_cast<void*>(output_data);
		}
		else
		{
			// PyErr_SetString(datalist.ptr(), "Return value[0] is not a data list. Check python script return value!");
			throw PyErr_NewException("Return value[0] is not a data list. Check python script return value!", datalist.ptr(), main_namespace.ptr());
		}
	}
	catch (...)
	{
		if (PyErr_Occurred())
			PyErr_Print();
		PyErr_Clear();
	}
	return nullptr;
};

IYapPython * PythonFactory::GetPython()
{
	return new YapPython();
}
