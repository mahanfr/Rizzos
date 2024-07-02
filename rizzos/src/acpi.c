#include "acpi.h"
#include <stdbool.h>
#include <stdint.h>

static bool StrEq(char* str1, char* str2, uint32_t lenght) {
    for (uint32_t i = 0; i < lenght; i++) {
        if(str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}

void* ACPI_FindTable(SDTHeader* sdtHeader, char* signature) {
    int entries = (sdtHeader->lenght - sizeof(SDTHeader)) / 8;

    for (int i = 0; i < entries; i++) {
        SDTHeader* header = (SDTHeader*)*(uint64_t*)((uint64_t) sdtHeader + sizeof(SDTHeader) + (i*8));
        if(StrEq((char*) header->signature, signature, 4)) {
            return header;
        }
    }
    return 0;
}
