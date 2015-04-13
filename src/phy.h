#ifndef PHY_H
#define PHY_H

#include "MK64f12.h"

#define PHY_BMCR 0
#define PHY_BMCR_AN_RESTART (1<<9)
#define PHY_BMCR_AN_ENABLE (1<<12)
#define MII_TIMEOUT 0xFFFF //ns
#define PHY_PHYIDR1 1
#define kEnetPhyId1 2U
#define kEnetPhyRegAll 0x20U


void PHY_Init(void);
uint32_t PHY_AddressDiscover(void);
int PHY_Write(int phy_addr, int reg_addr, int data);
int PHY_Read(int phy_addr, int reg_addr, int *data);
void enet_start_mii(void);
void Delay(void);

#endif /* PHY_H */

