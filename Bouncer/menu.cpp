#include "menu.h"

Menu::Menu()
{
	initialized = false;
}

Menu::~Menu()
{
	if (initialized)
	{
		for (int i = 0; i < MENU_MAX_ENTRIES; i++)
			free (menuText[i]);
	}
}

void Menu::init()
{
	menuSelected = 0;
	selStep = 0;
	menuActive = -1;	//-1 --> main menu is active
	timer = 0.0f;
	timeStep = (1.0f / env.syncRate);
	borderShift = 0.0f;

	//update screen size info
	sizeX = dbScreenWidth();
	sizeY = dbScreenHeight();

	//update text settings
	dbSetTextFont("Arial");
	dbSetTextSize(60); 
	dbSetTextTransparent();
	dbSetTextToBold(); 

	if (!initialized)
	{
		initialized = true;

		menuText[0] = (char*)malloc(sizeof(char) * 20); 
		strcpy(menuText[0], "Start Game");
		menuText[1] = (char*)malloc(sizeof(char) * 20); 
		strcpy(menuText[1], "Instructions");
		menuText[2] = (char*)malloc(sizeof(char) * 20); 
		strcpy(menuText[2], "Exit Game");

		instrText[0] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[0], "Game Goal");
		instrText[1] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[1], "Collect all crystal shards within a");
		instrText[2] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[2], "limited time period.");
		instrText[3] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[3], "By tilting the level and using the");
		instrText[4] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[4], "booster feature it is possible to even");
		instrText[5] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[5], "reach places far off.");

		instrText[6] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[6], "How to play");
		instrText[7] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[7], "Tilt and roll the Wiimote to move ball in");
		instrText[8] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[8], "desired direction.");
		instrText[9] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[9], "In order to use the booster, accelerate the");
		instrText[10] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[10], "Wiimote in the desired direction to speed up");
		instrText[11] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[11], "ball movement.");

		instrText[12] = (char*)malloc(sizeof(char) * 45); 
		strcpy(instrText[12], "Press Return key or A button to continue");

		dbLoadImage ("logo.tga", env.image_ctr, 1);
		dbSprite (env.sprite_ctr, sizeX / 2, 160, env.image_ctr);
		dbOffsetSprite (env.sprite_ctr, 256, 128);
		sprLogo = env.sprite_ctr;
		env.image_ctr++;
		env.sprite_ctr++;

		dbLoadImage ("wiimote.tga", env.image_ctr, 1);
		dbSprite (env.sprite_ctr, sizeX / 2 - 235, 360, env.image_ctr);
		dbOffsetSprite (env.sprite_ctr, 32, 128);
		sprWiimote = env.sprite_ctr;
		env.image_ctr++;
		env.sprite_ctr++;

		dbLoadImage ("crystal.tga", env.image_ctr, 1);
		dbSprite (env.sprite_ctr, sizeX / 2 + 210, 170, env.image_ctr);
		dbOffsetSprite (env.sprite_ctr, 25, 25);
		sprCrystal = env.sprite_ctr;
		env.image_ctr++;
		env.sprite_ctr++;

		dbLoadImage ("menuborder.bmp", env.image_ctr, 1);
		imgBorder = env.image_ctr;
		dbSprite (env.sprite_ctr, 0, 0, env.image_ctr);
		sprLBorder = env.sprite_ctr;
		env.sprite_ctr++;
		dbSprite (env.sprite_ctr, sizeX - 50, sizeY - 1000, env.image_ctr);
		dbMirrorSprite(env.sprite_ctr);
		sprRBorder = env.sprite_ctr;
		env.sprite_ctr++;
		env.image_ctr++;

		dbLoadSound ("updown.wav", env.sound_ctr);
		sndSelect = env.sound_ctr;
		env.sound_ctr++;

		dbLoadSound ("accept.wav", env.sound_ctr);
		sndAccept = env.sound_ctr;
		env.sound_ctr++;

		dbLoadMusic("intro.mp3", env.music_ctr);
		musIntro = env.music_ctr;
		env.music_ctr++;
	}
	dbShowSprite(sprLogo);
	dbShowSprite(sprLBorder);
	dbShowSprite(sprRBorder);
	dbHideSprite(sprWiimote);
	dbHideSprite(sprCrystal);
	dbLoopMusic(musIntro);
}

void Menu::deInit()
{

	dbHideSprite(sprLogo);
	dbHideSprite(sprLBorder);
	dbHideSprite(sprRBorder);
	dbHideSprite(sprWiimote);
	dbHideSprite(sprCrystal);
	dbStopMusic(musIntro);

}

void Menu::draw()
{
	dbInk(dbRGB(38,128,23), dbRGB(0,0,0));
	dbBox (0, 0, dbScreenWidth(), dbScreenHeight());

	dbRotateSprite(sprLogo, 2 * sin(timer * 2));
	borderShift += timeStep * 60;
	if (borderShift >= 60.0f)
		borderShift -= 60.0f;
	dbSprite (sprLBorder, 0, (int)-borderShift, imgBorder); 
	dbSprite (sprRBorder, sizeX - 50, sizeY - 1000 + (int)borderShift, imgBorder); 
	timer += timeStep;

	switch (menuActive)
	{
	case -1:	//main menu
		drawMain();
		break;

	case 0:	//start game
		//no action - state change follows
		break;

	case 1:	//instructions
		drawInstructions();
		break;

	case 2:	//exit game
		//no action - state change follows
		break;

	default:
		//should never be reached
		break;
	}
}

void Menu::drawMain()
{
	int offsY = 350;

	dbSetTextSize(60); 
	for (int i = 0; i < MENU_MAX_ENTRIES; i++)
	{
		if (i != menuSelected)	//unselected entry
			dbInk(dbRGB(107,214,171), dbRGB(0,0,0));
		else	//colorize selected entry
			dbInk(dbRGB(255, 150 + 105 * abs(sin(timer)), 0), dbRGB(0, 0, 0));

		dbCenterText(sizeX / 2 - 20 * sin(timer * 3 + i), offsY + i * 70, menuText[i]); 
	}

	dbRotateSprite(sprLogo, 2 * sin(timer * 2));
}

void Menu::drawInstructions()
{
	int offsY = 70;

	dbHideSprite(sprLogo);
	dbShowSprite(sprWiimote);
	dbShowSprite(sprCrystal);

	dbInk(dbRGB(107,214,171), dbRGB(0,0,0));
	dbSetTextSize(60); 
	dbCenterText(sizeX / 2, offsY, instrText[0]); 

	dbSetTextSize(20); 
	dbCenterText(sizeX / 2, offsY+70, instrText[1]); 
	dbCenterText(sizeX / 2, offsY+90, instrText[2]); 
	dbCenterText(sizeX / 2, offsY+110, instrText[3]); 
	dbCenterText(sizeX / 2, offsY+130, instrText[4]); 
	dbCenterText(sizeX / 2, offsY+150, instrText[5]); 

	dbSetTextSize(60); 
	dbCenterText(sizeX / 2, offsY+200, instrText[6]); 

	dbSetTextSize(20); 
	dbCenterText(sizeX / 2, offsY+270, instrText[7]); 
	dbCenterText(sizeX / 2, offsY+290, instrText[8]); 
	dbCenterText(sizeX / 2, offsY+310, instrText[9]); 
	dbCenterText(sizeX / 2, offsY+330, instrText[10]); 
	dbCenterText(sizeX / 2, offsY+350, instrText[11]); 

	dbInk(dbRGB(255, 150 + 105 * abs(sin(timer)), 0), dbRGB(0, 0, 0));
	dbCenterText(sizeX / 2, sizeY - 40, instrText[12]); 

}

void Menu::control()
{
	switch(menuActive)
	{
	case -1:	//main menu
		controlMain();
		break;

	case 0:	//start game
		//no action - state change follows
		break;

	case 1:	//instructions
		controlInstructions();
		break;

	case 2:	//exit game
		//no action - state change follows
		break;

	default:
		//should never be reached
		break;
	}
}

void Menu::controlMain()
{
	//selStep: 0 - ready for next key event
	//selStep: 1 - waiting for releasing up key
	//selStep: 2 - waiting for releasing down key

	//timer is calculated in drawing routine - delay key events after initialization
	if (selStep == 0 && timer > 1.0f)
	{
		if (dbUpKey() || env.wm->getButtonStatus(wUP))
		{
			selStep = 1;
			menuSelected--;
			if (menuSelected < 0) menuSelected += MENU_MAX_ENTRIES;
			dbPlaySound(sndSelect);
		}

		if (dbDownKey() || env.wm->getButtonStatus(wDOWN))
		{
			selStep = 2;
			menuSelected ++;
			if (menuSelected >= MENU_MAX_ENTRIES) menuSelected -= MENU_MAX_ENTRIES;
			dbPlaySound(sndSelect);
		}

		if (dbReturnKey() || dbMouseClick() || env.wm->getButtonStatus(wBUTA))
		{
			selStep = 3;
			dbPlaySound(sndAccept);
		}
	}
	else
	{
		//make sure keys are released before allowing new selection change
		if (!dbUpKey() && !env.wm->getButtonStatus(wUP) && selStep == 1)
		{
			selStep = 0;
		}

		if (!dbDownKey() && !env.wm->getButtonStatus(wDOWN) && selStep == 2)
		{
			selStep = 0;
		}

		if (!dbReturnKey() && !dbMouseClick() && !env.wm->getButtonStatus(wBUTA) && selStep == 3)
		{
			selStep = 0;
			menuActive = menuSelected;
		}
	}
}

void Menu::controlInstructions()
{
	//selStep: 0 - ready for next key event
	//selStep: 1 - waiting for releasing enter/esc key
	if (selStep == 0)
	{
		if (dbReturnKey() || dbEscapeKey() || dbMouseClick() 
			|| env.wm->getButtonStatus(wBUTA) || env.wm->getButtonStatus(wBUTB)
			|| env.wm->getButtonStatus(wHOME)
		)
		{
			selStep = 1;
			dbPlaySound(sndAccept);
		}
	}

	else
	{
		if (!dbReturnKey() && !dbEscapeKey() && !dbMouseClick()
			&& !env.wm->getButtonStatus(wBUTA) && !env.wm->getButtonStatus(wBUTB)
			&& !env.wm->getButtonStatus(wHOME)
		)
		{
			selStep = 0;
			menuActive = -1;	//back to main menu
			dbShowSprite(sprLogo);
			dbHideSprite(sprWiimote);
			dbHideSprite(sprCrystal);
		}
	}
}

bool Menu::isCancelled()
{
	return (menuActive == 2);
}

bool Menu::isReady()
{
	return (menuActive == 0);
}

//global instance of environment
Menu menu;