#include "ft_opcodes_utils.h"


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
