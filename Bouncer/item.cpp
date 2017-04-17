#include "item.h"

Item::Item()
{
	visible = NULL;
	maxItems = 0;
	objOffset = 0;
	parOffset = 0;
	timer = 0.0f;
	collectedItems = 0;
	parTimer = 0;
	
	initialized = false;
}

Item::~Item()
{
	if (visible != NULL)
		free (visible);
}


void Item::init()
{
	timer = 0.0f;
	collectedItems = 0;
	parTimer = 0;
	int i;

	if (!initialized)
	{
		initialized = true;

		dbLoadSound ("collect.wav", env.sound_ctr);
		sndCollect = env.sound_ctr;
		env.sound_ctr++;

		parOffset = env.particle_ctr;
		dbLoadImage("particle1.bmp", env.image_ctr);
		int parImage = env.image_ctr;
		env.image_ctr++;

		//master object - all other items will be cloned from this one
		dbLoadGsObject ("item.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oItem = env.object_ctr;
		env.object_ctr++;

		objOffset = env.object_ctr;


		int items = 0;
		float fTemp[3];

		dbOpenToRead(1, "itemdata.dat");
		dbReadLong(1, &items);
	
		for (i = 0; i < items && i < ITEM_MAX_NUMBER; i++)
		{
			dbReadFloat(1, &fTemp[0]);
			dbReadFloat(1, &fTemp[1]);
			dbReadFloat(1, &fTemp[2]);
		
			//copy and setup object
			dbInstanceObject (env.object_ctr, oItem);
			dbPositionObject (env.object_ctr, fTemp[0], fTemp[1] + 1, fTemp[2]);
			dbScaleObject (env.object_ctr, 50, 50, 50);
			itemYPos[i] = fTemp[1] + 1;

			//collision setup
			dbSetObjectCollisionOn(env.object_ctr);
			dbSetObjectCollisionToBoxes(env.object_ctr);

			//particle setup
			dbMakeParticles (env.particle_ctr, parImage, 1, 20);
			dbSetParticleLife (env.particle_ctr, 1);
			dbSetParticleEmissions (env.particle_ctr, 0);
			dbSetParticleSpeed (env.particle_ctr, 0.02 / env.syncRate);
			dbSetParticleGravity (env.particle_ctr, 0.0f);
			dbSetParticleFloor (env.particle_ctr, -50);

#ifdef DEBUG
			//add item to editing list
			//EXTREMELY ugly - you have never seen that, ok?
			game.obj[game.objCreated][0] = INDEX_ITEM;
			game.obj[game.objCreated][1] = 0;
			game.obj[game.objCreated][2] = env.object_ctr;
			game.obj2[game.objCreated][0] = fTemp[0];
			game.obj2[game.objCreated][1] = fTemp[1];
			game.obj2[game.objCreated][2] = fTemp[2];
			game.objCreated++;
#endif

			maxItems++;
			env.particle_ctr++;
			env.object_ctr++;
		}	
		dbCloseFile(1);
		//allocate and initialize visible flag array for all items
		visible = (bool*)malloc(sizeof(bool) * ITEM_MAX_NUMBER);

	}
	memset(visible, true, ITEM_MAX_NUMBER);

	//read back original Y position
	for (i = 0; i < maxItems && i < ITEM_MAX_NUMBER; i++)
	{
		dbPositionObject(objOffset + i,
			dbObjectPositionX (objOffset + i),
			itemYPos[i],
			dbObjectPositionZ (objOffset + i)
		);
		dbShowObject(objOffset + i);
	}
}

void Item::deInit()
{
	//hide particle emissions and items
	for (int i = 0; i < maxItems && i < ITEM_MAX_NUMBER; i++)
	{
		dbSetParticleEmissions (parOffset + i, 0);
		dbHideObject(objOffset + i);
	}
}

void Item::checkCollision(int obj)
{
	int i;
	for (i = 0; i < maxItems; i++)
	{
		if (dbObjectCollision (obj, objOffset + i) && visible[i])
		{
			visible[i] = false;
			dbHideObject (objOffset + i);
			dbSetParticleEmissions (parOffset + i, 0);
			dbPlaySound(sndCollect);
			collectedItems++;
		}
	}
}

void Item::animate()
{
	int i;
	int ctr;

	//workaround to reduce emissions of particles to once per second for each emitter
	//disable last emitters
	for(ctr = 0; ctr < maxItems; ctr += env.syncRate / 2)
	{
		if (parTimer + ctr < maxItems)
			if (visible[parTimer + ctr])
				dbSetParticleEmissions (parOffset + parTimer + ctr, 0);
	}

	//increment timer
	parTimer++;
	if (parTimer >= env.syncRate / 2)
		parTimer -= env.syncRate / 2;

	//enable next emitters
	for(ctr = 0; ctr < maxItems; ctr += env.syncRate / 2)
	{
		if (parTimer + ctr < maxItems)
			if (visible[parTimer + ctr]) 
				dbSetParticleEmissions (parOffset + parTimer + ctr, 1);
	}

	//timer is used for sinus wave
	timer += env.timeStep;
	for (i = 0; i < maxItems; i++)
	{
		if (visible[i])
		{
			dbYRotateObject (objOffset + i, 
				dbObjectAngleY (objOffset + i) + (float)ITEM_ROT_SPEED * env.timeStep
			);
			dbPositionObject (objOffset + i,
				dbObjectPositionX (objOffset + i),
				itemYPos[i] + 0.3 * sin (timer * 5 + i),
				dbObjectPositionZ (objOffset + i)
			);			
			dbPositionParticles/*Emissions*/ (parOffset + i,
				dbObjectPositionX(objOffset + i), 
				dbObjectPositionY(objOffset + i),
				dbObjectPositionZ(objOffset + i)
			);
		}
	}
}

int Item::getCollectedItems()
{
	return collectedItems;
}

int Item::getMaxItems()
{
	return maxItems;
}

//global instance of Item
Item item;
