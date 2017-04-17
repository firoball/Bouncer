//cWiimote 0.2 by Kevin Forbes (http://simulatedcomicproduct.com)
//This code is public domain, and comes with no warranty. The user takes full responsibility for anything that happens as a result from using this code.

#ifndef HIDDEVICE_H
#define HIDDEVICE_H


#include <windows.h>

class cHIDDevice
{
public:
	cHIDDevice();
	~cHIDDevice();
	bool Disconnect();
	bool Connect(unsigned short device_id, unsigned short vendor_id, int index=0);
	bool IsConnected() const {return mConnected;}
	int GetDevices(unsigned short device_id, unsigned short vendor_id, const unsigned char * mOutputBuffer, const int mOutputBufferSize);

	bool WriteToDevice(unsigned const char * buffer, int num_bytes);
	bool ReadFromDevice(unsigned const char * buffer, int max_bytes, int & bytes_read, int timeout=50);
private:

	bool OpenDevice(int index, unsigned short device_id, unsigned short vendor_id);

	HANDLE mHandle;
	HANDLE mEvent;
	OVERLAPPED mOverlapped;
	bool mConnected;

};

#define STACK_UNKNOWN 0
#define STACK_MS 1
#define STACK_BLUESOLEIL 2


#endif
