
#include "gdkwiimote.h"

gdkWiiMote::gdkWiiMote():cWiiMote()
{
	shutdown = false;
	dev_index = -1;
}

gdkWiiMote::~gdkWiiMote()
{
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	gdkWiiMote* wiimote = (gdkWiiMote*)lpParam;
	while(!wiimote->ShutdownRequested())
    {
		wiimote->HeartBeat();
    }
	return 0;
}

bool gdkWiiMote::ConnectToDevice(int index)
{
	active = false;
	if (cWiiMote::ConnectToDevice(index) && RequestExpansionReport())
    {
		active = true;
		dev_index = index;
        ThreadHandle = CreateThread(NULL,0,ThreadProc,(void*)this,0,&ThreadID);
		SetLEDs((bool)(index==0), (bool)(index==1), (bool)(index==2), (bool)(index==3));
   		return true;
    }
    else
		return false;
}
	
bool gdkWiiMote::Disconnect()	
{
	if (active == true)
	{
		SetVibration(false);
		SetLEDs(false, false, false, false);

		shutdown = true;
		WaitForSingleObject(ThreadHandle, INFINITE);
		CloseHandle(ThreadHandle);
		return cWiiMote::Disconnect();
	}
	else
		return 0;
}

void gdkWiiMote::GetOrientation(float & pan, float & tilt, float & roll)
{
	cWiiMote::GetOrientation(pan, tilt, roll);
/*
	pan = (int)(pan * 180 / PI);
	tilt = (int)(tilt * 180 / PI);
	roll = (int)(roll * 180 / PI); 
*/
	pan = (int)(pan * 180 / PI);
	tilt = (int)(tilt * 180 / PI);
	roll = (int)(roll * 180 / PI); 
}

bool gdkWiiMote::getButtonStatus(int button)
{
	bool retval = false;
	switch (button)
	{
	case wBUTA:
		retval = mLastButtonStatus.mA;
		break;

	case wBUTB:
		retval = mLastButtonStatus.mB;
		break;

	case wHOME:
		retval = mLastButtonStatus.mHome;
		break;

	case wUP:
		retval = mLastButtonStatus.mUp;
		break;

	case wDOWN:
		retval = mLastButtonStatus.mDown;
		break;

	case wLEFT:
		retval = mLastButtonStatus.mLeft;
		break;

	case wRIGHT:
		retval = mLastButtonStatus.mRight;
		break;

	default:
		retval = false;
		break;
	}

	return retval;
}