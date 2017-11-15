// EmbedBoostPython.cpp : Defines the entry point for the console application.
#include <stdafx.h>
#include <iostream>
#include <boost\python.hpp>
#include <Python.h>
#include <vector>
#include <boost\python\stl_iterator.hpp>
#include <list>

template< typename T>
inline std::vector<T> to_std_vector(const boost::python::object& iterable)
{
	return std::vector<T>(boost::python::stl_input_iterator<T>(iterable),
		boost::python::stl_input_iterator<T>());
}

bool another_main(void);

template<typename T> void Fine(const wchar_t* module_name, const wchar_t* method_name,
	T* data, T* out_data, size_t width, size_t height,
	size_t& out_width, size_t& out_height) {
	auto in_data = data;

}

template<> void Fine(const wchar_t* module_name, const wchar_t* method_name,
	std::complex<float>* data, std::complex<float>* out_data, size_t width, size_t height,
	size_t& out_width, size_t& out_height) {
	std::complex<float> * comp_data = data;
}

int getFunction()
{
	using namespace std;
	namespace bpy = boost::python;
	Py_Initialize();    // 检查初始化是否成功
	if (!Py_IsInitialized())
		return -1;

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyRun_SimpleString("sys.path.append(r'D:\\demoPython')");
	PyRun_SimpleString("print(sys.path)");
	bpy::object mainModule;
	bpy::object mainNamespace;
	try
	{
		mainModule = bpy::import("__main__");
		mainNamespace = mainModule.attr("__dict__");
		bpy::object simple = bpy::exec_file("D:\\demoPython\\example.py", mainNamespace, mainNamespace);
		//使用boost.python方式获得方法并运行
		bpy::object foo = mainNamespace["foo"];
		string val = bpy::extract<string>(foo("did nothing return source value"));
		bpy::object add = mainNamespace["add"];
		int url = bpy::extract<int>(add(12));
		bpy::object getsomething = mainNamespace["getsomething"];
		bpy::list retList = bpy::extract<bpy::list>(getsomething());

		bpy::list input_list;
		input_list.append('a');
		input_list.append('b');
		input_list.append('c');
		input_list.append('e');
		input_list.append('f');
		// input_list.append(bpy::list((1, 2, 3)));
		bpy::object dosomething = mainNamespace["dosomething"];
		bpy::list retList1 = bpy::extract<bpy::list>(dosomething(input_list));

		cout << "Python has caculated add as: " << val << endl;
		cout << "Python has caculated foo as: " << url << endl;
		cout << "Python has caculated getsomething as: ";
		
		vector<char> my_vect = to_std_vector<char>(retList1);
		for (auto iter : my_vect)
		{
			cout << "\t" << iter;
		}
		cout << endl;
		/*
		// exec, eval, extract<>() e.t. method using example.
		object main_module = import("__main__");
		object main_namespace = main_module.attr("__dict__");
		object ignored = exec("result = 2**10", main_namespace);
		int onekilo = extract<int>(main_namespace["result"]);
		printf("%d\n", onekilo);
		object result = eval("2**10",main_namespace);
		int onekilo_ = extract<int>(result);
		printf("%d\n", onekilo_);
		*/
		//extract 可以直接提取Python值，可以运行Python函数
	}
	catch (...)
	{
		if (PyErr_Occurred())
			PyErr_Print();
		PyErr_Clear();
	}
	// 关闭Python
	Py_Finalize();
	system("pause");
	return 0;
}

int main()
{
	//another_main();
	getFunction();
	
	return 1;

}

bool another_main(void)
{
	Py_Initialize();
	using namespace boost::python;
	handle<>* _module=nullptr; // Module handle.
	const char * path = "D:\\\\demoPython\\\\";
	const char * module = "example"; 
	object main_dictionary;
	bool result = false;
	std::string config_file;

	try
	{// add current script path to path.
		PyRun_SimpleString("import sys");
		auto st = std::string("if '") + path + "' not in sys.path: sys.path.append('" + path + "')";
		PyRun_SimpleString(st.c_str());
		PyRun_SimpleString("print(sys.path)");
		//run __main__ module.
		main_dictionary = import("__main__").attr("__dict__");
		// import new module.
		_module = new handle<>(PyImport_ImportModule(module));
		// invoking python function
		result = call_method<bool>(_module->get(), "getsomething"); //config_file.
	}
	catch (...)
	{
		PyErr_Print();
		PyErr_Clear();
		delete _module;
		_module = nullptr;
		return false;
	}

	if (Py_IsInitialized())
		Py_Finalize();
	return result;
} 


/*
#include "stdafx.h"
#include <boost/python.hpp>


int main()
{
	/*
	* 4 steps to embed python using boost.python
	* 1. #include <boost/python.hpp>
	* 2. call Py_Initialize() to start the interpreter, and Py_Finalize() to stop the interpreter.
	* 3. create the __main__ module.
	* 4. call other Python C API routines to use the interpreter.
	*
	using namespace boost::python;
	object main_module = import("__main__");
	object main_namespace = main_module.attr("__dict__");
	object ignored = exec("hello=file('hello.txt','w')\n"
		"hello.write('Hello world!')\n"
		"hello.close()", main_namespace);

    return 0;
}
*/
