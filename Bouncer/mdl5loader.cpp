#include "mdl5loader.h"

MDL5::MDL5(char* file)
{

	FILE *fp;
	int tmp_size;
	int i;
	valid = false;

	fp = fopen(file, "rb");
	fread(&header, sizeof(mdl_header), 1, fp);

	//check for correct version
	if (!strcmp("MDL5", header.version))
	{
		//allocate memory for all skins
		skin = (mdl5_skin_t*)malloc(sizeof(mdl5_skin_t) * header.numskins);
		//cycle through all skins and allocate data
		for (i = 0; i < header.numskins; i++)
		{
			//read basic skin data (type, width, height) -> 3 * long
			fread((&skin[i]), sizeof(long) * 3, 1, fp);

			//skin size depends on color depth and mode (bpp = bytes per pixel)
			switch(skin[i].skintype)
			{
			case 0:		//indexed 8 bit
				skin[i].bpp = 1;
				break;

			case 4:	//888
			case 12:	//888 mip mapped
				skin[i].bpp = 3;
				break;

			case 5:	//8888
			case 13:	//8888 mip mapped
				skin[i].bpp = 4;
				break;

			default:	//assume 565 or 4444 (both with optional mip maps)
				skin[i].bpp = 2;
				break;
			}

			//main skin
			tmp_size = skin[i].bpp * skin[i].width * skin[i].height; 
			skin[i].skin = (byte*)malloc(sizeof(byte) * tmp_size);
			fread(skin[i].skin, sizeof(byte) * tmp_size, 1, fp);

			//mip maps only for 16/32Bit color depth
			if (skin[i].skintype > 8)
			{
				tmp_size = skin[i].bpp * skin[i].width / 2 * skin[i].height / 2; 
				skin[i].skin1 = (byte*)malloc(sizeof(byte) * tmp_size);
				fread(skin[i].skin1, sizeof(byte) * tmp_size, 1, fp);

				tmp_size = skin[i].bpp * (skin[i].width) / 4 * (skin[i].height / 4); 
				skin[i].skin2 = (byte*)malloc(sizeof(byte) * tmp_size);
				fread(skin[i].skin2, sizeof(byte) * tmp_size, 1, fp);

				tmp_size = skin[i].bpp * (skin[i].width / 8) * (skin[i].height / 8); 
				skin[i].skin3 = (byte*)malloc(sizeof(byte) * tmp_size);
				fread(skin[i].skin3, sizeof(byte) * tmp_size, 1, fp);
			}
		}

		//load skin vertices
		skinvertex = (mdl_uvvert_t*)malloc(sizeof(mdl_uvvert_t) * header.numskinverts);
		fread(skinvertex, sizeof(mdl_uvvert_t) * header.numskinverts, 1, fp);

		//load mesh triangles
		triangle = (mdl_triangle_t*)malloc(sizeof(mdl_triangle_t) * header.numtris);
		fread(triangle, sizeof(mdl_triangle_t) * header.numtris, 1, fp);

		//load animation frames
		//only type 2 - short packed is implemented for now
		//read basic frame data (type, bounding box min/max, name) -> 1 long, 2 mdl_trivertxs_t, 16 char
		tmp_size = sizeof(long) + 16 * sizeof(char) + 2* sizeof(mdl_trivertxs_t);
		frame = (mdl_frame_t*)malloc(sizeof(mdl_frame_t) * header.numframes);
		for (i = 0; i < header.numframes; i++)
		{
			fread(&frame[i], tmp_size, 1, fp);
			//allocate vertex buffer for frame
			frame[i].vertex = (mdl_trivertxs_t*)malloc(sizeof(mdl_trivertxs_t) * header.numverts);
			fread(frame[i].vertex, sizeof(mdl_trivertxs_t) * header.numverts, 1, fp);
		}
		valid = true;
	}
	fclose(fp);

}

MDL5::~MDL5()
{
	int i;
	if (!strcmp("MDL5", header.version))
	{
		//deallocation
		for (i = 0; i < header.numframes; i++)
			free(frame[i].vertex);
		free(frame);
		free(triangle);
		free(skinvertex);

		for (i = 0; i < header.numskins; i++)
		{
			free(skin[i].skin);
			//check for mip maps
			if (skin[i].skintype > 8)
			{
				free(skin[i].skin1);
				free(skin[i].skin2);
				free(skin[i].skin3);
			}
		}
		free(skin);
	}
}

int MDL5::getFrames()
{
	if (valid)
		return header.numframes;
	else
		return 0;
}

int MDL5::getSkins()
{
	if (valid)
		return header.numskins;
	else
		return 0;
}


void MDL5::loadMesh(DWORD* memPtr, int frm)
{
	//get pointer to vertex data
	mdl_trivertxs_t* vertex = frame[frm - 1].vertex; 

	//pointer for float data
	float* fmemPtr = (float*)memPtr;

	//header info
	memPtr[0] = 274;	//FVF format
	memPtr[1] =  32;	//Vertex size in bytes
	memPtr[2] = header.numtris * 3;	//each triangle has 3 vertices

	//vertex data
	int offset, vi;	//addresses
	int v, i;	//loop counters
	int vIndex;	//vertex index

	for (i = 0; i < header.numtris; i++)
	{
		//base address calculation for each vertex: 32 byte --> 8 DWORDs per vertex
		//each triangle has 3 vertices --> 32 byte * 3
		offset = 3 + i * (3 * memPtr[1] / sizeof(DWORD));
		
		//read all three vertices of current triangle
		for (v = 0; v < 3; v++)
		{
			//calculate address offset for each vertex of the current triangle
			vi = v * (memPtr[1] / sizeof(DWORD));

			//get index of 3d vertex
			vIndex = triangle[i].index_xyz[v];

			//write vertex with scale and offset taken into account
			fmemPtr[offset + vi + 0] = ((vertex[vIndex].rawposition[0] * header.scale[0]) + header.offset[0]) * MDL_SCALEFAC; 
			fmemPtr[offset + vi + 1] = ((vertex[vIndex].rawposition[2] * header.scale[2]) + header.offset[2]) * MDL_SCALEFAC; 
			fmemPtr[offset + vi + 2] = ((vertex[vIndex].rawposition[1] * header.scale[1]) + header.offset[1]) * MDL_SCALEFAC; 

			//normal dir (taken from 162 predefined lightnormals)
			fmemPtr[offset + vi + 3] = lightnormals[vertex[vIndex].lightnormalindex][0];
			fmemPtr[offset + vi + 4] = lightnormals[vertex[vIndex].lightnormalindex][2];
			fmemPtr[offset + vi + 5] = lightnormals[vertex[vIndex].lightnormalindex][1];

			//get index of uv vertex
			vIndex = triangle[i].index_uv[v];

			//uv pos - normalized to 0..1
			fmemPtr[offset + vi + 6] = (float)skinvertex[vIndex].u / skin[0].width;
			fmemPtr[offset + vi + 7] = (float)skinvertex[vIndex].v / skin[0].height;
		}

	}
}

void MDL5::loadSkin(DWORD* memPtr, int s)
{

	s--;	//skin counter internally starts at 0, externally at 1
	byte* bmemPtr = (byte*)memPtr;
	int i;	//loop counter

	memPtr[0] = skin[s].width;
	memPtr[1] = skin[s].height;
	memPtr[2] = 32;

	int offset = sizeof(DWORD) * 3;
	byte color[4];
	//currently only 565/8888 color formats are converted!!
	for (i = 0; i < skin[s].bpp * skin[s].width * skin[s].height; i += skin[s].bpp)
	{
		switch(skin[s].skintype)
		{
		case 2:	//565 (RGB)
		case 10:	//565 mip mapped
			color[0] = (byte)(((skin[s].skin[i+1]>>3) & 0x1f) / (float)0x1f * (float)0xff);
			color[1] = (byte)((((skin[s].skin[i+1] & 0x7)<<3) | ((skin[s].skin[i]>>5) & 0x7)) / (float)0x3f * (float)0xff);
			color[2] = (byte)((skin[s].skin[i] & 0x1f) / (float)0x1f * (float)0xff);
			color[3] = 0xFF;
			break;

		case 3:	//4444 (untested, assumed RGBA)
		case 11:	//4444 mip mapped
			color[0] = skin[s].skin[i + 1] & 0xF0;
			color[1] = (skin[s].skin[i + 1] << 4) & 0xF0;
			color[2] = skin[s].skin[i] & 0xF0;
			color[3] = (skin[s].skin[i] << 4) & 0xF0;

		case 4:	//888 (RGB)
		case 12:	//888 mip mapped 
			color[0] = skin[s].skin[i + 2];
			color[1] = skin[s].skin[i + 1];
			color[2] = skin[s].skin[i + 0];
			color[3] = 0xFF;
			break;

		case 5:	//8888 (BGRA)
		case 13:	//8888 mip mapped
			color[0] = skin[s].skin[i];
			color[1] = skin[s].skin[i + 1];
			color[2] = skin[s].skin[i + 2];
			color[3] = skin[s].skin[i + 3];
			break;

		default:	//cannot convert - black image
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			color[3] = 0xFF;
			break;
		}

		bmemPtr[offset + 0] = color[2];	
		bmemPtr[offset + 1] = color[1];	
		bmemPtr[offset + 2] = color[0];	
		bmemPtr[offset + 3] = color[3];	
		offset += 4 * sizeof(byte);	//advance 4 bytes
	}
}

int MDL5::getMeshSize()
{
	if (valid)
		return (sizeof(DWORD) * 8) * 3 * header.numtris + (3 * sizeof(DWORD));
	else
		return 0;
}

int MDL5::getSkinSize(int s)
{
	s--;
	if (valid)
		return sizeof(DWORD) * skin[s].width * skin[s].height + (3 * sizeof(DWORD));
	else
		return 0;
}

//global functions
void dbLoadGsObject(char* file, int obj, int skin, int frame)
{
	DWORD* mb;
	MDL5* mdl = new MDL5(file);

	dbMakeMemblock(env.memblock_ctr, mdl->getMeshSize());
	mb = (DWORD*)dbGetMemblockPtr (env.memblock_ctr);
	mdl->loadMesh(mb, frame);
	dbMakeMeshFromMemblock(env.mesh_ctr, env.memblock_ctr);
	env.memblock_ctr++;

	int x = mdl->getSkinSize();
	dbMakeMemblock(env.memblock_ctr, mdl->getSkinSize(skin));
	mb = (DWORD*)dbGetMemblockPtr (env.memblock_ctr);
	mdl->loadSkin(mb, skin);
	dbMakeImageFromMemblock(env.image_ctr, env.memblock_ctr);

	dbMakeObject (obj, env.mesh_ctr, env.image_ctr);
	dbSetObjectTransparency (obj, 1);
	dbSetObjectTexture (obj, 0, 1);
	env.image_ctr++;

	//clean up
	dbDeleteMesh(env.mesh_ctr);
	dbDeleteMemblock(env.memblock_ctr);
	env.memblock_ctr--;
	dbDeleteMemblock(env.memblock_ctr);
	delete mdl;

}
