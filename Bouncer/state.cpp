#include "state.h"

//////////global functions

State::State()
{
	state = stSTARTUP;
}

State::~State()
{
}

void State::init()
{
	//initialize current state
	setState(state);
	shutdown = false;
}

int State::getState()
{
	return state;
}

void State::process()
{
	checkState();

	switch(state)
	{
	case stSTARTUP:
		logo.draw();
		break;

	case stMENU:
		menu.control();
		menu.draw();
		break;

	case stRUN:
		game.loop();
		break;

	case stGAMEOVER:
		gameover.draw();
		break;

	case stWON:
		winner.draw();
		break;

	case stEXIT:
	//only draw black screen
		drawShutdown();
		break;

	default:
	//should never happen
		break;
	}
}

//////////local functions

//cyclically called - check if state change has to be done
void State::checkState()
{
	switch(state)
	{


	case stSTARTUP:
		if (logo.isFinished() || 
			env.wm->getButtonStatus(wBUTA) || env.wm->getButtonStatus(wBUTB) ||
			env.wm->getButtonStatus(wHOME) ||
			dbMouseClick() || dbReturnKey() || dbEscapeKey()
			)
		{
			deInitState();
			setState(stMENU);
		}
		break;


	case stMENU:
		if (menu.isCancelled())
		{
			deInitState();
			setState(stEXIT);
		} else if (menu.isReady())
		{
			hud.setLives(3); //restore lives to 3 when run mode is set from menu
			deInitState();
			setState(stRUN);
		}
		break;


	case stRUN:
		if (game.won())
		{
			deInitState();
			setState(stWON);
		}

		//live lost or timeout?
		if (game.died())
		{
			hud.setLives(hud.getLives() - 1);
			//more lives left
			if (hud.getLives() > 0)
			{
				//live has been lost - don't restore lives to 3
				deInitState();
				setState(stRUN);
			}
			else
			//last life lost
			{
				deInitState();
				setState(stGAMEOVER);
			}
		} 
		else if (dbEscapeKey() || env.wm->getButtonStatus(wHOME))
		//ESCAPE or HOME key pressed
		{
			deInitState();
			setState(stMENU);
		}
		break;


	case stGAMEOVER:
		if (dbEscapeKey() || env.wm->getButtonStatus(wHOME))
		//ESCAPE or HOME key pressed
		{
			deInitState();
			setState(stEXIT);
		} 
		else if ( dbMouseClick() || dbReturnKey() ||
			env.wm->getButtonStatus(wBUTA) || env.wm->getButtonStatus(wBUTB)
		)
		{
			deInitState();
			setState(stMENU);
		}
		break;


	case stWON:
		if (dbEscapeKey() || env.wm->getButtonStatus(wHOME))
		//ESCAPE or HOME key pressed
		{
			deInitState();
			setState(stEXIT);
		} 
		else if ( dbMouseClick() || dbReturnKey() ||
			env.wm->getButtonStatus(wBUTA) || env.wm->getButtonStatus(wBUTB)
		)
		{
			deInitState();
			setState(stMENU);
		}
		break;


	case stEXIT:
		deInitState();
		break;


	default:
		//should never happen
		break;
	}
}

//deinitialize current State
void State::deInitState()
{
	switch (state)
	{
	case stSTARTUP:
		logo.deInit();
		break;

	case stMENU:
		menu.deInit();
		break;

	case stRUN:
		game.deInit();
		break;

	case stGAMEOVER:
		gameover.deInit();
		break;

	case stWON:
		winner.deInit();
		break;

	case stEXIT:
	//nothing to do
		break;

	default:
	//should never happen
		break;
	}
}

//set and initialize new State
void State::setState(int newState)
{
	state = newState;

	switch (state)
	{
	case stSTARTUP:
		logo.init();
		break;

	case stMENU:
		menu.init();
		break;

	case stRUN:
		game.init();
		break;

	case stGAMEOVER:
		gameover.init();
		break;

	case stWON:
		winner.init();
		break;

	case stEXIT:
		shutdown = true;
		break;

	default:
	//should never happen
		break;
	}
}

bool State::shutdownRequested()
{
	return shutdown;
}

void State::drawShutdown()
{
	dbInk(dbRGB(0,0,0), dbRGB(0,0,0));
	dbBox (0, 0, dbScreenWidth(), dbScreenHeight());
}

//global instance of environment
State state;
