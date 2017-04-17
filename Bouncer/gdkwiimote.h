#ifndef GDKWIIMOTE_H
#define GDKWIIMOTE_H

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "wiimote.h"

#define wUP                1
#define wDOWN              2
#define wLEFT              4
#define wRIGHT             8
#define wBUTA              16
#define wBUTB              32
#define wPLUS              64
#define wMINUS             128
#define wHOME              256
#define wBUT1              512
#define wBUT2              1024
#define wBUTC              2048
#define wBUTZ              4096

class gdkWiiMote:public cWiiMote
{
public:
	gdkWiiMote();
	~gdkWiiMote();

	bool ConnectToDevice(int index = 0);
	bool Disconnect();	
	bool ShutdownRequested() const {return shutdown;}
	void GetOrientation(float & pan, float & tilt, float & roll);
	bool getButtonStatus(int button);	

private:
	bool shutdown;
	bool active;
	HANDLE ThreadHandle;
    DWORD ThreadID;
    int dev_index;
};

#endif
