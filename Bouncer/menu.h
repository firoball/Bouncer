#ifndef MENU_H
#define MENU_H

#include <DarkGDK.h>
#include "environment.h"

#define MENU_MAX_ENTRIES	3
#define MENU_MAX_INST_ENTR 13

class Menu
{
public:
	Menu();
	~Menu();
	void init();
	void deInit();
	void draw();
	void control();
	bool isReady();
	bool isCancelled();

private:
	void controlMain();
	void controlInstructions();
	void drawMain();
	void drawInstructions();

	char* menuText[MENU_MAX_ENTRIES];
	char* instrText[MENU_MAX_INST_ENTR];
	int menuSelected;	//current menu selection
	int selStep;	//key event status variable
	int menuActive;	//current menu state (0 - 2)
	int sizeX, sizeY;	//screen size
	bool initialized;
	float timer;
	float timeStep;
	float borderShift;
	int sndSelect;
	int sndAccept;
	int sprLogo;
	int sprLBorder;
	int sprRBorder;
	int sprWiimote;
	int sprCrystal;
	int imgBorder;
	int musIntro;
};

extern Menu menu;
#endif