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

#include <stdint.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
// extracted and adapted from https://github.com/aiekick/cTools (LICENSE MIT)
// for make VoxWriter lib free
namespace ct
{
#define SAFE_DELETE(a) if (a != 0) delete a, a = 0
	template <typename T> ::std::string toStr(const T& t)
	{
		::std::ostringstream os;
		os << t;
		return os.str();
	}
	template <typename T> inline T mini(const T& a, const T& b) { return a < b ? a : b; }
	template <typename T> inline T maxi(const T& a, const T& b) { return a > b ? a : b; }
	template <typename T>
	struct vec3
	{
		T x, y, z;
		vec3() : x((T)0), y((T)0), z((T)0) {}
		vec3(T xyz) : x(xyz), y(xyz), z(xyz) {}
		vec3(T x, T y, T z) : x(x), y(y), z(z) {}
		vec3(::std::string vec, char c = ';', vec3<T> *def = 0)//may be in format "0.2f,0.3f,0.4f"
		{
			if (def)
			{
				x = def->x;
				y = def->y;
				z = def->z;
			}
			::std::vector<T> result = StringToNumberVector<T>(vec, c);
			size_t s = result.size();
			if (s > 0) x = result[0];
			if (s > 1) y = result[1];
			if (s > 2) z = result[2];
		}
		vec3 Offset(T vX, T vY, T vZ) { return vec3(x + vX, y + vY, z + vZ); }
		void Set(T vX, T vY, T vZ) { x = vX; y = vY; z = vZ; }
		vec3 operator -() const { return vec3(-x, -y, -z); }
		vec3 yzx() { return vec3(y, z, x); }
		void operator += (const vec3& v) { x += v.x; y += v.y; z += v.z; }
		bool operator == (const vec3& v) { return (x == v.x && y == v.y && z == v.z); }
		bool operator != (const vec3& v) { return (x != v.x || y != v.y || z != v.z); }
		void operator -= (const vec3& v) { x -= v.x; y -= v.y; z -= v.z; }
		void operator *= (T a) { x *= a; y *= a; z *= a; }
		void operator /= (T a) { x /= a; y /= a; z /= a; }
		T length() const { return sqrtf(lengthSquared()); }
		T lengthSquared() const { return x * x + y * y + z * z; }
		T normalize() { T _length = length(); if (_length < (T)1e-5) return (T)0; T _invLength = (T)1 / _length; x *= _invLength; y *= _invLength; z *= _invLength; return _length; }
		vec3<T> getNormalized() { vec3<T> n = vec3<T>(x, y, z); n.normalize(); return n; }
		T sum() { return x + y + z; }
		T sumAbs() { return abs<T>(x) + abs<T>(y) + abs<T>(z); }
		bool empty() { if (x == (T)0 && y == (T)0 && z == (T)0) return true; else return false; }
		std::string string(char c = ';') { return toStr(x) + c + toStr(y) + c + toStr(z); }
	};
	template <typename T> inline vec3<T> operator + (const vec3<T>& v, const T& f) { return vec3<T>(v.x + f, v.y + f, v.z + f); }
	template <typename T> inline vec3<T> operator + (const vec3<T>& v, vec3<T> f) { return vec3<T>(v.x + f.x, v.y + f.y, v.z + f.z); }
	template <typename T> inline vec3<T> operator - (const vec3<T>& v, const T& f) { return vec3<T>(v.x - f, v.y - f, v.z - f); }
	template <typename T> inline vec3<T> operator - (const vec3<T>& v, vec3<T> f) { return vec3<T>(v.x - f.x, v.y - f.y, v.z - f.z); }
	template <typename T> inline vec3<T> operator * (const vec3<T>& v, const T& f) { return vec3<T>(v.x * f, v.y * f, v.z * f); }
	template <typename T> inline vec3<T> operator * (const vec3<T>& v, vec3<T> f) { return vec3<T>(v.x * f.x, v.y * f.y, v.z * f.z); }
	template <typename T> inline vec3<T> operator / (const vec3<T>& v, const T& f) { return vec3<T>(v.x / f, v.y / f, v.z / f); }
	template <typename T> inline vec3<T> operator / (vec3<T>& v, const T& f) { return vec3<T>(v.x / f, v.y / f, v.z / f); }
	template <typename T> inline vec3<T> operator / (const T& f, vec3<T>& v) { return vec3<T>(f / v.x, f / v.y, f / v.z); }
	template <typename T> inline vec3<T> operator / (const vec3<T>& v, vec3<T> f) { return vec3<T>(v.x / f.x, v.y / f.y, v.z / f.z); }
	template <typename T> inline bool operator < (const vec3<T>& v, vec3<T> f) { return v.x < f.x && v.y < f.y && v.z < f.z; }
	template <typename T> inline bool operator < (const vec3<T>& v, const T& f) { return v.x < f && v.y < f && v.z < f; }
	template <typename T> inline bool operator > (const vec3<T>& v, vec3<T> f) { return v.x > f.x && v.y > f.y && v.z > f.z; }
	template <typename T> inline bool operator > (const vec3<T>& v, const T& f) { return v.x > f && v.y > f && v.z > f; }
	template <typename T> inline bool operator <= (const vec3<T>& v, vec3<T> f) { return v.x <= f.x && v.y <= f.y && v.z <= f.z; }
	template <typename T> inline bool operator <= (const vec3<T>& v, const T& f) { return v.x <= f && v.y <= f && v.z <= f; }
	template <typename T> inline bool operator >= (const vec3<T>& v, vec3<T> f) { return v.x >= f.x && v.y >= f.y && v.z >= f.z; }
	template <typename T> inline bool operator >= (const vec3<T>& v, const T& f) { return v.x >= f && v.y >= f && v.z >= f; }
	template <typename T> inline bool operator != (const vec3<T>& v, vec3<T> f) { return (f.x != v.x) && (f.y != v.y) && (f.z != v.z); }
	template <typename T> inline bool operator == (const vec3<T>& v, vec3<T> f) { return (f.x == v.x) && (f.y == v.y) && (f.z == v.z); }
	template <typename T> inline vec3<T> mini(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(mini<T>(a.x, b.x), mini<T>(a.y, b.y), mini<T>(a.z, b.z)); }
	template <typename T> inline vec3<T> maxi(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(maxi<T>(a.x, b.x), maxi<T>(a.y, b.y), maxi<T>(a.z, b.z)); }
	template <typename T> inline T dotS(vec3<T> a, vec3<T> b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	typedef vec3<double> dvec3;
	typedef vec3<float> fvec3;
	typedef vec3<bool> bvec3;
	typedef vec3<int> ivec3;
	typedef vec3<size_t> uvec3;
	template <typename T>
	struct AABBCC // copy of b2AABB struct
	{
		vec3<T> lowerBound;	///< the lower left vertex
		vec3<T> upperBound;	///< the upper right vertex

		AABBCC() : lowerBound((T)0), upperBound((T)0) {}
		AABBCC(vec3<T> vlowerBound, vec3<T> vUpperBound)
			//: lowerBound(vlowerBound), upperBound(vUpperBound) 
		{
			lowerBound.x = mini(vlowerBound.x, vUpperBound.x);
			lowerBound.y = mini(vlowerBound.y, vUpperBound.y);
			lowerBound.z = mini(vlowerBound.z, vUpperBound.z);
			upperBound.x = maxi(vlowerBound.x, vUpperBound.x);
			upperBound.y = maxi(vlowerBound.y, vUpperBound.y);
			upperBound.z = maxi(vlowerBound.z, vUpperBound.z);
		}

		/// Verify that the bounds are sorted.
		//bool IsValid() const;

		/// Add a vector to this vector.
		void operator += (const vec3<T>& v)
		{
			lowerBound += v; upperBound += v;
		}

		/// Subtract a vector from this vector.
		void operator -= (const vec3<T>& v)
		{
			lowerBound -= v; upperBound -= v;
		}

		/// Multiply this vector by a scalar.
		void operator *= (T a)
		{
			lowerBound *= a; upperBound *= a;
		}

		/// Divide this vector by a scalar.
		void operator /= (T a)
		{
			lowerBound /= a; upperBound /= a;
		}

		/// Get the center of the AABB.
		vec3<T> GetCenter() const
		{
			return (lowerBound + upperBound) / (T)2;
		}

		/// Get the extents of the AABB (half-widths).
		vec3<T> GetExtents() const
		{
			return (upperBound - lowerBound) / (T)2;
		}

		/// Get the perimeter length
		T GetPerimeter() const
		{
			float wx = upperBound.x - lowerBound.x;
			float wy = upperBound.y - lowerBound.y;
			float wz = upperBound.z - lowerBound.z;
			return (T)2 * (wx + wy + wz);
		}

		/// Combine an AABB into this one.
		void Combine(const AABBCC<T>& aabb)
		{
			lowerBound = mini(lowerBound, aabb.lowerBound);
			upperBound = maxi(upperBound, aabb.upperBound);
		}

		/// Combine two AABBs into this one.
		void Combine(const AABBCC<T>& aabb1, const AABBCC<T>& aabb2)
		{
			lowerBound = mini(aabb1.lowerBound, aabb2.lowerBound);
			upperBound = maxi(aabb1.upperBound, aabb2.upperBound);
		}

		/// Combine a point into this one.
		void Combine(const vec3<T>& pt)
		{
			lowerBound.x = mini(lowerBound.x, pt.x);
			lowerBound.y = mini(lowerBound.y, pt.y);
			lowerBound.z = mini(lowerBound.z, pt.z);
			upperBound.x = maxi(upperBound.x, pt.x);
			upperBound.y = maxi(upperBound.y, pt.y);
			upperBound.z = maxi(upperBound.z, pt.z);
		}

		/// Does this aabb contain the provided AABB.
		bool Contains(const AABBCC<T>& aabb) const
		{
			bool result = true;
			result = result && lowerBound.x <= aabb.lowerBound.x;
			result = result && lowerBound.y <= aabb.lowerBound.y;
			result = result && lowerBound.z <= aabb.lowerBound.z;
			result = result && aabb.upperBound.x <= upperBound.x;
			result = result && aabb.upperBound.y <= upperBound.y;
			result = result && aabb.upperBound.z <= upperBound.z;
			return result;
		}

		/// Does this aabb contain the provided vec2<T>.
		bool ContainsPoint(const vec3<T>& pt) const
		{
			bool result = true;
			result = result && lowerBound.x <= pt.x;
			result = result && lowerBound.y <= pt.y;
			result = result && lowerBound.z <= pt.z;
			result = result && pt.x <= upperBound.x;
			result = result && pt.y <= upperBound.y;
			result = result && pt.z <= upperBound.z;
			return result;
		}

		bool Intersects(const AABBCC<T>& other)
		{
			bool result = true;
			result = result || lowerBound.x <= other.lowerBound.x;
			result = result || lowerBound.y <= other.lowerBound.y;
			result = result || lowerBound.z <= other.lowerBound.z;
			result = result || other.upperBound.x <= upperBound.x;
			result = result || other.upperBound.y <= upperBound.y;
			result = result || other.upperBound.z <= upperBound.z;
			return result;
		}

		const vec3<T> Size()
		{
			return vec3<T>(upperBound - lowerBound);
		}
	};
	typedef AABBCC<double> dAABBCC;
	typedef AABBCC<float> fAABBCC;
	typedef AABBCC<int> iAABBCC;

	/// Add a float to a AABBCC<T>.
	template <typename T> inline AABBCC<T> operator + (const AABBCC<T>& v, float f)
	{
		return AABBCC<T>(v.lowerBound + f, v.upperBound + f);
	}

	/// Add a AABBCC<T> to a AABBCC<T>.
	template <typename T> inline AABBCC<T> operator + (const AABBCC<T>& v, AABBCC<T> f)
	{
		return AABBCC<T>(v.lowerBound + f.lowerBound, v.upperBound + f.upperBound);
	}

	/// Substract a float from a AABBCC<T>.
	template <typename T> inline AABBCC<T> operator - (const AABBCC<T>& v, float f)
	{
		return AABBCC<T>(v.lowerBound - f, v.upperBound - f);
	}

	/// Substract a AABBCC<T> to a AABBCC<T>.
	template <typename T> inline AABBCC<T> operator - (const AABBCC<T>& v, AABBCC<T> f)
	{
		return AABBCC<T>(v.lowerBound - f.lowerBound, v.upperBound - f.upperBound);
	}

	/// Multiply a float with a AABBCC<T>.
	template <typename T> inline AABBCC<T> operator * (const AABBCC<T>& v, float f)
	{
		return AABBCC<T>(v.lowerBound * f, v.upperBound * f);
	}

	/// Multiply a AABBCC<T> with a AABBCC<T>.
	template <typename T> inline AABBCC<T> operator * (const AABBCC<T>& v, AABBCC<T> f)
	{
		return AABBCC<T>(v.lowerBound * f.lowerBound, v.upperBound * f.upperBound);
	}

	/// Divide a AABBCC<T> by a float.
	template <typename T> inline AABBCC<T> operator / (const AABBCC<T>& v, float f)
	{
		return AABBCC<T>(v.lowerBound / f, v.upperBound / f);
	}

	/// Divide a AABBCC<T> by a float.
	template <typename T> inline AABBCC<T> operator / (AABBCC<T>& v, float f)
	{
		return AABBCC<T>(v.lowerBound / f, v.upperBound / f);
	}

	/// Divide a AABBCC<T> by a AABBCC<T>.
	template <typename T> inline AABBCC<T> operator / (const AABBCC<T>& v, AABBCC<T> f)
	{
		return AABBCC<T>(v.lowerBound / f.lowerBound, v.upperBound / f.upperBound);
	}
}

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
		static VoxWriter* Create(std::string vFilePathName, int32_t vLimitX, int32_t vLimitY, int32_t vLimitZ, errno_t *vError);
		static std::string GetErrnoMsg(errno_t vError);

	private:
		int32_t MV_VERSION;

		int32_t ID_VOX;
		int32_t ID_PACK;
		int32_t ID_MAIN;
		int32_t ID_SIZE;
		int32_t ID_XYZI;
		int32_t ID_RGBA;
		int32_t ID_NTRN;
		int32_t ID_NGRP;
		int32_t ID_NSHP;

		int32_t m_MaxVoxelPerCubeX;
		int32_t m_MaxVoxelPerCubeY;
		int32_t m_MaxVoxelPerCubeZ;

		FILE * m_File;
		ct::dAABBCC maxVolume;
		std::vector<uint32_t> colors;
		std::vector<VoxCube> cubes;
		int32_t maxCubeId;
		int32_t minCubeX;
		int32_t minCubeY;
		int32_t minCubeZ;
		std::map<int32_t, std::map<int32_t, std::map<int32_t, int32_t>>> cubesId;
		std::map<int32_t, std::map<int32_t, std::map<int32_t, int32_t>>> voxelId;

		errno_t lastError;

	public:
		VoxWriter(const uint32_t& vLimitX = 126, const uint32_t& vLimitY = 126, const uint32_t& vLimitZ = 126);
		~VoxWriter();
		errno_t IsOk(std::string vFilePathName);
		void ClearVoxels();
		void ClearColors();
		void AddColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a, const uint8_t& index);
		void AddVoxel(const int32_t& vX, const int32_t& vY, const int32_t& vZ, const uint8_t& vColorIndex);
		void SaveToFile(std::string vFilePathName);
		
	private:
		uint32_t GetID(const uint8_t& a, const uint8_t& b, const uint8_t& c, const uint8_t& d);
		bool OpenFileForWriting(std::string vFilePathName);
		void CloseFile();
		long GetFilePos();
		void SetFilePos(long vPos);
		int32_t GetCubeId(const int32_t& vX, const int32_t& vY, const int32_t& vZ);
		VoxCube* GetCube(const int32_t& vX, const int32_t& vY, const int32_t& vZ);
		void MergeVoxelInCube(const int32_t& vX, const int32_t& vY, const int32_t& vZ, const uint8_t& vColorIndex, VoxCube *vCube);
	};
}
#endif //__VOX_WRITER_H__
