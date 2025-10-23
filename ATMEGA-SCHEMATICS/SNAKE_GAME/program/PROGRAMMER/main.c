#include "main.h"
#include "conf.h"
#include "ft_opcodes_utils.h"

void displayInfoLists(FT_DEVICE_LIST_INFO_NODE *list, size_t n);
uint8_t uartConf(FT_HANDLE ftHandle);

unsigned int n_devices;
FT_STATUS status;
FT_DEVICE_LIST_INFO_NODE *list;

DWORD bytesWritten;
int main() {

    status = FT_CreateDeviceInfoList(&n_devices);
    if(status!=FT_OK){printf("Error_CREATDEVICELIST.. <%s>\n", getStatusName(status)); return 1;}
    
    
    list = malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*n_devices);
    status = FT_GetDeviceInfoList(list, &n_devices);
    if(status!=FT_OK){printf("Error_FT_GETDEVICELIST.. <%s>\n", getStatusName(status)); return 1;}
    displayInfoLists(list, n_devices);

    status = FT_Open(0, &list[0].ftHandle);
    if(status!=FT_OK){printf("Error_FT_OPEN.. <%s>\n", getStatusName(status)); return 1;}

    if(uartConf(list[0].ftHandle)) return 1;
    status = (FT_Write(list[0].ftHandle, "HELLO WORLD", 11, &bytesWritten));
    if(status!=FT_OK){printf("Error_FT_WRITE.. <%s>\n", getStatusName(status)); return 1;}
    printf("written: %d\n", bytesWritten);

    while(1){
        if(!uartConf(list[0].ftHandle)) return 1;
        status = (FT_Write(list[0].ftHandle, "HELLO WORLD", 11, &bytesWritten));
        if(status!=FT_OK){printf("Error_FT_WRITE.. <%s>\n", getStatusName(status)); return 1;}
        printf("written: %d\n", bytesWritten);

        sleep(3);
    }


    status = FT_Close(list[0].ftHandle);
    if(status!=FT_OK){printf("Error_FT_CLOSE.. <%s>\n", getStatusName(status)); return 1;}
    return 0;
}


uint8_t uartConf(FT_HANDLE ftHandle){
    status = FT_SetBaudRate(ftHandle, baud_rate);
    if(status!=FT_OK){printf("Error_FT_SETBAUDRATE.. <%s>\n", getStatusName(status)); return 1;}
    
    status = FT_SetDataCharacteristics(ftHandle, data_length, stop_bits, parity);
    if(status!=FT_OK){printf("Error_FT_SETDATACHARACTERISTICS.. <%s>\n", getStatusName(status)); return 1;}
    status = FT_Open(0, &list[0].ftHandle);

    status = FT_SetTimeouts(ftHandle, time_out, time_out);
    if(status!=FT_OK){printf("Error_FT_SETTIMEOUT.. <%s>\n", getStatusName(status)); return 1;}
    status = FT_Open(0, &list[0].ftHandle);

    return 0;
}

void displayInfoLists(FT_DEVICE_LIST_INFO_NODE *list, size_t n){
    for(size_t i=0; i<n; i++){
        printf("Device Index: %ld\n", i);
        printf("    Device Flags: %d\n", list[i].Flags);
        printf("    Device ID: %d\n", list[i].ID);
        printf("    Device LOCID: %d\n", list[i].LocId);
        printf("    Device SerialNumber: %p\n", list[i].SerialNumber);
        printf("    Device Type: %s\n", getTypeName(list[i].Type));
        printf("    Device Description: %s\n-------------------------\n", list[i].Description);
    }
}