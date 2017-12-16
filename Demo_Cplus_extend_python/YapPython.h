#pragma once
#ifndef _YapPython_H__
#define _YapPython_H__

struct IYapPython2;

class PythonFactory
{
public:
	static IYapPython2 * GetPython();
};

#endif // !_YapPython_H__

