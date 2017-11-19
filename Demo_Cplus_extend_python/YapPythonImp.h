#pragma once
#ifndef _YapPythonImp_H__

struct IYapPython;

class PythonFactory
{
public:
	static IYapPython * GetPython();
};


#define _YapPythonImp_H__

#endif // !_YapPython_H__

