#include "pci.h"
#include "acpi.h"
#include <stdint.h>

static void EnumerateBus(uint64_t baseAddress, uint64_t bus) {
    uint64_t offset = bus << 20;

    uint64_t busAddr = baseAddress + offset;
}

void PCI_EnumeratePCI(MCFGHeader* mcfg) {
    int enteries = ((mcfg->header.lenght) - sizeof(MCFGHeader)) / sizeof(DeviceConfig);
    for (int i = 0; i < enteries; i++) {
        DeviceConfig* dev_config = (DeviceConfig*)(
                (uint64_t)mcfg + 
                sizeof(MCFGHeader) + 
                (sizeof(DeviceConfig) * i)
            );
        for(uint64_t bus = dev_config->startBus; bus < dev_config->endBus; bus++) {
            EnumerateBus(dev_config->baseAddress, bus);
        }
    }
}
