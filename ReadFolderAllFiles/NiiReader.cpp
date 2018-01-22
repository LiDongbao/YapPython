#include "stdafx.h"
#include "NiiReader.h"
#include <iostream>
#include <string>
#include <fstream>

#include <assert.h>
#include <complex>
#include <memory>

#pragma warning(disable:4996)

using namespace std;

shared_ptr<NiiReader> NiiReader::s_instance;

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

std::string ToMbs(const wchar_t * wcs)
{
	assert(wcslen(wcs) < 500 && "The string cannot contain more than 500 characters.");

	static char buffer[1024];
	wcstombs(buffer, wcs, 1024);//wcstombs_s(wcs,buffer,1024);

	return std::string(buffer);
}

NiiReader::NiiReader()
	: _is_system_endian_same_data{ false }, 
	_data_type{ -1 }, 
	_data_ptr{ nullptr }, 
	_current_type{ TYPE_UNKNOWN }
{
	for (size_t i = 0; i < 8; ++i)
	{
		_dimensions[i] = 1;
	}
	cout << "NiiReader Constructor" << endl;
}

NiiReader::~NiiReader()
{
	cout << "NiiReader destructor" << endl;
}

int NiiReader::GetFileVersion(const wchar_t * file_path)
{
	return 1;
}

std::pair<void *, Nii_v1_FileHeaderInfo*> NiiReader::ReadNiiV1(const wchar_t * file_path)
{
	throw "error";
	return std::make_pair((void*)nullptr, new Nii_v1_FileHeaderInfo());
}

std::pair<void *, Nii_v2_FileHeaderInfo*> NiiReader::ReadNiiV2(const wchar_t * file_path)
{
	throw "error";
	return std::make_pair((void*)nullptr, new Nii_v2_FileHeaderInfo());
}

void* NiiReader::ReadFile(const wchar_t* nii_path)
{
	// do not read non-*.nii suffix data!!!!!
	string path = ToMbs(nii_path);
	auto suffix = path.substr(path.length() - 4, 4);
	if (strcmp(suffix.c_str(), ".nii"))
		return nullptr;

	void * data = nullptr;
	try
	{
		ifstream file(nii_path, ios::binary);
		if (!file.good())
			return (void*)nullptr;
		// make sure the int size=4 and double & longlong & int64_t=8;
		assert(sizeof(int) == 4 && sizeof(long long) == 8);

		// Check file version and check the data is or not same as system endian.
		switch (CheckVersion(file))
		{
		case VERSION_1:/// version NIFTI-1
		{
			Nii_v1_FileHeaderInfo v1_header_info;
			auto header_size = sizeof(Nii_v1_FileHeaderInfo);
			file.seekg(0, ios::beg);
			if (!file.read(reinterpret_cast<char*>(&v1_header_info), header_size))
				return nullptr;
			if (!_is_system_endian_same_data)
			{
				// ReverseV1Header(v1_header_info);
			}
			if (!ReadV1Dimension(v1_header_info))
				return nullptr;

			for (auto dim : _dimensions)
			{
				if (dim > 32768)
				{
					return nullptr;
					// throw std::ifstream::failure("out of range");
				}
			}

			// file stream shift to image data start.
			if (v1_header_info.vox_offset != 0)
				file.seekg(int64_t(v1_header_info.vox_offset), ios::beg);
			else if (v1_header_info.sizeof_hdr == 348)
				file.seekg(v1_header_info.sizeof_hdr, ios::beg);
			else
				return nullptr;
			// read image data
			data = ReadData(file, v1_header_info.datatype);
			_current_type = NiiDataType(v1_header_info.datatype);
			return data;
		}
			break;
		case VERSION_2:/// version NIFTI-2
		{
			Nii_v2_FileHeaderInfo v2_header_info;
			if (!file.read(reinterpret_cast<char*>(&v2_header_info), sizeof(Nii_v2_FileHeaderInfo) - 4)) //NIFTI-2 head size is 540, but Nii_v2_FileHeaderInfo size is 544.
				return nullptr;
			if (!_is_system_endian_same_data)
			{
				// ReverseHeader(v1_header_info);
			}

			if (!ReadV2Dimension(v2_header_info))
				return nullptr;

			for (auto dim : _dimensions)
			{
				if (dim > 32768)
				{
					return nullptr;
					// throw std::ifstream::failure("out of range");
				}
			}
			// file stream shift to image data start.
			if (v2_header_info.vox_offset != 0)
				file.seekg(v2_header_info.vox_offset, ios::beg);
			else if (v2_header_info.sizeof_hdr == 540)
				file.seekg(v2_header_info.sizeof_hdr, ios::beg);
			else
				return nullptr;
			// read image data
			data = ReadData(file, v2_header_info.data_type);
			_current_type = NiiDataType(v2_header_info.data_type);
			return data;
		}
			break;
		case VERSION_3:
		case VERSION_UNKNOWN:
		default:
			return nullptr;
			// throw new bad_alloc;//L"Error: No NIFTI-1 or NIFTI-2 supports type.";
		}
	}
	catch (std::ifstream::failure&)
	{
		return nullptr;
	}
	catch (bad_alloc&)
	{
		return nullptr;
	}

	return data;
}

unsigned NiiReader::CheckVersion(ifstream &file)
{
	int size_hdr;
	file.seekg(0, ios::beg);
	file.read(reinterpret_cast<char*>(&size_hdr), sizeof(int)); // int sizeof_hdr : 4 Bytes
	file.seekg(0, ios::beg);
	// system endian is or not same as data.
	int reverse_hdr = size_hdr;
	SwapByteOrder(reinterpret_cast<void*>(&reverse_hdr), 4);
	if (size_hdr==348||size_hdr==540)
	{
		_is_system_endian_same_data = true;
	}
	else if((reverse_hdr) == 384 || reverse_hdr == 540 )
	{
		_is_system_endian_same_data = false;
	}
	else
	{
		return VERSION_UNKNOWN;
	}
	
	if (size_hdr == 348 || reverse_hdr == 348)
	{
		//version 1
		char version_label[4];
		file.seekg(344, ios::beg);
		file.read(version_label, sizeof(char) * 4);
		file.seekg(0, ios::beg);
		if (strcmp(version_label, "n+1")==0 || strcmp(version_label,"ni1")==0)
		{
			return VERSION_1;
		}
		else // 对于<.hdr, .img> 文件对没有处理 (strcmp(version_label, "ni1")==0)
		{
			return VERSION_UNKNOWN;
		}
	}
	else
	{//version 2
		char version_label[8];
		file.seekg(4, ios::beg);
		// char 类型的数组数据不需要进行大小端转换，因为这些数据即使是小端系统，也都是块与块数据间，从大端到小端进行。
		file.read(version_label, sizeof(char) * 8); 
		file.seekg(0, ios::beg);
		if (strcmp(version_label, "n+2") == 0 || strcmp(version_label, "ni2")==0)
		{
			return VERSION_2;
		} 
		else
		{
			return VERSION_UNKNOWN;
		}
	}
}

bool NiiReader::ReadV1Dimension(Nii_v1_FileHeaderInfo & v1_header_info)
{
	/// 如果第0维不是(1-7)之间的数字，那么这个数据具有相反的字节顺序，所以应该进行字节交换(NIFTI标准没有提供字节顺序的字段，提倡使用dim[0])
	if (!_is_system_endian_same_data)
	{
		// The data is opposite endian compare the system. Need to reverse data order to system order.
		// temporary do not implement this.
		SwapByteOrder(reinterpret_cast<void*>(v1_header_info.dim),2,8);
	}
	// 存放generic matrix数据
	if (v1_header_info.intent_code == 1004 &&
		v1_header_info.dim[5] == int(v1_header_info.intent_p1) * int(v1_header_info.intent_p2))
	{
		_dimensions[0] = int(v1_header_info.intent_p1);
		_dimensions[1] = int(v1_header_info.intent_p2);
	}
	// 存放symmetric matrix数据
	else if (v1_header_info.intent_code == 1005 &&
		v1_header_info.dim[5] == int(v1_header_info.intent_p1 * (v1_header_info.intent_p1 + 1) / 2))
	{
		_dimensions[0] = int(v1_header_info.intent_p1);
		_dimensions[1] = int(v1_header_info.intent_p1);
	}
	//第五维存储的数据是一个rgb数据。
	else if (v1_header_info.intent_code == 2003 &&
		v1_header_info.dim[0] == 5 && v1_header_info.dim[2] == 1 &&
		v1_header_info.dim[3] == 1 && v1_header_info.dim[4] == 1 &&
		v1_header_info.dim[5] == 3)
	{
		if (v1_header_info.dim[1] != int(v1_header_info.intent_p1) * int(v1_header_info.intent_p2))
		{
			return false;
			// throw L"Error: From dimsion hint message, this should be a rgb data.\n but dimension 5 rgb data size check error: dim[1] != (intent_p1 * intent_p2)";
		}
		//handle rgb data
		_dimensions[0] = int(v1_header_info.intent_p1);
		_dimensions[1] = int(v1_header_info.intent_p2);
		_dimensions[2] = v1_header_info.dim[5];//保存rgb的值。
	}

	//第五维存储的数据是一个rgba数据。
	else if (v1_header_info.intent_code == 2004 &&
		v1_header_info.dim[0] == 5 && v1_header_info.dim[2] == 1 &&
		v1_header_info.dim[3] == 1 && v1_header_info.dim[4] == 1 &&
		v1_header_info.dim[5] == 4)
	{
		if (v1_header_info.dim[1] != int(v1_header_info.intent_p1 * v1_header_info.intent_p2))
		{
			return false;
			// throw L"Error: From dimsion hint message, this should be a rgba data.\n but dimension 5 rgba data size check error: dim[1] != (intent_p1 * intent_p2)";
		}
		//handle rgba data
		_dimensions[0] = int(v1_header_info.intent_p1);
		_dimensions[1] = int(v1_header_info.intent_p2);
		_dimensions[2] = v1_header_info.dim[5];//保存rgba的值。
	}
	// normal data dimension
	else if (v1_header_info.dim[0] >= 1)
	{
		int actual_dim = 0;
		for (int i = 1; i < 8; ++i)
		{
			if (v1_header_info.dim[i] > 1)
			{
				_dimensions[i - 1] = v1_header_info.dim[i];
				++actual_dim;
			}
		}
		if (actual_dim != v1_header_info.dim[0])
		{
			for (auto _dim : _dimensions) _dim = 1;
			return false;
			//throw L"Data dimension store have error! please check the data.";
		}
	}
	else
	{
		// Can NOT handle intent descript data: 
		// displacement vector, vector, point set, triangle, quanternion, time series, Node Index, Shape
		return false;
		// throw L"Data dimension can not be interpreted.";
	}
	return true;
}

bool NiiReader::ReadV2Dimension(Nii_v2_FileHeaderInfo &v2_header_info)
{
	if (!_is_system_endian_same_data)
	{
		SwapByteOrder(reinterpret_cast<void*>(v2_header_info.dim),8,8);
	}
	// 存放generic matrix数据
	if (v2_header_info.intent_code == 1004 &&
		v2_header_info.dim[5] == int(v2_header_info.intent_p1) * int(v2_header_info.intent_p2))
	{
		_dimensions[0] = int(v2_header_info.intent_p1);
		_dimensions[1] = int(v2_header_info.intent_p2);
	}
	// 存放symmetric matrix数据
	else if (v2_header_info.intent_code == 1005 &&
		v2_header_info.dim[5] == int(v2_header_info.intent_p1 * (v2_header_info.intent_p1 + 1) / 2))
	{
		_dimensions[0] = int(v2_header_info.intent_p1);
		_dimensions[1] = int(v2_header_info.intent_p1);
	}
	//第五维存储的数据是一个rgb数据。
	else if (v2_header_info.intent_code == 2003 &&
		v2_header_info.dim[0] == 5 && v2_header_info.dim[2] == 1 &&
		v2_header_info.dim[3] == 1 && v2_header_info.dim[4] == 1 &&
		v2_header_info.dim[5] == 3)
	{
		if (v2_header_info.dim[1] != int(v2_header_info.intent_p1) * int(v2_header_info.intent_p2))
		{
			return false;
			// throw L"Error: From dimsion hint message, this should be a rgb data.\n but dimension 5 rgb data size check error: dim[1] != (intent_p1 * intent_p2)";
		}
		//handle rgb data
		_dimensions[0] = int(v2_header_info.intent_p1);
		_dimensions[1] = int(v2_header_info.intent_p2);
		_dimensions[2] = v2_header_info.dim[5];//保存rgb的值。
	}

	//第五维存储的数据是一个rgba数据。
	else if (v2_header_info.intent_code == 2004 &&
		v2_header_info.dim[0] == 5 && v2_header_info.dim[2] == 1 &&
		v2_header_info.dim[3] == 1 && v2_header_info.dim[4] == 1 &&
		v2_header_info.dim[5] == 4)
	{
		if (v2_header_info.dim[1] != int(v2_header_info.intent_p1 * v2_header_info.intent_p2))
		{
			return false;
			// throw L"Error: From dimsion hint message, this should be a rgba data.\n but dimension 5 rgba data size check error: dim[1] != (intent_p1 * intent_p2)";
		}
		//handle rgba data
		_dimensions[0] = int(v2_header_info.intent_p1);
		_dimensions[1] = int(v2_header_info.intent_p2);
		_dimensions[2] = v2_header_info.dim[5];//保存rgba的值。
	}
	// normal data dimension
	else if (v2_header_info.dim[0] >= 1)
	{
		int actual_dim = 0;
		for (int i = 1; i < 8; ++i)
		{
			if (v2_header_info.dim[i] > 1)
			{
				_dimensions[i - 1] = v2_header_info.dim[i];
				++actual_dim;
			}
		}
		if (actual_dim != v2_header_info.dim[0])
		{
			for (int i = 0; i < 8; ++i) 
			{ 
				_dimensions[i] = 1; 
			}
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

void * NiiReader::ReadData(ifstream &file, short datatype)
{
	// shift the location to data start.
	int data_size = 1;
	for (auto dim:_dimensions)
	{
		data_size *= int(dim);
	}
	switch (datatype)
	{
	case TYPE_BOOL:
		return LoadData<bool>(file, 1 , data_size);
	case TYPE_UNSIGNEDCHAR:
		return LoadData<unsigned char>(file, 1, data_size);
	case TYPE_SHORT:
		return  LoadData<short>(file, 2, data_size);
	case TYPE_INT:
		return LoadData<int>(file, 4, data_size);
	case TYPE_FLOAT:
		return LoadData<float>(file, 4, data_size);
	case TYPE_COMPLEX:
		return LoadData<complex<float>>(file, 4, data_size * 2);
	case TYPE_DOUBLE:
		return LoadData<double>(file, 8, data_size);
	case TYPE_RGB:
		// rgb & rgba use unsigned char to store，虽然rgb是3字节，
		// 但是因为在_dimensions中已经保存了数据的rgb值信息，所以不许需要做特殊处理
		return LoadData<unsigned char>(file, 1, data_size);
	case TYPE_RGBA:
		return LoadData<unsigned char>(file, 1, data_size);
	case TYPE_CHAR:
		return LoadData<char>(file, 1, data_size);
	case TYPE_UNSIGNEDSHORT:
		return LoadData<unsigned short>(file, 2, data_size);
	case TYPE_UNSIGNEDINT:
		return LoadData<unsigned int>(file, 4, data_size);
	case TYPE_LONGLONG:
		return LoadData<long long>(file, 8, data_size);
	case TYPE_UNSIGNEDLONGLONG:
		return LoadData<unsigned long long>(file, 8, data_size);
	case TYPE_LONGDOUBLE:
	case TYPE_DOUBLEPAIR:
	case TYPE_LONGDOUBLEPAIR:
	case TYPE_ALL:
	case TYPE_UNKNOWN:
	default:
		return (void*)nullptr;
	}
	return (void*)nullptr;
}

int64_t * NiiReader::GetDimensions()
{
	return this->_dimensions;
}

enum NiiDataType NiiReader::GetDataType()
{
	return this->_current_type;
}

INiiReader& NiiReader::GetInstance()
{
	if (!s_instance)
	{
		try
		{
			s_instance = shared_ptr<NiiReader>(new NiiReader());
		}
		catch (std::bad_alloc&)
		{
			return *shared_ptr<NiiReader>(nullptr);
		}
	}
	return *s_instance;
}


void NiiReader::SwapByteOrder(void * data, size_t byte_count)
{
	auto buffer = reinterpret_cast<char*>(data);
	char temp;
	for (unsigned int i = 0; i < byte_count / 2; ++i)
	{
		temp = buffer[i];
		buffer[i] = buffer[byte_count - 1 - i];
		buffer[byte_count - 1 - i] = temp;
	}
}

void NiiReader::SwapByteOrder(void * data, size_t element_size, size_t array_size)
{
	if (element_size == 1)
		return;
	for (size_t i = 0; i < array_size; ++i)
	{
		SwapByteOrder(reinterpret_cast<char*>(data) + i * element_size, element_size);
	}
}

template<typename T>
void * NiiReader::LoadData(ifstream & file, size_t byte_count, size_t data_size)
{
	T * data;
	try
	{
		data = new  T[data_size];
	}
	catch (const std::bad_alloc&)
	{
		return (void*)nullptr;
	}
	if (!file.read(reinterpret_cast<char*>(data), data_size* sizeof(T)))
	{
		delete[] data;
		return (void*)nullptr;
	}
	else
	{
		if (!_is_system_endian_same_data)
		{
			SwapByteOrder(reinterpret_cast<void*>(data), byte_count, data_size);
		}
		return reinterpret_cast<void*>(data);
	}
}
