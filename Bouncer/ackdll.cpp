// standard includes
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>

// engine specific header file
#define DLL_USE	// always define before including adll.h
#include "adll.h"	

// project includes
#include "ackwiimote.h"

// DLL main entry point
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
// make sure the library is linked
	engine_bind();
    return TRUE;
}


DLLFUNC var wiimote_connect(var dev_index)
{
	ackWiiMote *wiimote = new ackWiiMote;

	if (wiimote->ConnectToDevice(_INT(dev_index)))
		return (var)wiimote;
	else
    	//not successful - return invalid handle
		return (var)NULL;
}

DLLFUNC var wiimote_connected(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle; 
	return _VAR((int)wiimote->IsConnected());
}

DLLFUNC var wiimote_nunchuk_connected(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle; 
	return _VAR((int)wiimote->nunchuk_active());
}

DLLFUNC var wiimote_classic_connected(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle; 
	return _VAR((int)wiimote->classic_active());
}

DLLFUNC var wiimote_ir_active(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle; 
	return _VAR((int)wiimote->ir_active());
}

DLLFUNC var wiimote_vibration_active(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle; 
	return _VAR((int)wiimote->vibration_active());
}

DLLFUNC var wiimote_disconnect(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle; 
	int disconnect = 0;

    wiimote->SetVibration(false);
	wiimote->SetLEDs(false, false, false, false);
	disconnect = wiimote->Disconnect();
	delete wiimote;
	return _VAR(disconnect);
}

DLLFUNC var wiimote_status(var handle, var buffer)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle; 
	wiimote->GetStatus((WIIMOTE*)buffer);
	return _VAR(0);
}

DLLFUNC var wiimote_led(var handle, var led)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle;
	wiimote->SetLEDs((bool)(_INT(led) & 1), (bool)(_INT(led) & 2), (bool)(_INT(led) & 4), (bool)(_INT(led) & 8));
    return _VAR(0);
}

DLLFUNC var wiimote_vibration(var handle, var vib_on)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle;
    wiimote->SetVibration((bool)(_INT(vib_on) & 1));
    return _VAR(0);
}

DLLFUNC var wiimote_battery(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle;
    return _VAR((int)wiimote->GetBatteryPercent());
}

DLLFUNC var wiimote_battery_raw(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle;
    return _VAR((int)wiimote->GetRawBatteryLevel());
}

DLLFUNC var wiimote_getdevices(void)
{
	return _VAR(cWiiMote::GetDevices());
}

DLLFUNC var wiimote_ir(var handle, var ir_on)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle;
	if (_INT(ir_on) != 0)
    	wiimote->ActivateIR();
    else
    	wiimote->DeactivateIR();
    	
    return _VAR(0);
}

