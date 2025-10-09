#ifndef ISP_H
#define ISP_H

#include "isp_opcodes.h"
#include "stdint.h"
#include "stdio.h"



/*=====================================================================================================
 *                                              CORE
 *=====================================================================================================

/**
 * @brief  Programming Enable — unlocks serial programming interface.
 * @wire   b1=0xAC, b2=0x53, b3=0x00, b4=0x00
 * @rx     none (echo not typically used)
 * @dir    Host → Target
 * @pre    RESET low; SCK within target limits
 * @post   Target ready for ISP commands
 * @spec   Datasheet: Serial Programming Instruction Set → "Programming Enable"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    const uint8_t inst_byte4;
}PRG_EN;


/**
 * @brief  Chip Erase — erase program memory.
 * @wire   b1=0xAC, b2=0x80, b3=0x00, b4=0x00
 * @rx     none
 * @dir    Host → Target
 * @pre    Programming enabled
 * @post   Device busy; poll RDY/!BUSY or wait tWD_ERASE before next op
 * @spec   Datasheet → "Chip Erase"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    const uint8_t inst_byte4;
}CHIP_ERASE;


/**
 * @brief  Poll RDY/!BUSY — check if target finished an internal write/erase.
 * @wire   b1=0xF0, b2=0x00, b3=0x00, b4=0x00
 * @rx     r4 contains ready/busy indication (mask is device-specific; often bit7)
 * @dir    Host ↔ Target
 * @pre    After erase/page write/EEPROM write
 * @post   None
 * @spec   Datasheet → "Read Program Memory Page Ready/Busy" (naming varies)
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_out_0;
}POLL_RDY;



/*=====================================================================================================
 *                                    LOAD (stage to page buffer) 
 *=====================================================================================================*/
 /**
 * @brief  Load Extended Address Byte — select high address bits (large memories).
 * @wire   b1=0x4D, b2=0x00, b3=<VAR: EXT_ADDR>, b4=0x00
 * @rx     none
 * @dir    Host → Target
 * @pre    Only for parts >64K words (not needed on ATmega168)
 * @post   Affects subsequent flash read/write commands
 * @spec   Datasheet → "Load Extended Address Byte"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    uint8_t       ext_addr;
    const uint8_t inst_byte4;
}LOAD_EXT_ADDR_BYTE;


/**
 * @brief  Load Program Memory Page (High byte) — stage high data byte into page buffer.
 * @wire   b1=0x48, b2=0x00, b3=<VAR: WORD_IDX_LSB>, b4=<VAR: DATA>
 * @rx     none
 * @dir    Host → Target
 * @pre    Programming enabled; page not yet committed
 * @post   Data staged; use WRITE_PRG_MEM_PAGE to commit
 * @spec   Datasheet → "Load Program Memory Page, High Byte"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    uint8_t       addr_lsb;
    uint8_t       high_byte_in;
}LOAD_PRG_MEM_PAGE_HIGH_BYTE;


/**
 * @brief  Load Program Memory Page (Low byte) — stage low data byte into page buffer.
 * @wire   b1=0x40, b2=0x00, b3=<VAR: WORD_IDX_LSB>, b4=<VAR: DATA>
 * @rx     none
 * @dir    Host → Target
 * @pre    Programming enabled; page not yet committed
 * @post   Data staged; use WRITE_PRG_MEM_PAGE to commit
 * @spec   Datasheet → "Load Program Memory Page, Low Byte"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    uint8_t       addr_lsb;
    uint8_t       low_byte_in;
}LOAD_PRG_MEM_PAGE_LOW_BYTE;



/**
 * @brief  Load EEPROM Memory Page — stage one byte into EEPROM page buffer.
 * @wire   b1=0xC1, b2=0x00, b3=<VAR: OFFSET_aa>, b4=<VAR: DATA>  (ATmega168: aa=0..3)
 * @rx     none
 * @dir    Host → Target
 * @pre    For paged EEPROM write mode
 * @post   Data staged; use WRITE_EEPROM_MEM_PAGE to commit
 * @spec   Datasheet → "Load EEPROM Memory Page"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    uint8_t       dummy7_bit1;
    uint8_t       data_in;
}LOAD_EEPROM_MEM_PAGE;



/*=====================================================================================================
 *                                                 READ 
 *=====================================================================================================*/
/**
 * @brief  Read Program Memory (High byte) — read high byte at word address.
 * @wire   b1=0x28, b2=<VAR: ADDR_MSB>, b3=<VAR: ADDR_LSB>, b4=0x00
 * @rx     r4 = high data byte
 * @dir    Host ↔ Target
 * @pre    Programming enabled (or read-only mode per device rules)
 * @post   None
 * @spec   Datasheet → "Read Program Memory, High Byte"
 */
 typedef struct{
    const uint8_t inst_byte1;
    uint8_t       addr_msb;
    uint8_t       addr_lsb;
    uint8_t       low_byte_data_out;
}READ_PRG_MEM_HIGH_BYTE;



/**
 * @brief  Read Program Memory (Low byte) — read low byte at word address.
 * @wire   b1=0x20, b2=<VAR: ADDR_MSB>, b3=<VAR: ADDR_LSB>, b4=0x00
 * @rx     r4 = low data byte
 * @dir    Host ↔ Target
 * @pre    Programming enabled (or read-only mode per device rules)
 * @post   None
 * @spec   Datasheet → "Read Program Memory, Low Byte"
 */
typedef struct{
    const uint8_t inst_byte1;
    uint8_t       addr_msb;
    uint8_t       addr_lsb;
    uint8_t       high_byte_data_out;
}READ_PRG_MEM_LOW_BYTE;



/**
 * @brief  Read EEPROM Memory (byte) — read one EEPROM byte at address.
 * @wire   b1=0xA0, b2=<VAR: ADDR_MSB>, b3=<VAR: ADDR_LSB>, b4=0x00
 * @rx     r4 = EEPROM data byte
 * @dir    Host ↔ Target
 * @pre    None (within device read rules)
 * @post   None
 * @spec   Datasheet → "Read EEPROM Memory"
 */
typedef struct{
    const uint8_t inst_byte1;
    uint8_t       dummy6_bit2;
    uint8_t       address_lsb;
    uint8_t       low_byte_data_out;
}READ_EEPROM_MEM;



/**
 * @brief  Read Lock Bits — read current lock bit state.
 * @wire   b1=0x58, b2=0x00, b3=0x00, b4=0x00
 * @rx     r4 = lock bits (see datasheet bit mapping)
 * @dir    Host ↔ Target
 * @pre    Programming enabled
 * @post   None
 * @spec   Datasheet → "Read Lock Bits"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_out;
}READ_LOCK_BITS;



/**
 * @brief  Read Signature Byte — return device signature (index 0..2).
 * @wire   b1=0x30, b2=0x00, b3=<VAR: INDEX>, b4=0x00
 * @rx     r4 = signature byte
 * @dir    Host ↔ Target
 * @pre    Programming enabled
 * @post   None
 * @spec   Datasheet → "Read Signature Byte"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    uint8_t       dummy7_bit1;
    uint8_t       data_byte_out;
}READ_SIGNATURE_BYTE;


/**
 * @brief  Read Fuse Bits (low) — read low fuse byte.
 * @wire   b1=0x50, b2=0x00, b3=0x00, b4=0x00
 * @rx     r4 = low fuse byte
 * @dir    Host ↔ Target
 * @pre    Programming enabled
 * @post   None
 * @spec   Datasheet → "Read Fuse Bits"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_out;
}READ_FUSE_BITS;



/**
 * @brief  Read High Fuse Bits — read high fuse byte.
 * @wire   b1=0x58, b2=0x08, b3=0x00, b4=0x00
 * @rx     r4 = high fuse byte
 * @dir    Host ↔ Target
 * @pre    Programming enabled
 * @post   None
 * @spec   Datasheet → "Read High Fuse Bits"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_out;
}READ_FUSE_HIGH_BITS;



/**
 * @brief  Read Extended Fuse Bits — read extended fuse byte.
 * @wire   b1=0x50, b2=0x08, b3=0x00, b4=0x00
 * @rx     r4 = extended fuse byte
 * @dir    Host ↔ Target
 * @pre    Programming enabled
 * @post   None
 * @spec   Datasheet → "Read Extended Fuse Bits"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_out;
}READ_EXT_FUSE_BITS;



/**
 * @brief  Read Calibration Byte — read internal calibration value(s).
 * @wire   b1=0x38, b2=0x00, b3=<VAR: INDEX/ADDR per device>, b4=0x00
 * @rx     r4 = calibration byte
 * @dir    Host ↔ Target
 * @pre    Programming enabled
 * @post   None
 * @spec   Datasheet → "Read Calibration Byte" (index/address usage varies by device)
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_out;
}READ_CAL_BYTE;





/*=====================================================================================================
 *                                              WRITE
 *=====================================================================================================*/
/**
 * @brief  Write Program Memory Page — commit staged flash page buffer.
 * @wire   b1=0x4C, b2=<VAR: PAGE_MSB>, b3=<VAR: PAGE_LSB>, b4=0x00
 * @rx     none
 * @dir    Host → Target
 * @pre    Page buffer loaded via 0x40/0x48
 * @post   Device busy; poll RDY/!BUSY or wait tWD_FLASH
 * @spec   Datasheet → "Write Program Memory Page"
 */
 typedef struct{
   const uint8_t  inst_byte1;
    uint8_t       addr_msb;
    uint8_t       addr_lsb;
    const uint8_t inst_byte4;
}WRITE_PRG_MEM_PAGE;



/**
 * @brief  Write EEPROM Memory (byte) — start EEPROM write cycle for one address.
 * @wire   b1=0xC0, b2=<VAR: ADDR_MSB>, b3=<VAR: ADDR_LSB>, b4=<VAR: DATA>
 * @rx     none
 * @dir    Host → Target
 * @pre    None
 * @post   Device busy; poll RDY/!BUSY or wait tWD_EEPROM
 * @spec   Datasheet → "Write EEPROM Memory"
 */
typedef struct{
    const uint8_t inst_byte1;
    uint8_t       dummy6_bit2;
    uint8_t       addr_lsb;
    uint8_t       data_byte_in;
}WRITE_EEPROM_MEM;



/**
 * @brief  Write EEPROM Memory Page — commit staged EEPROM page buffer.
 * @wire   b1=0xC2, b2=<VAR: PAGE_MSB>, b3=<VAR: PAGE_LSB>, b4=0x00
 * @rx     none
 * @dir    Host → Target
 * @pre    Page staged via LOAD_EEPROM_MEM_PAGE
 * @post   Device busy; poll RDY/!BUSY or wait tWD_EEPROM
 * @spec   Datasheet → "Write EEPROM Memory Page"
 */
typedef struct{
    const uint8_t inst_byte1;
    uint8_t       dummy6_bit2;
    uint8_t       bit6_dummy2;
    const uint8_t inst_byte4;
}WRITE_EEPROM_MEM_PAGE;



/**
 * @brief  Write Lock Bits — set lock configuration.
 * @wire   b1=0xAC, b2=0xE0, b3=0x00, b4=<VAR: DATA>
 * @rx     none
 * @dir    Host → Target
 * @pre    Understand lock semantics (irreversible paths until chip erase)
 * @post   May restrict future programming until erase
 * @spec   Datasheet → "Write Lock Bits"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_in;
}WRITE_LOCK_BITS;


/**
 * @brief  Write Fuse Bits (low) — set low fuse byte.
 * @wire   b1=0xAC, b2=0xA0, b3=0x00, b4=<VAR: DATA>
 * @rx     none
 * @dir    Host → Target
 * @pre    Verify fuse meaning; risk of disabling RESET/clock changes
 * @post   Device busy briefly; settings take effect immediately/after reset
 * @spec   Datasheet → "Write Fuse Bits (low)"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_in;
}WRITE_FUSE_BITS;



/**
 * @brief  Write High Fuse Bits — set high fuse byte.
 * @wire   b1=0xAC, b2=0xA8, b3=0x00, b4=<VAR: DATA>
 * @rx     none
 * @dir    Host → Target
 * @pre    Verify fuse meaning (e.g., BOOTSZ, BOOTRST, etc.)
 * @post   Device busy briefly; settings take effect after reset
 * @spec   Datasheet → "Write High Fuse Bits"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_in;
}WRITE_FUSE_HIGH_BITS;



/**
 * @brief  Write Extended Fuse Bits — set extended fuse byte.
 * @wire   b1=0xAC, b2=0xA4, b3=0x00, b4=<VAR: DATA>
 * @rx     none
 * @dir    Host → Target
 * @pre    Verify fuse meaning (e.g., BOD level, etc.)
 * @post   Device busy briefly; settings take effect after reset
 * @spec   Datasheet → "Write Extended Fuse Bits"
 */
typedef struct{
    const uint8_t inst_byte1;
    const uint8_t inst_byte2;
    const uint8_t inst_byte3;
    uint8_t       data_byte_in;
}WRITE_EXT_FUSE_BITS;


typedef struct{
    /*==================CORE======================*/
    PRG_EN                          prg_en;
    CHIP_ERASE                      chip_erase;
    POLL_RDY                        poll_rdy;
    
    /*==================LOAD=====================*/
    LOAD_EXT_ADDR_BYTE              load_ext_addr_byte;
    LOAD_PRG_MEM_PAGE_HIGH_BYTE     load_prg_mem_page_high_byte;
    LOAD_PRG_MEM_PAGE_LOW_BYTE      load_prg_mem_page_low_byte;
    LOAD_EEPROM_MEM_PAGE            load_eeprom_mem_page;
   
   /*==================READ=====================*/
    READ_PRG_MEM_HIGH_BYTE          read_prg_mem_high_byte;
    READ_PRG_MEM_LOW_BYTE           read_prg_mem_low_byte;
    READ_EEPROM_MEM                 read_eeprom_mem;
    READ_LOCK_BITS                  read_lock_bits;
    READ_SIGNATURE_BYTE             read_signature_byte;
    READ_FUSE_BITS                  read_fuse_bits;
    READ_FUSE_HIGH_BITS             read_fuse_high_bits;
    READ_EXT_FUSE_BITS              read_ext_fuse_bits;
    READ_CAL_BYTE                   read_cal_bytel;
   
    /*==================WRITE==================*/
    WRITE_PRG_MEM_PAGE              write_prg_mem_page;
    WRITE_EEPROM_MEM                write_eeprom_mem;
    WRITE_EEPROM_MEM_PAGE           write_eeprom_mem_page;
    WRITE_LOCK_BITS                 write_lock_bits;
    WRITE_FUSE_BITS                 write_fuse_bits;
    WRITE_FUSE_HIGH_BITS            write_fuse_high_bits;
    WRITE_EXT_FUSE_BITS             write_ext_fuse_bits;

}ISP_PROTOCOL;


extern ISP_PROTOCOL isp_protocol;
#endif