#pragma once
#ifndef _YapPython_H__
#define _YapPython_H__

struct IYapPython;

class PythonFactory
{
public:
	static IYapPython * GetPython();
};

#endif // !_YapPython_H__

