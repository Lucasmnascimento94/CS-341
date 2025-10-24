#include "main.h"
#include "conf.h"
#include "ft_utils.h"

uint8_t uartConf(FT_HANDLE ftHandle);

FT_PROGRAM_DATA program;
FT_STATUS status;
FT_DEVICE_LIST_INFO_NODE *list;

DWORD n_devices;
DWORD bytesWritten;
DWORD vid;
DWORD id;
int main() {
    /*_____________Create List of Devices______________*/
    status = FT_CreateDeviceInfoList(&n_devices);
    if(status!=FT_OK){printf("Error_CREATDEVICELIST.. <%s>\n", getStatusName(status)); return 1;}
    
    
    /*_____________Print Devices Info______________*/
    list = malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*n_devices);
    status = FT_GetDeviceInfoList(list, &n_devices);
    if(status!=FT_OK){printf("Error_FT_GETDEVICELIST.. <%s>\n", getStatusName(status)); return 1;}
    displayInfoLists(list, n_devices);

    /*_____________Open Device index 0______________*/
    status = FT_Open(0, &list[0].ftHandle);
    if(status!=FT_OK){printf("Error_FT_OPEN.. <%s>\n", getStatusName(status)); return 1;}


    /*_____________Set Uart conf for device index 0______________*/
    if(uartConf(list[0].ftHandle)) return 1;
    status = (FT_Write(list[0].ftHandle, "HELLO WORLD", 11, &bytesWritten));
    if(status!=FT_OK){printf("Error_FT_WRITE.. <%s>\n", getStatusName(status)); return 1;}
    printf("written: %d\n", bytesWritten);


    /*______________Read device index 0 EEPROM_________________*/
    status = FT_GetVIDPID(&vid, &id);
    if(status!=FT_OK){printf("Error.. <%s>\n", getStatusName(status)); return 1;}
    eepromDataStructInit(&program, vid, id);

    status = FT_EE_Read(list[0].ftHandle, &program);
    if(status!=FT_OK){printf("Error_FT_EEREAD.. <%s>\n", getStatusName(status)); return 1;}
    displayEeprom(&program);


    FT_Purge(list[0].ftHandle, FT_PURGE_RX | FT_PURGE_TX);
    FT_ResetDevice(list[0].ftHandle);

    // (Optional) close and re-open to guarantee a quiet handle:
    FT_Close(list[0].ftHandle);
    FT_Open(0, &list[0].ftHandle);

    if (!is_idle_for_eeprom(list[0].ftHandle)) {
        // try a short sleep and retry, or bail out
        printf("NOT IDLES\n");
    }


   program.Cbus4 = FT_232R_CBUS_RXLED;
   // status = FT_EEPROM_Program(list[0].ftHandle, &ee232r, sizeof(ee232r), program.Manufacturer, program.ManufacturerId, program.Description, program.SerialNumber);
    if(status!=FT_OK){printf("Error_FT_EEPROGRAM.. <%s>\n", getStatusName(status)); return 1;}

    FT_ResetDevice(list[0].ftHandle);
    status = FT_ResetDevice(list[0].ftHandle);
    if(status!=FT_OK){printf("Error_FT_RESET.. <%s>\n", getStatusName(status)); return 1;}
    sleep(4);
    status = FT_EE_Program(list[0].ftHandle, &program);
    if(status!=FT_OK){printf("Error_FT_EEPROGRAM.. <%s>\n", getStatusName(status)); return 1;}


    status = FT_EE_Read(list[0].ftHandle, &program);
    if(status!=FT_OK){printf("Error_FT_EEREAD.. <%s>\n", getStatusName(status)); return 1;}
    displayEeprom(&program);

    //while(1){
        if(uartConf(list[0].ftHandle)) return 1;
        status = (FT_Write(list[0].ftHandle, "HELLO WORLD", 11, &bytesWritten));
        if(status!=FT_OK){printf("Error_FT_WRITE.. <%s>\n", getStatusName(status)); return 1;}
        printf("written: %d\n", bytesWritten);

        //sleep(3);
    //}


    status = FT_Close(list[0].ftHandle);
    if(status!=FT_OK){printf("Error_FT_CLOSE.. <%s>\n", getStatusName(status)); return 1;}


    free(program.Manufacturer);
    free(program.ManufacturerId);
    free(program.Description);
    free(program.SerialNumber);
    return 0;
}


uint8_t uartConf(FT_HANDLE ftHandle){
    status = FT_SetBaudRate(ftHandle, baud_rate);
    if(status!=FT_OK){printf("Error_FT_SETBAUDRATE.. <%s>\n", getStatusName(status)); return 1;}
    
    status = FT_SetDataCharacteristics(ftHandle, data_length, stop_bits, parity);
    if(status!=FT_OK){printf("Error_FT_SETDATACHARACTERISTICS.. <%s>\n", getStatusName(status)); return 1;}

    status = FT_SetTimeouts(ftHandle, time_out, time_out);
    if(status!=FT_OK){printf("Error_FT_SETTIMEOUT.. <%s>\n", getStatusName(status)); return 1;}

    return 0;
}