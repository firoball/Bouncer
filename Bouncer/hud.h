#ifndef HUD_H
#define HUD_H

#include <string.h>

#include <DarkGDK.h>
#include "gdkwiimote.h"
#include "environment.h"
#include "item.h"

#define HUD_BOOST_LOADTIME 5.0f	//5 seconds
#define HUD_BAT_REFRESHTIME 60.0f	//60 seconds
#define IMG_BOOSTBAR 0
#define IMG_WIIMOTEBAR 1
#define IMG_BOOSTBG 2
#define IMG_WIIMOTEBG 3
#define IMG_LIVES 4
#define IMG_ITEMS 5
#define IMG_BOOST 6
#define IMG_WIIMOTE 7

class Hud
{
public:
	Hud();
	~Hud();
	void init();
	void deInit();
	void refresh();
	void draw();
	void setLives(int lives);
	int timeLeft();
	int getLives();
	bool boosterReady();
	void resetBooster();

private:
	int items;
	int max_items;
	float booster;
	float oldBooster;
	int level;
	char battery;

	int lives;
	int minutes;	//real remaining minutes
	int seconds;	//real remaining seconds

	bool initialized;	//initialization routine run at least once?
	float bat_refresh_timer; //wiimote battery refresh timer
	int sizeX, sizeY;	//screen size
	float fSeconds;	//partial seconds (used for calculation)
	int img[8];	//image index storage
	int spr[2];	//sprite index storage
	int sndBooster;
};

extern Hud hud;

#endif