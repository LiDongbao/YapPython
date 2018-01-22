#include "stdafx.h"
#include <Python.h>

#include "..\ReadFolderAllFiles\INiiReader.h"

#include <boost\python.hpp>
#include <boost\python\stl_iterator.hpp>
#include <boost\python\list.hpp>
#include <boost\python\list.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <iostream>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include <mutex>
#include <vcruntime.h>
#include <list>
#include <vector>
#include <regex>
#include <io.h>
#include <fstream>

#pragma warning(disable:4996)

using namespace std;

bool CheckSysBigOrLittleEndian()
{
	unsigned int i = 0x12345678;
	cout << hex << i << endl;
	cout << dec << i << endl;
	unsigned char *p = (unsigned char*)&i; //将i的地址传给数组指针p，实际上p指向的地址是i在内存中存储的第一个字节，大端就是0x12，小端就是0x78
	if ((*p == 0x78) && (*(p + 1) == 0x56))
		return false;// little endian
	else
		return true;// big endian 
}

const wstring ToWC(const char *c)
{
	size_t cSize = strlen(c) + 1;
	static wchar_t wc[1024];
	mbstowcs_s(&cSize, wc, cSize, c, (size_t)1024);

	return wstring(wc);
}

std::string ToMbs(const wchar_t * wcs)
{
	assert(wcslen(wcs) < 500 && "The string cannot contain more than 500 characters.");

	static char buffer[1024];
	size_t size;

	wcstombs_s(&size, buffer, (size_t)1024, wcs, (size_t)1024);
	return std::string(buffer);
}

int runPythonScript()
{
	using namespace std;
	namespace bpy = boost::python;
	Py_Initialize();    // 检查初始化是否成功
	if (!Py_IsInitialized())
		return -1;

	try
	{
		bpy::object mainModule = bpy::import("__main__");
		bpy::object mainNamespace = mainModule.attr("__dict__");
		bpy::object simple = bpy::exec_file("D:\\demoPython\\example.py", mainNamespace, mainNamespace);
		/*
		bpy::list retList = bpy::extract<bpy::list>(getsomething());
		*/

		std::vector<int> v(10, 10);
		// bpy::object get_iter = bpy::iterator<std::vector<int> >();
		// bpy::object iter = get_iter(v);

		bpy::list li(v.data());
		// result(iter);

		bpy::list outside_list;
		bpy::list inside_list;
		inside_list.append(1);
		inside_list.append(2);
		inside_list.append(3);
		inside_list.append(4);
		outside_list.append(inside_list);
		outside_list.append(inside_list);
		outside_list.append(inside_list);
		outside_list.append(inside_list);
		bpy::object dosomething = mainNamespace["dosomething"];
		bpy::object retList1 = bpy::extract<bpy::list>(dosomething(outside_list));
		bpy::list out_data_list = bpy::extract<bpy::list>(retList1[0]);
		int width = bpy::extract<int>(retList1[1]);
		int height = bpy::extract<int>(retList1[2]);

	}
	catch (...)
	{
		if (PyErr_Occurred())
			PyErr_Print();
		PyErr_Clear();
	}
	// 关闭Python
	Py_Finalize();
	std::system("pause");
	return 0;

	//////////////////////////////////////////////////////////////////////////
	Py_InitializeEx(0);
	if (!Py_IsInitialized())
	{
		return 1;
	}

	// PyThreadState* state = Py_NewInterpreter();	/* start a new sub interpreter */
	namespace bpy = boost::python;
	bpy::object mainModule = bpy::import("__main__");
	bpy::object mainNamespace = mainModule.attr("__dict__");
	bpy::object a = bpy::eval("2**10", mainNamespace, mainNamespace);
	bpy::object simple = bpy::exec_file("D:\\demoPython\\example.py", mainNamespace, mainNamespace);

	// Py_EndInterpreter(state); /* end the 'state' labeled sub interpreter */
	// Py_DECREF(state);

	Py_DECREF(a.ptr());
	Py_DECREF(simple.ptr());
	// Py_XDECREF(mainModule.ptr());
	Py_XDECREF(mainNamespace.ptr());
	Py_DECREF(mainNamespace.ptr());
	Py_Finalize();
	return 1;
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
	std::cout << accept1 << endl;
}

void loadDll()
{
	auto _module = ::LoadLibrary(L"..\\x64\\Debug\\ReadNiiFile.dll");
	if (!_module)
		return ;
	auto get_func = (INiiReader*(*)())::GetProcAddress(_module, "GetNiiData");
	if (!get_func)
		return ;
	auto nii_reader = get_func();

	auto data = nii_reader->ReadFile(L"D:\\test_data\\003_t2_tse_sag.nii");

	switch (nii_reader->GetDataType())
	{
	case TYPE_INT:
	case TYPE_SHORT:

	case TYPE_UNSIGNEDSHORT:

	default:
		break;
	}
}

void Item3Test()
{
	const int *a;
	int b = 12;
	int c = 123;
	a = &b;
	a = &c;
	// *a = 12;// error!
	class A {
	public:
		A() : pText{ "Doron" } {};
		~A() {};
		const char& getChar(size_t index)const
		{
			// _name = "AA"; error! _name is const string  in this function actually.
			return pText[index];
		};
	private:
		char * pText;
	};
	const A obj;
	obj.getChar(1);
	// obj = *(new A()); Error!
	std::cout << obj.getChar(0)<<obj.getChar(1)<<obj.getChar(2) << endl;

	class Date
	{
	public:
		string getCache() const 
		{
			if (!_cache_able)
			{
				Date * th = const_cast<Date*>(this);
				th->_cache_able = true;
				th->computeCacheValue();
			}
			return _cache;
		};

	private:
		bool _cache_able = false;
		string _cache;
		void computeCacheValue() { _cache = "Something"; };
	};
	Date d1;
	const Date d2;
	std::cout << d1.getCache() << endl;
	std::cout << d2.getCache() << endl;
}

void EffectiveC_Chapter2()
{
	class NameClass {
	public:
		NameClass() : _value{114},_name { string("my") } {};
		NameClass(int a, string& st) :_value{ a }, _name{ st } {};
		NameClass(const NameClass & rhs) :_value{ rhs._value }, _name{ rhs._name } {};
		NameClass& operator=(const NameClass& rhs) { return *this; };

		~NameClass() {};
		const int _value;
		string& _name;
	};

	string name("Doron");
	NameClass aobject(1, name);
	NameClass bobject;
	bobject = aobject;
	NameClass cobject(aobject);
	name = "Lee";
	std::cout << bobject._value << bobject._name << endl;

	class Base{
	public:
		Base() {};
		~Base() {};
	private:
		Base(const Base & rhs)=delete;
		Base& operator=(const Base& rhs)=delete;
	};

	class ExtendClass : public Base {
	public:
		void getName() {};
		static ExtendClass * GetInstance() 
		{
			static ExtendClass* s_instance;
			return (s_instance==nullptr ? (new ExtendClass() ): s_instance ); 
		};
	protected:
		ExtendClass() { };
	private:
		int a;
	};
	Base* baseptr= ExtendClass::GetInstance();
	delete baseptr;// this is WRONG, for Base part is deleted but ExtendClass part is still residual.

	// ExtendClass  copy = exc; // Error!! keep ExtendClass from being copied.
}

bool IsLetter(const string& input_string, size_t i)
{
	return (input_string[i] >= 'a'&& input_string[i] <= 'z') ||
		(input_string[i] >= 'A'&& input_string[i] <= 'Z');
}

void countWord()
{
	int count = 0;
	// string input_string;
	char input_string[510];// = new  char[1000];
	cin.get(input_string, 510);
	int size_str = int(std::strlen(input_string));
	if (size_str > 500)
	{
		std::cout << -1 << endl;
		return ;
	}
	if (size_str == 0)
	{
		std::cout << 0 << endl;
		return ;
	}

	for (size_t i = 0; i < 510; ++i)
	{
		bool is_now_letter = IsLetter(input_string, i);
		bool is_split = input_string[i] == ' ' || input_string[i] == ',' || input_string[i] == '.';
		bool is_last_leter = i > 0 ? IsLetter(input_string, i - 1) : true;
		if (input_string[i] == '\n' || input_string[i] == '\0')
		{
			std::cout << (is_last_leter ? ++count : count) << endl;
			return ;
		}
		if (!is_now_letter && !is_split)
		{
			std::cout << -1 << endl;
			return ;
		}
		if (is_split && is_last_leter)
		{
			++count;
		}
	}
	std::cout << count << endl;
}

void didit()
{
	class AA {
	public:
		AA() {};
		virtual ~AA() {};
		AA(const AA&)=delete;
	};
	class BB :public AA {};
	class SpecialString : public string {
	private:
		int * a=new  int[1000000000];
	};

	SpecialString * my_speical=new SpecialString();
	std::auto_ptr<string> string_ptr(my_speical);
	string * std_string = my_speical;
	delete std_string;// no debug error. but this is WRONG for my_special's SpecialString part is not destructed.
	
}

void EffectiveC_Chapter3()
{
	class MutexSource
	{
	public:
		MutexSource(mutex * pm) : _mutex_ptr(pm) 
		{ 
			_mutex_ptr->lock();
			// lock(_mutex_ptr); 
		};
		~MutexSource() 
		{
			_mutex_ptr->unlock();
			// unlock(_mutex_ptr); 
		};
	private:
		mutex* _mutex_ptr;
	};
	class FontResource {
	private:
		int _font_size;
	};

	class Fonts1 {
	public:
		explicit Fonts1(FontResource& font_re) :_font{ &font_re} {};
		FontResource & get()
		{
			return *_font;
		}
		operator FontResource() const{ /// How to convert from a Fonts object to a FontResource object ????
			return *_font;
		};
		int operator [] (int x) {
			return 1;
		}
		int operator () () {
			return 1;
		};
		int operator == (int x) {
			return 1;
		}
		int operator != (int x) { return 1; }
	private:
		FontResource * _font;
	};
	FontResource fontr;
	Fonts1 fs(fontr);
	FontResource ffr = fs;
	int x = fs[1];
	auto y = fs();
	auto size_string = sizeof(string);
	string * string_ptr1 = new string;
	string * string_ptr2 = new string[40];
	string_ptr2[0] = "I hate you so much, can you leave me alone?";
	string_ptr2[1] = "I hate you so much, can you leave me alone? Do you?";
	string my_string = "I hate you so much, can you leave me alone? Do you?";
	delete string_ptr1;
	delete[] string_ptr2;
}

class Name {
public:
	Name(string name) :_first_name{ name } {};
	string GetName()
	{
		return _first_name + _last_name;
	}
	string & GetFirstName()
	{
		return _first_name;
	}

	string & GetLastName()
	{
		return _last_name;
	}
private:
	string _first_name;
	string _last_name;
	string * _string_ptr;
};

void DemoMutex() {
	mutex * use_mutex = new mutex();
	use_mutex->try_lock();
	use_mutex->unlock();
	shared_ptr<Name>(new Name("Doron"));
}


void GetFiles(std::string file_path, std::vector<std::string>& folders, bool is_subfolder)
{
	assert(file_path.length() != 0);
	if (file_path.length() >= _MAX_FNAME || int(file_path.find(' ')) != -1)
		return;

	if (file_path.rfind('\\') == (file_path.length() - 1))
		file_path.pop_back();

	long long   hFile = 0;
	_finddata_t file_info;// file information struct.
	string temp;

	if ((hFile = _findfirst(((temp.assign(file_path)).append("/*")).c_str(), &file_info)) != -1)
	{
		do
		{
			if ((file_info.attrib & _A_SUBDIR))
			{
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0)
				{
					if (is_subfolder)
					{
						GetFiles(temp.assign(file_path).append("//").append(file_info.name), folders, is_subfolder);
					}
				}
			}
			else
			{
				folders.push_back(((temp.assign(file_path)).append("//")).append(file_info.name));
			}
		} while (_findnext(hFile, &file_info) == 0);
		_findclose(hFile);
	}
}


class Base {
public:
	virtual ~Base();
};

Base::~Base() {

};

class Derived : public Base {
public: 
	Derived() : value{12} {};
	~Derived() {};
	int return_value(int& out_value) { 
		int tempt = value;
		out_value = tempt;
		return value; 
	}
private:
	int value;
};

class Derived2 : public Base
{
public:
	Derived2() :my_value{100} {};
	~Derived2() {};
	int return_value() { return my_value; }
private:
	int my_value;
};

int main()
{
	// another_main();
	// runPythonScript();
	// ReadTxtFile();
	// return loadDll();
	// uint64_t tmp = ntohll()
	// Item3Test();
	// EffectiveC_Chapter2();
	// countWord();
	// didit();
	// EffectiveC_Chapter3();
	// DemoMutex();

	using namespace std;
	wstring cs(L"I hate you so much");
	cs = L"I love you so much";
	wcout << cs << endl;
	std::system("pause");

	return 1;
}

