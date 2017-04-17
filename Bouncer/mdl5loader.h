#ifndef MDL5LOADER_H
#define MDL5LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <DarkGDK.h>

#include "environment.h"

//struct definitions and lightnormals taken from Acknex Gamestudio Manual
typedef float vec3[3];
#define byte unsigned char
#define word unsigned short

#define MDL_SCALEFAC	0.1f

//file header
typedef struct {
  char version[4]; // "MDL3", "MDL4", or "MDL5"
  long unused1;  // not used
  vec3 scale;    // 3D position scale factors.
  vec3 offset;   // 3D position offset.
  long unused2;  // not used
  vec3 unused3;  // not used
  long numskins; // number of skin textures
  long skinwidth; // width of skin texture, for MDL3 and MDL4;
  long skinheight; // height of skin texture, for MDL3 and MDL4;
  long numverts; // number of 3d wireframe vertices
  long numtris;  // number of triangles surfaces
  long numframes; // number of frames
  long numskinverts; // number of 2D skin vertices
  long flags;    // always 0
  long unused4;  // not used
} mdl_header;

//skins
typedef struct {
  long skintype; // 2 for 565 RGB, 3 for 4444 ARGB, 10 for 565 mipmapped, 11 for 4444 mipmapped (bpp = 2),
                 // 12 for 888 RGB mipmapped (bpp = 3), 13 for 8888 ARGB mipmapped (bpp = 4)
  long width,height; // size of the texture
  int bpp;	//bits per pixel
  byte* skin; // the texture image
  byte* skin1; // the 1st mipmap (if any)
  byte* skin2; // the 2nd mipmap (if any)
  byte* skin3; // the 3rd mipmap (if any)
} mdl5_skin_t;

//skin vertices
typedef struct
{
  short u; // position, horizontally in range 0..skinwidth-1
  short v; // position, vertically in range 0..skinheight-1
} mdl_uvvert_t;

//triangles
typedef struct {
  short index_xyz[3]; // Index of 3 3D vertices in range 0..numverts
  short index_uv[3]; // Index of 3 skin vertices in range 0..numskinverts
} mdl_triangle_t;

typedef struct {
	byte rawposition[3]; // X,Y,Z coordinate, packed on 0..255
	byte lightnormalindex; // index of the vertex normal
} mdl_trivertxb_t;

typedef struct {
	unsigned short rawposition[3]; // X,Y,Z coordinate, packed on 0..65536
	byte lightnormalindex; // index of the vertex normal
	byte unused;
} mdl_trivertxs_t;

typedef struct {
	long type; // 0 for byte-packed positions, and 2 for word-packed positions
	mdl_trivertxs_t bboxmin,bboxmax; // bounding box of the frame
	char name[16]; // name of frame, used for animation
	mdl_trivertxs_t* vertex; // array of vertices, either byte or short packed
} mdl_frame_t;


//class definition
class MDL5
{
public:
	MDL5(char* file);
	~MDL5();
	void loadMesh(DWORD* memPtr, int frame = 1);
	void loadSkin(DWORD* memPtr, int skin = 1);
	int getSkins();
	int getFrames();
	int getMeshSize();
	int getSkinSize(int skin = 1);

private:
	mdl_header header;

	mdl5_skin_t* skin;
	mdl_uvvert_t* skinvertex;
	mdl_triangle_t* triangle;
	mdl_frame_t* frame;

	bool valid;	//indicator whether valid file was loaded
};


//global definitions
const float lightnormals[162][3] = {
	{-0.525725, 0.000000, 0.850650}, {-0.442863, 0.238856, 0.864188}, {-0.295242, 0.000000, 0.955423},
	{-0.309017, 0.500000, 0.809017}, {-0.162460, 0.262866, 0.951056}, {0.000000, 0.000000, 1.000000},
	{0.000000, 0.850651, 0.525731}, {-0.147621, 0.716567, 0.681718}, {0.147621, 0.716567, 0.681718},
	{0.000000, 0.525731, 0.850651}, {0.309017, 0.500000, 0.809017}, {0.525731, 0.000000, 0.850651},
	{0.295242, 0.000000, 0.955423}, {0.442863, 0.238856, 0.864188}, {0.162460, 0.262866, 0.951056},
	{-0.681718, 0.147621, 0.716567}, {-0.809017, 0.309017, 0.500000}, {-0.587785, 0.425325, 0.688191},
	{-0.850651, 0.525731, 0.000000}, {-0.864188, 0.442863, 0.238856}, {-0.716567, 0.681718, 0.147621},
	{-0.688191, 0.587785, 0.425325}, {-0.500000, 0.809017, 0.309017}, {-0.238856, 0.864188, 0.442863},
	{-0.425325, 0.688191, 0.587785}, {-0.716567, 0.681718, -0.147621}, {-0.500000, 0.809017, -0.309017},
	{-0.525731, 0.850651, 0.000000}, {0.000000, 0.850651, -0.525731}, {-0.238856, 0.864188, -0.442863},
	{0.000000, 0.955423, -0.295242}, {-0.262866, 0.951056, -0.162460}, {0.000000, 1.000000, 0.000000},
	{0.000000, 0.955423, 0.295242}, {-0.262866, 0.951056, 0.162460}, {0.238856, 0.864188, 0.442863},
	{0.262866, 0.951056, 0.162460}, {0.500000, 0.809017, 0.309017}, {0.238856, 0.864188, -0.442863},
	{0.262866, 0.951056, -0.162460}, {0.500000, 0.809017, -0.309017}, {0.850651, 0.525731, 0.000000},
	{0.716567, 0.681718, 0.147621}, {0.716567, 0.681718, -0.147621}, {0.525731, 0.850651, 0.000000},
	{0.425325, 0.688191, 0.587785}, {0.864188, 0.442863, 0.238856}, {0.688191, 0.587785, 0.425325},
	{0.809017, 0.309017, 0.500000}, {0.681718, 0.147621, 0.716567}, {0.587785, 0.425325, 0.688191},
	{0.955423, 0.295242, 0.000000}, {1.000000, 0.000000, 0.000000}, {0.951056, 0.162460, 0.262866},
	{0.850651, -0.525731, 0.000000}, {0.955423, -0.295242, 0.000000}, {0.864188, -0.442863, 0.238856},
	{0.951056, -0.162460, 0.262866}, {0.809017, -0.309017, 0.500000}, {0.681718, -0.147621, 0.716567},
	{0.850651, 0.000000, 0.525731}, {0.864188, 0.442863, -0.238856}, {0.809017, 0.309017, -0.500000},
	{0.951056, 0.162460, -0.262866}, {0.525731, 0.000000, -0.850651}, {0.681718, 0.147621, -0.716567},
	{0.681718, -0.147621, -0.716567}, {0.850651, 0.000000, -0.525731}, {0.809017, -0.309017, -0.500000},
	{0.864188, -0.442863, -0.238856}, {0.951056, -0.162460, -0.262866}, {0.147621, 0.716567, -0.681718},
	{0.309017, 0.500000, -0.809017}, {0.425325, 0.688191, -0.587785}, {0.442863, 0.238856, -0.864188},
	{0.587785, 0.425325, -0.688191}, {0.688197, 0.587780, -0.425327}, {-0.147621, 0.716567, -0.681718},
	{-0.309017, 0.500000, -0.809017}, {0.000000, 0.525731, -0.850651}, {-0.525731, 0.000000, -0.850651},
	{-0.442863, 0.238856, -0.864188}, {-0.295242, 0.000000, -0.955423}, {-0.162460, 0.262866, -0.951056},
	{0.000000, 0.000000, -1.000000}, {0.295242, 0.000000, -0.955423}, {0.162460, 0.262866, -0.951056},
	{-0.442863,-0.238856, -0.864188}, {-0.309017,-0.500000, -0.809017}, {-0.162460, -0.262866, -0.951056},
	{0.000000, -0.850651, -0.525731}, {-0.147621, -0.716567, -0.681718}, {0.147621, -0.716567, -0.681718},
	{0.000000, -0.525731, -0.850651}, {0.309017, -0.500000, -0.809017}, {0.442863, -0.238856, -0.864188},
	{0.162460, -0.262866, -0.951056}, {0.238856, -0.864188, -0.442863}, {0.500000, -0.809017, -0.309017},
	{0.425325, -0.688191, -0.587785}, {0.716567, -0.681718, -0.147621}, {0.688191, -0.587785, -0.425325},
	{0.587785, -0.425325, -0.688191}, {0.000000, -0.955423, -0.295242}, {0.000000, -1.000000, 0.000000},
	{0.262866, -0.951056, -0.162460}, {0.000000, -0.850651, 0.525731}, {0.000000, -0.955423, 0.295242},
	{0.238856, -0.864188, 0.442863}, {0.262866, -0.951056, 0.162460}, {0.500000, -0.809017, 0.309017},
	{0.716567, -0.681718, 0.147621}, {0.525731, -0.850651, 0.000000}, {-0.238856, -0.864188, -0.442863},
	{-0.500000, -0.809017, -0.309017}, {-0.262866, -0.951056, -0.162460}, {-0.850651, -0.525731, 0.000000},
	{-0.716567, -0.681718, -0.147621}, {-0.716567, -0.681718, 0.147621}, {-0.525731, -0.850651, 0.000000},
	{-0.500000, -0.809017, 0.309017}, {-0.238856, -0.864188, 0.442863}, {-0.262866, -0.951056, 0.162460},
	{-0.864188, -0.442863, 0.238856}, {-0.809017, -0.309017, 0.500000}, {-0.688191, -0.587785, 0.425325},
	{-0.681718, -0.147621, 0.716567}, {-0.442863, -0.238856, 0.864188}, {-0.587785, -0.425325, 0.688191},
	{-0.309017, -0.500000, 0.809017}, {-0.147621, -0.716567, 0.681718}, {-0.425325, -0.688191, 0.587785},
	{-0.162460, -0.262866, 0.951056}, {0.442863, -0.238856, 0.864188}, {0.162460, -0.262866, 0.951056},
	{0.309017, -0.500000, 0.809017}, {0.147621, -0.716567, 0.681718}, {0.000000, -0.525731, 0.850651},
	{0.425325, -0.688191, 0.587785}, {0.587785, -0.425325, 0.688191}, {0.688191, -0.587785, 0.425325},
	{-0.955423, 0.295242, 0.000000}, {-0.951056, 0.162460, 0.262866}, {-1.000000, 0.000000, 0.000000},
	{-0.850651, 0.000000, 0.525731}, {-0.955423, -0.295242, 0.000000}, {-0.951056, -0.162460, 0.262866},
	{-0.864188, 0.442863, -0.238856}, {-0.951056, 0.162460, -0.262866}, {-0.809017, 0.309017, -0.500000},
	{-0.864188,-0.442863, -0.238856}, {-0.951056,-0.162460, -0.262866}, {-0.809017, -0.309017, -0.500000},
	{-0.681718, 0.147621, -0.716567}, {-0.681718, -0.147621, -0.716567}, {-0.850651, 0.000000, -0.525731},
	{-0.688191, 0.587785, -0.425325}, {-0.587785, 0.425325, -0.688191}, {-0.425325, 0.688191, -0.587785},
	{-0.425325,-0.688191, -0.587785}, {-0.587785,-0.425325, -0.688191}, {-0.688197,-0.587780, -0.425327}
}; 

//prototypes
void dbLoadGsObject(char* file, int obj, int skin = 1, int frame = 1);
#endif