#include "../include/crccheck.h"

uint16_t get_crc16_checksum(unsigned char *message, uint32_t length) {
  uint16_t crc_16_val = 0x0000;
  for (size_t i = 0; i != length; ++i)
    crc_16_val = update_crc_16(crc_16_val, message[i]);
  return crc_16_val;
}

bool verify_crc16_checksum(unsigned char *message, uint32_t length) {
  if ((message == NULL) || (length <= 2)) {
    return false;
  }
  uint16_t expected = get_crc16_checksum(message, length - 2);
  return ((expected & 0xff) == message[length - 2] &&
          ((expected >> 8) & 0xff) == message[length - 1]);
}

void append_crc16_checksum(unsigned char *message, uint32_t length) {
  if ((message == NULL) || (length <= 2)) {
    return;
  }
  uint16_t CRC16 = get_crc16_checksum(message, length - 2);
  message[length - 2] = (unsigned char)(CRC16 & 0x00ff);
  message[length - 1] = (unsigned char)((CRC16 >> 8) & 0x00ff);
}
