
#include <stdint.h>

const char* uefiMemTypeName(uint32_t type) {
    switch (type) {
        case 0:
            return "EfiReservedMemoryType";
            break;
        case 1:
            return "EfiLoaderCode";
            break;
        case 2:
            return "EfiLoaderData";
            break;
        case 3:
            return "EfiBootServicesCode";
            break;
        case 4:
            return "EfiBootServicesData";
            break;
        case 5:
            return "EfiRuntimeServicesCode";
            break;
        case 6:
            return "EfiRuntimeServicesData";
            break;
        case 7:
            return "EfiConventionalMemory";
            break;
        case 8:
            return "EfiUnusableMemory";
            break;
        case 9:
            return "EfiACPIReclaimMemory";
            break;
        case 10:
            return "EfiACPIMemoryNVS";
            break;
        case 11:
            return "EfiMemoryMappedIO";
            break;
        case 12:
            return "EfiMemoryMappedIOPortSpace";
            break;
        case 13:
            return "EfiPalCode";
            break;
        default:
            return "UNREACHABLE";
            break;
    };
}
