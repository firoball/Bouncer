//ich hasse hasse HASSE Darkbasic!!!
/* Die Datenkapselung ist der absolute Horror
und ich will dieses Ding nie nie wieder sehn */

#include "main.h"

void dbInit()
{
	dbSyncOn();
	dbSyncRate(env.syncRate);
	dbRandomize(dbTimer());
	dbSetWindowTitle ("Bouncer"); 
	int sizeX = 1024;//800;
	int sizeY = 768;//600;
	dbSetDisplayMode(sizeX, sizeY, 32);
	dbSetWindowSize(sizeX, sizeY); 
	//dbSetWindowOff ();
	dbHideMouse();
	dbDisableEscapekey();
	SetCurrentDirectory ("media");
}


// main entry point for the application
void DarkGDK()
{
	env.init();
	state.init();

	while (LoopGDK() && !state.shutdownRequested())
	{
		state.process();
		dbSync();
	}

	//free all allocated DB resources before exit
	env.cleanUp();
	return;

}
