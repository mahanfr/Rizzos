#ifndef _AHCI_FIS_H_
#define _AHCI_FIS_H_

#include <stdint.h>
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define ATA_CMD_READ_DMA_EX 0x25

typedef enum {
    FIS_TYPE_REG_H2D = 0x27,
    FIS_TYPE_REG_D2H = 0x34,
    FIS_TYPE_DMA_ACT = 0x39,
    FIS_TYPE_DMA_SETUP = 0x41,
    FIS_TYPE_DATA = 0x46,
    FIS_TYPE_BIST = 0x58,
    FIS_TYPE_PIO_SETUP = 0x5F,
    FIS_TYPE_DEV_BITS = 0xA1,
} FIS_TYPE;

typedef struct {
    uint32_t dataBaseAddress;
    uint32_t dataBaseAddressUpper;
    uint32_t resv0;

    uint32_t byteCount : 22;
    uint32_t resv1 : 9;
    uint32_t interruptOnCompletion : 1;
} HBAPRDTEntry;

typedef struct {
    uint8_t commandFIS[64];
    uint8_t atapiCommand[16];
    uint8_t rsv[48];
    HBAPRDTEntry prdtEntry[];
} HBACommandTable;

typedef struct {
    uint8_t fisType;

    uint8_t portMultiplier:4;
    uint8_t rsv0:3;
    uint8_t commandControl:1;

    uint8_t command;
    uint8_t featureLow;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t deviceRegister;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t featureHigh;

    uint8_t countLow;
    uint8_t countHigh;
    uint8_t isoCommandCompletion;
    uint8_t control;

    uint8_t rsv1[4];
} FIS_REG_H2D;

#endif
