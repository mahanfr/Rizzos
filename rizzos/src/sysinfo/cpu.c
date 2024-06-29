#include "cpu.h"
#include <stdint.h>
#include <stdbool.h>

static CpuInfo g_cpuInfo = {0};
static bool is_intialized = false;
static uint64_t g_featureInfo;

static inline void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
    asm volatile("cpuid"
            : "=a" (*eax), "=b" (*ebx),
              "=c" (*ecx), "=d" (*edx)
            : "0" (*eax), "2" (*ecx));
}

static void Set_ManufacturerID(uint32_t ebx, uint32_t ecx, uint32_t edx) {
    g_cpuInfo.manufacturerID[3] = (ebx & 0xFF000000) >> 24;
    g_cpuInfo.manufacturerID[2] = (ebx & 0x00FF0000) >> 16;
    g_cpuInfo.manufacturerID[1] = (ebx & 0x0000FF00) >> 8;
    g_cpuInfo.manufacturerID[0] = (ebx & 0x000000FF);

    g_cpuInfo.manufacturerID[7] = (edx & 0xFF000000) >> 24;
    g_cpuInfo.manufacturerID[6] = (edx & 0x00FF0000) >> 16;
    g_cpuInfo.manufacturerID[5] = (edx & 0x0000FF00) >> 8;
    g_cpuInfo.manufacturerID[4] = (edx & 0x000000FF);

    g_cpuInfo.manufacturerID[11] = (ecx & 0xFF000000) >> 24;
    g_cpuInfo.manufacturerID[10] = (ecx & 0x00FF0000) >> 16;
    g_cpuInfo.manufacturerID[9]  = (ecx & 0x0000FF00) >> 8;
    g_cpuInfo.manufacturerID[8]  = (ecx & 0x000000FF);

    g_cpuInfo.manufacturerID[12] = 0;
}

bool SysInfo_IsFeatureAvailable(uint8_t flag) {
    (void) SysInfo_GetCpuInfo();
    return (g_featureInfo & (1 << flag)) > 0;
}

CpuInfo SysInfo_GetCpuInfo(void) {
    if (is_intialized) {
        return g_cpuInfo;
    }
    is_intialized = true;
    uint32_t eax, ebx, ecx, edx;

    eax = 0;
    cpuid(&eax, &ebx, &ecx, &edx);
    g_cpuInfo.hfp = eax;
    Set_ManufacturerID(ebx, ecx, edx);

    eax = 1; ebx = 0; ecx = 0; edx = 0;
    cpuid(&eax, &ebx, &ecx, &edx);
    g_cpuInfo.steppingID = (eax & 0xF);
    g_cpuInfo.model = (eax >> 4) & 0xF;
    g_cpuInfo.familyID = (eax >> 8) & 0xF;
    g_cpuInfo.processorType = (eax >> 12) & 0x3;
    g_cpuInfo.extendedModelID = (eax >> 16) & 0xF;
    g_cpuInfo.extendedFamilyID = (eax >> 20) & 0xFF;
    g_featureInfo = ((uint64_t)ecx << 32) | edx;

    return g_cpuInfo;
}
