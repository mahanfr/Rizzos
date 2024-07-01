#ifndef _COMMON_RSDP_
#define _COMMON_RSDP_

#include <stdint.h>

typedef struct {
        unsigned char signature[8];
        uint8_t checksum;
        uint8_t OEMId[6];
        uint8_t revision;
        uint32_t RSDTAddress;
        uint32_t length;
        uint64_t XSDTAddress;
        uint8_t extendedChecksum;
        uint8_t reserved[3];
} __attribute__((packed)) RSDP2;

#endif
