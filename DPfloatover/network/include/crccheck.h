/*
***********************************************************************
* crccheck.h:
*	routines to calculate several kinds of CRC16 checksums. This header file
* can be read by both C and C++ compilers
*
*  by Hu.ZH(Mr. SJTU)
***********************************************************************
*/
#ifndef _CRCCHECK_H_
#define _CRCCHECK_H_

#ifdef __cplusplus // call a C function from C++
extern "C" {
#endif

#include "../libcrc/include/checksum.h"
#include <stdbool.h>

#define MAX_STRING_SIZE 2048

// we have to add a parameter(e.g. length)
uint16_t get_crc16_checksum(unsigned char *message, uint32_t length);

/*
** Descriptions: CRC16 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
bool verify_crc16_checksum(unsigned char *message, uint32_t length);

// /*
// ** Descriptions: append CRC16 to the end of data
// ** Input: Data to CRC and append,Stream length = Data + checksum
// ** Output: True or False (CRC Verify Result)
// */
void append_crc16_checksum(unsigned char *message, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /*_CRCCHECK_H_*/
