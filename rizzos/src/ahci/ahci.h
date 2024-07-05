#ifndef _AHCI_H_
#define _AHCI_H_

#include "../pci.h"

enum AHCI_PortType {
    ACHI_PT_None = 0,
    ACHI_PT_SATA = 1,
    ACHI_PT_SEMB = 2,
    ACHI_PT_PM = 3,
    ACHI_PT_SATAPI = 4,
};

typedef struct {
    uint32_t commandListBase;
    uint32_t commandListBaseUpper;
    uint32_t fisBaseAddress;
    uint32_t fisBaseAddressUpper;
    uint32_t interruptStatus;
    uint32_t interruptEnable;
    uint32_t cmdSts;
    uint32_t rsv0;
    uint32_t taskFileData;
    uint32_t signature;
    uint32_t sataStatus;
    uint32_t sataControl;
    uint32_t sataError;
    uint32_t sataActive;
    uint32_t commandIssue;
    uint32_t sataNotification;
    uint32_t fisSwitchControl;
    uint32_t rsv1[11];
    uint32_t vendor[4];
} HBAPort;

typedef struct  {
    uint32_t hostCapability;
    uint32_t globalHostControl;
    uint32_t interruptStatus;
    uint32_t portsImplemented;
    uint32_t version;
    uint32_t cccControl;
    uint32_t cccPorts;
    uint32_t enclosureManagementLocation;
    uint32_t enclosureManagementControl;
    uint32_t hostCapabilitiesExtended;
    uint32_t biosHandoffCtrlSts;
    uint8_t rsv0[0x74];
    uint8_t vendor[0x60];
    HBAPort ports[1];
} HBAMemory;

void AHCI_AHCIDriverInit(PCIDeviceHeader* pciBaseAddress);
void AHCI_AHCIDriverProbePorts(PCIDeviceHeader* pciBaseAddress);

#endif
