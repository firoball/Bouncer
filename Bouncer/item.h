#ifndef ITEM_H
#define ITEM_H

#include <DarkGDK.h>
#include "environment.h"

#define ITEM_ROT_SPEED 120.0f	//120 degrees per second
#define ITEM_MAX_NUMBER 50	//maximum of collectable items used in game

class Item
{
public:
	Item();
	~Item();
	void init();
	void deInit();
	void checkCollision(int obj);
	void animate();
	int getCollectedItems();
	int getMaxItems();

private:

	int maxItems;	//maximum no. of items in level
	float itemYPos[ITEM_MAX_NUMBER];
	int collectedItems;	//items not collected yet
	int objOffset;	//offset of object counter
	int oItem;	//master item - all othe ritems are cloned from this one
	int parOffset;	//offset of particle counter
	bool* visible;	//visibility flag array for all objects
	float timer;	//timer variable which is used for cyclic moving/rotation/scaling
	int parTimer;	//timer which counts frames per second to distribute particle emission
	int sndCollect;
	bool initialized; //initialization already run?
};

extern Item item;

#endif