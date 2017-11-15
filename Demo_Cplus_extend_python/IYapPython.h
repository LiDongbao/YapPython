#pragma once
#ifndef _IYapPython_H__
#define _IYapPython_H__

#include <complex>
#include <map>

const int DataTypeChar = 0x00000001;			///< char (1 byte)
const int DataTypeUnsignedChar = 0x00000002;	///< unsigned char (1 byte)
const int DataTypeShort = 0x00000004;			///< short (2 bytes)
const int DataTypeUnsignedShort = 0x00000008;	///< unsigned short (2 bytes)
const int DataTypeFloat = 0x00000010;			///< float (4 bytes)
const int DataTypeDouble = 0x00000020;			///< double (8 bytes)
const int DataTypeInt = 0x00000040;				///< int (4 bytes)
const int DataTypeUnsignedInt = 0x00000080;		///< unsigned int (4 bytes)
const int DataTypeComplexFloat = 0x00000100;	///< complex<float> (2 * 4 bytes)
const int DataTypeComplexDouble = 0x00000200;	///< complex<double> (2 * 8 bytes)
const int DataTypeBool = 0x00000400;			///< bool (1 bytes)
const int DataTypeAll = 0xFFFFFFFF;				///< all types accepted

template<typename T> struct data_type_id
{
	static const int type = 0;
};

template <> struct data_type_id<double> { static const int type = DataTypeDouble; };
template <> struct data_type_id<char> { static const int type = DataTypeChar; };
template <> struct data_type_id<unsigned char> { static const int type = DataTypeUnsignedChar; };
template <> struct data_type_id<short> { static const int type = DataTypeShort; };
template <> struct data_type_id<unsigned short> { static const int type = DataTypeUnsignedShort; };
template <> struct data_type_id<float> { static const int type = DataTypeFloat; };
template <> struct data_type_id<unsigned int> { static const int type = DataTypeUnsignedInt; };
template <> struct data_type_id<int> { static const int type = DataTypeInt; };
template <> struct data_type_id<std::complex<float>> { static const int type = DataTypeComplexFloat; };
template <> struct data_type_id<std::complex<double>> { static const int type = DataTypeComplexDouble; };
template <> struct data_type_id<bool> { static const int type = DataTypeBool; };

namespace PyDataType {
	//dynamic python arguments type convert define.
	template<typename T> struct pydata_type_id { const char * type = ""; };
	// as below is implement struct pydata_type_id.
	template<> struct pydata_type_id<double> { const char * type = "d"; };
	template<> struct pydata_type_id<char> { const char * type = "s"; };
	template<> struct pydata_type_id<unsigned char> { const char* type = "b"; };
	template<> struct pydata_type_id<short> { const char * type = "h"; };
	template<> struct pydata_type_id<unsigned short> { const char * type = "h"; };
	template<> struct pydata_type_id<float> { const char * type = "f"; };
	template<> struct pydata_type_id<unsigned int> { const char * type = "i"; };
	template<> struct pydata_type_id<int> { const char * type = "i"; };
	template<> struct pydata_type_id<std::complex<float>> { const char * type = "D"; };// convert to Python object. if error, return NULL.
	template<> struct pydata_type_id<std::complex<double>> { const char * type = "D"; };
	template<> struct pydata_type_id<bool> { const char * type = "i"; };
};

struct IYapPython
{
	virtual void* Process2D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
		void * data, size_t width, size_t height, size_t& out_width, size_t& out_height) = 0;
	virtual void* Process3D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
		void * data, size_t width, size_t height, size_t slice, size_t& out_width,
		size_t& out_height, size_t& out_slice) = 0;
};


#endif