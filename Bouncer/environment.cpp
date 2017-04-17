#include "environment.h"


Environment::Environment()
{
	//must not contain any Dark GDK stuff!!
	object_ctr = 1;
	image_ctr = 1;
	sprite_ctr = 1;
	mesh_ctr = 1;
	memblock_ctr = 1;
	particle_ctr = 1;
	sound_ctr = 1;
	music_ctr = 1;
	syncRate = 30;
	timeStep = 1.0f / (float)syncRate;

	wiimote_active = false;
}

Environment::~Environment()
{
	//must not contain any Dark GDK stuff!!
	//remove Wiimote Object
	wm->Disconnect();
	delete wm;
}

void Environment::init()
{
	//Wiimote activation
	wm = new gdkWiiMote;
	if (wm->GetDevices()>0)
	{
		if (wm->ConnectToDevice(0))
		{
			wiimote_active = true;
		}
	}
/*
	else
		MessageBox( NULL, "No Wiimote device found", "info", MB_OK );
*/
	//initialize Newton Physics
	NewtonInit();

	//initialize DarkBasic
	dbInit();
}

void Environment::cleanUp()
{
	int i;

	//delete all allocated entities

	for (i = 1; i < object_ctr; i++)
		dbDeleteObject(i);

	for (i = 1; i < sprite_ctr; i++)
		dbDeleteSprite(i);

	for (i = 1; i < mesh_ctr; i++)
		dbDeleteMesh(i);

	for (i = 1; i < memblock_ctr; i++)
		dbDeleteMemblock(i);

	for (i = 1; i < particle_ctr; i++)
	{
		// non emitting particle generators would cause crash here
		dbSetParticleEmissions (i, 1);
		dbDeleteParticles(i);
	}

	for (i = 1; i < image_ctr; i++)
		dbDeleteImage(i);

	for (i = 1; i < sound_ctr; i++)
		dbDeleteSound(i);

	for (i = 1; i < music_ctr; i++)
		dbDeleteMusic(i);

}
//global instance of environment
Environment env;

