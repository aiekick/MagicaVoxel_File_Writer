// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include "VoxWriter.h"
#include <cstdio>
#include <iostream>

// #define VERBOSE

namespace vox {
DICTstring::DICTstring() { bufferSize = 0; }

void DICTstring::write(FILE* fp) {
    bufferSize = (int32_t)buffer.size();
    fwrite(&bufferSize, sizeof(int32_t), 1, fp);
    fwrite(buffer.data(), sizeof(char), bufferSize, fp);
}

size_t DICTstring::getSize() {
    bufferSize = (int32_t)buffer.size();
    return sizeof(int32_t) + sizeof(char) * bufferSize;
}

//////////////////////////////////////////////////////////////////

DICTitem::DICTitem() {}

DICTitem::DICTitem(std::string vKey, std::string vValue) {
    key.buffer   = vKey;
    value.buffer = vValue;
}

void DICTitem::write(FILE* fp) {
    key.write(fp);
    value.write(fp);
}

size_t DICTitem::getSize() { return key.getSize() + value.getSize(); }

//////////////////////////////////////////////////////////////////

DICT::DICT() { count = 0; }

void DICT::write(FILE* fp) {
    count = (int32_t)keys.size();
    fwrite(&count, sizeof(int32_t), 1, fp);
    for (int i = 0; i < count; i++)
        keys[i].write(fp);
}

size_t DICT::getSize() {
    count    = (int32_t)keys.size();
    size_t s = sizeof(int32_t);
    for (int i = 0; i < count; i++)
        s += keys[i].getSize();
    return s;
}

void DICT::Add(std::string vKey, std::string vValue) { keys.push_back(DICTitem(vKey, vValue)); }

//////////////////////////////////////////////////////////////////

nTRN::nTRN(int32_t countFrames) {
    nodeId      = 0;
    reservedId  = -1;
    childNodeId = 0;
    numFrames   = 1;
    layerId     = -1;
    numFrames   = countFrames;
    while ((int32_t)frames.size() < numFrames)
        frames.push_back(DICT());
}

void nTRN::write(FILE* fp) {
    // chunk header
    int32_t id = GetMVID('n', 'T', 'R', 'N');
    fwrite(&id, sizeof(int32_t), 1, fp);
    size_t contentSize = getSize();
    fwrite(&contentSize, sizeof(int32_t), 1, fp);
    size_t childSize = 0;
    fwrite(&childSize, sizeof(int32_t), 1, fp);

    // datas's
    fwrite(&nodeId, sizeof(int32_t), 1, fp);
    nodeAttribs.write(fp);
    fwrite(&childNodeId, sizeof(int32_t), 1, fp);
    fwrite(&reservedId, sizeof(int32_t), 1, fp);
    fwrite(&layerId, sizeof(int32_t), 1, fp);
    fwrite(&numFrames, sizeof(int32_t), 1, fp);
    for (int i = 0; i < numFrames; i++)
        frames[i].write(fp);
}

size_t nTRN::getSize() {
    size_t s = sizeof(int32_t) * 5 + nodeAttribs.getSize();
    for (int i = 0; i < numFrames; i++)
        s += frames[i].getSize();
    return s;
}

//////////////////////////////////////////////////////////////////

nGRP::nGRP(int32_t vCount) {
    nodeId            = 0;
    nodeChildrenNodes = vCount;
    while ((int32_t)childNodes.size() < nodeChildrenNodes)
        childNodes.push_back(0);
}

void nGRP::write(FILE* fp) {
    // chunk header
    int32_t id = GetMVID('n', 'G', 'R', 'P');
    fwrite(&id, sizeof(int32_t), 1, fp);
    size_t contentSize = getSize();
    fwrite(&contentSize, sizeof(int32_t), 1, fp);
    size_t childSize = 0;
    fwrite(&childSize, sizeof(int32_t), 1, fp);

    // datas's
    fwrite(&nodeId, sizeof(int32_t), 1, fp);
    nodeAttribs.write(fp);
    fwrite(&nodeChildrenNodes, sizeof(int32_t), 1, fp);
    fwrite(childNodes.data(), sizeof(int32_t), nodeChildrenNodes, fp);
}

size_t nGRP::getSize() { return sizeof(int32_t) * (2 + nodeChildrenNodes) + nodeAttribs.getSize(); }

//////////////////////////////////////////////////////////////////

MODEL::MODEL() { modelId = 0; }

void MODEL::write(FILE* fp) {
    fwrite(&modelId, sizeof(int32_t), 1, fp);
    modelAttribs.write(fp);
}

size_t MODEL::getSize() { return sizeof(int32_t) + modelAttribs.getSize(); }

//////////////////////////////////////////////////////////////////

nSHP::nSHP(int32_t vCount) {
    nodeId    = 0;
    numModels = vCount;
    models.resize(numModels);
}

void nSHP::write(FILE* fp) {
    // chunk header
    int32_t id = GetMVID('n', 'S', 'H', 'P');
    fwrite(&id, sizeof(int32_t), 1, fp);
    size_t contentSize = getSize();
    fwrite(&contentSize, sizeof(int32_t), 1, fp);
    size_t childSize = 0;
    fwrite(&childSize, sizeof(int32_t), 1, fp);

    // datas's
    fwrite(&nodeId, sizeof(int32_t), 1, fp);
    nodeAttribs.write(fp);
    fwrite(&numModels, sizeof(int32_t), 1, fp);
    for (int i = 0; i < numModels; i++)
        models[i].write(fp);
}

size_t nSHP::getSize() {
    size_t s = sizeof(int32_t) * 2 + nodeAttribs.getSize();
    for (int i = 0; i < numModels; i++)
        s += models[i].getSize();
    return s;
}

//////////////////////////////////////////////////////////////////

LAYR::LAYR() {
    nodeId     = 0;
    reservedId = -1;
}

void LAYR::write(FILE* fp) {
    // chunk header
    int32_t id = GetMVID('L', 'A', 'Y', 'R');
    fwrite(&id, sizeof(int32_t), 1, fp);
    size_t contentSize = getSize();
    fwrite(&contentSize, sizeof(int32_t), 1, fp);
    size_t childSize = 0;
    fwrite(&childSize, sizeof(int32_t), 1, fp);

    // datas's
    fwrite(&nodeId, sizeof(int32_t), 1, fp);
    nodeAttribs.write(fp);
    fwrite(&reservedId, sizeof(int32_t), 1, fp);
}

size_t LAYR::getSize() { return sizeof(int32_t) * 2 + nodeAttribs.getSize(); }

//////////////////////////////////////////////////////////////////

SIZE::SIZE() {
    sizex = 0;
    sizey = 0;
    sizez = 0;
}

void SIZE::write(FILE* fp) {
    // chunk header
    int32_t id = GetMVID('S', 'I', 'Z', 'E');
    fwrite(&id, sizeof(int32_t), 1, fp);
    size_t contentSize = getSize();
    fwrite(&contentSize, sizeof(int32_t), 1, fp);
    size_t childSize = 0;
    fwrite(&childSize, sizeof(int32_t), 1, fp);

    // datas's
    fwrite(&sizex, sizeof(int32_t), 1, fp);
    fwrite(&sizey, sizeof(int32_t), 1, fp);
    fwrite(&sizez, sizeof(int32_t), 1, fp);
}

size_t SIZE::getSize() { return sizeof(int32_t) * 3; }

//////////////////////////////////////////////////////////////////

XYZI::XYZI() { numVoxels = 0; }

void XYZI::write(FILE* fp) {
    // chunk header
    int32_t id = GetMVID('X', 'Y', 'Z', 'I');
    fwrite(&id, sizeof(int32_t), 1, fp);
    size_t contentSize = getSize();
    fwrite(&contentSize, sizeof(int32_t), 1, fp);
    size_t childSize = 0;
    fwrite(&childSize, sizeof(int32_t), 1, fp);

    // datas's
    fwrite(&numVoxels, sizeof(int32_t), 1, fp);
    fwrite(voxels.data(), sizeof(uint8_t), voxels.size(), fp);
}

size_t XYZI::getSize() {
    numVoxels = (int32_t)voxels.size() / 4;
    return sizeof(int32_t) * (1 + numVoxels);
}

//////////////////////////////////////////////////////////////////

RGBA::RGBA() {}

void RGBA::write(FILE* fp) {
    // chunk header
    int32_t id = GetMVID('R', 'G', 'B', 'A');
    fwrite(&id, sizeof(int32_t), 1, fp);
    size_t contentSize = getSize();
    fwrite(&contentSize, sizeof(int32_t), 1, fp);
    size_t childSize = 0;
    fwrite(&childSize, sizeof(int32_t), 1, fp);

    // datas's
    fwrite(colors, sizeof(uint8_t), contentSize, fp);
}

size_t RGBA::getSize() { return sizeof(uint8_t) * 4 * 256; }

//////////////////////////////////////////////////////////////////

VoxCube::VoxCube() {
    id = 0;
    tx = 0;
    ty = 0;
    tz = 0;
}

void VoxCube::write(FILE* fp) {
    for (auto& xyzi : xyzis) {
        size.write(fp);
        xyzi.second.write(fp);
    }
}

//////////////////////////////////////////////////////////////////

VoxWriter* VoxWriter::Create(const std::string& vFilePathName, const uint32_t& vLimitX, const uint32_t& vLimitY, const uint32_t& vLimitZ, int32_t* vError) {
    VoxWriter* vox = new VoxWriter(vLimitX, vLimitY, vLimitZ);

    *vError = vox->IsOk(vFilePathName);

    if (*vError == 0) {
        return vox;
    } else {
        printf("Vox file creation failed, err : %s", GetErrnoMsg(*vError).c_str());

        SAFE_DELETE(vox);
    }

    return vox;
}

std::string VoxWriter::GetErrnoMsg(const int32_t& vError) {
    std::string res;

    switch (vError) {
        case 1: res = "Operation not permitted"; break;
        case 2: res = "No such file or directory"; break;
        case 3: res = "No such process"; break;
        case 4: res = "Interrupted function"; break;
        case 5: res = "I / O error"; break;
        case 6: res = "No such device or address"; break;
        case 7: res = "Argument list too long"; break;
        case 8: res = "Exec format error"; break;
        case 9: res = "Bad file number"; break;
        case 10: res = "No spawned processes"; break;
        case 11: res = "No more processes or not enough memory or maximum nesting level reached"; break;
        case 12: res = "Not enough memory"; break;
        case 13: res = "Permission denied"; break;
        case 14: res = "Bad address"; break;
        case 16: res = "Device or resource busy"; break;
        case 17: res = "File exists"; break;
        case 18: res = "Cross - device link"; break;
        case 19: res = "No such device"; break;
        case 20: res = "Not a director"; break;
        case 21: res = "Is a directory"; break;
        case 22: res = "Invalid argument"; break;
        case 23: res = "Too many files open in system"; break;
        case 24: res = "Too many open files"; break;
        case 25: res = "Inappropriate I / O control operation"; break;
        case 27: res = "File too large"; break;
        case 28: res = "No space left on device"; break;
        case 29: res = "Invalid seek"; break;
        case 30: res = "Read - only file system"; break;
        case 31: res = "Too many links"; break;
        case 32: res = "Broken pipe"; break;
        case 33: res = "Math argument"; break;
        case 34: res = "Result too large"; break;
        case 36: res = "Resource deadlock would occur"; break;
        case 38: res = "Filename too long"; break;
        case 39: res = "No locks available"; break;
        case 40: res = "Function not supported"; break;
        case 41: res = "Directory not empty"; break;
        case 42: res = "Illegal byte sequence"; break;
        case 80: res = "String was truncated"; break;
    }

    return res;
}


//////////////////////////////////////////////////////////////////
// the limit of magicavoxel is 127 for one cube, is 127 voxels (indexs : 0 -> 126)
// vMaxVoxelPerCubeX,Y,Z define the limit of one cube
VoxWriter::VoxWriter(const VoxelX& vMaxVoxelPerCubeX, const VoxelY& vMaxVoxelPerCubeY, const VoxelZ& vMaxVoxelPerCubeZ) {
    // the limit of magicavoxel is 127 because the first voxel is 1 not 0
    // so this is 0 to 126
    // index limit, size is 127
    m_MaxVoxelPerCubeX = ct::clamp<int32_t>(vMaxVoxelPerCubeX, 0, 126);
    m_MaxVoxelPerCubeY = ct::clamp<int32_t>(vMaxVoxelPerCubeY, 0, 126);
    m_MaxVoxelPerCubeZ = ct::clamp<int32_t>(vMaxVoxelPerCubeZ, 0, 126);

    SetKeyFrame(0U);
}

VoxWriter::~VoxWriter() {}

int32_t VoxWriter::IsOk(const std::string& vFilePathName) {
    if (OpenFileForWriting(vFilePathName)) {
        CloseFile();
    }
    return lastError;
}

void VoxWriter::ClearVoxels() {
    cubes.clear();
    cubesId.clear();
    voxelId.clear();
}

void VoxWriter::ClearColors() { colors.clear(); }

void VoxWriter::SetKeyFrame(uint32_t vKeyFrame) { 
    m_KeyFrame = vKeyFrame;
}

void VoxWriter::AddColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a, const uint8_t& index) {
    while (colors.size() <= index)
        colors.push_back(0);
    colors[index] = GetID(r, g, b, a);
}

void VoxWriter::AddVoxel(const int32_t& vX, const int32_t& vY, const int32_t& vZ, const uint8_t& vColorIndex) {
    // cube pos
    int32_t ox = (int32_t)std::floor((double)vX / (double)m_MaxVoxelPerCubeX);
    int32_t oy = (int32_t)std::floor((double)vY / (double)m_MaxVoxelPerCubeY);
    int32_t oz = (int32_t)std::floor((double)vZ / (double)m_MaxVoxelPerCubeZ);

    minCubeX = ct::mini<int32_t>(minCubeX, ox);
    minCubeY = ct::mini<int32_t>(minCubeX, oy);
    minCubeZ = ct::mini<int32_t>(minCubeX, oz);

    auto cube = GetCube(ox, oy, oz);

    MergeVoxelInCube(vX, vY, vZ, vColorIndex, cube);
}

void VoxWriter::SaveToFile(const std::string& vFilePathName) {
    if (OpenFileForWriting(vFilePathName)) {
        int32_t zero = 0;

        fwrite(&ID_VOX, sizeof(int32_t), 1, m_File);
        fwrite(&MV_VERSION, sizeof(int32_t), 1, m_File);

        // MAIN CHUNCK
        fwrite(&ID_MAIN, sizeof(int32_t), 1, m_File);
        fwrite(&zero, sizeof(int32_t), 1, m_File);

        long numBytesMainChunkPos = GetFilePos();
        fwrite(&zero, sizeof(int32_t), 1, m_File);

        long headerSize = GetFilePos();

        int32_t count   = (int32_t)cubes.size();

        //////////////////////////////////////
        /// FIRST STEP : WRITE ALL CUBES /////
        //////////////////////////////////////

        for (auto& cube_key : cubes) {
            for (auto& key_frame : cube_key) {
                key_frame.second.write(m_File);
            }
        }

        //////////////////////////////////////
        /// SECOND STEP : WRITE ROOT GROUP ///
        //////////////////////////////////////

        nTRN rootTransform(1);
        rootTransform.nodeId      = 0;
        rootTransform.childNodeId = 1;

        nGRP rootGroup(count);
        rootGroup.nodeId            = 1;
        rootGroup.nodeChildrenNodes = count;

        int32_t cube_idx = 0;
        for (auto& cube_key : cubes) {
            rootGroup.childNodes[cube_idx] = 2 + cube_idx * 2;
            ++cube_idx;
        }

        rootTransform.write(m_File);
        rootGroup.write(m_File);

        //////////////////////////////////////
        /// THIRD STEP : WRITE SHAPES ////////
        //////////////////////////////////////

        std::vector<nSHP> shapes;
        std::vector<nTRN> shapeTransforms;
        int32_t           nodeIds  = 1;
        size_t            cube_key_idx = 0U;
        for (auto& cube_key : cubes) {

            // cube trans
            auto& first_cube = cube_key.begin()->second;
            nTRN  trans(1);  // no trans anim so one frame only
            trans.nodeId                   = ++nodeIds;
            trans.childNodeId              = ++nodeIds;
            trans.layerId                  = 0;
            first_cube.tx                  = (int)std::floor((first_cube.tx - minCubeX + 0.5f) * m_MaxVoxelPerCubeX - maxVolume.lowerBound.x - maxVolume.Size().x * 0.5);
            first_cube.ty                  = (int)std::floor((first_cube.ty - minCubeY + 0.5f) * m_MaxVoxelPerCubeY - maxVolume.lowerBound.y - maxVolume.Size().y * 0.5);
            first_cube.tz                  = (int)std::floor((first_cube.tz - minCubeZ + 0.5f) * m_MaxVoxelPerCubeZ);
            trans.frames[0].Add("_t", ct::toStr(first_cube.tx) + " " + ct::toStr(first_cube.ty) + " " + ct::toStr(first_cube.tz));
            shapeTransforms.push_back(trans);


            // one model per key frame
            nSHP shape(cube_key.size());
            shape.nodeId     = nodeIds;
            size_t model_idx = 0U;
            for (auto& key_frame : cube_key) {
                shape.models[model_idx].modelId = cube_key_idx + model_idx;
                shape.models[model_idx].modelAttribs.Add("_f", ct::toStr(key_frame.first));
                ++model_idx;
            }
            shapes.push_back(shape);

            ++cube_key_idx;
        }

        // write transforms and shapes
        for (int i = 0; i < count; i++) {
            shapeTransforms[i].write(m_File);
            shapes[i].write(m_File);
        }

        // no layr in my cases

        // layr
        /*for (int i = 0; i < 8; i++)
        {
            LAYR layr;
            layr.nodeId = i;
            layr.nodeAttribs.Add("_name", ct::toStr(i));
            layr.write(m_File);
        }*/

        // RGBA Palette
        if (colors.size() > 0) {
            RGBA palette;
            for (int32_t i = 0; i < 255; i++) {
                if (i < (int32_t)colors.size()) {
                    palette.colors[i] = colors[i];
                } else {
                    palette.colors[i] = 0;
                }
            }

            palette.write(m_File);
        }

        const long mainChildChunkSize = GetFilePos() - headerSize;
        SetFilePos(numBytesMainChunkPos);
        uint32_t size = (uint32_t)mainChildChunkSize;
        fwrite(&size, sizeof(uint32_t), 1, m_File);

        CloseFile();
    }
}

const size_t VoxWriter::GetVoxelsCount(const KeyFrame& vKeyFrame) const {
    size_t voxel_count = 0U;
    for (const auto& cube : cubes) {
        if (cube.xyzis.find(vKeyFrame) != cube.xyzis.end()) {
            voxel_count += cube.xyzis.at(vKeyFrame).numVoxels;
        }
    }
    return voxel_count;
}

const size_t VoxWriter::GetVoxelsCount() const {
    size_t voxel_count = 0U;
    for (const auto& cube : cubes) {
        for (auto& key_xyzi : cube.xyzis) {
            voxel_count += key_xyzi.second.numVoxels;
        }
    }
    return voxel_count;
}

const size_t VoxWriter::GetKeyFramesCount() const {
    return cubes.size();
}

void VoxWriter::PrintStats() const {
    std::cout << "---- Stats -----" << std::endl;
    std::cout << "Volume : " << maxVolume.Size().x << " x " << maxVolume.Size().y << " x " << maxVolume.Size().z << std::endl;
    std::cout << "count cubes : " << cubes.size() << std::endl;
    std::cout << "count key frames : " << GetKeyFramesCount() << std::endl;
    std::cout << "----------------" << std::endl;
    size_t voxels_total = 0U;
    for (const auto& cube : cubes) {
        for (auto& key_xyzi : cube.xyzis) {
            std::cout << "-|-> key frame : " << key_xyzi.first << std::endl;
            const auto& voxels_count = key_xyzi.second.numVoxels;
            std::cout << " |-> voxels count : " << voxels_count << std::endl;
            voxels_total += voxels_count;
        }
    }
    std::cout << "----------------" << std::endl;
    std::cout << "voxels total : " << voxels_total << std::endl;
    std::cout << "----------------" << std::endl;
}

bool VoxWriter::OpenFileForWriting(const std::string& vFilePathName) {
#if _MSC_VER
    lastError = fopen_s(&m_File, vFilePathName.c_str(), "wb");
#else
    m_File    = fopen(vFilePathName.c_str(), "wb");
    lastError = m_File ? 0 : errno;
#endif
    if (lastError != 0)
        return false;
    return true;
}

void VoxWriter::CloseFile() { fclose(m_File); }

long VoxWriter::GetFilePos() const { return ftell(m_File); }

void VoxWriter::SetFilePos(const long& vPos) {
    //  SEEK_SET	Beginning of file
    //  SEEK_CUR	Current position of the file pointer
    //	SEEK_END	End of file
    fseek(m_File, vPos, SEEK_SET);
}

const int32_t VoxWriter::GetCubeId(const int32_t& vX, const int32_t& vY, const int32_t& vZ) {
    if (cubesId.find(vX) != cubesId.end()) {
        if (cubesId[vX].find(vY) != cubesId[vX].end()) {
            if (cubesId[vX][vY].find(vZ) != cubesId[vX][vY].end()) {
                return cubesId[vX][vY][vZ];
            }
        }
    }

    cubesId[vX][vY][vZ] = maxCubeId++;

    return cubesId[vX][vY][vZ];
}

// Wrap a position inside a particular cube dimension
inline uint8_t Wrap(int v, int lim) {
    v = v % lim;
    if (v < 0) {
        v += lim;
    }
    return (uint8_t)v;
}

void VoxWriter::MergeVoxelInCube(const int32_t& vX, const int32_t& vY, const int32_t& vZ, const uint8_t& vColorIndex, VoxCube* vCube) {
    maxVolume.Combine(ct::dvec3((double)vX, (double)vY, (double)vZ));

    bool exist = false;

    if (voxelId.find(vX) != voxelId.end()) {
        if (voxelId[vX].find(vY) != voxelId[vX].end()) {
            if (voxelId[vX][vY].find(vZ) != voxelId[vX][vY].end()) {
                exist = true;
            }
        }
    }

    if (exist == false) {
        auto& xyzi = vCube->xyzis[m_KeyFrame];
        xyzi.voxels.push_back(Wrap(vX, m_MaxVoxelPerCubeX));                      // x
        xyzi.voxels.push_back(Wrap(vY, m_MaxVoxelPerCubeY));                      // y
        xyzi.voxels.push_back(Wrap(vZ, m_MaxVoxelPerCubeZ));                      // z

        // correspond a la loc de la couleur du voxel en question
        voxelId[vX][vY][vZ] = (int)xyzi.voxels.size();

        xyzi.voxels.push_back(vColorIndex);  // color index
    }
}

VoxCube* VoxWriter::GetCube(const int32_t& vX, const int32_t& vY, const int32_t& vZ) {
    int32_t id = GetCubeId(vX, vY, vZ);

    if (id == cubes.size()) {
        VoxCube c;

        c.id = id;

        c.tx = vX;
        c.ty = vY;
        c.tz = vZ;

        c.size.sizex = m_MaxVoxelPerCubeX;
        c.size.sizey = m_MaxVoxelPerCubeY;
        c.size.sizez = m_MaxVoxelPerCubeZ;

        cubes.push_back(c);
    }

    if (id < (int32_t)cubes.size()) {
        return &cubes[id];
    }

    return nullptr;
}

}  // namespace vox
