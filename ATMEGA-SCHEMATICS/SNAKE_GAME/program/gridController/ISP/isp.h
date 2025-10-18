#ifndef ISP_H
#define ISP_H

#include "isp_opcodes.h"
#include "stdint.h"
#include "stdio.h"

#define PAGE_SIZE 32


#define NOT_SYNC 0x01

/*SIGNATURES ADDRESSES*/
#define SIGNATURE_VENDOR 0x00
#define SIGNATURE_FAMILY 0x01
#define SIGNATURE_NUMBER 0x02

/*FUSE BITS CODE*/
#define HFUSE 0x00
#define LFUSE 0x01
#define EXTFUSE 0x02


typedef struct {
    uint8_t signature_vendor;
    uint8_t signature_family;
    uint8_t signature_number;
    uint32_t signature;

    uint8_t data_out_0;
    uint8_t data_out_1;
    uint8_t data_out_2;
    uint8_t data_out_3;

    uint8_t lfuse;
    uint8_t hfuse;
    uint8_t exfuse;
    uint8_t lockbits;
    uint8_t calibyte;
    uint8_t status;
    uint8_t mc;

}TARGET;

typedef struct {
    uint8_t signature_0;
    uint8_t sginature_1;
    uint8_t signature_2;
}PROGRAMMER_CONF;

typedef struct {
    uint16_t current_page;
    uint16_t current_addr;
    PROGRAMMER_CONF *conf;
    uint16_t *buffer;
    size_t buffer_size;
    uint8_t status;
    uint8_t page_counter;
    uint8_t page_number;
}PROGRAMMER;



uint8_t ispInit(TARGET *target);
void powerUp();
void ispProgrammingEnable(TARGET *target) ;
void ispReadSignatureByte(TARGET *target, uint8_t signature);
void ispReadFuseBits(TARGET *target, uint8_t fuse);
#endif