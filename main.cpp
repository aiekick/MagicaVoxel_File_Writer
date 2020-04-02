#include "VoxWriter.h"

// example from @unphased on this topic : https://github.com/aiekick/MagicaVoxel_File_Writer/issues/2

int main() 
{
	vox::VoxWriter vox;
	for (int i = 0; i < 1000; ++i) {
		for (int j = 0; j < 1000; ++j) {
			vox.AddVoxel(i, j, floor(sin((float)(i * i + j * j) / 50000) * 150) + 150, (i + j) % 255 + 1);
		}
	}

	vox.SaveToFile("output_voxwriter.vox");
}