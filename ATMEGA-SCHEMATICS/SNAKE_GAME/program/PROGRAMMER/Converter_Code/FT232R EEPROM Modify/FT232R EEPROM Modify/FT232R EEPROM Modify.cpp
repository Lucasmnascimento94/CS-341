// FT232R EEPROM Modify.cpp : Defines the entry point for the console application.
//
// NOTE:	This code is provided as an example only and is not supported or guaranteed by FTDI.
//			It is the responsibility of the recipient/user to ensure the correct operation of 
//			any software which is created based upon this example.

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "ftd2xx.h"

int main()
{
	//********************************************************
	//Definitions
	//********************************************************

	FT_HANDLE fthandle;
	FT_STATUS status;

	BOOLEAN Dev_Found = FALSE;

	FT_PROGRAM_DATA ftData;

	WORD VendorIdBuf = 0x0403;
	WORD ProductIdBuf = 0x6001;
	char ManufacturerBuf[32];
	char ManufacturerIdBuf[16];
	char DescriptionBuf[64];
	char SerialNumberBuf[16];

	ftData.Signature1 = 0x00000000;		// Always 0x00000000
	ftData.Signature2 = 0xffffffff;		// Always 0xFFFFFFFF
	ftData.Version = 2;	// Header - FT_PROGRAM_DATA version 0 = original (FT232B), 1 = FT2232 extensions, 2 = FT232R extensions, 3 = FT2232H extensions, 4 = FT4232H extensions, 5 = FT232H extensions

	ftData.VendorId = VendorIdBuf;
	ftData.ProductId = ProductIdBuf;
	ftData.Manufacturer = ManufacturerBuf;
	ftData.ManufacturerId = ManufacturerIdBuf;
	ftData.Description = DescriptionBuf;
	ftData.SerialNumber = SerialNumberBuf;

	ftData.MaxPower;
	ftData.PnP;
	ftData.SelfPowered;
	ftData.RemoteWakeup;
	//'R features require section below
	ftData.UseExtOsc; // Use External Oscillator
	ftData.HighDriveIOs; // High Drive I/Os
	ftData.EndpointSize; // Endpoint size
	ftData.PullDownEnableR; // non-zero if pull down enabled
	ftData.SerNumEnableR; // non-zero if serial number to be used
	ftData.InvertTXD; // non-zero if invert TXD
	ftData.InvertRXD; // non-zero if invert RXD
	ftData.InvertRTS; // non-zero if invert RTS
	ftData.InvertCTS; // non-zero if invert CTS
	ftData.InvertDTR; // non-zero if invert DTR
	ftData.InvertDSR; // non-zero if invert DSR
	ftData.InvertDCD; // non-zero if invert DCD
	ftData.InvertRI; // non-zero if invert RI
	ftData.Cbus0; // Cbus Mux control
	ftData.Cbus1; // Cbus Mux control
	ftData.Cbus2; // Cbus Mux control
	ftData.Cbus3; // Cbus Mux control
	ftData.Cbus4; // Cbus Mux control
	ftData.RIsD2XX; // non-zero if using D2XX drivers

	//********************************************************
	//List Devices
	//********************************************************

	FT_DEVICE_LIST_INFO_NODE *devInfo;
	DWORD numDevs;
	DWORD i;

	// create the device information list 
	status = FT_CreateDeviceInfoList(&numDevs);

	if (status != FT_OK) {
		printf("FT_CreateDeviceInfoList status not ok %d\n", status);
		return 0;
	}
	else
	{
		printf("Number of devices is %d\n", numDevs);
		if (numDevs > 0) {
			// allocate storage for list based on numDevs 
			devInfo =
				(FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
			// get the device information list 
			status = FT_GetDeviceInfoList(devInfo, &numDevs);
			if (status == FT_OK) {
				for (i = 0; i < numDevs; i++) {
					printf("Dev %d:\n", i);
					printf("Flags=0x%x\n", devInfo[i].Flags);
					printf("Type=0x%x\n", devInfo[i].Type);
					printf("ID=0x%x\n", devInfo[i].ID);
					printf("LocId=0x%x\n", devInfo[i].LocId);
					printf("SerialNumber=%s\n", devInfo[i].SerialNumber);
					printf("Description=%s\n", devInfo[i].Description);
					printf("\n");
				}
			}
		}
	}

	//********************************************************
	//Open the port
	//********************************************************

	for (i = 0; i < numDevs; i++)
	{
		if ((Dev_Found == FALSE) && (devInfo[i].Type == FT_DEVICE_232R))
		{
			Dev_Found = TRUE;

			status = FT_OpenEx("USB <-> Serial Converter", FT_OPEN_BY_DESCRIPTION, &fthandle);

			if (status != FT_OK)
			{
				printf("Open status not ok %d\n", status);
				printf("Trying to open default EEPROM device...\n");
				status = FT_OpenEx("FT232R USB UART", FT_OPEN_BY_DESCRIPTION, &fthandle);
				if (status != FT_OK)
				{
					printf("Open status not ok %d\n", status);
					printf("\n");
					return 0;
				}
				else
				{
					printf("Open status OK %d\n", status);
					printf("\n");
				}

				printf("\n");
			}
			else
			{
				printf("Open status OK %d\n", status);
				printf("\n");
			}


			//********************************************************
			//Read the EEPROM
			//********************************************************

			status = FT_EE_Read(fthandle, &ftData);

			if (status != FT_OK)
			{
				printf("EE_Read status not ok %d\n", status);
				return 0;
			}
			else
			{
				printf("Signature1 =  0x%04x\n", ftData.Signature1);
				printf("Signature2 =  0x%04x\n", ftData.Signature2);
				printf("Version =  0x%04x\n", ftData.Version);


				printf("VendorID =  0x%04x\n", ftData.VendorId);
				printf("ProductID =  0x%04x\n", ftData.ProductId);
				printf("Manufacturer =  %s\n", ftData.Manufacturer);
				printf("ManufacturerID =  %s\n", ftData.ManufacturerId);
				printf("Description =  %s\n", ftData.Description);
				printf("SerialNumber =  %s\n", ftData.SerialNumber);
				printf("MaxPower =  %d\n", ftData.MaxPower);
				printf("PnP =  %x\n", ftData.PnP);
				printf("SelfPowered =  %x\n", ftData.SelfPowered);
				printf("RemoteWakeup =  %x\n", ftData.RemoteWakeup);

				printf("UseExtOsc =  %x\n", ftData.UseExtOsc);
				printf("HighDriveIOs =  %x\n", ftData.HighDriveIOs);
				printf("EndpointSize =  %x\n", ftData.EndpointSize);
				printf("PullDownEnableR =  %x\n", ftData.PullDownEnableR);
				printf("SerNumEnableR =  %x\n", ftData.SerNumEnableR);
				printf("InvertTXD =  %x\n", ftData.InvertTXD);
				printf("InvertRXD =  %x\n", ftData.InvertRXD);
				printf("InvertRTS =  %x\n", ftData.InvertRTS);
				printf("InvertCTS =  %x\n", ftData.InvertCTS);
				printf("InvertDTR =  %x\n", ftData.InvertDTR);
				printf("InvertDSR =  %x\n", ftData.InvertDSR);
				printf("InvertDCD =  %x\n", ftData.InvertDCD);
				printf("InvertRI =  %x\n", ftData.InvertRI);
				printf("CBUS0 =  %x\n", ftData.Cbus0);
				printf("CBUS1 =  %x\n", ftData.Cbus1);
				printf("CBUS2 =  %x\n", ftData.Cbus2);
				printf("CBUS3 =  %x\n", ftData.Cbus3);
				printf("CBUS4 =  %x\n", ftData.Cbus4);
				printf("RIsD2XX =  %x\n", ftData.RIsD2XX);
				printf("\n");
			}

			//********************************************************
			//change serial number from one that was read.
			//********************************************************

			ftData.SerialNumber = "FT12345";

			status = FT_EE_Program(fthandle, &ftData);

			if (status != FT_OK)
			{
				printf("EE_Program status not ok %d\n", status);
				return 0;
			}
			else
			{
				printf("EE_Program status ok %d\n", status);
				printf("\n");
			}


			//********************************************************
			// Delay
			//********************************************************

			Sleep(1000);


			//********************************************************
			// Re - Read
			//********************************************************


			ftData.SerialNumber = SerialNumberBuf; //Reset to variable

			status = FT_EE_Read(fthandle, &ftData);

			if (status != FT_OK)
			{
				printf("EE_Read status not ok %d\n", status);
				return 0;
			}
			else
			{
				printf("Signature1 =  0x%04x\n", ftData.Signature1);
				printf("Signature2 =  0x%04x\n", ftData.Signature2);
				printf("Version =  0x%04x\n", ftData.Version);


				printf("VendorID =  0x%04x\n", ftData.VendorId);
				printf("ProductID =  0x%04x\n", ftData.ProductId);
				printf("Manufacturer =  %s\n", ftData.Manufacturer);
				printf("ManufacturerID =  %s\n", ftData.ManufacturerId);
				printf("Description =  %s\n", ftData.Description);
				printf("SerialNumber =  %s\n", ftData.SerialNumber);
				printf("MaxPower =  %d\n", ftData.MaxPower);
				printf("PnP =  %x\n", ftData.PnP);
				printf("SelfPowered =  %x\n", ftData.SelfPowered);
				printf("RemoteWakeup =  %x\n", ftData.RemoteWakeup);

				printf("UseExtOsc =  %x\n", ftData.UseExtOsc);
				printf("HighDriveIOs =  %x\n", ftData.HighDriveIOs);
				printf("EndpointSize =  %x\n", ftData.EndpointSize);
				printf("PullDownEnableR =  %x\n", ftData.PullDownEnableR);
				printf("SerNumEnableR =  %x\n", ftData.SerNumEnableR);
				printf("InvertTXD =  %x\n", ftData.InvertTXD);
				printf("InvertRXD =  %x\n", ftData.InvertRXD);
				printf("InvertRTS =  %x\n", ftData.InvertRTS);
				printf("InvertCTS =  %x\n", ftData.InvertCTS);
				printf("InvertDTR =  %x\n", ftData.InvertDTR);
				printf("InvertDSR =  %x\n", ftData.InvertDSR);
				printf("InvertDCD =  %x\n", ftData.InvertDCD);
				printf("InvertRI =  %x\n", ftData.InvertRI);
				printf("CBUS0 =  %x\n", ftData.Cbus0);
				printf("CBUS1 =  %x\n", ftData.Cbus1);
				printf("CBUS2 =  %x\n", ftData.Cbus2);
				printf("CBUS3 =  %x\n", ftData.Cbus3);
				printf("CBUS4 =  %x\n", ftData.Cbus4);
				printf("RIsD2XX =  %x\n", ftData.RIsD2XX);
				printf("\n");
			}

			//*****************************************************
			//Close the port
			//*****************************************************

			// Close the device
			status = FT_Close(fthandle);

		}

	}

	printf("Press Return To End Program");
	getchar();
	printf("closed \n");
	
    return 0;
}

