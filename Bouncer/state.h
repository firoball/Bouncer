#ifndef STATE_H
#define STATE_H

#include <DarkGDK.h>
#include "hud.h"
#include "gdkwiimote.h"
#include "logo.h"
#include "menu.h"
#include "gameover.h"
#include "winner.h"
#include "game.h"

#define stSTARTUP 0
#define stMENU 1
#define stRUN 2
#define stGAMEOVER 3
#define stEXIT 4
#define stWON 5

class State
{
public:
	State();
	~State();
	
	void init();
	void process();
	int getState();

	//function for game shutdown
	bool shutdownRequested();
	void drawShutdown();

private:
	int state;

	void checkState();
	void deInitState();
	void setState(int newState);

	//flag for engine/game shutdown
	bool shutdown;
};

extern State state;
#endif