#pragma once
#ifndef _INiiReader_H__
#define _INiiReader_H__
#include <stdint.h>
#include <utility>

/*
the NIFTI-1 version head size must be 348 bytes. least unit size is 4 bytes.
and the head info actual size is 348 bytes.
*/
struct Nii_v1_FileHeaderInfo
{
	//OFFSET	SIZE	Description
	int32_t	sizeof_hdr;		//0B		4B		Size of the header.Must be 348 (bytes).
	char	data_type[10];	//4B		10B		Not used; compatibility with analyze.
	char	db_name[18];	//14B		18B		Not used; compatibility with analyze.
	int32_t	extents;		//32B		4B		Not used; compatibality with analyze.
	short	session_error;	//36B		2B		Not used; compatibility with analyze.
	char	regular;		//38B		1B		Not used; compatibility with analyze.
	char	dim_info;		//39B		1B		Encoding directions(phase, frequency, slice).
	int16_t	dim[8];			//40B		16B		Data array dimensions.
	float	intent_p1;		//56B		4B		1st intent parameter.
	float	intent_p2;		//60B		4B		2nd intent parameter.
	float	intent_p3;		//64B		4B		3rd intent parameter.
	int16_t	intent_code;	//68B		2B		nifti intent.
	int16_t	datatype;		//70B		2B		Data type.
	int16_t	bitpix;			//72B		2B		Number of bits per voxel.
	int16_t	slice_start;	//74B		2B		First slice index.
	float	pixdim[8];		//76B		32B		Grid spacings(unit per dimension).
	float	vox_offset;		/*108B		4B		Offset into a.nii file. !Data true start point.*/
	float	scl_slope;		//112B		4B		Data scaling, slope.
	float	scl_inter;		//116B		4B		Data scaling, offset.
	int16_t	slice_end;		//120B		2B		Last slice index.
	char	slice_code;		//122B		1B		Slice timing order.
	char	xyzt_units;		//123B		1B		Units of pixdim[1..4].
	float	cal_max;		//124B		4B		Maximum display intensity.
	float	cal_min;		//128B		4B		Minimum display intensity.
	float	slice_duration;	//132B		4B		Time for one slice.
	float	toffset;		//136B		4B		Time axis shift.
	int32_t	glmax;			//140B		4B		Not used; compatibility with analyze.
	int32_t	glmin;			//144B		4B		Not used; compatibility with analyze.
	char	descrip[80];	//148B		80B		Any text.
	char	aux_file[24];	//228B		24B		Auxiliary filename.
	int16_t	qform_code;		//252B		2B		Use the quaternion fields.
	int16_t	sform_code;		//254B		2B		Use of the affine fields.
	float	quatern_b;		//256B		4B		Quaternion b parameter.
	float	quatern_c;		//260B		4B		Quaternion c parameter.
	float	quatern_d;		//264B		4B		Quaternion d parameter.
	float	qoffset_x;		//268B		4B		Quaternion x shift.
	float	qoffset_y;		//272B		4B		Quaternion y shift.
	float	qoffset_z;		//276B		4B		Quaternion z shift.
	float	srow_x[4];		//280B		16B		1st row affine transform
	float	srow_y[4];		//296B		16B		2nd row affine transform.
	float	srow_z[4];		//312B		16B		3rd row affine transform.
	char	intent_name[16];//328B		16B		Name or meaning of the data.
	char	magic[4];		//344B		4B		Magic string.

};

/*
the NIFTI-2 version head size must be 540 bytes. least unit size is 8 bytes.
and the head info actual size is 544 bytes.
*/
struct Nii_v2_FileHeaderInfo {
	//OFFSET	SIZE	DESCRIPTION
	int32_t	sizeof_hdr;		//0B		4B		Size of the header.Must be 540 (bytes).
	char	magic[8];		//4B		8B		Magic string, defining a valid signature.
	int16_t	data_type;		//12B		2B		Data type.
	int16_t	bitpix;			//14B		2B		Number of bits per voxel.
	int64_t	dim[8];			//16B		64B		Data array dimensions.
	double	intent_p1;		//80B		8B		1st intent parameter.
	double	intent_p2;		//88B		8B		2nd intent parameter.
	double	intent_p3;		//96B		8B		3rd intent parameter.
	double	pixdim[8];		//104B		64B		Grid spacings(unit per dimension).
	int64_t	vox_offset;		//168B		8B		Offset into a.nii file.
	double	scl_slope;		//176B		8B		Data scaling, slope.
	double	scl_inter;		//184B		8B		Data scaling, offset.
	double	cal_max;		//192B		8B		Maximum display intensity.
	double	cal_min;		//200B		8B		Minimum display intensity.
	double	slice_duration;	//208B		8B		Time for one slice.
	double	toffset;		//216B		8B		Time axis shift.
	int64_t	slice_start;	//224B		8B		First slice index.
	int64_t	slice_end;		//232B		8B		Last slice index.
	char	descrip[80];	//240B		80B		Any text.
	char	aux_file[24];	//320B		24B		Auxiliary filename.
	int32_t	qform_code;		//344B		4B		Use the quaternion fields.
	int32_t	sform_code;		//348B		4B		Use of the affine fields.
	double	quatern_b;		//352B		8B		Quaternion b parameter.
	double	quatern_c;		//360B		8B		Quaternion c parameter.
	double	quatern_d;		//368B		8B		Quaternion d parameter.
	double	qoffset_x;		//376B		8B		Quaternion x shift.
	double	qoffset_y;		//384B		8B		Quaternion y shift.
	double	qoffset_z;		//392B		8B		Quaternion z shift.
	double	srow_x[4];		//400B		32B		1st row affine transform.
	double	srow_y[4];		//432B		32B		2nd row affine transform.
	double	srow_z[4];		//464B		32B		3rd row affine transform.
	int32_t	slice_code;		//496B		4B		Slice timing order.
	int32_t	xyzt_units;		//500B		4B		Units of pixdim[1..4].
	int32_t	intent_code;	//504B		4B		nifti intent.
	char	intent_name[16];//508B		16B		Name or meaning of the data.
	char	dim_info;		//524B		1B		Encoding directions.
	char	unused_str[15];	//525B		15B		Unused, to be padded with with zeroes.
};

struct INiiReader
{
	virtual void * ReadFile(const wchar_t* nii_path) = 0;

	virtual int64_t* GetDimensions() = 0;

	virtual enum NiiDataType GetDataType() = 0;

	virtual int GetFileVersion(const wchar_t * file_path) = 0;
	
	virtual std::pair<void *, Nii_v1_FileHeaderInfo*> ReadNiiV1(const wchar_t * file_path) = 0; // 在外部可以获得文件的头有用信息，并且可以在外部空间使用这些信息。

	virtual std::pair<void *, Nii_v2_FileHeaderInfo*> ReadNiiV2(const wchar_t * file_path) = 0;
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
