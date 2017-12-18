#include "stdafx.h"
#include <vcruntime.h>
#include <Python.h>
#include <iostream>
#include <boost\python.hpp>
#include <boost\python\stl_iterator.hpp>
#include <boost\python\list.hpp>
#include "..\ReadFolderAllFiles\NiiReader.h"
#include <boost\python\list.hpp>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include <mutex>
 
#include <list>
#include <vector>

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
	/*
	D:\\test_data\\003_t2_tse_sag.nii
	\\\\alg-cloud\\Public\\MRI Data\\MIDA\\MIDA_v1.0\\MIDA_v1_voxels\\MIDA_v1.nii
	\\\\alg-cloud\\Public\\MRI Data\\QSM-GYLi\\bobinshi\\QSM_32bit.hdr
	*/
	auto _module = ::LoadLibrary(L"..\\x64\\Debug\\ReadFolderAllFiles.dll");
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

const int add(const int a, const int b) 
{
	return a + b;
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

void RemoveName(string* st) {
	delete st;
}

void DemoMutex() {
	string * my_name = new string("Doron");
	shared_ptr<string> name_ptr;
	name_ptr.reset(my_name, RemoveName);

	shared_ptr<string> name_ptr2(my_name, RemoveName);

	mutex * use_mutex = new mutex();
	use_mutex->try_lock();
	use_mutex->unlock();
	shared_ptr<Name>(new Name("Doron"));
}

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

	vector<string> vec;
	vec.push_back("C++");
	vec.push_back("Java");
	vec.push_back("PHP");
	string * ptr = vec.data();
	for (size_t i = 0; i < vec.size(); ++i)
	{
		std::cout << *(ptr++) << std::endl;
	}
	std::system("pause");
	return 1;
}
