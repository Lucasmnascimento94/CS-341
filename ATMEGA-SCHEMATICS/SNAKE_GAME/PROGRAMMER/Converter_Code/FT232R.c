#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ftd2xx.h>

// typedef struct EEPROM_FT232R {
//     // Common header
//     // FT_EEPROM_HEADER common; // common elements for all device EEPROMs
//     //-------------------------------------------------------------------//
//     // Drive options
//     UCHAR IsHighCurrent; // non-zero if interface is high current
//     //-------------------------------------------------------------------//
//     // Hardware options
//     UCHAR UseExtOsc; // Use External Oscillator
//     UCHAR InvertTXD; // non-zero if invert TXD
//     UCHAR InvertRXD; // non-zero if invert RXD
//     UCHAR InvertRTS; // non-zero if invert RTS
//     UCHAR InvertCTS; // non-zero if invert CTS
//     UCHAR InvertDTR; // non-zero if invert DTR
//     UCHAR InvertDSR; // non-zero if invert DSR
//     UCHAR InvertDCD; // non-zero if invert DCD
//     UCHAR InvertRI; // non-zero if invert RI
//     UCHAR Cbus0; // Cbus Mux control
//     UCHAR Cbus1; // Cbus Mux control
//     UCHAR Cbus2; // Cbus Mux control
//     UCHAR Cbus3; // Cbus Mux control
//     UCHAR Cbus4; // Cbus Mux control
//     //-------------------------------------------------------------------//
//     // Driver option
//     UCHAR DriverType; // non-zero if using D2XX driver
// } EEPROM_FT232R, *P_EEPROM_FT232R;


// #define FT_BAUD_300			300
// #define FT_BAUD_600			600
// #define FT_BAUD_1200		1200
// #define FT_BAUD_2400		2400
// #define FT_BAUD_4800		4800
// #define FT_BAUD_9600		9600
// #define FT_BAUD_14400		14400
// #define FT_BAUD_19200		19200
// #define FT_BAUD_38400		38400
// #define FT_BAUD_57600		57600
// #define FT_BAUD_115200		115200
// #define FT_BAUD_230400		230400
// #define FT_BAUD_460800		460800
// #define FT_BAUD_921600		921600


// #define FT_232R_CBUS_TXDEN					0x00	//	Tx Data Enable
// #define FT_232R_CBUS_PWRON					0x01	//	Power On
// #define FT_232R_CBUS_RXLED					0x02	//	Rx LED
// #define FT_232R_CBUS_TXLED					0x03	//	Tx LED
// #define FT_232R_CBUS_TXRXLED				0x04	//	Tx and Rx LED
// #define FT_232R_CBUS_SLEEP					0x05	//	Sleep
// #define FT_232R_CBUS_CLK48					0x06	//	48MHz clock
// #define FT_232R_CBUS_CLK24					0x07	//	24MHz clock
// #define FT_232R_CBUS_CLK12					0x08	//	12MHz clock
// #define FT_232R_CBUS_CLK6					0x09	//	6MHz clock
// #define FT_232R_CBUS_IOMODE					0x0A	//	IO Mode for CBUS bit-bang
// #define FT_232R_CBUS_BITBANG_WR				0x0B	//	Bit-bang write strobe
// #define FT_232R_CBUS_BITBANG_RD				0x0C	//	Bit-bang read strobe
// #define FT_232R_CBUS0_RXF					0x0D	//	CBUS0 RXF#
// #define FT_232R_CBUS1_TXE					0x0D	//	CBUS1 TXE#
// #define FT_232R_CBUS2_RD					0x0D	//	CBUS2 RD#
// #define FT_232R_CBUS3_WR					0x0D	//	CBUS3 WR#

int main() {
    FT_STATUS ftStatus;
    DWORD numDevices;
    DWORD devIndex = 0; // Index of the device to open
    char DeviceName[32]; // Device name 
    char Description[64]; // Device description
    char* DescriptionPtr[2]; // Buffer for the device description
    
    // Initialize the device description pointer
    DescriptionPtr[0] = Description;
    DescriptionPtr[1] = NULL; // Null terminator

    // Device information list
    FT_DEVICE_LIST_INFO_NODE *devInfo;


    // Open the device
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus1;

    // Device EEPROM Data in bits
    char Manufacture[64]; // Manufacturer name
    char ManufacturerId[64]; // Manufacturer ID
    char EEPROMDescription[64]; // Description
    char SerialNumber[64]; // Serial Number

    WORD MaxPower; // Max Power
    WORD PnP; // PnP
    WORD SelfPowered; // Self Powered
    WORD RemoteWakeup; // Remote Wakeup
    UCHAR UseExtOsc;			/// Use External Oscillator
	UCHAR HighDriveIOs;			/// High Drive I/Os
	UCHAR EndpointSize;			/// Endpoint size
	UCHAR PullDownEnableR;		/// non-zero if pull down enabled
	UCHAR SerNumEnableR;		/// non-zero if serial number to be used
	UCHAR InvertTXD;			/// non-zero if invert TXD
	UCHAR InvertRXD;			/// non-zero if invert RXD
	UCHAR InvertRTS;			/// non-zero if invert RTS
	UCHAR InvertCTS;			/// non-zero if invert CTS
	UCHAR InvertDTR;			/// non-zero if invert DTR
	UCHAR InvertDSR;			/// non-zero if invert DSR
	UCHAR InvertDCD;			/// non-zero if invert DCD
	UCHAR InvertRI;				/// non-zero if invert RI
	UCHAR Cbus0;				/// Cbus Mux control
	UCHAR Cbus1;				/// Cbus Mux control
	UCHAR Cbus2;				/// Cbus Mux control
	UCHAR Cbus3;				/// Cbus Mux control
	UCHAR Cbus4;				/// Cbus Mux control
	UCHAR RIsD2XX;				/// non-zero if using D2XX driver
 

    FT_EEPROM_HEADER ft_eeprom_header;
    ft_eeprom_header.deviceType = FT_DEVICE_232R;
    FT_EEPROM_232R ft_eeprom_232r;
    ft_eeprom_232r.common = ft_eeprom_header;
    ft_eeprom_232r.common.deviceType = FT_DEVICE_232R;

    // ftStatus = FT_ListDevices(&numDevices, NULL, FT_LIST_NUMBER_ONLY);
    // if (ftStatus == FT_OK) {
    //     printf("Found %d FTDI devices\n", numDevices);
    // } else {
    //     printf("Error: Could not get device count. Error code: %d\n", ftStatus);
    //     return 1;
    // }

    // ftStatus = FT_ListDevices((PVOID)devIndex, DeviceName, FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER);
    // if (ftStatus == FT_OK) {
    //     printf("Found device: %s\n", DeviceName);
    // } else {
    //     printf("Error: Could not get device. Error code: %d\n", ftStatus);
    //     return 1;
    // }


    // Create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevices);
    if (ftStatus == FT_OK) {
        printf("--------------------------------\n");
        printf("Number of devices: %d\n", numDevices);
        printf("--------------------------------\n");
        // Allocate storage for the device information list
        devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevices);
        if (devInfo == NULL) {
            printf("Error: Could not allocate memory for device information list. Error code: %d\n", ftStatus);
            return 1;
        }
        // Get the device information list
        ftStatus = FT_GetDeviceInfoList(devInfo, &numDevices);
        if (ftStatus == FT_OK) {
            printf("Device %d\n", devIndex);
            printf("Flags = 0x%x\n", devInfo[devIndex].Flags);
            printf("Type = 0x%x\n", devInfo[devIndex].Type);
            printf("ID = 0x%x\n", devInfo[devIndex].ID);
            printf("LocId = 0x%x\n", devInfo[devIndex].LocId);
            printf("SerialNumber = %s\n", devInfo[devIndex].SerialNumber);
            printf("Description = %s\n", devInfo[devIndex].Description);
        } else {
            printf("Error: Could not get device information list. Error code: %d\n", ftStatus);
            return 1;
        }
    } else {
        printf("Error: Could not create device information list. Error code: %d\n", ftStatus);
        return 1;
    }

    // Open the device
    ftStatus1 = FT_OpenEx((PVOID) "A50285BI", FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
    if (ftStatus1 == FT_OK) {
        printf("--------------------------------\n");
        // Device EEPROM data
        FT_PROGRAM_DATA ftData;
        char ManufactureBuf[32];
        char ManufacturerIdBuf[16]; 
        char DescriptionBuf[64]; 
        char SerialNumberBuf[16]; 
        ftData.Signature1 = 0x00000000; // Header - must be 0x00000000
        ftData.Signature2 = 0xFFFFFFFF; // Header - must be 0xffffffff
        ftData.Version = 0x00000002; // FT232R extensions
        ftData.VendorId = 0x0403; // Vendor ID - 0x0403
        ftData.ProductId = 0x6001; // Product ID - 0x6001
        ftData.Manufacturer = ManufactureBuf;
        ftData.ManufacturerId = ManufacturerIdBuf;
        ftData.Description = DescriptionBuf;
        ftData.SerialNumber = SerialNumberBuf;
        printf("Device: %s opened successfully\n", devInfo[devIndex].Description);
        
        // Read Opened Device EEPROM
        ftStatus1 = FT_EE_Read(ftHandle, &ftData);
        if (ftStatus1 == FT_OK) {
            // Print all EEPROM contents
            printf("--------------------------------\n");
            printf("EEPROM Contents:\n");
            printf("--------------------------------\n");
            // Basic EEPROM contents
            printf("[Basic EEPROM Contents:]\n");
            printf("Signature1 = 0x%04x\n", ftData.Signature1);
            printf("Signature2 = 0x%04x\n", ftData.Signature2);
            printf("Version = 0x%04x\n", ftData.Version);
            printf("VendorId = 0x%04x\n", ftData.VendorId);
            printf("ProductId = 0x%04x\n", ftData.ProductId);
            printf("Manufacturer = %s\n", ftData.Manufacturer);
            printf("ManufacturerId = %s\n", ftData.ManufacturerId);
            printf("Description = %s\n", ftData.Description);
            printf("SerialNumber = %s\n", ftData.SerialNumber);
            printf("MaxPower = %d mA\n", ftData.MaxPower);
            printf("PnP = %d\n", ftData.PnP);
            printf("SelfPowered = %d\n", ftData.SelfPowered);
            printf("RemoteWakeup = %d\n", ftData.RemoteWakeup);
            printf("--------------------------------\n");

            /// Rev 6 (FT232R) extensions
            printf("[FT232R Specific Settings:]\n");
            printf("UseExtOsc = %d\n", ftData.UseExtOsc);
            printf("HighDriveIOs = %d\n", ftData.HighDriveIOs);
            printf("EndpointSize = %d\n", ftData.EndpointSize);
            printf("PullDownEnableR = %d\n", ftData.PullDownEnableR);
            printf("SerNumEnableR = %d\n", ftData.SerNumEnableR);
            printf("--------------------------------\n");

            /// Signal Settings
            printf("[Signal Settings:]\n");
            printf("InvertTXD = %d\n", ftData.InvertTXD);
            printf("InvertRXD = %d\n", ftData.InvertRXD);
            printf("InvertRTS = %d\n", ftData.InvertRTS);
            printf("InvertCTS = %d\n", ftData.InvertCTS);
            printf("InvertDTR = %d\n", ftData.InvertDTR);
            printf("InvertDSR = %d\n", ftData.InvertDSR);
            printf("InvertDCD = %d\n", ftData.InvertDCD);
            printf("InvertRI = %d\n", ftData.InvertRI);
            printf("--------------------------------\n");

            /// Cbus Settings
            printf("[Cbus Settings:]\n");
            printf("Cbus0 = %d\n", ftData.Cbus0);
            printf("Cbus1 = %d\n", ftData.Cbus1);
            printf("Cbus2 = %d\n", ftData.Cbus2);
            printf("Cbus3 = %d\n", ftData.Cbus3);
            printf("Cbus4 = %d\n", ftData.Cbus4);
            printf("RIsD2XX = %d\n", ftData.RIsD2XX);
            printf("--------------------------------\n");
        
            // Read Device EEPROM Using FT_EEPROM_Read
            ftStatus1 = FT_EEPROM_Read(ftHandle, 
                &ft_eeprom_232r, 
                sizeof(FT_EEPROM_232R), 
                Manufacture, 
                ManufacturerId, 
                EEPROMDescription, 
                SerialNumber);
            if (ftStatus1 == FT_OK) {
                /// EEPROM contents
                printf("--------------------------------\n");
                printf("[Results:]\n");
                printf("--------------------------------\n");
                printf("Vendor ID = 0x%04x\n", ft_eeprom_232r.common.VendorId);
                printf("Product ID = 0x%04x\n", ft_eeprom_232r.common.ProductId);
                printf("Serial Number Enable = %d\n", ft_eeprom_232r.common.SerNumEnable);
                printf("Max Power = %d mA\n", ft_eeprom_232r.common.MaxPower);
                printf("Self Powered = %d\n", ft_eeprom_232r.common.SelfPowered);
                printf("Remote Wakeup = %d\n", ft_eeprom_232r.common.RemoteWakeup);
                printf("Pull Down Enable = %d\n", ft_eeprom_232r.common.PullDownEnable);
                printf("--------------------------------\n");
                
                /// Settings
                printf("[Settings:]\n");
                printf("Is High Current = %d\n", ft_eeprom_232r.IsHighCurrent);
                printf("Use External Oscillator = %d\n", ft_eeprom_232r.UseExtOsc);
                printf("Invert TXD = %d\n", ft_eeprom_232r.InvertTXD);
                printf("Invert RXD = %d\n", ft_eeprom_232r.InvertRXD);
                printf("Invert RTS = %d\n", ft_eeprom_232r.InvertRTS);
                printf("Invert CTS = %d\n", ft_eeprom_232r.InvertCTS);
                printf("Invert DTR = %d\n", ft_eeprom_232r.InvertDTR);
                printf("Invert DSR = %d\n", ft_eeprom_232r.InvertDSR);
                printf("Invert DCD = %d\n", ft_eeprom_232r.InvertDCD);
                printf("Invert RI = %d\n", ft_eeprom_232r.InvertRI);
                printf("--------------------------------\n");
                
                /// CBUS settings
                printf("[CBUS Settings:]\n");
                printf("CBUS0 = 0x%02x\n", ft_eeprom_232r.Cbus0);
                printf("CBUS1 = 0x%02x\n", ft_eeprom_232r.Cbus1);
                printf("CBUS2 = 0x%02x\n", ft_eeprom_232r.Cbus2);
                printf("CBUS3 = 0x%02x\n", ft_eeprom_232r.Cbus3);
                printf("CBUS4 = 0x%02x\n", ft_eeprom_232r.Cbus4);
                printf("Driver Type = %d\n", ft_eeprom_232r.DriverType);
                printf("--------------------------------\n");
                
                /// String data
                printf("String Data:\n");
                printf("Manufacturer = %s\n", Manufacture);
                printf("Manufacturer ID = %s\n", ManufacturerId);
                printf("Description = %s\n", EEPROMDescription);
                printf("Serial Number = %s\n", SerialNumber);
                printf("--------------------------------\n");

            } else {
                printf("Error: Could not read device EEPROM. Error code: %d\n", ftStatus1);
                return 1;
            }
        } else {
            printf("Error: Could not read device EEPROM. Error code: %d\n", ftStatus1);
            return 1;
        }
    } else {
        printf("Error: Could not open device. Error code: %d\n", ftStatus1);
        return 1;
    }

    // Close the device
    ftStatus = FT_Close(ftHandle);
    if (ftStatus == FT_OK) {
        printf("Device closed successfully\n");
        printf("--------------------------------\n");
    } else {
        printf("Error: Could not close device. Error code: %d\n", ftStatus);
        return 1;
    }

    return 0;
}