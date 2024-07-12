#include "fat.h"
#include <stdint.h>
#include "../graphics/basic_graphics.h"
#include "../memory/heap.h"

FAT16* FAT_ParseFat16_12(uint8_t* buffer) {
    FAT_Bpb* bpb = (FAT_Bpb*) buffer;
    if (bpb->jmpToCode[0] != 0xEB || bpb->jmpToCode[2] != 0x90) {
        return 0;
    }
    FAT_Ebpb16* ebpb = (FAT_Ebpb16*) (buffer + sizeof(FAT_Bpb));
    if (ebpb->signature == 0x28 || ebpb->signature == 0x29) {
        return 0;
    }
    FAT16* fat16 = MEM_Malloc(sizeof(FAT16));
    fat16->bpb = *bpb;
    fat16->ebpb = *ebpb;
    return fat16;
}
