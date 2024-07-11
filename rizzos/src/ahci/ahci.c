#include "ahci.h"
#include "../graphics/basic_graphics.h"
#include "../pci.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../paging/page_table_manager.h"
#include "../paging/page_frame_allocator.h"
#include "../memory/heap.h"
#include "../memory.h"
#include "fis.h"

#define MAX_AHCI_PORTS 32
#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE 0x1
#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000
#define HBA_PxIS_TFES (1 << 30)

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

AHCIDriver* AHCI_AHCIDriver(PCIDeviceHeader *pciBaseAddress) {
    AHCIDriver* ahci_driver = MEM_Malloc(sizeof(AHCIDriver));
    ahci_driver->pciBaseAddress = pciBaseAddress;

    ahci_driver->abar = (HBAMemory*)((uint64_t)((PCIHeader0*)pciBaseAddress)->BAR5);

    PTM_MapMemory(ahci_driver->abar, ahci_driver->abar);
    AHCI_AHCIDriver_ProbePorts(ahci_driver);

    for (int i=0; i < ahci_driver->portCount; i++) {
        AHCI_Port* port = ahci_driver->ports[i];

        AHCI_Port_Configure(port);

        port->buffer = (uint8_t*) PFA_RequestPage();
        MEM_Set(port->buffer, 0, 0x1000);

        print("printing the buffer!\n");
        AHCI_Port_Read(port, 0, 4, port->buffer);
        for(int t = 0; t < 1024; t++) {
            BG_PutChar(port->buffer[t]);
        }
        print("\n");
    }

    return ahci_driver;
}

void AHCI_AHCIDriver_ProbePorts(AHCIDriver *self) {
    uint32_t portsImpelmented = self->abar->portsImplemented;
    for(int i=0; i< MAX_AHCI_PORTS; i++) {
        if(portsImpelmented & (1 << i)){
            enum AHCI_PortType portType = CheckPortType(&self->abar->ports[i]);

            if (portType == AHCI_PT_SATA || portType == AHCI_PT_SATAPI) {
                self->ports[self->portCount] = MEM_Malloc(sizeof(AHCI_Port));
                self->ports[self->portCount]->portType = portType;
                self->ports[self->portCount]->hbaPort = &self->abar->ports[i];
                self->ports[self->portCount]->portNumber = self->portCount;
                self->portCount += 1;
            }
        }
    }
}

bool AHCI_Port_Read(AHCI_Port *self, uint64_t sector, uint32_t sectorCount, void *buffer) {
    uint32_t sectorL = (uint32_t) sector;
    uint32_t sectorH = (uint32_t) (sector >> 32);

    self->hbaPort->interruptStatus = (uint32_t) -1;
    
    HBACommandHeader* cmdHeader = (HBACommandHeader*) (uint64_t) self->hbaPort->commandListBase;
    cmdHeader->commandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
    cmdHeader->write = 0;
    cmdHeader->prdtLength = 1;

    HBACommandTable* commandTable = (HBACommandTable*)(uint64_t)(cmdHeader->commandTableBaseAddress);
    MEM_Set(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HBAPRDTEntry));
    
    commandTable->prdtEntry[0].dataBaseAddress = (uint32_t) (uint64_t) buffer;
    commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t) ((uint64_t)buffer >> 32);
    commandTable->prdtEntry[0].byteCount = (sectorCount << 9) - 1;
    commandTable->prdtEntry[0].interruptOnCompletion = 1;

    FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*) (&commandTable->commandFIS);
    
    cmdFIS->fisType = FIS_TYPE_REG_H2D;
    cmdFIS->commandControl = 1;
    cmdFIS->command = ATA_CMD_READ_DMA_EX;

    cmdFIS->lba0 = (uint8_t) sectorL;
    cmdFIS->lba1 = (uint8_t) (sectorL >> 8);
    cmdFIS->lba2 = (uint8_t) (sectorL >> 16);
    cmdFIS->lba3 = (uint8_t) sectorH;
    cmdFIS->lba4 = (uint8_t) (sectorH >> 8);
    cmdFIS->lba5 = (uint8_t) (sectorH >> 16);
   
    cmdFIS->deviceRegister = 1 << 6;

    cmdFIS->countLow = sectorCount & 0xFF;
    cmdFIS->countHigh = (sectorCount >> 8) & 0xFF;

    uint64_t spin = 0;

    while((self->hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
        spin++;
    }
    if (spin == 1000000) {
         return false;
    }

    self->hbaPort->commandIssue = 1;
    while(true) {
        if (self->hbaPort->commandIssue == 0) break;
        if (self->hbaPort->interruptStatus & HBA_PxIS_TFES) {
            return false;
        }
    }
    return true;
}

void AHCI_Port_Configure(AHCI_Port *self) {
    AHCI_Port_StopCMD(self);

    void* newBase = PFA_RequestPage();
    self->hbaPort->commandListBase = (uint32_t)(uint64_t) newBase;
    self->hbaPort->commandListBaseUpper = (uint32_t) ((uint64_t) newBase >> 32);

    void* fisBase = PFA_RequestPage();
    self->hbaPort->fisBaseAddress = (uint32_t)(uint64_t) fisBase;
    self->hbaPort->fisBaseAddressUpper = (uint32_t) ((uint64_t) fisBase >> 32);
    MEM_Set(fisBase, 0, 256);

    HBACommandHeader* cmdHeader = (HBACommandHeader*) ((uint64_t) self->hbaPort->commandListBase +
            ((uint64_t) self->hbaPort->commandListBaseUpper << 32));

    for (int i=0; i< 32; i++) {
        cmdHeader[i].prdtLength = 8;

        void* cmdTableAddress = PFA_RequestPage();
        uint64_t address = (uint64_t) cmdTableAddress + (i<<8);
        cmdHeader[i].commandTableBaseAddress = (uint32_t)(uint64_t) address;
        cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t) address >> 32);
        MEM_Set(cmdTableAddress, 0, 256);
    }

    AHCI_Port_StartCMD(self);
}

void AHCI_Port_StopCMD(AHCI_Port *self) {
    self->hbaPort->cmdSts &= ~HBA_PxCMD_ST;
    self->hbaPort->cmdSts &= ~HBA_PxCMD_FRE;

    while(true) {
        if (self->hbaPort->cmdSts & HBA_PxCMD_FR) continue;
        if (self->hbaPort->cmdSts & HBA_PxCMD_CR) continue;

        break;
    }
}

void AHCI_Port_StartCMD(AHCI_Port *self) {
    while (self->hbaPort->cmdSts & HBA_PxCMD_CR);

    self->hbaPort->cmdSts |= HBA_PxCMD_FRE;
    self->hbaPort->cmdSts |= HBA_PxCMD_ST;
}
