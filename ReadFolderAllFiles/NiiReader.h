#pragma once
#ifndef FolderFilesReader_h__20171117
#define FolderFilesReader_h__20171117

#include "INiiReader.h"
#include <fstream>
#include <memory>

using namespace std;

bool CheckSysBigOrLittleEndian();

class NiiReader :public INiiReader
{
public:
	~NiiReader();

	int GetFileVersion(const wchar_t * file_path);

	std::pair<void *, Nii_v1_FileHeaderInfo*> ReadNiiV1(const wchar_t * file_path);

	std::pair<void *, Nii_v2_FileHeaderInfo*> ReadNiiV2(const wchar_t * file_path);

	virtual void * ReadFile(const wchar_t* nii_path) override;

	virtual int64_t * GetDimensions() override;

	virtual enum NiiDataType GetDataType() override;

	static INiiReader& GetInstance();

private:
	bool ReadV1Dimension(Nii_v1_FileHeaderInfo &v1_header_info);

	bool ReadV2Dimension(Nii_v2_FileHeaderInfo &v2_header_info);

	unsigned CheckVersion(ifstream &file);

	void * ReadData(ifstream &file, short datatype);

	void SwapByteOrder(void * data, size_t byte_count);

	void SwapByteOrder(void * data, size_t element_size, size_t array_size);

	template<typename T>
	void * LoadData(ifstream & file, size_t byte_count, size_t data_size);


	NiiReader();
	static std::shared_ptr<NiiReader> s_instance;
	int64_t _dimensions[8];
	void * _data_ptr;
	bool _is_system_endian_same_data;
	int _data_type;
	NiiDataType _current_type;
};

#endif