#include "VoxWriter.h"

// example from @unphased on this topic : https://github.com/aiekick/MagicaVoxel_File_Writer/issues/2

int main() 
{
	vox::VoxWriter vox;
	for(int f = 0; f < 3; f++)
	{
		for (int i = 0; i < 126; ++i)
		{
			for (int j = 0; j < 126; ++j)
			{
				float z = sinf(i * j * 1e-3f + f);
				vox.AddVoxel(i, j, (int)std::floor(z * 126) + 126, (i + j) % 255 + 1, f);
			}
		}
	}
	
	vox.SaveToFile("output_voxwriter.vox");
}