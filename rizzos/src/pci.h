#ifndef _PCI_H_
#define _PCI_H_

#include "acpi.h"
#include <stdint.h>

typedef struct{
    uint16_t vendorID;
    uint16_t deviceID;
    uint16_t command;
    uint16_t status;
    uint8_t revisionID;
    uint8_t progIF;
    uint8_t subclass;
    uint8_t Class;
    uint8_t cacheLineSize;
    uint8_t latencyTimer;
    uint8_t headerType;
    uint8_t BIST;
} PCIDeviceHeader;

void PCI_EnumeratePCI(MCFGHeader* mcfg);

#endif
