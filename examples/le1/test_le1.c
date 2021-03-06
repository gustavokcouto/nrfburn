#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "reg24le1.h"
#include "nrfdbg.h"
#include "nrfutils.h"

#include "nRF24L.h"

#define NRF_ADDR_SIZE	5
#define CHANNEL_NUM		2

__code const uint8_t DongleAddr[] = {0x36, 0xC4, 0x31, 0x40, 0x03};
	
void main()
{
	P0DIR = 0x00;	// all out
	P1DIR = 0x00;	// all out

	// init in receiver mode
	nRF_Init();
	
	nRF_WriteAddrReg(RX_ADDR_P0, DongleAddr, NRF_ADDR_SIZE);

	//nRF_WriteReg(EN_AA, vENAA_P0);		// enable auto acknowledge
	nRF_WriteReg(EN_AA, 0);					// disable auto acknowledge
	nRF_WriteReg(SETUP_RETR, vARD_250us);	// ARD=250us, ARC=disabled
	nRF_WriteReg(RF_SETUP, vRF_DR_2MBPS		// data rate
						| vRF_PWR_0DBM);	// output power

	nRF_WriteReg(FEATURE, vEN_DPL | vEN_ACK_PAY);	// enable dynamic payload length and ACK payload
	nRF_WriteReg(DYNPD, vDPL_P0);					// enable dynamic payload length for pipe 0

	nRF_FlushRX();
	nRF_FlushTX();
	
	nRF_WriteReg(EN_RXADDR, vERX_P0);					// enable RX address
	nRF_WriteReg(STATUS, vRX_DR | vTX_DS | vMAX_RT);	// reset the IRQ flags

	nRF_WriteReg(RF_CH, CHANNEL_NUM);			// set the channel
	nRF_WriteReg(CONFIG, vEN_CRC | vCRCO 		// enable a 2 byte CRC
								| vMASK_TX_DS	// we don't care about the TX_DS status flag
								| vPRIM_RX		// RX mode
								| vPWR_UP);		// power up the transceiver

	nRF_CE_hi();		// start receiving

	while (1)
	{
		if (RFIRQ)
		{
			nRF_ReadRxPayload(7);
			nRF_WriteReg(STATUS, vRX_DR);
			RFIRQ = 0;
			
			P10 = 1;
			delay_us(100);
			P10 = 0;
			/*
		} else {
			P11 = 1;
			delay_us(100);
			P11 = 0;
			*/
		}
		
		//printf("asd %i\n", 12);
		//putchar('a');
	}
}
