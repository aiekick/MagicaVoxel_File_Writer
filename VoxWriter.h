// Copyright 2018 Stephane Cuillerdier @Aiekick

// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without 
// limitation the rights to use, copy, modify, merge, publish, distribute, 
// sublicense, and/or sell copies of the Software, and to permit persons to 
// whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// This File is a helper for write a vox file after 0.99 release to support 
// the world mode editor
// just add all color with the color Index with AddColor
// And add all voxels with the method AddVoxel with the voxel in world position, and finally save the model
// that's all, the file was initially created for my Proecedural soft 
// "SdfMesher" cf :https://twitter.com/hashtag/sdfmesher?src=hash
// it support just my needs for the moment, but i put here because its a basis for more i thinck

#ifndef __VOX_WRITER_H__
#define __VOX_WRITER_H__

#include "Tools.hpp"
#include <stdint.h>
#include <string>
#include <map>
#include <vector>

namespace vox
{
	inline uint32_t GetMVID(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
	{
		return (a) | (b << 8) | (c << 16) | (d << 24);
	}
		
	struct DICTstring
	{
		int32_t bufferSize;
		std::string buffer;
		
		DICTstring();
		
		void write(FILE *fp);
		size_t getSize();
	};

	struct DICTitem
	{
		DICTstring key;
		DICTstring value;
		
		DICTitem();
		DICTitem(std::string vKey, std::string vValue);
	
		void write(FILE *fp);
		size_t getSize();
	};

	struct DICT
	{
		int32_t count;
		std::vector<DICTitem> keys;

		DICT();
		void write(FILE *fp);
		size_t getSize();
		void Add(std::string vKey, std::string vValue);
	};

	struct nTRN
	{
		int32_t nodeId;
		DICT nodeAttribs;
		int32_t childNodeId;
		int32_t reservedId;
		int32_t layerId;
		int32_t numFrames;
		std::vector<DICT> frames;

		nTRN(int32_t countFrames);

		void write(FILE *fp);
		size_t getSize();
	};

	struct nGRP
	{
		int32_t nodeId;
		DICT nodeAttribs;
		int32_t nodeChildrenNodes;
		std::vector<int32_t> childNodes;

		nGRP(int32_t vCount);
		
		void write(FILE *fp);
		size_t getSize();
	};

	struct MODEL
	{
		int32_t modelId;
		DICT modelAttribs;

		MODEL();
		
		void write(FILE *fp);
		size_t getSize();
	};
	
	struct nSHP
	{
		int32_t nodeId;
		DICT nodeAttribs;
		int32_t numModels;
		std::vector<MODEL> models;

		nSHP(int32_t vCount);
		
		void write(FILE *fp);
		size_t getSize();
	};

	struct LAYR
	{
		int32_t nodeId;
		DICT nodeAttribs;
		int32_t reservedId;

		LAYR();
		void write(FILE *fp);
		size_t getSize();
	};

	struct SIZE
	{
		int32_t sizex;
		int32_t sizey;
		int32_t sizez;
		
		SIZE();
		
		void write(FILE *fp);
		size_t getSize();
	};

	struct XYZI
	{
		int32_t numVoxels;
		std::vector<uint8_t> voxels;

		XYZI();
		void write(FILE *fp);
		size_t getSize();
	};

	struct RGBA
	{
		int32_t colors[256];

		RGBA();
		void write(FILE *fp);
		size_t getSize();
	};	

	struct VoxCube
	{
		int id;

		// size
		SIZE size;

		// translate
		int tx;
		int ty;
		int tz;

		XYZI xyzi;

		VoxCube();

		void write(FILE *fp);
	};

	class VoxWriter
	{
	public:
		static VoxWriter* Create(std::string vFilePathName, int vLimitX, int vLimitY, int vLimitZ, errno_t *vError);
		static std::string GetErrnoMsg(errno_t vError);

	private:
		int MV_VERSION;

		int ID_VOX;
		int ID_PACK;
		int ID_MAIN;
		int ID_SIZE;
		int ID_XYZI;
		int ID_RGBA;
		int ID_NTRN;
		int ID_NGRP;
		int ID_NSHP;

		int m_LimitX;
		int m_LimitY;
		int m_LimitZ;

		FILE * m_File;
		cAABBCC maxVolume;
		std::vector<uint32_t> colors;
		std::vector<VoxCube> cubes;
		int maxCubeId;
		int minCubeX;
		int minCubeY;
		int minCubeZ;
		int maxCubeX;
		int maxCubeY;
		int maxCubeZ;
		std::map<int, std::map<int, std::map<int, int>>> cubesId;
		std::map<int, std::map<int, std::map<int, int>>> voxelId;

		errno_t lastError;

	public:
		VoxWriter(int vLimitX, int vLimitY, int vLimitZ);
		~VoxWriter();
		errno_t IsOk(std::string vFilePathName);
		void ClearVoxels();
		void ClearColors();
		void AddColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint8_t index);
		void AddVoxel(uint32_t vX, uint32_t vY, uint32_t vZ, uint8_t vColorIndex);
		void SaveToFile(std::string vFilePathName);
		
	private:
		uint32_t GetID(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
		bool OpenFileForWriting(std::string vFilePathName);
		void CloseFile();
		long GetFilePos();
		void SetFilePos(long vPos);
		int GetCubeId(int vX, int vY, int vZ);
		VoxCube* GetCube(int vX, int vY, int vZ);
		void MergeVoxelInCube(uint32_t vX, uint32_t vY, uint32_t vZ, uint32_t vColorIndex, VoxCube *vCube);
	};
}
#endif //__VOX_WRITER_H__
