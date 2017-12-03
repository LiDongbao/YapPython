// EmbedBoostPython.cpp : Defines the entry point for the console application.
#include <stdafx.h>
#include <Python.h>
#include <iostream>
#include <vector>
#include <boost\python.hpp>
#include <boost\python\stl_iterator.hpp>
#include "..\ReadFolderAllFiles\NiiReader.h"
#include <boost\python\list.hpp>
#include <windows.h>

using namespace std;

template< typename T>
inline std::vector<T> to_std_vector(const boost::python::object& iterable)
{
	return std::vector<T>(boost::python::stl_input_iterator<T>(iterable),
		boost::python::stl_input_iterator<T>());
}

int getFunction()
{
	using namespace std;
	namespace bpy = boost::python;
	Py_Initialize();    // 检查初始化是否成功
	if (!Py_IsInitialized())
		return -1;

	bpy::object mainModule;
	bpy::object mainNamespace;
	try
	{
		mainModule = bpy::import("__main__");
		mainNamespace = mainModule.attr("__dict__");
		bpy::object simple = bpy::exec_file("D:\\demoPython\\example.py", mainNamespace, mainNamespace);
		//使用boost.python方式获得方法并运行
		/*bpy::object foo = mainNamespace["foo"];
		string val = bpy::extract<string>(foo("did nothing return source value"));
		bpy::object add = mainNamespace["add"];
		int url = bpy::extract<int>(add(12));
		bpy::object getsomething = mainNamespace["getsomething"];
		bpy::list retList = bpy::extract<bpy::list>(getsomething());
		*/

		bpy::list input_list;
		input_list.append('a');
		input_list.append('b');
		input_list.append('c');
		input_list.append('e');
		input_list.append('f');
		// input_list.append(bpy::list((1, 2, 3)));
		bpy::object dosomething = mainNamespace["dosomething"];
		bpy::list retList1 = bpy::extract<bpy::list>(dosomething(input_list));
		cout << "Python has caculated getsomething as: ";
		
		vector<char> my_vect = to_std_vector<char>(retList1);
		for (auto iter : my_vect)
		{
			cout << "\t" << iter;
		}
		cout << endl;
		bpy::object result = eval("2**10", mainNamespace, mainNamespace);
		int onekilo_ = bpy::extract<int>(result);
		bpy::object result1 = exec("result = 2**10", mainNamespace, mainNamespace);
		bpy::object simple1 = bpy::exec_file("D:\\demoPython\\example.py", mainNamespace, mainNamespace);
		auto extract_result = bpy::extract<bpy::list>(bpy::extract<bpy::list>(dosomething(input_list)));
		/*
		// exec, eval, extract<>() e.t. method using example.
		bpy::object main_module = import("__main__");
		bpy::object main_namespace = main_module.attr("__dict__");
		bpy::object ignored = exec("result = 2**10", main_namespace, main_namespace);
		int onekilo = extract<int>(main_namespace["result"]);
		printf("%d\n", onekilo);
		object result = eval("2**10",main_namespace);
		int onekilo_ = extract<int>(result);
		printf("%d\n", onekilo_);
		*/
		// extract 可以直接提取Python值，可以运行Python函数
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

bool another_main(void)
{
	Py_Initialize();
	using namespace boost::python;
	handle<>* _module = nullptr; // Module handle.
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

void ReadTxtFile()
{
	ifstream file("D:\\test_data\\my.txt", ios::beg);
	char accept1[10] = { '\0' };
	file.read(accept1, 7);
	cout << accept1 << endl;
}

int main()
{
	/*
	D:\\test_data\\003_t2_tse_sag.nii
	\\\\alg-cloud\\Public\\MRI Data\\MIDA\\MIDA_v1.0\\MIDA_v1_voxels\\MIDA_v1.nii
	\\\\alg-cloud\\Public\\MRI Data\\QSM-GYLi\\bobinshi\\QSM_32bit.hdr
	*/
	// another_main();
	// getFunction();
	// ReadTxtFile();
	
	auto _module = ::LoadLibrary(L"..\\x64\\Debug\\ReadFolderAllFiles.dll");
	if (!_module)
		return 0;
	auto get_func = (INiiReader*(*)())::GetProcAddress(_module, "GetNiiData");
	if (!get_func)
		return 0;
	auto nii_reader = get_func();

	auto data = nii_reader->ReadFile(L"D:\\test_data\\003_t2_tse_sag.nii");
	
	switch (nii_reader->GetDataType())
	{
	case TYPE_INT:

	case TYPE_SHORT:
	default:
		break;
	}
	// uint64_t tmp = ntohll()
	system("pause");
	return 1;
}

