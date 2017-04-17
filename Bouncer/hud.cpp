#include "hud.h"

Hud::Hud()
{
	//must not contain any Dark GDK stuff!!
	initialized = false;	
}

Hud::~Hud()
{
	//must not contain any Dark GDK stuff!!
}

void Hud::init()
{
	//lives = 3; --- set after "start game" was selected in menu
	//           --- reduced after live loss in state manager

	minutes = 13;
	seconds = 0;
	items = item.getCollectedItems();
	max_items = item.getMaxItems();
	booster = 0.0f;
	level = 1;	//only one level in this version

	bat_refresh_timer = 0.0f;
	fSeconds = (float)seconds;

	if (env.wiimote_active)
	{
		battery = env.wm->GetBatteryPercent();
		battery = __max(10, __min(battery, 100));
	}
	else
		battery = 0;

	if (!initialized)
	{
		initialized = true;
		//load HUD images

		//lives icon
		dbLoadImage ("hud2.bmp", env.image_ctr, 1);
		img[IMG_LIVES] = env.image_ctr;
		env.image_ctr++;

		//items icon
		dbLoadImage ("hud1.bmp", env.image_ctr, 1);
		img[IMG_ITEMS] = env.image_ctr;
		env.image_ctr++;

		//Wiimote battery icon
		dbLoadImage ("hud7.bmp", env.image_ctr, 1);
		img[IMG_WIIMOTE] = env.image_ctr;
		env.image_ctr++;

		//Wiimote battery background
		dbLoadImage ("hud3.bmp", env.image_ctr, 1);
		img[IMG_WIIMOTEBG] = env.image_ctr;
		env.image_ctr++;

		//Wiimote battery bar
		dbCreateAnimatedSprite (env.sprite_ctr, "hud4.bmp", 1, 10, env.image_ctr);
		dbRotateSprite(env.sprite_ctr, 270);
		spr[IMG_WIIMOTEBAR] = env.sprite_ctr;
		env.sprite_ctr++;
		img[IMG_WIIMOTEBAR] = env.image_ctr;
		env.image_ctr++;

		//Booster bar icon
		dbLoadImage ("hud5.tga", env.image_ctr, 1);
		img[IMG_BOOST] = env.image_ctr;
		env.image_ctr++;

		//Booster bar background
		dbLoadImage ("hud6.bmp", env.image_ctr, 1);
		img[IMG_BOOSTBG] = env.image_ctr;
		env.image_ctr++;

		//Booster bar
		dbCreateAnimatedSprite (env.sprite_ctr, "hud4.bmp", 1, 10, env.image_ctr);
		spr[IMG_BOOSTBAR] = env.sprite_ctr;
		env.sprite_ctr++;
		img[IMG_BOOSTBAR] = env.image_ctr;
		env.image_ctr++;

		dbLoadSound ("booster.wav", env.sound_ctr);
		sndBooster = env.sound_ctr;
		env.sound_ctr++;

	}

	dbInk(dbRGB(255,255,255), dbRGB(0,0,0));
	dbSetTextFont("Arial");
	dbSetTextSize(34); 
	dbSetTextTransparent();
	dbSetTextToBold(); 

	dbShowSprite(spr[IMG_WIIMOTEBAR]);
	dbShowSprite(spr[IMG_BOOSTBAR]);
}

void Hud::deInit()
{
	dbHideSprite(spr[IMG_WIIMOTEBAR]);
	dbHideSprite(spr[IMG_BOOSTBAR]);
}

void Hud::refresh()
{
	static float timeStep;
	timeStep = (1.0f / env.syncRate); 	

	//refresh item stats
	items = item.getCollectedItems();
	max_items = item.getMaxItems();

	//refresh Wiimote battery level
	if (env.wiimote_active)
	{
		bat_refresh_timer += timeStep;
		if (bat_refresh_timer >= HUD_BAT_REFRESHTIME)
		{
			bat_refresh_timer -= HUD_BAT_REFRESHTIME;
			battery = env.wm->GetBatteryPercent();
			//limit battery value to 10..100 percent
			battery = __max(10, __min(battery, 100));
		}
	}
	else
	{
		bat_refresh_timer = 0.0f;
		battery = 0;
	//battery = (int)(fSeconds*10/6); //temp
	}

	//refresh booster value, play sound if maxed
	oldBooster = booster;
	booster = __min(booster + timeStep, HUD_BOOST_LOADTIME);
	if (oldBooster != booster && booster == HUD_BOOST_LOADTIME)
		dbPlaySound(sndBooster);
/*
	//moved to init function for HIGH speed increase

	//update text settings
	dbInk(dbRGB(255,255,255), dbRGB(0,0,0));
	dbSetTextFont("Arial");
	dbSetTextSize(34); 
	dbSetTextTransparent();
	dbSetTextToBold(); 
*/
	//update screen size info
	sizeX = dbScreenWidth();
	sizeY = dbScreenHeight();

	//update remaining time
	if (minutes > 0 || seconds > 0)
	{
		fSeconds -= timeStep;
		if (fSeconds < 0)
		{
			fSeconds += 60.0f;
			minutes--;
		}
		seconds = (int)fSeconds;
	}
	else
	{
		minutes = 0;
		seconds = 0;
	}

}

void Hud::draw()
{
	//refresh HUD before drawing
	refresh();

	static char temp_str[20];

	//collected items
	sprintf(temp_str, "%d/%d", items, max_items);
	dbText(65, 13, temp_str); 
	dbPasteImage (img[IMG_ITEMS], 5, 5, 1);

	//remaining lives
	sprintf(temp_str, "%d", lives);
	dbText(65, sizeY - 47, temp_str); 
	dbPasteImage (img[IMG_LIVES], 5, sizeY - 55, 1);

	//remaining time
	if (seconds < 10)
		sprintf(temp_str, "%d:0%d", minutes, seconds);
	else
		sprintf(temp_str, "%d:%d", minutes, seconds);
	dbCenterText(sizeX / 2, 13, temp_str); 

	if (env.wiimote_active) 
	{
		//Wiimote battery bar
		dbPasteImage (img[IMG_WIIMOTEBG], sizeX - 21, sizeY - 105, 1);
		dbSetSpriteFrame (spr[IMG_WIIMOTEBAR], (int)(battery / 10)); 
		dbStretchSprite (spr[IMG_WIIMOTEBAR], battery, 100);
		dbPasteSprite (spr[IMG_WIIMOTEBAR], sizeX - 21, sizeY - 6);
		dbPasteImage (img[IMG_WIIMOTE], sizeX - 55, sizeY - 55, 1);
	}

	//Booster bar
	dbPasteImage (img[IMG_BOOSTBG], sizeX - 105, 39, 1);
	dbSetSpriteFrame (spr[IMG_BOOSTBAR], (int)(booster * 10.0f / HUD_BOOST_LOADTIME)); 
	dbStretchSprite (spr[IMG_BOOSTBAR], booster * 20, 100);
	dbPasteSprite (spr[IMG_BOOSTBAR], sizeX - 105, 39);
	dbPasteImage (img[IMG_BOOST], sizeX - 55, 5, 1);

	//Current level - only one level available for now
//	sprintf(temp_str, "Level %d", level);
//	dbCenterText(sizeX / 2, sizeY - 47, temp_str); 

	//show FPS only in bird mode (no Wiimote active)
	if (!env.wiimote_active)
	{
		sprintf(temp_str, "FPS:%d", dbScreenFPS());
		dbCenterText(sizeX / 2, sizeY - 50, temp_str); 
	}
}

int Hud::timeLeft()
{
	return (minutes * 60 + seconds);
}

void Hud::setLives(int lives)
{
	this->lives = lives;
}

int Hud::getLives()
{
	return lives;
}

bool Hud::boosterReady()
{
	return (booster >= HUD_BOOST_LOADTIME);
}

void Hud::resetBooster()
{
	booster = 0.0f;
}

//global instance of Hud
Hud hud;