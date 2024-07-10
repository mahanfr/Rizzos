#include "ahci.h"
#include "../graphics/basic_graphics.h"
#include "../pci.h"
#include <stdint.h>
#include "../paging/page_table_manager.h"

#define MAX_AHCI_PORTS 32
#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1

#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101

static enum AHCI_PortType CheckPortType(HBAPort* port) {
    uint32_t sataStatus = port->sataStatus;

    uint8_t interfacePowerManagment = (sataStatus >> 8) & 0b111;
    uint8_t deviceDetection = sataStatus & 0b111;
    
    if (deviceDetection != HBA_PORT_DEV_PRESENT) return AHCI_PT_None;
    if (interfacePowerManagment != HBA_PORT_IPM_ACTIVE) return AHCI_PT_None;

    switch (port->signature) {
        case SATA_SIG_ATAPI:
            return AHCI_PT_SATAPI;
        case SATA_SIG_ATA:
            return AHCI_PT_SATA;
        case SATA_SIG_PM:
            return AHCI_PT_PM;
        case SATA_SIG_SEMB:
            return AHCI_PT_SEMB;
        default:
            return AHCI_PT_None;
    }
} 

AHCIDriver AHCI_AHCIDriver(PCIDeviceHeader *pciBaseAddress) {
    AHCIDriver ahci_driver = {0};
    ahci_driver.pciBaseAddress = pciBaseAddress;
    print("AHCI Driver Initialized!\n");
    
    ahci_driver.abar = (HBAMemory*)((uint64_t)((PCIHeader0*)pciBaseAddress)->BAR5);
    
    PTM_MapMemory(ahci_driver.abar, ahci_driver.abar);
    AHCI_AHCIDriver_ProbePorts(&ahci_driver);

    return ahci_driver;
}

void AHCI_AHCIDriver_ProbePorts(AHCIDriver *self) {
    uint32_t portsImpelmented = self->abar->portsImplemented;
    for(int i=0; i< MAX_AHCI_PORTS; i++) {
        if(portsImpelmented & (1 << i)){
            enum AHCI_PortType portType = CheckPortType(&self->abar->ports[i]);
            
            if (portType == AHCI_PT_SATA) {
                print("SATA Drive\n");
            }else if (portType == AHCI_PT_SATAPI) {
                print("SATAPI Drive\n");
            } else {
                print("Not Implemented!\n");
            }
        }
    }
}
