#ifndef _SYSINFO_CPU_
#define _SYSINFO_CPU_

#include <stdint.h>
#include <stdbool.h>

#define SYSINFO_CPUF_FPU 0
#define SYSINFO_CPUF_VME 1
#define SYSINFO_CPUF_PSE 3
#define SYSINFO_CPUF_TSC 4
#define SYSINFO_CPUF_APIC 9
#define SYSINFO_CPUF_MMX 23
#define SYSINFO_CPUF_SSE 25
#define SYSINFO_CPUF_SSE2 26
#define SYSINFO_CPUF_SSE3 32
#define SYSINFO_CPUF_SSSE3 41
#define SYSINFO_CPUF_SSE41 51
#define SYSINFO_CPUF_SSE42 52
#define SYSINFO_CPUF_AVX 60

typedef struct {
    unsigned char manufacturerID[13];
    uint32_t hfp;
    uint8_t steppingID;
    uint8_t model;
    uint8_t familyID;
    uint8_t processorType;
    uint8_t extendedModelID;
    uint8_t extendedFamilyID;
} CpuInfo;

CpuInfo SysInfo_GetCpuInfo(void);
bool SysInfo_IsFeatureAvailable(uint8_t flag);

#endif
