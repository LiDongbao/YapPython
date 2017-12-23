#pragma once
#ifndef FolderFilesReader_h__20171117
#define FolderFilesReader_h__20171117

#include <string.h>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <complex>
#include <memory>
#include "INiiReader.h"

using namespace std;

bool CheckSysBigOrLittleEndian();

class NiiReader :public INiiReader
{
public:
	~NiiReader();

	int GetFileVersion(const wchar_t * file_path) { return 1; };
	std::pair<void *, Nii_v1_FileHeaderInfo*> ReadNiiV1(const wchar_t * file_path) 
	{
		throw "error";
		return std::make_pair((void*)nullptr, new Nii_v1_FileHeaderInfo());
	};

	std::pair<void *, Nii_v2_FileHeaderInfo*> ReadNiiV2(const wchar_t * file_path)
	{
		throw "error";
		return std::make_pair((void*)nullptr, new Nii_v2_FileHeaderInfo());
	};

	virtual void * ReadFile(const wchar_t* nii_path) override;

	virtual int64_t* GetDimensions() override;

	virtual enum NiiDataType GetDataType() override;

	static INiiReader& GetInstance();

private:
	bool ReadV1Dimension(Nii_v1_FileHeaderInfo &v1_header_info);

	bool ReadV2Dimension(Nii_v2_FileHeaderInfo &v2_header_info);

	unsigned CheckVersion(ifstream &file);

	void * ReadData(ifstream &file, short datatype);

	void SwapByteOrder(void * data, size_t byte_count)
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

	void SwapByteOrder(void * data, size_t element_size, size_t array_size)
	{
		if (element_size==1)
			return;
		for (size_t i = 0; i < array_size; ++i)
		{
			SwapByteOrder(reinterpret_cast<char*>(data) + i * element_size, element_size);
		}
	}

	template<typename T>
	void * LoadData(ifstream & file, size_t byte_count, size_t data_size);


	NiiReader();
	static std::shared_ptr<NiiReader> s_instance;
	int64_t _dimensions[8] = { 1,1,1,1,1,1,1,1 };
	void * _data_ptr;
	bool _is_system_endian_same_data;
	int _data_type;
	NiiDataType _current_type;
};

#endif