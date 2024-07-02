#ifndef _ACPI_H_
#define _ACPI_H_

#include <stdint.h>
typedef struct {
    unsigned char signature[4];
    uint32_t lenght;
    uint8_t revision;
    uint8_t checksum;
    uint8_t OEMId[6];
    uint8_t OEMTableId[8];
    uint32_t OEMRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;
} __attribute__((packed)) SDTHeader;

typedef struct {
    SDTHeader header;
    uint64_t reserved;
} __attribute__((packed)) MCFGHeader;

typedef struct {
        uint64_t baseAddress;
        uint16_t PCISegGroup;
        uint8_t startBus;
        uint8_t endBus;
        uint32_t reserved;
}__attribute__((packed)) DeviceConfig;

void* ACPI_FindTable(SDTHeader* sdtHeader, char* signature);

#endif
