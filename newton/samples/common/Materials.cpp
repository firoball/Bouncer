// Materials.cpp: implementation of the Materials class.
//
//////////////////////////////////////////////////////////////////////

#include "Newton.h"
#include "Materials.h"
 


int woodID; 
int metalID; 
int levelID; 

void SetupMaterials (NewtonWorld* nWorld)
{
	int defaultID;

	// get the defuult material ID
	defaultID = NewtonMaterialGetDefaultGroupID (nWorld);

	// create all materials ID
	woodID = NewtonMaterialCreateGroupID(nWorld);
	metalID = NewtonMaterialCreateGroupID(nWorld);
	levelID = NewtonMaterialCreateGroupID(nWorld);
}

