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

class YapPythonImpl : public IYapPython
{

public:
	YapPythonImpl();
	~YapPythonImpl();

	virtual void* Process2D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
		void * data, size_t width, size_t height, size_t& out_width, size_t& out_height) override;

	virtual void* Process3D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
		void * data, size_t width, size_t height, size_t slice,
		size_t& out_width, size_t& out_height, size_t& out_slice) override;

	virtual void* Process4D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
		void * data, size_t width, size_t height, size_t slice, size_t time, size_t& out_width,
		size_t& out_height, size_t& out_slice, size_t& out_time) override;

protected:
	template<typename T>
	void* DoProcess2D(const wchar_t* module_name, const wchar_t* method_name,
		T* data, size_t width, size_t height,
		size_t& out_width, size_t& out_height);

	template<typename T>
	void* DoProcess3D(const wchar_t* module_name, const wchar_t* method_name,
		T* data, size_t width, size_t height, size_t slice,
		size_t& out_width, size_t& out_height, size_t& out_slice);

	template<typename T>
	void* DoProcess4D(const wchar_t* module_name, const wchar_t* method_name,
		T* data, size_t width, size_t height, size_t slice, size_t time,
		size_t& out_width, size_t& out_height, size_t& out_slice, size_t &out_time);

	/* transmit data through 1 Dimension */
	template< typename T>
	std::vector<T> Pylist2Vector(const boostpy::object& iterable);

	template<typename T>
	boostpy::list Vector2Pylist(const std::vector<T>& v);

	template< typename T>
	std::list<T> Pylist2list(const boostpy::object& iterable);

	template<typename T>
	boostpy::list List2Pylist(const std::list<T>& v);

	/* transmit data through 2 Dimensions */
	template<typename T>
	boostpy::list CArray2Pylist2D(const T* data, size_t width,size_t height);

	template<typename T>
	void Pylist2D2CArray(const boostpy::list& li, T* out_data, size_t width, size_t height);

	template<typename T>
	void pylist2d2Carray_complex(const boostpy::list &li, complex<T>* out_data, size_t width, size_t height);

	/* transmit data through 3 Dimensions */
	template<typename T>
	boostpy::list CArray2Pylist3D(const T* data, size_t width, size_t height, size_t slice);

	template<typename T>
	void Pylist3D2CArray(const boostpy::list& li, T* out_data, size_t width, size_t height, size_t slice);

	template<typename T>
	void pylist3d2Carray_complex(const boostpy::list &li, complex<T>* out_data, size_t width, size_t height, size_t slice);

	/* transmit data through 4 Dimensions */
	template<typename T>
	boostpy::list CArray2Pylist4D(const T* data, size_t width, size_t height, size_t slice, size_t time);

	template<typename T>
	void Pylist4D2CArray(const boostpy::list& li, T* out_data, size_t width, size_t height, size_t slice, size_t time);

	template<typename T>
	void pylist4d2Carray_complex(const boostpy::list &li, complex<T>* out_data, size_t width, size_t height, size_t slice, size_t time);
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

void* YapPythonImpl::Process2D(const wchar_t* module, const wchar_t* method, int data_type, 
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

void* YapPythonImpl::Process3D(const wchar_t* module, const wchar_t* method, int data_type,
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

void* YapPythonImpl::Process4D(const wchar_t* module, const wchar_t* method, int data_type,
	void * data, size_t width, size_t height, size_t slice, size_t time,
	size_t& out_width, size_t& out_height, size_t& out_slice, size_t& out_time)
{
	switch (data_type)
	{
	case DataTypeInt:
		return DoProcess4D(module, method,
			reinterpret_cast<int*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeUnsignedInt:
		return DoProcess4D(module, method,
			reinterpret_cast<unsigned int*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeChar:
		return DoProcess4D(module, method,
			reinterpret_cast<char*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeUnsignedChar:
		return DoProcess4D(module, method,
			reinterpret_cast<unsigned char*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeShort:
		return DoProcess4D(module, method,
			reinterpret_cast<short*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeUnsignedShort:
		return DoProcess4D(module, method,
			reinterpret_cast<unsigned short*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeFloat:
		return DoProcess4D(module, method,
			reinterpret_cast<float*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeDouble:
		return DoProcess4D(module, method,
			reinterpret_cast<double*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeComplexFloat:
		return DoProcess4D(module, method,
			reinterpret_cast<std::complex<float>*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeComplexDouble:
		return DoProcess4D(module, method,
			reinterpret_cast<std::complex<double>*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	case DataTypeBool:
		return DoProcess4D(module, method,
			reinterpret_cast<bool*>(data), width, height, slice, time, out_width, out_height, out_slice, out_time);
	default:
		return nullptr;
	}
}

template< typename T>
std::vector<T>
YapPythonImpl::Pylist2Vector(const boostpy::object& iterable)
{
	return std::vector<T>(boostpy::stl_input_iterator<T>(iterable),
		boostpy::stl_input_iterator<T>());
}

template<> std::vector<bool> YapPythonImpl::Pylist2Vector(const boostpy::object& iterable)
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
std::list<T> YapPythonImpl::Pylist2list(const boostpy::object& iterable)
{
	return std::list<T>(boostpy::stl_input_iterator<T>(iterable),
		boostpy::stl_input_iterator<T>());
}

template< typename T>
boostpy::list YapPythonImpl::Vector2Pylist(const std::vector<T>& v)
{
	return CArray2Pylist2D(v.data(), v.size());
}

template<typename T>
boostpy::list YapPythonImpl::List2Pylist(const std::list<T>& v)
{
	boostpy::list li;
	for (auto element : v)
		li.append(element);

	return li;
}

/*convert 1D array to python 2D list*/
template<typename T>
boostpy::list YapPythonImpl::CArray2Pylist2D(const T* data, size_t width,size_t height)
{
	boostpy::list outsider;
	for (size_t i = 0; i < height; ++i)
	{
		boostpy::list inner;
		for (size_t j = 0; j < width; ++j)
		{
			inner.append(*(data + i * width + j));
		}
		outsider.append(inner);
	}
	return outsider;
}

/*convert 2D python list to c++ array*/
template<typename T>
void YapPythonImpl::Pylist2D2CArray(const boostpy::list& li, T* out_data, size_t width, size_t height)
{
	T* p = out_data;
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			*(p++) = boostpy::extract<T>(li[i][j]);
		}
	}
}

template <>
void YapPythonImpl::Pylist2D2CArray(const boostpy::list& li, complex<double>* out_data, size_t width, size_t height)
{
	pylist2d2Carray_complex(li, out_data, width, height);
}

template <>
void YapPythonImpl::Pylist2D2CArray(const boostpy::list& li, complex<float>* out_data, size_t width, size_t height)
{
	pylist2d2Carray_complex(li, out_data, width, height);
}

template<typename T>
void YapPythonImpl::pylist2d2Carray_complex(const boostpy::list &li, complex<T>* out_data, size_t width, size_t height)
{
	Py_complex s;
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			PyArg_Parse(PyList_GET_ITEM(li.ptr(), i), "D", &s);
			out_data[i].real(float(s.real));
			out_data[i].imag(float(s.imag));
		}
	}
}

/* convert c++ array to python 3D list */
template<typename T>
boostpy::list YapPythonImpl::CArray2Pylist3D(const T* data, size_t width, size_t height, size_t slice)
{
	boostpy::list outsider;
	for (size_t i = 0; i < slice; ++i)
	{
		boostpy::list midder;
		for (size_t j = 0; j < height; ++j)
		{
			boostpy::list inner;
			for (size_t k = 0; k < width; ++k)
			{
				inner.append(*(data + i * width * height + j * width + k));
			}
			midder.append(inner);
		}
		outsider.append(midder);
	}
	return outsider;
}

/* convert 3D python list to c++ array */
template<typename T>
void YapPythonImpl::Pylist3D2CArray(const boostpy::list& li, T* out_data, size_t width, size_t height, size_t slice)
{
	T* p = out_data;
	for (size_t k = 0; k < slice; ++k)
	{
		for (size_t i = 0; i < height; ++i)
		{
			for (size_t j = 0; j < width; ++j)
			{
				*(p++) = boostpy::extract<T>(li[k][i][j][k]);
			}
		}
	}
}

template<>
void YapPythonImpl::Pylist3D2CArray(const boostpy::list& li, complex<double>* out_data, size_t width, size_t height, size_t slice)
{
	pylist3d2Carray_complex(li, out_data, width, height, slice);
}

template<>
void YapPythonImpl::Pylist3D2CArray(const boostpy::list& li, complex<float>* out_data, size_t width, size_t height, size_t slice)
{
	pylist3d2Carray_complex(li, out_data, width, height, slice);
}

template<typename T>
void YapPythonImpl::pylist3d2Carray_complex(const boostpy::list &li, complex<T>* out_data, size_t width, size_t height, size_t slice)
{
	Py_complex s;
	for (size_t k = 0; k < slice; ++k)
	{
		for (size_t i = 0; i < height; ++i)
		{
			for (size_t j = 0; j < width; ++j)
			{
				PyArg_Parse(PyList_GET_ITEM(li.ptr(), (k*width*height + i*width + j)), "D", &s);
				out_data[i].real(s.real);
				out_data[i].imag(s.imag);
			}
		}
	}
}

/* convert c++ array to 4D python list*/
template<typename T>
boostpy::list YapPythonImpl::CArray2Pylist4D(const T* data, size_t width, size_t height, size_t slice, size_t time)
{
	boostpy::list outsider;
	for (size_t l = 0; l < time; ++l)
	{
		boostpy::list middle;
		for (size_t i = 0; i < slice; ++i)
		{
			boostpy::list midder;
			for (size_t j = 0; j < height; ++j)
			{
				boostpy::list inner;
				for (size_t k = 0; k < width; ++k)
				{
					inner.append(*(data + l*slice*height*width + i * width * height + j * width + k));
				}
				midder.append(inner);
			}
			middle.append(midder);
		}
		outsider.append(middle);
	}
	return outsider;
}

template<typename T>
void YapPythonImpl::Pylist4D2CArray(const boostpy::list& li, T* out_data, size_t width, size_t height, size_t slice, size_t time)
{
	T* p = out_data;
	for (size_t l = 0; l < time; ++l)
	{
		for (size_t k = 0; k < slice; ++k)
		{
			for (size_t i = 0; i < height; ++i)
			{
				for (size_t j = 0; j < width; ++j)
				{
					*(p++) = boostpy::extract<T>(li[k][i][j][k][l]);
				}
			}
		}
	}
}

template<>
void YapPythonImpl::Pylist4D2CArray(const boostpy::list& li, complex<double>* out_data, size_t width, size_t height, size_t slice, size_t time)
{
	pylist4d2Carray_complex(li, out_data, width, height, slice, time);
}

template<>
void YapPythonImpl::Pylist4D2CArray(const boostpy::list& li, complex<float>* out_data, size_t width, size_t height, size_t slice, size_t time)
{
	pylist4d2Carray_complex(li, out_data, width, height, slice, time);
}

template<typename T>
void YapPythonImpl::pylist4d2Carray_complex(const boostpy::list &li, complex<T>* out_data, size_t width, size_t height, size_t slice, size_t time)
{
	Py_complex s;
	for (size_t l = 0; l < time; ++l)
	{
		for (size_t k = 0; k < slice; ++k)
		{
			for (size_t i = 0; i < height; ++i)
			{
				for (size_t j = 0; j < width; ++j)
				{
					PyArg_Parse(PyList_GET_ITEM(li.ptr(), (l*slice*width*height + k*width*height + i*width + j)), "D", &s);
					out_data[i].real(s.real);
					out_data[i].imag(s.imag);
				}
			}
		}
	}
}

template<typename T>
void* YapPythonImpl::DoProcess2D(const wchar_t* module_name, const wchar_t* method_name,
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

		// convert T* to python 2D list	!complex still can NOT be converted.
		boostpy::list input_pylist = CArray2Pylist2D(data, width, height);

		boostpy::list return_list = boostpy::extract<boostpy::list>(method(input_pylist, width, height));

		// convert vector to T* out_data: [[..., data, ...], width, height];
		boostpy::list data_matrix_list = boostpy::extract<boostpy::list>(return_list[0]);
		if (PyList_Check(data_matrix_list.ptr()) && PyList_Size(return_list.ptr())==3)
		{
			out_width = boostpy::extract<size_t>(return_list[1]);
			out_height = boostpy::extract<size_t>(return_list[2]);

			T* output_data = new T[out_width * out_height];
			Pylist2D2CArray(data_matrix_list, output_data, out_width, out_height);

			return reinterpret_cast<void*>(output_data);
		}
		else
		{
			throw PyErr_NewException("Return value[0] is not a data list. Check python script return value!", 
				data_matrix_list.ptr(), main_namespace.ptr());
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
void* YapPythonImpl::DoProcess3D(const wchar_t* module_name, const wchar_t * method_name,
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
		boostpy::list input_matrix_list = CArray2Pylist3D(data, width, height, slice);

		boostpy::list return_List = boostpy::extract<boostpy::list>(method(input_matrix_list, width, height, slice));

		// convert vector to T* out_data
		boostpy::list data_matrix_list = boostpy::extract<boostpy::list>(return_List[0]);
		if (PyList_Check(data_matrix_list.ptr()) && PyList_Size(return_List.ptr()) == 4)
		{
			out_width = boostpy::extract<size_t>(return_List[1]);
			out_height = boostpy::extract<size_t>(return_List[2]);
			out_slice = boostpy::extract<size_t>(return_List[3]);
			if (PyList_Size(data_matrix_list.ptr()) != out_width * out_height * out_slice)
				throw PyErr_NewException("Python return data list size != return list marked size", data_matrix_list.ptr(), main_namespace.ptr());

			T* output_data = new T[out_width * out_height * out_slice];
			Pylist3D2CArray(data_matrix_list, output_data, out_width, out_height, out_slice);

			return reinterpret_cast<void*>(output_data);
		}
		else
		{
			throw PyErr_NewException("Return value[0] is not a data list. Check python script return value!", data_matrix_list.ptr(), main_namespace.ptr());
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
void* YapPythonImpl::DoProcess4D(const wchar_t* module_name, const wchar_t * method_name,
	T * data, size_t width, size_t height, size_t slice, size_t time,
	size_t& out_width, size_t& out_height, size_t& out_slice, size_t &out_time)
{
	if (!Py_IsInitialized())
		return nullptr;

	boostpy::object main_module;
	boostpy::object main_namespace;
	try
	{
		main_module = boostpy::import("__main__");
		main_namespace = main_module.attr("__dict__");
		boostpy::object simple = boostpy::exec_file(ToMbs(module_name).c_str(), main_namespace, main_namespace);
		boostpy::object method = main_namespace[ToMbs(method_name).c_str()];

		// convert T* to python list
		boostpy::list input_matrix_list = CArray2Pylist4D(data, width, height, slice,time);

		boostpy::list return_List = boostpy::extract<boostpy::list>(method(input_matrix_list, width, height, slice, time));

		// convert vector to T* out_data
		boostpy::list data_matrix_list = boostpy::extract<boostpy::list>(return_List[0]);
		if (PyList_Check(data_matrix_list.ptr()) && PyList_Size(return_List.ptr()) == 5)
		{
			out_width = boostpy::extract<size_t>(return_List[1]);
			out_height = boostpy::extract<size_t>(return_List[2]);
			out_slice = boostpy::extract<size_t>(return_List[3]);
			out_time = boostpy::extract<size_t>(return_List[4]);

			T* output_data = new T[out_width * out_height * out_slice];
			Pylist4D2CArray(data_matrix_list, output_data, out_width, out_height, out_slice, out_time);

			return reinterpret_cast<void*>(output_data);
		}
		else
		{
			throw PyErr_NewException("Return value[0] is not a data list. Check python script return value!", data_matrix_list.ptr(), main_namespace.ptr());
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
	return new YapPythonImpl();
}
