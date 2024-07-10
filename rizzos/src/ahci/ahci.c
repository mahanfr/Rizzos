#include "ahci.h"
#include "../graphics/basic_graphics.h"

AHCIDriver AHCI_AHCIDriver(PCIDeviceHeader *pciBaseAddress) {
    AHCIDriver ahci_driver = {0};
    ahci_driver.pciBaseAddress = pciBaseAddress;
    print("AHCI Driver Initialized!\n");
    return ahci_driver;
}
