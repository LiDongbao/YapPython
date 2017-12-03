#pragma once
#ifndef _INiiReader_H__
#define _INiiReader_H__

struct INiiReader
{
	virtual void * ReadFile(const wchar_t* nii_path) = 0;

	virtual int64_t* GetDimensions() = 0;

	virtual enum NiiDataType GetDataType() = 0;
};


enum NiiVersion
{
	VERSION_1 = 1,
	VERSION_2 = 2,
	VERSION_3 = 3,
	VERSION_4 = 4,
	VERSION_UNKNOWN = (unsigned(-1))
};

enum NiiDataType {
	TYPE_UNKNOWN = 0,
	TYPE_BOOL = 1,
	TYPE_UNSIGNEDCHAR = 2,
	TYPE_SHORT = 4,
	TYPE_INT = 8,
	TYPE_FLOAT = 16,
	TYPE_COMPLEX = 32,
	TYPE_DOUBLE = 64,
	TYPE_RGB = 128,
	TYPE_ALL = 255,
	TYPE_CHAR = 256,
	TYPE_UNSIGNEDSHORT = 512,
	TYPE_UNSIGNEDINT = 768,
	TYPE_LONGLONG = 1024,
	TYPE_UNSIGNEDLONGLONG = 1280,
	TYPE_LONGDOUBLE = 1536,
	TYPE_DOUBLEPAIR = 1792,
	TYPE_LONGDOUBLEPAIR = 2048,
	TYPE_RGBA = 2304
};
#endif
