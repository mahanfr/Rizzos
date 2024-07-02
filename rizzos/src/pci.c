#include "pci.h"
#include "acpi.h"
#include "paging/page_table_manager.h"
#include "graphics/basic_graphics.h"
#include <stdint.h>

static void EnumerateFunction(uint64_t deviceAddress, uint64_t func) {
    uint64_t offset = func << 12;

    uint64_t functionAddr = deviceAddress + offset;
    PTM_MapMemory((void*) functionAddr, (void*) functionAddr);
    PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*) functionAddr;
    
    if (pciDeviceHeader->deviceID == 0) return;
    if (pciDeviceHeader->deviceID == 0xFFFF) return;

    print("venderID: %X, DeviceID: %d\n", pciDeviceHeader->vendorID, pciDeviceHeader->deviceID);
}

static void EnumerateDevice(uint64_t busAddress, uint64_t device) {
    uint64_t offset = device << 15;

    uint64_t deviceAddr = busAddress + offset;
    PTM_MapMemory((void*) deviceAddr, (void*) deviceAddr);
    PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*) deviceAddr;
    
    if (pciDeviceHeader->deviceID == 0) return;
    if (pciDeviceHeader->deviceID == 0xFFFF) return;

    for(uint64_t func = 0; func < 8; func++) {
        EnumerateFunction(deviceAddr, func);        
    }
}

static void EnumerateBus(uint64_t baseAddress, uint64_t bus) {
    uint64_t offset = bus << 20;

    uint64_t busAddr = baseAddress + offset;
    PTM_MapMemory((void*) busAddr, (void*) busAddr);

    PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*) busAddr;

    if (pciDeviceHeader->deviceID == 0) return;
    if (pciDeviceHeader->deviceID == 0xFFFF) return;

    for(uint64_t dev = 0; dev < 32; dev++) {
        EnumerateDevice(busAddr, dev);    
    }
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
