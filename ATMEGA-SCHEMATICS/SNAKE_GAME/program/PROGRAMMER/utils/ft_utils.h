#ifndef FT_OPCODES_UTILS_H
#define FT_OPCODES_UTILS_H

#include "ftd2xx.h"
#include "stdint.h"
#include "stdlib.h"

#define MAX_BUFFER 48

char *getStatusName(FT_STATUS status);
char *getTypeName(FT_DEVICE type);
void displayEeprom(FT_PROGRAM_DATA *_p);
void displayInfoLists(FT_DEVICE_LIST_INFO_NODE *list, size_t n);
void eepromDataStructInit(FT_PROGRAM_DATA *eeprom, WORD vid, WORD id);
#endif