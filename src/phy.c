#include "phy.h"

static uint32_t phyAddr;

void enet_start_mii(void)
{
	int debug_tmp;
	PHY_Init();
	phyAddr = PHY_AddressDiscover();
	PHY_Write( phyAddr, PHY_BMCR, ( PHY_BMCR_AN_RESTART | PHY_BMCR_AN_ENABLE ) ); /* Start auto negotiate. */
	PHY_Read(phyAddr, PHY_BMCR, &debug_tmp);
}

/*********************************
	GPIO & CLOCK SETTINGS

Initialize communication with PHY

*********************************/

void PHY_Init(){

PORTB_PCR0 = PORT_PCR_MUX(4);					//RMII0_MDIO/MII0_MDIO
PORTB_PCR1 = PORT_PCR_MUX(4);					//RMII0_MDC/MII0_MDC 

ENET->MSCR = ENET_MSCR_MII_SPEED(59);	//Set MII speed
	
}

/********************************
			Discover PHY address
*********************************/

uint32_t PHY_AddressDiscover(){

uint32_t addrIdx = 0 , phyAddr = 0;
int data = 0;
uint32_t result;
	
	for (addrIdx = 0; addrIdx < kEnetPhyRegAll; addrIdx++)
    {
        result = PHY_Read(addrIdx, kEnetPhyId1, &data);
        if ((result == 0) && (data != 0) && (data != 0xffff) )
        {
            phyAddr = addrIdx;
						return phyAddr;
        }
    }
	
}

/**********************************
				Write to PHY register
**********************************/


int PHY_Write(int phy_addr, int reg_addr, int data){

uint32_t value; 				//What write to MMFR register
	
/* Initiatate the MII Management write */
value |= 
	ENET_MMFR_ST(0x01)  			//Start of frame to PHY
| ENET_MMFR_OP(0x01)				//Write frame
| ENET_MMFR_PA(phy_addr)		//Address of PHY
| ENET_MMFR_RA(reg_addr) 		//Which register of PHY
| ENET_MMFR_TA(0x02) 				//Turn around
| ENET_MMFR_DATA(data);

//Clear MII interrupt flag
ENET->EIR = ENET_EIR_MII_MASK;
	
//Start a write operation
ENET->MMFR = value;

//Wait for the write to complete
while(!(ENET->EIR & ENET_EIR_MII_MASK));
	
return 0;
}
/********************************************************************/


/**********************************
			Read from PHY register
**********************************/


int PHY_Read(int phy_addr, int reg_addr, int *data)
{

uint32_t value;
	
/* Initiatate the MII Management read */
value |=
  ENET_MMFR_ST(0x01)
| ENET_MMFR_OP(0x2)
| ENET_MMFR_PA(phy_addr)
| ENET_MMFR_RA(reg_addr)
| ENET_MMFR_TA(0x02);
	
ENET->EIR = ENET_EIR_MII_MASK; //Clear MII interrupt flag
	
ENET->MMFR = value; 					 //Start a read operation
	
while(!(ENET->EIR & ENET_EIR_MII_MASK)); //Wait for the read to complete
	
*data = ENET_MMFR & 0x0000FFFF;			//Read data from PHY
return 0; 													//If success return 0
	
}
