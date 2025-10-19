/**
 * @file    isp_opcodes.h
 * @brief   opcodes for programmer commands
 *
 * @author  Lucas Nascimento <lucas@hausintelligence.com>
 * @version 1.0.0
 * @date    2025-09-11
 *
 * @target  ATmega168 @ 16 MHz (F_CPU=16000000UL)
 *
 * @copyright
 * Copyright (c) 2025 Lucas Nascimento
 * 
 * Documentation for this information will be found at: 
 * ./documentation/isp/
 * 
 */


/**
 * @file    isp_opcodes.h
 * @brief   Fixed byte fields for AVR ISP instructions (ATmega168-class)
 * @author  Lucas Nascimento
 * @version 1.0.0
 * @date    2025-09-11
 *
 * Notes
 * - Each ISP instruction is exactly 4 bytes on MOSI (b1->b2->b3->b4).
 * - This header defines only the FIXED bytes. Bytes marked “VAR” are set by code.
 * - Data returned (when applicable) is sampled on the last byte (b4) unless noted.
 */

 #ifndef ISP_OPCODES_H
 #define ISP_OPCODES_H
 
 /* ========================= CORE ========================= */
 
 /* Enable Programming: AC 53 00 00 */
 #define ENABLE_PROGRAMMING_Byte1   0xAC
 #define ENABLE_PROGRAMMING_Byte2   0x53
 #define ENABLE_PROGRAMMING_Byte3   0x00
 #define ENABLE_PROGRAMMING_Byte4   0x00
 
 /* Chip Erase: AC 80 00 00 */
 #define CHIP_ERASE_Byte1           0xAC
 #define CHIP_ERASE_Byte2           0x80
 #define CHIP_ERASE_Byte3           0x00
 #define CHIP_ERASE_Byte4           0x00
 
 /* Poll RDY/!BUSY (Flash/EEPROM): F0 00 00 00 — read status in b4 */
 #define POLL_RDY_NBUSY_Byte1       0xF0
 #define POLL_RDY_NBUSY_Byte2       0x00
 #define POLL_RDY_NBUSY_Byte3       0x00
 #define POLL_RDY_NBUSY_Byte4       0x00
 

 /* ========================= LOAD (stage to page buffer) ========================= */
 
 /* Load Extended Address Byte (for >64K words; not needed on ATmega168) : 4D 00 <ADDR_EXT VAR> 00 */
 #define LOAD_EXT_ADDR_BY_Byte1     0x4D
 #define LOAD_EXT_ADDR_BY_Byte2     0x00
 /* LOAD_EXT_ADDR_BY_Byte3 = <ADDR_EXT VAR> */
 #define LOAD_EXT_ADDR_BY_Byte4     0x00
 
 /* Load Program Memory Page, High byte: 48 00 <WORD_IDX_LSB VAR> <DATA VAR> */
 #define LOAD_PRG_MEM_PG_HG_BY_Byte1 0x48
 #define LOAD_PRG_MEM_PG_HG_BY_Byte2 0x00
 /* Byte3 = <WORD_IDX_LSB VAR> */
 /* Byte4 = <DATA VAR> */
 
 /* Load Program Memory Page, Low byte: 40 00 <WORD_IDX_LSB VAR> <DATA VAR> */
 #define LOAD_PRG_MEM_PG_LW_BY_Byte1 0x40
 #define LOAD_PRG_MEM_PG_LW_BY_Byte2 0x00
 /* Byte3 = <WORD_IDX_LSB VAR> */
 /* Byte4 = <DATA VAR> */
 
 /* Load EEPROM Memory (paged): C1 00 <OFFSET_aa VAR> <DATA VAR>  (aa = 0..3 for ATmega168) */
 #define LOAD_EEPROM_MEM_Byte1      0xC1
 #define LOAD_EEPROM_MEM_Byte2      0x00
 /* Byte3 = <OFFSET_aa VAR> */
 /* Byte4 = <DATA VAR> */



 /* ========================= READ ========================= */
 
 /* Read Program Memory, High byte: 28 <ADDR_MSB VAR> <ADDR_LSB VAR> 00 */
 #define READ_PRG_MEM_HG_BY_Byte1   0x28
 /* Byte2 = <ADDR_MSB VAR> */
 /* Byte3 = <ADDR_LSB VAR> */
 /* Byte4 = 0x00 (data out on MISO) */
 
 /* Read Program Memory, Low byte: 20 <ADDR_MSB VAR> <ADDR_LSB VAR> 00 */
 #define READ_PRG_MEM_LW_BY_Byte1   0x20
 /* Byte2 = <ADDR_MSB VAR> */
 /* Byte3 = <ADDR_LSB VAR> */
 /* Byte4 = 0x00 (data out on MISO) */
 
 /* Read EEPROM Memory (byte): A0 <ADDR_MSB VAR> <ADDR_LSB VAR> 00 */
 #define READ_EEPROM_MEM_Byte1      0xA0
 /* Byte2 = <ADDR_MSB VAR> */
 /* Byte3 = <ADDR_LSB VAR> */
 /* Byte4 = 0x00 (data out on MISO) */
 
 /* Read Lock Bits: 58 00 00 00 */
 #define READ_LCK_BITS_Byte1        0x58
 #define READ_LCK_BITS_Byte2        0x00
 #define READ_LCK_BITS_Byte3        0x00
 #define READ_LCK_BITS_Byte4        0x00
 
 /* Read Signature Byte: 30 00 <INDEX VAR> 00  (index = 0,1,2) */
 #define READ_SIGNATURE_BY_Byte1    0x30
 #define READ_SIGNATURE_BY_Byte2    0x00
 /* Byte3 = <INDEX VAR> */
 #define READ_SIGNATURE_BY_Byte4    0x00
 
 /* Read Fuse bits (low): 50 00 00 00 */
 #define READ_FUSE_BITS_Byte1       0x50
 #define READ_FUSE_BITS_Byte2       0x00
 #define READ_FUSE_BITS_Byte3       0x00
 #define READ_FUSE_BITS_Byte4       0x00
 
 /* Read High Fuse bits: 58 08 00 00 */
 #define READ_HG_FUSE_BITS_Byte1    0x58
 #define READ_HG_FUSE_BITS_Byte2    0x08
 #define READ_HG_FUSE_BITS_Byte3    0x00
 #define READ_HG_FUSE_BITS_Byte4    0x00
 
 /* Read Extended Fuse bits: 50 08 00 00 */
 #define READ_EXT_FUSE_BITS_Byte1   0x50
 #define READ_EXT_FUSE_BITS_Byte2   0x08
 #define READ_EXT_FUSE_BITS_Byte3   0x00
 #define READ_EXT_FUSE_BITS_Byte4   0x00
 
 /* Read Calibration Byte: 38 00 00 00 */
 #define READ_CAL_BY_BITS_Byte1     0x38
 #define READ_CAL_BY_BITS_Byte2     0x00
 #define READ_CAL_BY_BITS_Byte3     0x00
 #define READ_CAL_BY_BITS_Byte4     0x00
 

 
 /* ========================= WRITE ========================= */
 
 /* Write Program Memory Page (commit): 4C <PAGE_MSB VAR> <PAGE_LSB VAR> 00 */
 #define WRITE_PRG_MEM_PG_Byte1     0x4C
 /* Byte2 = <PAGE_MSB VAR> */
 /* Byte3 = <PAGE_LSB VAR> */
 #define WRITE_PRG_MEM_PG_Byte4     0x00
 
 /* Write EEPROM Memory (byte): C0 <ADDR_MSB VAR> <ADDR_LSB VAR> <DATA VAR> */
 #define WRITE_EEPROM_MEM_Byte1     0xC0
 /* Byte2 = <ADDR_MSB VAR> */
 /* Byte3 = <ADDR_LSB VAR> */
 /* Byte4 = <DATA VAR> */
 
 /* Write EEPROM Memory Page (commit): C2 <PAGE_MSB VAR> <PAGE_LSB VAR> 00 */
 #define WRITE_EEPROM_MEM_PG_Byte1  0xC2
 /* Byte2 = <PAGE_MSB VAR> */
 /* Byte3 = <PAGE_LSB VAR> */
 #define WRITE_EEPROM_MEM_PG_Byte4  0x00
 
 /* Write Lock Bits: AC E0 00 <DATA VAR> */
 #define WRITE_LCK_BITS_Byte1       0xAC
 #define WRITE_LCK_BITS_Byte2       0xE0
 #define WRITE_LCK_BITS_Byte3       0x00
 /* Byte4 = <DATA VAR> */
 
 /* Write Fuse Bits (low): AC A0 00 <DATA VAR> */
 #define WRITE_FUSE_BITS_Byte1      0xAC
 #define WRITE_FUSE_BITS_Byte2      0xA0
 #define WRITE_FUSE_BITS_Byte3      0x00
 /* Byte4 = <DATA VAR> */
 
 /* Write High Fuse Bits: AC A8 00 <DATA VAR> */
 #define WRITE_HG_FUSE_BITS_Byte1   0xAC
 #define WRITE_HG_FUSE_BITS_Byte2   0xA8
 #define WRITE_HG_FUSE_BITS_Byte3   0x00
 /* Byte4 = <DATA VAR> */
 
 /* Write Extended Fuse Bits: AC A4 00 <DATA VAR> */
 #define WRITE_EXT_FUSE_BITS_Byte1  0xAC
 #define WRITE_EXT_FUSE_BITS_Byte2  0xA4
 #define WRITE_EXT_FUSE_BITS_Byte3  0x00
 /* Byte4 = <DATA VAR> */
 
 #endif /* ISP_OPCODES_H */
 