#include "stdafx.h"
#include "YapPythonImp.h"
#include <Python.h>
#include <numpy/arrayobject.h>
#include <iostream>
#include <atldbcli.h>
#include <atlconv.h>
#include <comutil.h>
#include <complex>
#include <map>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/import.hpp>
#include <ddeml.h>
#include <boost/python/stl_iterator.hpp>
#include <vcruntime_exception.h>

using namespace std;

std::map<int, char*> DataType2Char{
	{ DataTypeChar, "s" },
	{ DataTypeUnsignedChar, "b" },
	{ DataTypeShort, "h" },
	{ DataTypeUnsignedShort, "h" },
	{ DataTypeFloat, "f" },
	{ DataTypeDouble, "d" },
	{ DataTypeInt, "i" },
	{ DataTypeUnsignedInt, "i" },
	{ DataTypeComplexFloat, "O&" },
	{ DataTypeComplexDouble, "O&" },
	{ DataTypeBool, "i" }
};

YapPython::YapPython()
{
	Py_Initialize();
	if (!Py_IsInitialized())
		throw PyErr_Occurred;
}

YapPython::~YapPython()
{
	if (Py_IsInitialized()) { Py_Finalize(); }
};

void* YapPython::Process2D(const wchar_t* module_name, const wchar_t* method_name, int data_type, 
	void * data, size_t width, size_t height, size_t & out_width, size_t & out_height)
{
	switch (data_type)
	{
	case DataTypeInt:
		return Process2D(module_name, method_name, reinterpret_cast<int*>(data), width, height, out_width, out_height);
		break;
	case DataTypeUnsignedInt:
		return Process2D(module_name, method_name, reinterpret_cast<unsigned int*>(data),width, height, out_width, out_height);
		break;
	case DataTypeChar:
		return Process2D(module_name, method_name, reinterpret_cast<char*>(data), width, height, out_width, out_height);
		break;
	case DataTypeUnsignedChar:
		return Process2D(module_name, method_name, reinterpret_cast<unsigned char*>(data), width, height, out_width, out_height);
		break;
	case DataTypeShort:
		return Process2D(module_name, method_name, reinterpret_cast<short*>(data), width, height, out_width, out_height);
		break;
	case DataTypeUnsignedShort:
		return Process2D(module_name, method_name, reinterpret_cast<unsigned short*>(data), width, height, out_width, out_height);
		break;
	case DataTypeFloat:
		return Process2D(module_name, method_name, reinterpret_cast<float*>(data),  width, height, out_width, out_height);
		break;
	case DataTypeDouble:
		return Process2D(module_name, method_name, reinterpret_cast<double*>(data), width, height, out_width, out_height);
		break;
	case DataTypeComplexFloat:
		return Process2D(module_name, method_name, reinterpret_cast<std::complex<float>*>(data), width, height, out_width, out_height);
		break;
	case DataTypeComplexDouble:
		return Process2D(module_name, method_name, reinterpret_cast<std::complex<double>*>(data), width, height, out_width, out_height);
		break;
	case DataTypeBool:
		return Process2D(module_name, method_name, reinterpret_cast<bool*>(data), width, height, out_width, out_height);
		break;
	default:
		break;
	}
}

void* YapPython::Process3D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
	void * data, size_t width, size_t height, size_t slice, 
	size_t& out_width, size_t& out_height, size_t& out_slice)
{
	switch (data_type)
	{
	case DataTypeInt:
		return Process3D(module_name, method_name, reinterpret_cast<int*>(data), width, height, slice, out_width, out_height, out_slice);
		break;
	case DataTypeUnsignedInt:
		return Process3D(module_name, method_name, reinterpret_cast<unsigned int*>(data), width, height, slice, out_width, out_height, out_slice);
		break;
	case DataTypeChar:
		return Process3D(module_name, method_name, reinterpret_cast<char*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	case DataTypeUnsignedChar:
		return Process3D(module_name, method_name, reinterpret_cast<unsigned char*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	case DataTypeShort:
		return Process3D(module_name, method_name, reinterpret_cast<short*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	case DataTypeUnsignedShort:
		return Process3D(module_name, method_name, reinterpret_cast<unsigned short*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	case DataTypeFloat:
		return Process3D(module_name, method_name, reinterpret_cast<float*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	case DataTypeDouble:
		return Process3D(module_name, method_name, reinterpret_cast<double*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	case DataTypeComplexFloat:
		return Process3D(module_name, method_name, reinterpret_cast<std::complex<float>*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	case DataTypeComplexDouble:
		return Process3D(module_name, method_name, reinterpret_cast<std::complex<double>*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	case DataTypeBool:
		return Process3D(module_name, method_name, reinterpret_cast<bool*>(data), width, height,slice, out_width, out_height, out_slice);
		break;
	default:
		return nullptr;
		break;
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
	std::vector<bool> vector_bool;
	try
	{
		for (size_t i = 0; i < len(iterable); ++i)
			vector_bool.push_back(boostpy::extract<bool>(iterable[i]));
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
	boostpy::list li;
	typename std::vector<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it)
		li.append(*it);
	return li;
}

template<typename T>
boostpy::list YapPython::List2Pylist(const std::list<T>& v)
{
	boostpy::list li;
	typename std::list<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it)
		li.append(*it);
	return li;
}

template<typename T>
boostpy::list YapPython::CArrary2Pylist(T* data, size_t width, size_t height, size_t slice)
{
	boostpy::list li;
	T* p = data;
	for (int i = 0; i < width*height*slice; ++i)
	{
		li.append(*(p++));
	}

	return li;
}

template<typename T>
void YapPython::Pylist2CArray(boostpy::list li, T* out_data, size_t out_width, size_t out_height, size_t out_slice)
{
	try
	{
		if (PyList_Size(li.ptr()) != out_width * out_height * out_slice)
			throw PyErr_NewException("Python return data list size != return list marked size", li.ptr(), NULL);
		T* p = out_data;
		for (size_t i = 0; i < out_width*out_height*out_slice; ++i)
		{
			*(p++) = boostpy::extract<T>(li[i]);
		}
	}
	catch (...)
	{
		if (PyErr_Occurred())
			PyErr_Print();
		PyErr_Clear();
	}
}

template<typename T>
void* YapPython::Process2D(const wchar_t* module_name, const wchar_t* method_name,
	T* data, size_t width, size_t height, 
	size_t& out_width, size_t& out_height)
{
	namespace boostpy = boost::python;
	std::string module_str = ATL::CW2A(module_name);
	std::string method_str = ATL::CW2A(method_name);
	const char* module_name_ = module_str.data();
	const char* method_name_ = method_str.data();

	if (!Py_IsInitialized())
		return nullptr;

	boostpy::object main_module;
	boostpy::object main_namespace;
	try
	{
		main_module = boostpy::import("__main__");
		main_namespace = main_module.attr("__dict__");
		boostpy::object simple = boostpy::exec_file(module_name_, main_namespace, main_namespace); // "D:\\demoPython\\example.py"
		boostpy::object method = main_namespace[method_name_];

		// convert T* to python list
		boostpy::list pylist = CArrary2Pylist(data, width, height);

		boostpy::list retList = boostpy::extract<boostpy::list>(method(pylist, width, height));

		// convert vector to T* out_data: [[..., data, ...], width, height];
		boostpy::list datalist = boostpy::extract<boostpy::list>(retList[0]);
		if (PyList_Check(datalist.ptr()) && PyList_Size(retList.ptr())==3)
		{
			out_width = boostpy::extract<size_t>(retList[1]);
			out_height = boostpy::extract<size_t>(retList[2]);
			T* output_data = new T[out_width*out_height];
			Pylist2CArray(datalist, output_data, out_width, out_height);
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

template<>
void* YapPython::Process2D(const wchar_t* module_name, const wchar_t* method_name, 
	std::complex<float>* data, size_t width, size_t height, size_t& out_width, size_t& out_height)
{
		namespace boostpy = boost::python;
		string module_str = ATL::CW2A(module_name);
		string method_str = ATL::CW2A(method_name);
		const char* module_name_ = module_str.data();
		const char* method_name_ = method_str.data();

		if (!Py_IsInitialized())
			return nullptr;
		boostpy::object main_module;
		boostpy::object main_namespace;
		try
		{
			main_module = boostpy::import("__main__");
			main_namespace = main_module.attr("__dict__");
			boostpy::object simple = boostpy::exec_file(module_name_, main_namespace, main_namespace); // "D:\\demoPython\\example.py"
			boostpy::object method = main_namespace[method_name_];

			// convert T* to python list
			boostpy::list pylist = CArrary2Pylist(data, width, height);

			if (PyList_Size(pylist.ptr()) != width * height)
				throw PyErr_NewException("reverse data list size != width * height", 
					pylist.ptr(), main_namespace.ptr());

			boostpy::list retList = boostpy::extract<boostpy::list>(method(pylist, width, height));

			// convert vector to T* out_data: [[..., data, ...], width, height];
			boostpy::list datalist = boostpy::extract<boostpy::list>(retList[0]);
			auto py_data_list = datalist.ptr();
			if (PyList_Check(py_data_list) && PyList_Size(retList.ptr()) == 3)
			{
				out_width = boostpy::extract<size_t>(retList[1]);
				out_height = boostpy::extract<size_t>(retList[2]);
				size_t list_size = PyList_Size(py_data_list);
				if (list_size != out_width*out_height)
					throw PyErr_NewException("Return data list size != width*height", datalist.ptr(), main_namespace.ptr());
				std::complex<float>* output_data = new std::complex<float>[out_width*out_height];
				Py_complex s;
				for (size_t i = 0; i < list_size; ++i)
				{
					PyArg_Parse(PyList_GET_ITEM(py_data_list, i), "D", &s);
					output_data[i].real(s.real);
					output_data[i].imag(s.imag);
				}
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
}

template<>
void* YapPython::Process2D(const wchar_t* module_name, const wchar_t* method_name, 
	std::complex<double>* data,	size_t width, size_t height, size_t& out_width, size_t& out_height)
{
		namespace boostpy = boost::python;
		string module_str = ATL::CW2A(module_name);
		string method_str = ATL::CW2A(method_name);
		const char* module_name_ = module_str.data();
		const char* method_name_ = method_str.data();

		if (!Py_IsInitialized())
			return nullptr;
		boostpy::object main_module;
		boostpy::object main_namespace;
		try
		{
			main_module = boostpy::import("__main__");
			main_namespace = main_module.attr("__dict__");
			boostpy::object simple = boostpy::exec_file(module_name_, main_namespace, main_namespace); // "D:\\demoPython\\example.py"
			boostpy::object method = main_namespace[method_name_];

			// convert T* to python list
			boostpy::list pylist = CArrary2Pylist(data, width, height);

			boostpy::list retList = boostpy::extract<boostpy::list>(method(pylist, width, height));

			// convert vector to T* out_data: [[..., data, ...], width, height];
			boostpy::list datalist = boostpy::extract<boostpy::list>(retList[0]);
			auto py_data_list = datalist.ptr();
			if (PyList_Check(py_data_list) && PyList_Size(retList.ptr()) == 3)
			{
				out_width = boostpy::extract<size_t>(retList[1]);
				out_height = boostpy::extract<size_t>(retList[2]);
				size_t list_size = PyList_Size(py_data_list);
				if (list_size != out_width*out_height)
					throw PyErr_NewException("Return data list size != width*height", datalist.ptr(), main_namespace.ptr());
				std::complex<double>* output_data = new std::complex<double>[out_width*out_height];
				Py_complex s;
				for (size_t i = 0; i < list_size; ++i)
				{
					PyArg_Parse(PyList_GET_ITEM(py_data_list, i), "D", &s);
					output_data[i].real(s.real);
					output_data[i].imag(s.imag);
				}
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
}

template<typename T>
void* YapPython::Process3D(const wchar_t* module_name, const wchar_t * method_name,
	T * data, size_t width, size_t height, size_t slice, 
	size_t& out_width, size_t& out_height, size_t& out_slice)
{
	namespace boostpy = boost::python;
	const char* module_name_ = ATL::CW2A(module_name);
	const char* method_name_ = ATL::CW2A(method_name);
	/*
	_bstr_t b(module_name);
	_bstr_t c(method_name);
	const char * module_name_ = b;
	const char * method_name_ = c;
	*/
	if (!Py_IsInitialized())
		return nullptr;
	boostpy::object main_module;
	boostpy::object main_namespace;
	try
	{
		main_module = boostpy::import("__main__");
		main_namespace = main_module.attr("__dict__");
		boostpy::object simple = boostpy::exec_file(module_name_, main_namespace, main_namespace); // "D:\\demoPython\\example.py"
		boostpy::object method = main_namespace[method_name_];

		// convert T* to python list
		auto pylist = CArrary2Pylist(data, width, height, slice);

		boostpy::list retList = boostpy::extract<boostpy::list>(method(pylist, width, height, slice));

		// convert vector to T* out_data
		boostpy::list datalist = boostpy::extract<boostpy::list>(retList[0]);
		if (PyList_Check(datalist.ptr()) && PyList_Size(retList.ptr()) == 4)
		{
			out_width = boostpy::extract<size_t>(retList[1]);
			out_height = boostpy::extract<size_t>(retList[2]);
			out_slice = boostpy::extract<size_t>(retList[3]);
			T* output_data = new T[out_width*out_height*out_slice];
			Pylist2CArray(datalist, output_data, out_width, out_height, out_slice);
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

template<>
void* YapPython::Process3D(const wchar_t* module_name, const wchar_t* method_name, 
	std::complex<float>* data, size_t width, size_t height, size_t slice, size_t& out_width, size_t& out_height, size_t& out_slice)
{
	namespace boostpy = boost::python;
	const char* module_name_ = ATL::CW2A(module_name);
	const char* method_name_ = ATL::CW2A(method_name);
	/*
	_bstr_t b(module_name);
	_bstr_t c(method_name);
	const char * module_name_ = b;
	const char * method_name_ = c;
	*/
	if (!Py_IsInitialized())
		return nullptr;
	boostpy::object main_module;
	boostpy::object main_namespace;
	try
	{
		main_module = boostpy::import("__main__");
		main_namespace = main_module.attr("__dict__");
		boostpy::object simple = boostpy::exec_file(module_name_, main_namespace, main_namespace); // "D:\\demoPython\\example.py"
		boostpy::object method = main_namespace[method_name_];

		// convert T* to python list
		auto pylist = CArrary2Pylist(data, width, height, slice);

		boostpy::list retList = boostpy::extract<boostpy::list>(method(pylist, width, height, slice));
			
		// convert vector to T* out_data
		boostpy::list datalist = boostpy::extract<boostpy::list>(retList[0]);
		auto py_data_list = datalist.ptr();
		if (PyList_Check(py_data_list) && PyList_Size(retList.ptr()) == 4)
		{
			out_width = boostpy::extract<size_t>(retList[1]);
			out_height = boostpy::extract<size_t>(retList[2]);
			out_slice = boostpy::extract<size_t>(retList[3]);
			size_t list_size = PyList_Size(py_data_list);
			if (list_size != out_width*out_height*out_slice)
				throw PyErr_NewException("Return data list size != width*height", datalist.ptr(), main_namespace.ptr());
			std::complex<float>* output_data = new std::complex<float>[out_width*out_height*out_slice];
			Py_complex s;
			for (size_t i = 0; i < list_size; ++i)
			{
				PyArg_Parse(PyList_GET_ITEM(py_data_list, i), "D", &s);
				output_data[i].real(s.real);
				output_data[i].imag(s.imag);
			}
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
}

template<>
void* YapPython::Process3D(const wchar_t* module_name, const wchar_t* method_name, 
	std::complex<double>* data,	size_t width, size_t height, size_t slice, size_t& out_width, size_t& out_height, size_t& out_slice)
{
	namespace boostpy = boost::python;
	const char* module_name_ = ATL::CW2A(module_name);
	const char* method_name_ = ATL::CW2A(method_name);

	if (!Py_IsInitialized())
		return nullptr;
	boostpy::object main_module;
	boostpy::object main_namespace;
	try
	{
		main_module = boostpy::import("__main__");
		main_namespace = main_module.attr("__dict__");
		boostpy::object simple = boostpy::exec_file(module_name_, main_namespace, main_namespace); // "D:\\demoPython\\example.py"
		boostpy::object method = main_namespace[method_name_];

		// convert T* to python list
		auto pylist = CArrary2Pylist(data, width, height, slice);

		boostpy::list retList = boostpy::extract<boostpy::list>(method(pylist, width, height, slice));

		// convert vector to T* out_data
		boostpy::list datalist = boostpy::extract<boostpy::list>(retList[0]);
		auto py_data_list = datalist.ptr();
		if (PyList_Check(py_data_list) && PyList_Size(retList.ptr()) == 4)
		{
			out_width = boostpy::extract<size_t>(retList[1]);
			out_height = boostpy::extract<size_t>(retList[2]);
			out_slice = boostpy::extract<size_t>(retList[3]);
			size_t list_size = PyList_Size(py_data_list);
			if (list_size != out_width*out_height*out_slice)
				throw PyErr_NewException("Return data list size != width*height", datalist.ptr(), main_namespace.ptr());
			std::complex<double>* output_data = new std::complex<double>[out_width*out_height*out_slice];
			Py_complex s;
			for (size_t i = 0; i < list_size; ++i)
			{
				PyArg_Parse(PyList_GET_ITEM(py_data_list, i), "D", &s);
				output_data[i].real(s.real);
				output_data[i].imag(s.imag);
			}
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
}

