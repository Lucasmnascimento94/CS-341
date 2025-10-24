#include "ft_utils.h"
#include <inttypes.h>
#include <stdio.h>


void eepromDataStructInit(FT_PROGRAM_DATA *eeprom, WORD vid, WORD id){
    eeprom->Signature1 = 0x00000000;
    eeprom->Signature2 = 0xffffffff;
    eeprom->Manufacturer   = (char*)malloc(STRBUF);
    eeprom->ManufacturerId = (char*)malloc(8);
    eeprom->Description    = (char*)malloc(STRBUF);
    eeprom->SerialNumber   = (char*)malloc(32);
    eeprom->VendorId = vid;
    eeprom->ProductId = id;

    memset(eeprom->Manufacturer,   0, STRBUF);
    memset(eeprom->ManufacturerId, 0, 8);
    memset(eeprom->Description,    0, STRBUF);
    memset(eeprom->SerialNumber,   0, 32);
}

char *getStatusName(FT_STATUS status){
    switch (status){
        case FT_OK:                             return "FT_OK";
        case FT_INVALID_HANDLE:                 return "FT_INVALID_HANDLE";
        case FT_DEVICE_NOT_FOUND:               return "FT_DEVICE_NOT_FOUND";
        case FT_DEVICE_NOT_OPENED:              return "FT_DEVICE_NOT_OPENED";
        case FT_IO_ERROR:                       return "FT_IO_ERROR";
        case FT_INSUFFICIENT_RESOURCES:         return "FT_INSUFFICIENT_RESOURCES";
        case FT_INVALID_PARAMETER:              return "FT_INVALID_PARAMETER";
        case FT_INVALID_BAUD_RATE:              return "FT_INVALID_BAUD_RATE";
        case FT_DEVICE_NOT_OPENED_FOR_ERASE:    return "FT_DEVICE_NOT_OPENED_FOR_ERASE";
        case FT_DEVICE_NOT_OPENED_FOR_WRITE:    return "FT_DEVICE_NOT_OPENED_FOR_WRITE";
        case FT_FAILED_TO_WRITE_DEVICE:         return "FT_FAILED_TO_WRITE_DEVICE";
        case FT_EEPROM_READ_FAILED:             return "FT_EEPROM_READ_FAILED";
        case FT_EEPROM_WRITE_FAILED:            return "FT_EEPROM_WRITE_FAILED";
        case FT_EEPROM_ERASE_FAILED:            return "FT_EEPROM_ERASE_FAILED";
        case FT_EEPROM_NOT_PRESENT:             return "FT_EEPROM_NOT_PRESENT";
        case FT_EEPROM_NOT_PROGRAMMED:          return "FT_EEPROM_NOT_PROGRAMMED";
        case FT_INVALID_ARGS:                   return "FT_INVALID_ARGS";
        case FT_NOT_SUPPORTED:                  return "FT_NOT_SUPPORTED";
        case FT_OTHER_ERROR:                    return "FT_OTHER_ERROR";
        case FT_DEVICE_LIST_NOT_READY:          return "FT_DEVICE_LIST_NOT_READY";
        default:                                return "UNKOWN STATUS";        
    };
}

char *getTypeName(FT_DEVICE type){
    switch (type){
        case FT_DEVICE_BM:                      return "FT_DEVICE_BM";
        case FT_DEVICE_AM:                      return "FT_DEVICE_AM";
        case FT_DEVICE_100AX:                   return "FT_DEVICE_100AX";
        case FT_DEVICE_UNKNOWN:                 return "FT_DEVICE_UNKNOWN";
        case FT_DEVICE_2232C:                   return "FT_DEVICE_2232C";
        case FT_DEVICE_232R:                    return "FT_DEVICE_232R";
        case FT_DEVICE_2232H:                   return "FT_DEVICE_2232H";
        case FT_DEVICE_4232H:                   return "FT_DEVICE_4232H";
        case FT_DEVICE_232H:                    return "FT_DEVICE_232H";
        case FT_DEVICE_X_SERIES:                return "FT_DEVICE_X_SERIES";
        case FT_DEVICE_4222H_0:                 return "FT_DEVICE_4222H_0";
        case FT_DEVICE_4222H_1_2:               return "FT_DEVICE_4222H_1_2";
        case FT_DEVICE_4222H_3:                 return "FT_DEVICE_4222H_3";
        case FT_DEVICE_4222_PROG:               return "FT_DEVICE_4222_PROG";
        case FT_DEVICE_900:                     return "FT_DEVICE_900";
        case FT_DEVICE_930:                     return "FT_DEVICE_930";
        case FT_DEVICE_UMFTPD3A:                return "FT_DEVICE_UMFTPD3A";
        case FT_DEVICE_2233HP:                  return "FT_DEVICE_2233HP";
        case FT_DEVICE_4233HP:                  return "FT_DEVICE_4233HP";
        case FT_DEVICE_2232HP:                  return "FT_DEVICE_2232HP";
        case FT_DEVICE_4232HP:                  return "FT_DEVICE_4232HP";
        case FT_DEVICE_233HP:                   return "FT_DEVICE_233HP";
        case FT_DEVICE_232HP:                   return "FT_DEVICE_232HP";
        case FT_DEVICE_2232HA:                  return "FT_DEVICE_2232HA";
        case FT_DEVICE_4232HA:                  return "FT_DEVICE_4232HA";
        default:                                return "UNKOWN STATUS";        
    };
}

void displayInfoLists(FT_DEVICE_LIST_INFO_NODE *list, size_t n){
    for(size_t i=0; i<n; i++){
        printf("Device Index: %zu\n", i);
        printf("    Device Flags: %lu\n", (unsigned long)list[i].Flags);
        printf("    Device ID: %lu\n",    (unsigned long)list[i].ID);
        printf("    Device LOCID: %lu\n", (unsigned long)list[i].LocId);
        printf("    Device SerialNumber: %s\n", list[i].SerialNumber);   // <-- %s
        printf("    Device Type: %s\n", getTypeName(list[i].Type));
        printf("    Device Description: %s\n-------------------------\n",
               list[i].Description);
    }
}

char *displayFTFunction(UCHAR opcode){
    switch (opcode){
        case FT_X_SERIES_CBUS_TRISTATE:             return "FT_X_SERIES_CBUS_TRISTATE";
        case FT_X_SERIES_CBUS_TXLED:                return "FT_X_SERIES_CBUS_TXLED";
        case FT_X_SERIES_CBUS_RXLED:                return "FT_X_SERIES_CBUS_RXLED";
        case FT_X_SERIES_CBUS_TXRXLED:              return "FT_X_SERIES_CBUS_TXRXLED";
        case FT_X_SERIES_CBUS_PWREN:                return "FT_X_SERIES_CBUS_PWREN";
        case FT_X_SERIES_CBUS_SLEEP:                return "FT_X_SERIES_CBUS_SLEEP";
        case FT_X_SERIES_CBUS_DRIVE_0:              return "FT_X_SERIES_CBUS_DRIVE_0";
        case FT_X_SERIES_CBUS_DRIVE_1:              return "FT_X_SERIES_CBUS_DRIVE_1";
        case FT_X_SERIES_CBUS_IOMODE:               return "FT_X_SERIES_CBUS_IOMODE";
        case FT_X_SERIES_CBUS_TXDEN:                return "FT_X_SERIES_CBUS_TXDEN";
        case FT_X_SERIES_CBUS_CLK24:                return "FT_X_SERIES_CBUS_CLK24";
        case FT_X_SERIES_CBUS_CLK12:                return "FT_X_SERIES_CBUS_CLK12";
        case FT_X_SERIES_CBUS_CLK6:                 return "FT_X_SERIES_CBUS_CLK6";
        case FT_X_SERIES_CBUS_BCD_CHARGER:          return "FT_X_SERIES_CBUS_BCD_CHARGER";
        case FT_X_SERIES_CBUS_BCD_CHARGER_N:        return "FT_X_SERIES_CBUS_BCD_CHARGER_N";
        case FT_X_SERIES_CBUS_I2C_TXE:              return "FT_X_SERIES_CBUS_I2C_TXE";
        case FT_X_SERIES_CBUS_I2C_RXF:              return "FT_X_SERIES_CBUS_I2C_RXF";
        case FT_X_SERIES_CBUS_VBUS_SENSE:           return "FT_X_SERIES_CBUS_VBUS_SENSE";
        case FT_X_SERIES_CBUS_BITBANG_WR:           return "FT_X_SERIES_CBUS_BITBANG_WR";
        case FT_X_SERIES_CBUS_BITBANG_RD:           return "FT_X_SERIES_CBUS_BITBANG_RD";
        case FT_X_SERIES_CBUS_TIMESTAMP:            return "FT_X_SERIES_CBUS_TIMESTAMP";
        case FT_X_SERIES_CBUS_KEEP_AWAKE:           return "FT_X_SERIES_CBUS_KEEP_AWAKE";
        default:                                    return "UNKNOWN FUNCTION TYPE";
    }
}


void displayEeprom(FT_PROGRAM_DATA *_p){                                                                                                  
    printf("EEPROM DATA:\n");                                                      
    /* --- Core header --- */                                                      
    printf("    Signature1: 0x%08" PRIX32 "\n", (uint32_t)_p->Signature1);         
    printf("    Signature2: 0x%08" PRIX32 "\n", (uint32_t)_p->Signature2);         
    printf("    Version:    %" PRIu32 "\n",        (uint32_t)_p->Version);         
    /* --- IDs & strings --- */                                                    
    printf("    VendorId:   0x%04" PRIX16 "\n", (uint16_t)_p->VendorId);           
    printf("    ProductId:  0x%04" PRIX16 "\n", (uint16_t)_p->ProductId);          
    printf("    Manufacturer:   %s\n", _p->Manufacturer   ? _p->Manufacturer   : "(null)"); 
    printf("    ManufacturerId: %s\n", _p->ManufacturerId ? _p->ManufacturerId : "(null)"); 
    printf("    Description:    %s\n", _p->Description    ? _p->Description    : "(null)"); 
    printf("    SerialNumber:   %s\n", _p->SerialNumber   ? _p->SerialNumber   : "(null)"); 
    /* --- Power & basic flags --- */                                              
    printf("    MaxPower:   %" PRIu16 " mA\n", (uint16_t)_p->MaxPower);           
    printf("    PnP:        %s\n", _p->PnP ? "yes" : "no");                        
    printf("    SelfPowered:%s\n", _p->SelfPowered ? "yes" : "no");                
    printf("    RemoteWakeup:%s\n", _p->RemoteWakeup ? "yes" : "no");              
    /* --- Rev4 (FT232B) --- */                                                    
    printf("    Rev4:           %s\n", _p->Rev4 ? "yes" : "no");                   
    printf("    IsoIn:          %s\n", _p->IsoIn ? "yes" : "no");                  
    printf("    IsoOut:         %s\n", _p->IsoOut ? "yes" : "no");                 
    printf("    PullDownEnable: %s\n", _p->PullDownEnable ? "yes" : "no");         
    printf("    SerNumEnable:   %s\n", _p->SerNumEnable ? "yes" : "no");           
    printf("    USBVersionEnable:%s\n", _p->USBVersionEnable ? "yes" : "no");     
    printf("    USBVersion:     0x%04" PRIX16 "\n", (uint16_t)_p->USBVersion);     
    /* --- Rev5 (FT2232) subset --- */                                             
    printf("    Rev5:            %s\n", _p->Rev5 ? "yes" : "no");                  
    printf("    IsoInA:          %s\n", _p->IsoInA ? "yes" : "no");                
    printf("    IsoInB:          %s\n", _p->IsoInB ? "yes" : "no");                
    printf("    IsoOutA:         %s\n", _p->IsoOutA ? "yes" : "no");               
    printf("    IsoOutB:         %s\n", _p->IsoOutB ? "yes" : "no");               
    printf("    PullDownEnable5: %s\n", _p->PullDownEnable5 ? "yes" : "no");       
    printf("    SerNumEnable5:   %s\n", _p->SerNumEnable5 ? "yes" : "no");         
    printf("    USBVersionEnable5:%s\n", _p->USBVersionEnable5 ? "yes" : "no");    
    printf("    USBVersion5:     0x%04" PRIX16 "\n", (uint16_t)_p->USBVersion5);   
    printf("    AIsHighCurrent:  %s\n", _p->AIsHighCurrent ? "yes" : "no");        
    printf("    BIsHighCurrent:  %s\n", _p->BIsHighCurrent ? "yes" : "no");        
    printf("    IFAIsFifo:       %s\n", _p->IFAIsFifo ? "yes" : "no");             
    printf("    IFAIsFifoTar:    %s\n", _p->IFAIsFifoTar ? "yes" : "no");          
    printf("    IFAIsFastSer:    %s\n", _p->IFAIsFastSer ? "yes" : "no");          
    printf("    AIsVCP:          %s\n", _p->AIsVCP ? "yes" : "no");               
    printf("    IFBIsFifo:       %s\n", _p->IFBIsFifo ? "yes" : "no");             
    printf("    IFBIsFifoTar:    %s\n", _p->IFBIsFifoTar ? "yes" : "no");          
    printf("    IFBIsFastSer:    %s\n", _p->IFBIsFastSer ? "yes" : "no");          
    printf("    BIsVCP:          %s\n", _p->BIsVCP ? "yes" : "no");                
    /* --- Rev6 (FT232R) subset --- */                                             
    printf("    UseExtOsc:       %s\n", _p->UseExtOsc ? "yes" : "no");             
    printf("    HighDriveIOs:    %s\n", _p->HighDriveIOs ? "yes" : "no");          
    printf("    EndpointSize:    %" PRIu8  "\n", (uint8_t)_p->EndpointSize);       
    printf("    PullDownEnableR: %s\n", _p->PullDownEnableR ? "yes" : "no");       
    printf("    SerNumEnableR:   %s\n", _p->SerNumEnableR ? "yes" : "no");         
    printf("    InvertTXD:       %s\n", _p->InvertTXD ? "yes" : "no");             
    printf("    InvertRXD:       %s\n", _p->InvertRXD ? "yes" : "no");             
    printf("    InvertRTS:       %s\n", _p->InvertRTS ? "yes" : "no");             
    printf("    InvertCTS:       %s\n", _p->InvertCTS ? "yes" : "no");             
    printf("    InvertDTR:       %s\n", _p->InvertDTR ? "yes" : "no");             
    printf("    InvertDSR:       %s\n", _p->InvertDSR ? "yes" : "no");            
    printf("    InvertDCD:       %s\n", _p->InvertDCD ? "yes" : "no");             
    printf("    InvertRI:        %s\n", _p->InvertRI ? "yes" : "no");              
    printf("    Cbus0:           %s\n", displayFTFunction((unsigned)_p->Cbus0)); 
    printf("    Cbus1:           %s\n", displayFTFunction((unsigned)_p->Cbus1)); 
    printf("    Cbus2:           %s\n", displayFTFunction((unsigned)_p->Cbus2)); 
    printf("    Cbus3:           %s\n", displayFTFunction((unsigned)_p->Cbus3));  
    printf("    Cbus4:           %s\n", displayFTFunction((unsigned)_p->Cbus4)); 
    printf("    RIsD2XX:         %s\n", _p->RIsD2XX ? "yes" : "no");                
}
