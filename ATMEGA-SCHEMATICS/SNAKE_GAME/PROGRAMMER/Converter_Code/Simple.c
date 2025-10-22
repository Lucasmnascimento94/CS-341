#include "ftd2xx.h"
#include <stdio.h>

int main() {
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;
    DWORD BytesWritten;
    char TxBuffer[] = "Bitcoin!";


    // Open the first FTDI device
    ftStatus = FT_Open(0, &ftHandle);
    // FT_Open failed
    if (ftStatus != FT_OK) {
        printf("--------------------------------\n");
        printf("Failed to open device. Error: %d\n", ftStatus);
        printf("--------------------------------\n");
        return 0;
    }
    // FT_Open succeeded
    else {
        printf("--------------------------------\n");
        printf("Device opened successfully!\n");

        // Set baud rate (9600)
        ftStatus = FT_SetBaudRate(ftHandle, 9600);
        if (ftStatus != FT_OK) {
            printf("--------------------------------\n");
            printf("Failed to set baud rate. Error: %d\n", ftStatus);
            printf("--------------------------------\n");
            FT_Close(ftHandle);  
            return 0;
        }
        else {
            printf("--------------------------------\n");
            printf("Baud rate set to 9600bps\n");
        }

        // Set 8 data bits, 1 stop bit and no parity 
        ftStatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
        if (ftStatus != FT_OK) {
            printf("--------------------------------\n");
            printf("Failed to set data bitts, parity, and stop bits. Error: %d\n", ftStatus);
            printf("--------------------------------\n");
            FT_Close(ftHandle);
            return 0;
        }
        else {
            printf("--------------------------------\n");
            printf("Frame format is set successfully\n");
        }

        // Set Timeouts (Set Read and Write Timeouts) (ftHandle, Read Timeout, Write Timeout)
        ftStatus = FT_SetTimeouts(ftHandle, 5000, 5000);
        if (ftStatus != FT_OK) {
            printf("--------------------------------\n");
            printf("Failed to set timeouts. Error: %d\n", ftStatus);
            printf("--------------------------------\n");
            FT_Close(ftHandle);
            return 0;
        }
        else {
            printf("--------------------------------\n");
            printf("Timeouts set successfully\n");
        }

        // Write data to the device
        ftStatus = FT_Write(ftHandle, TxBuffer, sizeof(TxBuffer), &BytesWritten);
        if (ftStatus != FT_OK) {
            printf("--------------------------------\n");
            printf("Failed to write data. Error: %d\n", ftStatus);
            printf("--------------------------------\n");
            FT_Close(ftHandle);
            return 0;
        }
        else {
            printf("--------------------------------\n");
            printf("Written successfully! Bytes written: %d\n", BytesWritten);
        }

    }

    // Close the device
    FT_Close(ftHandle);
    return 0;
}
