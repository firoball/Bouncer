#include "game.h"

//Diese Klasse ist ein einziger scheiß greisliger Mülleimer
Game::Game()
{
	initialized = false;
}

Game::~Game()
{
}

void Game::init()
{
	dead = false;
	deadTimeout = false;
	winner = false;
	winnerTimeout = false;
	delayTimer = 0.0f;
	fCameraAngleX = 0.0f;
	fCameraAngleY = 0.0f;

	boostTemp = 0;
	boostTimer = 0.0f;

	//calculated force vector without 'G' influence for Wiimote
	wX = wY = wZ = 0.0f;

	//'G' vector for Wiimote
	wmG.m_x = 0.0f;
	wmG.m_y = 0.0f;
	wmG.m_z = 1.0f;

#ifdef DEBUG
	dbPositionMouse (dbScreenWidth() / 2, dbScreenHeight() / 2);
	dbShowMouse();
#endif

	if (!initialized)
	{
		initialized = true;

		// IMPORTANT!!! Define all physics objects BEFORE defining
		// any other objects to reduce memory usage.
		// adjust NEWTON_MAXBODIES in environment.h for enough phys. obj.

		//level
		dbLoadNewtonMeshObject		("bouncer.x", env.object_ctr, 10);
		dbPositionNewtonObject		(env.object_ctr, 0, -200, 0);
		dbSetObjectSmoothing		(env.object_ctr, 70); 
		oLevel = env.object_ctr;
		env.object_ctr++;

		//player
		dbMakeObjectSphere(env.object_ctr, 1);
		//moved outside if condition - needs to be recreated every level restart
//		dbMakeNewtonObjectSphere	(env.object_ctr, 1, PLAYER_MASS);
		dbLoadImage					("player.bmp", env.image_ctr, 0);
		dbTextureObject				(env.object_ctr, env.image_ctr);
		dbSetShadowShadingOn		(env.object_ctr);
		env.image_ctr++;
		oPlayer = env.object_ctr;
		env.object_ctr++;

		//skybox
		dbLoadObject				("skybox.x", env.object_ctr);
		dbSetObjectLight			(env.object_ctr, 0);
		dbSetObjectTexture			(env.object_ctr, 3, 2);
		dbScaleObject				(env.object_ctr, 5000, 5000, 5000);
		oSkybox = env.object_ctr;
		env.object_ctr++;

		dbLoadSound ("die.wav", env.sound_ctr);
		sndDie = env.sound_ctr;
		env.sound_ctr++;

		dbLoadSound ("win.wav", env.sound_ctr);
		sndWin = env.sound_ctr;
		env.sound_ctr++;

		dbLoadSound ("boost.wav", env.sound_ctr);
		sndBoost = env.sound_ctr;
		env.sound_ctr++;

		dbLoadMusic("game.mp3", env.music_ctr);
		musGame = env.music_ctr;
		env.music_ctr++;

		//load all environment models once - create instances later
		dbLoadGsObject ("grass.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oEnv[0] = env.object_ctr;
		oScale[0] = 50;
		env.object_ctr++;

		dbLoadGsObject ("stone.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oEnv[1] = env.object_ctr;
		oScale[1] = 30;
		env.object_ctr++;

		dbLoadGsObject ("dorns.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oEnv[2] = env.object_ctr;
		oScale[2] = 30;
		env.object_ctr++;

		dbLoadGsObject ("mushroom.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oEnv[3] = env.object_ctr;
		oScale[3] = 30;
		env.object_ctr++;

		dbLoadGsObject ("treetrunk.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oEnv[4] = env.object_ctr;
		oScale[4] = 250;
		env.object_ctr++;

		dbLoadGsObject ("bush.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oEnv[5] = env.object_ctr;
		oScale[5] = 70;
		env.object_ctr++;

#ifdef DEBUG
		//only used for placing objects in debug mode - will be read and generated in item.cpp
		dbLoadGsObject ("item.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oEnv[6] = env.object_ctr;
		oScale[6] = 60;
		env.object_ctr++;

		objCreated = 0;
		objSelector = 0;
#endif

		//handle tree crowns seperately - created together with trunks
		dbLoadGsObject ("treecrown.mdl", env.object_ctr);
		dbHideObject(env.object_ctr);
		oTree = env.object_ctr;
		env.object_ctr++;

		//now load all objects with positions from data file
		loadObjects();

		//does not work as intended!?
		dbMakeLight(1);
		dbColorLight (1, dbRGB(234, 149, 21));
		dbSetDirectionalLight (1, -3, -1, -3);
		dbSetNormalizationOn ();
		dbSetAmbientLight (30);
		dbColorAmbientLight (dbRGB(234, 149, 21));
	}
	hud.init();
	item.init();
	dbLoopMusic(musGame);

	dbMakeNewtonObjectSphere	(oPlayer, 1, PLAYER_MASS);
	dbPositionNewtonObject		(oPlayer, 0, -185, 0);
	NewtonSetPlayer				(oPlayer);
	dbShowObject				(oPlayer);

	dbPositionCamera ( 0, -150, 0);


}

void Game::deInit()
{	
	hud.deInit();
	item.deInit();
	dbStopMusic(musGame);
	dbSetMusicVolume(musGame, 100);
	dbDeleteNewtonObject(oPlayer);
	dbRotateObject (oSkybox, 0.0f, 0.0f, 0.0f);
	dbHideObject (oPlayer);
}

void Game::loop()
{
	hud.draw();

#ifdef DEBUG
	placeObjects();
#endif
/*
	char buffer [150];
	int n;
	n=sprintf (buffer, "x: %.2lf  y: %.2lf  z: %.2lf", dbCameraPositionX(), dbCameraPositionY(), dbCameraPositionZ());
	dbText ( 0, 0, buffer );
*/
	if (env.wiimote_active && !dead)
		checkBooster();

	if (env.wiimote_active && !deadTimeout)
		setOrientation();
	else
		moveCamera();

	item.checkCollision(oPlayer);
	item.animate();

	dbNewtonSync();
	checkDeath();
	checkWinner();
}

void Game::checkBooster()
{
	if (hud.boosterReady())
	{

		//get current accelerator values
		env.wm->GetCalibratedAcceleration(wmFrc.m_x, wmFrc.m_y, wmFrc.m_z);

		//get orientation of force vector
		//wiimote of course uses different coordinate system
		//--> yaw and pitch is required instead of roll and tilt
		env.wm->GetOrientation(wmAng.m_x, wmAng.m_y, wmAng.m_z);
		matrix = dgYawMatrix(-wmAng.m_z * 3.1416f / 180.0f);	//roll
		matrix = matrix * dgPitchMatrix(-wmAng.m_y * 3.1416f / 180.0f);	//tilt

		//now rotate force vector according to Wiimote angle to get forces
		//without any rotation information
		wmFrc = matrix.RotateVector(wmFrc);

		//calculate real force values by removing 'g' from forces
		wX = wmFrc.m_x - wmG.m_x;
		wY = wmFrc.m_y - wmG.m_y;
		wZ = wmFrc.m_z - wmG.m_z;
		
		if (boostTemp == 0)
		{
			if (wY > 1.0f && wX < 2.0f && wZ < 2.0f && !(boostTemp & 2))
			{
				boostTemp |= 1;
			}

			if (wY < -1.0f && wX > -2.0f && wZ > -2.0f && !(boostTemp & 1))
			{
				boostTemp |= 2;
			}

			if (wX > 1.1f && wY < 2.0f && wZ < 2.0f && !(boostTemp & 8))
			{
				boostTemp |= 4;
			}

			if (wX < -1.1f && wY > -2.0f && wZ > -2.0f && !(boostTemp & 4))
			{
				boostTemp |= 8;
			}
			//most natural movement - less sensitive
			if (wZ > 1.2f && wY < 2.0f && wX < 2.0f && !(boostTemp & 32))
			{
				boostTemp |= 16;
			}

			if (wZ < -1.2f && wX > -2.0f && wY > -2.0f && !(boostTemp & 16))
			{
				boostTemp |= 32;
			}
		}
		else
		{
			//if move detection failed, start timeout timer
			//and reset boost variables afterwards
			boostTimer += env.timeStep;
			if (boostTimer > 0.4f)
			{
				boostTemp = 0;
				boostTimer = 0.0f;
			}

			if (wY < -1.0f && (boostTemp & 1))
			{
				hud.resetBooster();
				boostTemp = 0;
				dbPlaySound(sndBoost);
				//MessageBox( NULL, "Boost: Z-", "info", MB_OK );
				dVector v(0,0,-BOOST_FAC);
				applyImpulse(oPlayer, &v);
			}

			else if (wY > 1.0f && (boostTemp & 2))
			{
				hud.resetBooster();
				boostTemp = 0;
				dbPlaySound(sndBoost);
//				MessageBox( NULL, "Boost: Z+", "info", MB_OK );
				dVector v(0,0,BOOST_FAC);
				applyImpulse(oPlayer, &v);
			}

			else if (wX < -1.1f && (boostTemp & 4))
			{
				hud.resetBooster();
				boostTemp = 0;
				dbPlaySound(sndBoost);
//				MessageBox( NULL, "Boost: X-", "info", MB_OK );
				dVector v(-BOOST_FAC,0,0);
				applyImpulse(oPlayer, &v);
			}

			else if (wX > 1.1f && (boostTemp & 8))
			{
				hud.resetBooster();
				boostTemp = 0;
				dbPlaySound(sndBoost);
//				MessageBox( NULL, "Boost: X+", "info", MB_OK );
				dVector v(BOOST_FAC,0,0);
				applyImpulse(oPlayer, &v);
			}
			//most natural movement - less sensitive
			else if (wZ < -1.2f && (boostTemp & 16))
			{
				hud.resetBooster();
				boostTemp = 0;
				dbPlaySound(sndBoost);
//				MessageBox( NULL, "Boost: Y+", "info", MB_OK );
				dVector v(0,BOOST_FAC,0);
				applyImpulse(oPlayer, &v);
			}

			else if (wZ > 1.2f && (boostTemp & 32))
			{
				hud.resetBooster();
				boostTemp = 0;
				dbPlaySound(sndBoost);
//				MessageBox( NULL, "Boost: Y-", "info", MB_OK );
				dVector v(0,-BOOST_FAC,0);
				applyImpulse(oPlayer, &v);
			}
		}
	}
/*
	char buffer [150];
	int n;
	n = sprintf(buffer, "W:[%+1.2f %+1.2f %+1.2f] ",wX,wY,wZ);		
	dbText ( 0, 60, buffer );
	n=sprintf (buffer, "boostTemp: %d", boostTemp);
	dbText ( 0, 400, buffer );
*/
}

void Game::moveCamera()
{

	// move the camera using the arrow keys
	dbControlCameraUsingArrowKeys ( 0, 5.0f, 0.3f );
	// create a rotation axis based on mouse movement
	fCameraAngleX = dbWrapValue ( fCameraAngleX + dbMouseMoveY ( ) * 0.4f );
	fCameraAngleY = dbWrapValue ( fCameraAngleY + dbMouseMoveX ( ) * 0.4f );

	// rotate camera
	dbXRotateCamera ( fCameraAngleX );
	dbYRotateCamera ( fCameraAngleY );

	//no text formatting - use the current one to speed things up
	dbCenterText(dbScreenWidth() / 2, dbScreenHeight() - 165, 
		"WARNING: Bird mode. Move camera with arrows.");
	dbCenterText(dbScreenWidth() / 2, dbScreenHeight() - 135, 
		"In order to play this game, a Wiimote is required!");
	dbCenterText(dbScreenWidth() / 2, dbScreenHeight() - 105, 
		"Please connect Wiimote and restart game."); 

}

void Game::attachCamera(int ctr, float tilt, float roll)
{
	dVector pos(dbObjectPositionX(ctr), dbObjectPositionY(ctr), dbObjectPositionZ(ctr));
	dVector speed;
	NewtonBodyGetVelocity(env.body[ctr - 1], &speed.m_x);
	float offsTilt = tilt + 25;
	/* camera distance depends on ball speed */
	float fac = sqrt(speed % speed) * 0.4;
	dVector dist(0.0f, 0.0f, -10.0f - fac);
	dMatrix matrix (dgPitchMatrix(offsTilt * 3.1416f / 180.0f));
	matrix = matrix * dgRollMatrix(roll * 3.1416f / 180.0f);

	dist = matrix.RotateVector(dist);
	pos += dist;
	//pos += dVector(dbCameraPositionX(), dbCameraPositionY(), dbCameraPositionZ());
	//pos.Scale(0.5f);
	dbPositionCamera (pos.m_x,pos.m_y,pos.m_z); 
	dbRotateCamera (offsTilt, 0, roll);
}

void Game::setOrientation()
{
	env.wm->GetOrientation(pan, tilt, roll);
	//limit angles to -25.5..+25.5 deg
	pan = 0.85* CLAMP(pan, -30, 30);
	tilt = 0.85* CLAMP(tilt, -30, 30);
	roll = 0.85* CLAMP(roll, -30, 30);
	//avoid hysteresis
	if (abs(dbObjectAngleX(oSkybox) - tilt) < 3 || abs(dbObjectAngleX(oSkybox) - tilt) > 45) tilt = dbObjectAngleX(oSkybox);
	if (abs(dbObjectAngleY(oSkybox) - pan) < 3 || abs(dbObjectAngleY(oSkybox) - pan) > 45) pan = dbObjectAngleY(oSkybox);
	if (abs(dbObjectAngleZ(oSkybox) - roll) < 3 || abs(dbObjectAngleZ(oSkybox) - roll) > 45) roll = dbObjectAngleZ(oSkybox);

	attachCamera (oPlayer, 0.8* dbObjectAngleX(oSkybox)+ 0.2*tilt, 0.8* dbObjectAngleZ(oSkybox)+ 0.2*roll);
	NewtonSetGravity (0.8* dbObjectAngleX(oSkybox)+ 0.2*tilt, 0.8* dbObjectAngleY(oSkybox)+ 0.2*pan, 0.8* dbObjectAngleZ(oSkybox)+ 0.2*roll);
	dbRotateObject (oSkybox, 0.8* dbObjectAngleX(oSkybox)+ 0.2*tilt, 0.8* dbObjectAngleY(oSkybox)+ 0.2*pan, 0.8* dbObjectAngleZ(oSkybox)+ 0.2*roll);
}

void Game::applyImpulse(int obj, dVector *vel)
{
	dMatrix matrix;
	NewtonBodyGetMatrix (env.body[obj - 1], &matrix[0][0]);
	NewtonAddBodyImpulse(env.body[obj - 1], &(vel->m_x), &matrix.m_posit.m_x);
}

void Game::checkDeath()
{
	if ((hud.timeLeft() == 0 || dbObjectPositionY(oPlayer) < -202 || dead) && !winner)
	{
		if (delayTimer > 1.5f)
		{
			if (!deadTimeout)
			{
				deadTimeout = true;
			}
		}
		else
		{
			if (!dead)
			{
				dead = true;
				dbPlaySound(sndDie);
			}
			delayTimer += env.timeStep;
			dbSetMusicVolume(musGame, 40 * __max(1.5f - delayTimer, 0));
		}
	}
}

bool Game::died()
{
	return deadTimeout;
}

void Game::checkWinner()
{
	if (((item.getCollectedItems() >= item.getMaxItems()) || winner) && !dead)
	{
		if (delayTimer > 1.5f)
		{
			if (!winnerTimeout)
			{
				winnerTimeout = true;
			}
		}
		else
		{
			if (!winner)
			{
				winner = true;
				dbPlaySound(sndWin);
			}
			delayTimer += env.timeStep;
			dbSetMusicVolume(musGame, 40 * __max(1.5f - delayTimer, 0));
		}
	}
}

bool Game::won()
{
	return winnerTimeout;
}

void Game::loadObjects()
{
	int objects = 0;
	float fTemp[3];
	int iTemp[2];

	dbOpenToRead(1, "objdata.dat");
	dbReadLong(1, &objects);
	
	for (int i = 0; i < objects; i++)
	{
		//object type
		dbReadLong(1, &iTemp[0]);
		//rotation
		dbReadLong(1, &iTemp[1]);
		//position
		dbReadFloat(1, &fTemp[0]);
		dbReadFloat(1, &fTemp[1]);
		dbReadFloat(1, &fTemp[2]);
		
		//copy and setup object
		dbInstanceObject (env.object_ctr, oEnv[iTemp[0]]);
		dbYRotateObject (env.object_ctr, iTemp[1]);
		dbPositionObject (env.object_ctr, fTemp[0], fTemp[1], fTemp[2]);
		dbScaleObject (env.object_ctr, oScale[iTemp[0]], oScale[iTemp[0]], oScale[iTemp[0]]);

#ifdef DEBUG
		//add object to editing list
		obj[objCreated][0] = iTemp[0];
		obj[objCreated][1] = dbObjectAngleY (env.object_ctr);
		obj[objCreated][2] = env.object_ctr;
		obj2[objCreated][0] = dbObjectPositionX (env.object_ctr);
		obj2[objCreated][1] = dbObjectPositionY (env.object_ctr);
		obj2[objCreated][2] = dbObjectPositionZ (env.object_ctr);
		objCreated++;
#endif

		env.object_ctr++;

		if (iTemp[0] == INDEX_TREE) //tree trunk
		{
			dbInstanceObject (env.object_ctr, oTree);
			dbYRotateObject (env.object_ctr, iTemp[1]);
			dbPositionObject (env.object_ctr, fTemp[0], fTemp[1], fTemp[2]);
			dbScaleObject (env.object_ctr, oScale[iTemp[0]], oScale[iTemp[0]], oScale[iTemp[0]]);
			env.object_ctr++;
		}

	}
	dbCloseFile(1);
}

#ifdef DEBUG
/* place objects and items realtime in the level 
- left mousebutton: place object/item
- right mousebutton: choose next object type
- middle mousebutton: place mouse cursor to center
- space key: delete last object/item
- return key: save object and item position lists

This function should be moved to an object class
along with all object related stuff found here,
but I have no time left for cleaning all this stuff.
It's ugly and hacked together very rapidly, 
but it's working fine. */
void Game::placeObjects()
{
	delayTimer = 0.0f;	//avoid timeout

	static bool click = false;
	static bool spacelock = false;
	static bool returnlock = false;

	static char temp_str[20];
	sprintf(temp_str, "Item type: %d", objSelector);
	dbCenterText(dbScreenWidth() / 2, dbScreenHeight() / 2, temp_str); 

	if (dbSpaceKey() && objCreated > 0 && !spacelock)
	{
		spacelock = true;
		//dbDeleteObject (obj[objCreated - 1][2]);
		dbHideObject (obj[objCreated - 1][2]);		
		objCreated--;
	}
	if (!dbSpaceKey())
		spacelock = false;

	if (!click)
	{
		click = true;
		switch (dbMouseClick())
		{
		case 1:	//left mouse button - pick and place object
			if (dbPickObject (dbMouseX(), dbMouseY(), oLevel, oLevel))
			{
				dbInstanceObject (env.object_ctr, oEnv[objSelector]);
				dbPositionObject (env.object_ctr, dbGetPickVectorX() + dbCameraPositionX(), 
					dbGetPickVectorY() + dbCameraPositionY(), 
					dbGetPickVectorZ() + dbCameraPositionZ()
				);
				dbShowObject (env.object_ctr);
				//some random angle to make things look less generated
				dbYRotateObject (env.object_ctr, dbRND (360));
				dbScaleObject (env.object_ctr, oScale[objSelector], oScale[objSelector], oScale[objSelector]);
				obj[objCreated][0] = objSelector;
				obj[objCreated][1] = dbObjectAngleY (env.object_ctr);
				obj[objCreated][2] = env.object_ctr;
				obj2[objCreated][0] = dbObjectPositionX (env.object_ctr);
				obj2[objCreated][1] = dbObjectPositionY (env.object_ctr);
				obj2[objCreated][2] = dbObjectPositionZ (env.object_ctr);
				env.object_ctr++;
				objCreated ++;
			}
			break;

		case 2:	//right mouse button - cycle through objects
			 objSelector ++;
			 if (objSelector >= NUM_ENVOBJECTS)
				 objSelector = 0;
			break;

		case 4:
			dbPositionMouse (dbScreenWidth() / 2, dbScreenHeight() / 2);
			break;

		case 0:
			click = false;
			break;

		default:
			break;
		}
	}
	click = !(dbMouseClick() == 0);

	//write data to file
	if (dbReturnKey() && !returnlock)
	{
		returnlock = true;
		dbDeleteFile("objdata.dat");
		dbDeleteFile("itemdata.dat");

		dbOpenToWrite(1, "objdata.dat");
		dbOpenToWrite(2, "itemdata.dat");

		int numObj = 0;
		int numItem = 0;
		for (int i = 0; i < objCreated; i++)
		{
			//item or object? 
			if (obj[i][0] == INDEX_ITEM)
				numItem++;
			else
				numObj++;

		}
		dbWriteLong(1, numObj);
		dbWriteLong(2, numItem);
		
		for (int i = 0; i < objCreated; i++)
		{

			if (obj[i][0] == INDEX_ITEM)
			{
				//item - only position data is required
				dbWriteFloat(2, obj2[i][0]);
				dbWriteFloat(2, obj2[i][1]);
				dbWriteFloat(2, obj2[i][2]);
			}
			else
			{

				//object - store type and angle as well
				dbWriteLong(1, obj[i][0]);
				dbWriteLong(1, obj[i][1]);

				dbWriteFloat(1, obj2[i][0]);
				dbWriteFloat(1, obj2[i][1]);
				dbWriteFloat(1, obj2[i][2]);
			}

		}
		dbCloseFile(1);
		dbCloseFile(2);
	}
	if (!dbReturnKey())
		returnlock = false;
}
#endif

//global instance of Game
Game game;