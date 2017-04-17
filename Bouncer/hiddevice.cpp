//cWiimote 0.2 by Kevin Forbes (http://simulatedcomicproduct.com)
//improved by Robert Jäger (http://www.firoball.de)
//This code is public domain, and comes with no warranty. The user takes full responsibility for anything that happens as a result from using this code.

//  This was based in part on Alan Macek <www.alanmacek.com>'s USB interface library

#include "hiddevice.h"
extern "C" 
{
	#include "hidsdi.h"
//	#include "Setupapi.h"
//	#include <hidsdi.h>
	#include <Setupapi.h>
}
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")

cHIDDevice::cHIDDevice() : mConnected(false), mHandle(NULL), mEvent(NULL)
{

}


cHIDDevice::~cHIDDevice()
{
	if (mConnected)
	{
		Disconnect();
	}
}

bool cHIDDevice::Disconnect()
{
	bool retval = false;
	if (mConnected)
	{
		retval = (CloseHandle(mHandle) == TRUE && CloseHandle(mEvent) == TRUE);
	
		mConnected = false;
	}

	return retval;
}

bool cHIDDevice::Connect(unsigned short device_id, unsigned short vendor_id, int index)
{
	if (mConnected)
	{
		if (!Disconnect())
		{
			return false;
		}
	}

	mConnected = OpenDevice(index, device_id, vendor_id);
	return mConnected;
}

int cHIDDevice::GetDevices(unsigned short device_id, unsigned short vendor_id, const unsigned char * mOutputBuffer, const int mOutputBufferSize)
{
	int dev_index = 0;
	for (;;)
	{
		if (!OpenDevice(dev_index, device_id, vendor_id))
			break;

		CloseHandle(mHandle);
		CloseHandle(mEvent);	
		dev_index++;
	}
	return dev_index;
}

bool cHIDDevice::OpenDevice(int index, unsigned short device_id, unsigned short vendor_id)
{
	bool retval = false;
	int matching_devs_found = 0;
	int dev_ctr = -1;
	GUID guid;
	HIDD_ATTRIBUTES attrib;
    HidD_GetHidGuid (&guid);
	HDEVINFO devinfo = SetupDiGetClassDevs(&guid,NULL,NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	SP_DEVICE_INTERFACE_DATA dev_int_data;
	dev_int_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	while (matching_devs_found != index + 1)
	{
		dev_ctr++;
		if (SetupDiEnumDeviceInterfaces(devinfo, NULL, &guid, dev_ctr, &dev_int_data) == TRUE)
		{
			DWORD size;
			SetupDiGetDeviceInterfaceDetail (devinfo, &dev_int_data, NULL, 0, &size,0);
		
			PSP_INTERFACE_DEVICE_DETAIL_DATA detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(size);
			detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		
			if (SetupDiGetDeviceInterfaceDetail(devinfo, &dev_int_data, detail, size, NULL, NULL))
			{
				mHandle = CreateFile (detail->DevicePath, 
									GENERIC_READ | GENERIC_WRITE,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									NULL,
									OPEN_EXISTING, 
									FILE_FLAG_OVERLAPPED, 
									NULL);
				free(detail);
				if (mHandle == INVALID_HANDLE_VALUE)
				{
					CloseHandle(mHandle);		
					continue;
				}
				if(!HidD_GetAttributes (mHandle, &attrib))
				{
					CloseHandle(mHandle);		
					continue;
				}
				if (attrib.ProductID == device_id &&
					attrib.VendorID == vendor_id)
				{
					mEvent = CreateEvent(NULL, TRUE, TRUE, "");
					mOverlapped.Offset = 0;
					mOverlapped.OffsetHigh = 0;
					mOverlapped.hEvent = mEvent;
					matching_devs_found++;
				}
				else
				{
					CloseHandle(mHandle);		
					continue;
				}
			}
			else
			{
				free(detail);
				continue;
			}
		}
		else
			break;
	}
	retval = (matching_devs_found == index + 1);
		
	SetupDiDestroyDeviceInfoList (devinfo);
	return retval;
}
 
bool cHIDDevice::WriteToDevice(unsigned const char * buffer, int num_bytes)
{
	bool retval = false;
	DWORD bytes_written;
    static int mStack = STACK_UNKNOWN;

	if (mConnected)
	{

		switch(mStack)
		{
            
			default:
			case STACK_UNKNOWN:
				/* try to auto-detect the stack type */
				retval = (WriteFile( mHandle, buffer,num_bytes,&bytes_written,&mOverlapped) == TRUE);
				retval = retval && (bytes_written == num_bytes);
				if (retval)
				{
					/* bluesoleil will always return 1 here, even if it's not connected */
					mStack = STACK_BLUESOLEIL;
					break;
				}

				retval = (HidD_SetOutputReport( mHandle, (void*)buffer, num_bytes) == TRUE);
				if (retval)
				{
					mStack = STACK_MS;
					break;
				}
				break;

			case STACK_BLUESOLEIL:
				retval = (WriteFile( mHandle, buffer,num_bytes,&bytes_written,&mOverlapped) == TRUE);
				retval = retval && (bytes_written == num_bytes);
				break;
		        
			case STACK_MS:
				retval = (HidD_SetOutputReport( mHandle, (void*)buffer, num_bytes) == TRUE);
				break;
		}
	}
	return retval;

}

bool cHIDDevice::ReadFromDevice(unsigned const char * buffer, int max_bytes, int & bytes_read, int timeout)
{
	bool retval = false;
	if (mConnected)
	{
		ReadFile( mHandle, (LPVOID)buffer,max_bytes,(LPDWORD)&bytes_read,&mOverlapped); 
		DWORD Result = WaitForSingleObject(mEvent, timeout);
		if (Result == WAIT_OBJECT_0) 
		{		
			retval = true;
		}
		else 
		{
			CancelIo(mHandle);			
		}
		ResetEvent(mEvent);
	}
	return retval;
}

