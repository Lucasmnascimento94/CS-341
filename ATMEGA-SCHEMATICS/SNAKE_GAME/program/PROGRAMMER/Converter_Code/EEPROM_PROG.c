#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "./linux-x86_64/ftd2xx.h"

// CBUS function definitions
#define FT_232R_CBUS_TXDEN     0x00    // Tx Data Enable
#define FT_232R_CBUS_PWRON     0x01    // Power On
#define FT_232R_CBUS_RXLED     0x02    // Rx LED
#define FT_232R_CBUS_TXLED     0x03    // Tx LED
#define FT_232R_CBUS_TXRXLED   0x04    // Tx and Rx LED
#define FT_232R_CBUS_SLEEP     0x05    // Sleep
#define FT_232R_CBUS_CLK48     0x06    // 48MHz clock
#define FT_232R_CBUS_CLK24     0x07    // 24MHz clock
#define FT_232R_CBUS_CLK12     0x08    // 12MHz clock
#define FT_232R_CBUS_CLK6      0x09    // 6MHz clock
#define FT_232R_CBUS_IOMODE    0x0A    // IO Mode for CBUS bit-bang
#define FT_232R_CBUS_BITBANG_WR 0x0B   // Bit-bang write strobe
#define FT_232R_CBUS_BITBANG_RD 0x0C   // Bit-bang read strobe

int main() {
    FT_STATUS ftStatus;
    DWORD numDevices;
    //DWORD devIndex = 0;
    //char DeviceName[32];
    //char Description[64];
    //char* DescriptionPtr[2];
    
    // Initialize the device description pointer
    //DescriptionPtr[0] = Description;
    //DescriptionPtr[1] = NULL;

    // Device information list
    FT_DEVICE_LIST_INFO_NODE *devInfo;

    // Open the device
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus1;

    // EEPROM data structure
    FT_EEPROM_232R ft_eeprom_232r;
    
    // Initialize the structure to zero first (GPT SUGGESTED)
    memset(&ft_eeprom_232r, 0, sizeof(FT_EEPROM_232R));
    
    // Set the device type (GPT SUGGESTED)
    ft_eeprom_232r.common.deviceType = FT_DEVICE_232R;
    
    // Set required common header fields 
    ft_eeprom_232r.common.VendorId = 0x0403;        // Vendor ID 
    ft_eeprom_232r.common.ProductId = 0x6001;       // Product ID
    ft_eeprom_232r.common.MaxPower = 90;            
    ft_eeprom_232r.common.PullDownEnable = 0;       
    ft_eeprom_232r.common.RemoteWakeup = 0;         
    ft_eeprom_232r.common.SerNumEnable = 1;         // Enable serial number
    ft_eeprom_232r.common.SelfPowered = 1;         // Self powered
    
    // Set FT232R specific fields (GPT SUGGESTED)
    ft_eeprom_232r.IsHighCurrent = 0;               // Not high current
    ft_eeprom_232r.UseExtOsc = 0;                   // Use internal oscillator
    ft_eeprom_232r.DriverType = 0;                 // Use VCP driver (0) or D2XX driver (1)

    // String data for programming
    char Manufacture[64];
    char ManufacturerId[64];
    char EEPROMDescription[64];
    char SerialNumber[64];

    printf("--------------------------------\n");
    printf("Configuring EEPROM...\n");
    printf("--------------------------------\n");

    // Create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevices);
    if (ftStatus == FT_OK) {
        printf("Number of devices: %d\n", numDevices);
        printf("--------------------------------\n");
        
        // Allocate storage for the device information list
        devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevices);
        if (devInfo == NULL) {
            printf("Error: Could not allocate memory for device information list.\n");
            return 1;
        }
        
        // Get the device information list
        ftStatus = FT_GetDeviceInfoList(devInfo, &numDevices);
        if (ftStatus != FT_OK) {
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
        printf("Device opened successfully\n");
        printf("--------------------------------\n");

        // Initialize string buffers to zero first (GPT SUGGESTED)
        memset(Manufacture, 0, sizeof(Manufacture));
        memset(ManufacturerId, 0, sizeof(ManufacturerId));
        memset(EEPROMDescription, 0, sizeof(EEPROMDescription));
        memset(SerialNumber, 0, sizeof(SerialNumber));
        
        // Copy strings safely (GPT SUGGESTED)
        strncpy(Manufacture, "FTDI", sizeof(Manufacture) - 1);
        strncpy(ManufacturerId, "FT", sizeof(ManufacturerId) - 1);
        strncpy(EEPROMDescription, "Custom FT232R Device", sizeof(EEPROMDescription) - 1);
        strncpy(SerialNumber, "A50285BI", sizeof(SerialNumber) - 1);

        
        
        printf("Configure settings\n");
        
        printf("Enable InvertTXD\n");
        ft_eeprom_232r.InvertTXD = 1;

        printf("Enable InvertRXD\n");
        ft_eeprom_232r.InvertRXD = 1;

        printf("Enable InvertRTS\n");
        ft_eeprom_232r.InvertRTS = 1;

        printf("Enable InvertCTS\n");
        ft_eeprom_232r.InvertCTS = 1;

        printf("Enable CBUS0\n");
        ft_eeprom_232r.Cbus0 = FT_232R_CBUS_CLK6; // 6MHz clock 0x09

        printf("Enable CBUS1\n");
        ft_eeprom_232r.Cbus1 = FT_232R_CBUS_TXLED; // Tx LED 0x03

        printf("Enable CBUS2\n");
        ft_eeprom_232r.Cbus2 = FT_232R_CBUS_RXLED; // Rx LED 0x02

        printf("Enable CBUS3\n");
        ft_eeprom_232r.Cbus3 = FT_232R_CBUS_IOMODE; // IO Mode for CBUS bit-bang 0x0A

        printf("Enable CBUS4\n");
        ft_eeprom_232r.Cbus4 = FT_232R_CBUS_SLEEP; // Sleep 0x05

        printf("--------------------------------\n");
        
        // Reset device before programming (GPT SUGGESTED)
        printf("Resetting device before programming...\n");
        ftStatus = FT_ResetDevice(ftHandle);
        if (ftStatus != FT_OK) {
            printf("Warning: Could not reset device. Error code: %d\n", ftStatus);
        }
        
        // Program EEPROM
        printf("Programming EEPROM...\n");
        ftStatus1 = FT_EEPROM_Program(ftHandle, &ft_eeprom_232r, sizeof(ft_eeprom_232r), Manufacture, ManufacturerId, EEPROMDescription, SerialNumber);
        if (ftStatus1 == FT_OK) {
            printf("EEPROM programmed successfully\n");
            printf("Manufacturer: %s\n", Manufacture);
            printf("Manufacturer ID: %s\n", ManufacturerId);
            printf("Description: %s\n", EEPROMDescription);
            printf("Serial Number: %s\n", SerialNumber);

            printf("--------------------------------\n");
            printf("New Settings:\n");
            printf("--------------------------------\n");
            printf("Self Powered: %d\n", ft_eeprom_232r.common.SelfPowered);
            printf("Serial Number: %d\n", ft_eeprom_232r.common.SerNumEnable);
            printf("InvertTXD: %d\n", ft_eeprom_232r.InvertTXD);
            printf("InvertRXD: %d\n", ft_eeprom_232r.InvertRXD);
            printf("InvertRTS: %d\n", ft_eeprom_232r.InvertRTS);
            printf("InvertCTS: %d\n", ft_eeprom_232r.InvertCTS);
            printf("Cbus0: %d\n", ft_eeprom_232r.Cbus0);
            printf("Cbus1: %d\n", ft_eeprom_232r.Cbus1);
            printf("Cbus2: %d\n", ft_eeprom_232r.Cbus2);
            printf("Cbus3: %d\n", ft_eeprom_232r.Cbus3);
            printf("Cbus4: %d\n", ft_eeprom_232r.Cbus4);
            printf("--------------------------------\n");
            
        } else {
            printf("Error: Could not program EEPROM. Error code: %d\n", ftStatus1);
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
    } else {
        printf("Error: Could not close device. Error code: %d\n", ftStatus);
        return 1;
    }

    return 0;
}
