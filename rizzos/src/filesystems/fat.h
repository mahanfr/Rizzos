#ifndef _FILESYSTEMS_FAT32_H_
#define _FILESYSTEMS_FAT32_H_

#include <stdbool.h>
#include <stdint.h>
typedef struct {
    uint8_t jmpToCode[3];
    uint8_t OEMIdentifier[8];
    uint16_t numBytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t numReservedSectors;
    uint8_t numFileAlocationTables;
    uint16_t numRootDirEntries;
    uint16_t numLogicalSectors;
    uint8_t mediaDescriptorType;
    uint16_t numSectorsPerFat;
    uint16_t numSectorsPerTrack;
    uint16_t numHeads;
    uint32_t numHiddenSectors;
    uint32_t largeSectorCount;
} FAT_Bpb;

typedef struct {
    uint8_t driveNumber;
    uint8_t resv;
    uint8_t signature;
    uint32_t volumeID;
    uint8_t volumeLable[11];
    uint8_t systemIdentString[8];
} FAT_Ebpb16;

typedef struct {
    uint32_t sectorsPerFat;
    uint16_t flags;
    uint16_t versionNumber;
    uint32_t clusterNumRootDir;
    uint16_t sectorNumFSInfo;
    uint16_t sectorBackupBootSector;
    uint8_t resv0[12];
    uint8_t driveNumber;
    uint8_t resv1;
    uint8_t signature;
    uint32_t volumeID;
    uint8_t volumeLable[11];
    uint8_t systemIdentString[8];
} FAT_Ebpb32;

typedef struct {
    FAT_Bpb bpb;
    FAT_Ebpb32 ebpb;
} FAT32;

typedef struct {
    FAT_Bpb bpb;
    FAT_Ebpb16 ebpb;
} FAT16;

FAT32* FAT_ParseFat32(uint8_t* buffer);
FAT16* FAT_ParseFat16_12(uint8_t* buffer);

#endif
