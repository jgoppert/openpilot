/**
 ******************************************************************************
 *
 * @file       rfm22b.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      RF Module hardware layer
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

// *****************************************************************
// RFM22B hardware layer
//
// This module uses the RFM22B's internal packet handling hardware to encapsulate our own packet data.
//
// The RFM22B internal hardware packet handler configuration is as follows ..
//
// 4-byte (32-bit) preamble .. alternating 0's & 1's
// 4-byte (32-bit) sync
// 1-byte packet length (number of data bytes to follow)
// 0 to 255 user data bytes
//
// Our own packet data will also contain it's own header and 32-bit CRC as a single 16-bit CRC is not sufficient for wireless comms.
//
// *****************************************************************

#include <string.h>		// memmove

#include "stm32f10x.h"
#include "main.h"
#include "stopwatch.h"
#include "gpio_in.h"
#include "rfm22b.h"

#if defined(PIOS_COM_DEBUG)
//      #define RFM22_DEBUG
//      #define RFM22_INT_TIMEOUT_DEBUG
#endif

// *****************************************************************
// forward delarations

#if defined(RFM22_EXT_INT_USE)
	void rfm22_processInt(void);
#endif

// ************************************
// this is too adjust the RF module so that it is on frequency

#define OSC_LOAD_CAP					0x7F	// cap = 12.5pf .. default

#define OSC_LOAD_CAP_1					0x7D	// board 1
#define OSC_LOAD_CAP_2					0x7B	// board 2
#define OSC_LOAD_CAP_3					0x7E	// board 3
#define OSC_LOAD_CAP_4					0x7F	// board 4

// ************************************

#define TX_TEST_MODE_TIMELIMIT_MS		30000	// TX test modes time limit (in ms)

#define TX_PREAMBLE_NIBBLES				8		// 7 to 511 (number of nibbles)
#define RX_PREAMBLE_NIBBLES				5		// 5 to 31 (number of nibbles)

#define FIFO_SIZE						64		// the size of the rf modules internal FIFO buffers

#define TX_FIFO_HI_WATERMARK			62		// 0-63
#define TX_FIFO_LO_WATERMARK			32		// 0-63

#define RX_FIFO_HI_WATERMARK			32		// 0-63

#define SYNC_BYTE_1						0x2D    // RF sync bytes (32-bit in all)
#define SYNC_BYTE_2						0xD4    //
#define SYNC_BYTE_3						0x4B    //
#define SYNC_BYTE_4						0x59    //

// ************************************
// the default TX power level

#define RFM22_DEFAULT_RF_POWER          rfm22_tx_pwr_txpow_0    // +1dBm ... 1.25mW
//#define RFM22_DEFAULT_RF_POWER          rfm22_tx_pwr_txpow_1    // +2dBm ... 1.6mW
//#define RFM22_DEFAULT_RF_POWER          rfm22_tx_pwr_txpow_2    // +5dBm ... 3.16mW
//#define RFM22_DEFAULT_RF_POWER          rfm22_tx_pwr_txpow_3    // +8dBm ... 6.3mW
//#define RFM22_DEFAULT_RF_POWER          rfm22_tx_pwr_txpow_4    // +11dBm .. 12.6mW
//#define RFM22_DEFAULT_RF_POWER          rfm22_tx_pwr_txpow_5    // +14dBm .. 25mW
//#define RFM22_DEFAULT_RF_POWER          rfm22_tx_pwr_txpow_6    // +17dBm .. 50mW
//#define RFM22_DEFAULT_RF_POWER          rfm22_tx_pwr_txpow_7    // +20dBm .. 100mW

// ************************************
// the default RF datarate

//#define RFM22_DEFAULT_RF_DATARATE       500          // 500 bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       1000         // 1k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       2000         // 2k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       4000         // 4k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       8000         // 8k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       9600         // 9.6k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       16000        // 16k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       19200        // 19k2 bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       24000        // 24k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       32000        // 32k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       64000        // 64k bits per sec
#define RFM22_DEFAULT_RF_DATARATE       128000       // 128k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       192000       // 192k bits per sec
//#define RFM22_DEFAULT_RF_DATARATE       256000       // 256k bits per sec .. NOT YET WORKING

// ************************************
// GFSK modulation
// no manchester encoding
// data whitening
// FIFO mode
//  5-nibble rx preamble length detection
// 10-nibble tx preamble length

#define LOOKUP_SIZE	 14

/*
// xtal 20 ppm
*/

// xtal 10 ppm, 434MHz
uint32_t            data_rate[LOOKUP_SIZE] = {   500,  1000,  2000,  4000,  8000,  9600, 16000, 19200, 24000,  32000,  64000, 128000, 192000, 256000};
uint8_t      modulation_index[LOOKUP_SIZE] = {    16,     8,     4,     2,     1,     1,     1,     1,     1,      1,      1,      1,      1,      1};
uint32_t       freq_deviation[LOOKUP_SIZE] = {  4000,  4000,  4000,  4000,  4000,  4800,  8000,  9600, 12000,  16000,  32000,  64000,  96000, 128000};
uint32_t         rx_bandwidth[LOOKUP_SIZE] = { 17500, 17500, 17500, 17500, 17500, 19400, 32200, 38600, 51200,  64100, 137900, 269300, 420200, 518800};
int8_t        est_rx_sens_dBm[LOOKUP_SIZE] = {  -118,  -118,  -117,  -116,  -115,  -115,  -112,  -112,  -110,   -109,   -106,   -103,   -101,   -100}; // estimated receiver sensitivity for BER = 1E-3

uint8_t                reg_1C[LOOKUP_SIZE] = {  0x3A,  0x3A,  0x3A,  0x3A,  0x3A,  0x3B,  0x26,  0x2B,  0x2E,   0x16,   0x07,   0x83,   0x8A,   0x8C}; // rfm22_if_filter_bandwidth
uint8_t                reg_1D[LOOKUP_SIZE] = {  0x40,  0x40,  0x40,  0x40,  0x40,  0x40,  0x40,  0x40,  0x40,   0x40,   0x40,   0x40,   0x40,   0x40}; // rfm22_afc_loop_gearshift_override

uint8_t                reg_20[LOOKUP_SIZE] = {  0xE8,  0xF4,  0xFA,  0x7D,  0x3F,  0x34,  0x3F,  0x34,  0x2A,   0x3F,   0x3F,   0x5E,   0x3F,   0x2F}; // rfm22_clk_recovery_oversampling_ratio
uint8_t                reg_21[LOOKUP_SIZE] = {  0x60,  0x20,  0x00,  0x01,  0x02,  0x02,  0x02,  0x02,  0x03,   0x02,   0x02,   0x01,   0x02,   0x02}; // rfm22_clk_recovery_offset2
uint8_t                reg_22[LOOKUP_SIZE] = {  0x20,  0x41,  0x83,  0x06,  0x0C,  0x75,  0x0C,  0x75,  0x12,   0x0C,   0x0C,   0x5D,   0x0C,   0xBB}; // rfm22_clk_recovery_offset1
uint8_t                reg_23[LOOKUP_SIZE] = {  0xC5,  0x89,  0x12,  0x25,  0x4A,  0x25,  0x4A,  0x25,  0x6F,   0x4A,   0x4A,   0x86,   0x4A,   0x0D}; // rfm22_clk_recovery_offset0
uint8_t                reg_24[LOOKUP_SIZE] = {  0x00,  0x00,  0x00,  0x02,  0x07,  0x07,  0x07,  0x07,  0x07,   0x07,   0x07,   0x02,   0x04,   0x05}; // rfm22_clk_recovery_timing_loop_gain1
uint8_t                reg_25[LOOKUP_SIZE] = {  0x0A,  0x23,  0x85,  0x0E,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,   0xFF,   0xFF,   0xBB,   0x12,   0x74}; // rfm22_clk_recovery_timing_loop_gain0

uint8_t                reg_2A[LOOKUP_SIZE] = {  0x0E,  0x0E,  0x0E,  0x0E,  0x0E,  0x0D,  0x0D,  0x0E,  0x12,   0x17,   0x31,   0x50,   0x50,   0x50}; // rfm22_afc_limiter .. AFC_pull_in_range = �AFCLimiter[7:0] x (hbsel+1) x 625 Hz

uint8_t                reg_6E[LOOKUP_SIZE] = {  0x04,  0x08,  0x10,  0x20,  0x41,  0x4E,  0x83,  0x9D,  0xC4,   0x08,   0x10,   0x20,   0x31,   0x41}; // rfm22_tx_data_rate1
uint8_t                reg_6F[LOOKUP_SIZE] = {  0x19,  0x31,  0x62,  0xC5,  0x89,  0xA5,  0x12,  0x49,  0x9C,   0x31,   0x62,   0xC5,   0x27,   0x89}; // rfm22_tx_data_rate0

uint8_t                reg_70[LOOKUP_SIZE] = {  0x2D,  0x2D,  0x2D,  0x2D,  0x2D,  0x2D,  0x2D,  0x2D,  0x2D,   0x0D,   0x0D,   0x0D,   0x0D,   0x0D}; // rfm22_modulation_mode_control1
uint8_t                reg_71[LOOKUP_SIZE] = {  0x23,  0x23,  0x23,  0x23,  0x23,  0x23,  0x23,  0x23,  0x23,   0x23,   0x23,   0x23,   0x23,   0x23}; // rfm22_modulation_mode_control2

uint8_t                reg_72[LOOKUP_SIZE] = {  0x06,  0x06,  0x06,  0x06,  0x06,  0x08,  0x0D,  0x0F,  0x13,   0x1A,   0x33,   0x66,   0x9A,   0xCD}; // rfm22_frequency_deviation

// ************************************

volatile bool		initialized = false;

#if defined(RFM22_EXT_INT_USE)
	volatile bool		exec_using_spi;					// set this if you want to access the SPI bus outside of the interrupt
	volatile bool		inside_ext_int;					// this is set whenever we are inside the interrupt
#endif

uint8_t				device_type;						// the RF chips device ID number
uint8_t				device_version;						// the RF chips revision number

volatile uint8_t	rf_mode;							// holds our current RF mode

uint32_t			lower_carrier_frequency_limit_Hz;	// the minimum RF frequency we can use
uint32_t			upper_carrier_frequency_limit_Hz;	// the maximum RF frequency we can use
uint32_t			carrier_frequency_hz;				// the current RF frequency we are on

uint32_t			carrier_datarate_bps;				// the RF data rate we are using

uint8_t				hbsel;								// holds the hbsel (1 or 2)
float				frequency_step_size;				//

uint8_t				frequency_hop_channel;				// current frequency hop channel

uint8_t				frequency_hop_step_size_reg;		//

uint8_t				adc_config;							// holds the adc config reg value

volatile uint8_t	device_status;						// device status register
volatile uint8_t	int_status1;						// interrupt status register 1
volatile uint8_t	int_status2;						// interrupt status register 2
volatile uint8_t	ezmac_status;						// ezmac status register

volatile int16_t	afc_correction;						// afc correction reading
volatile int32_t	afc_correction_Hz;					// afc correction reading (in Hz)

volatile int16_t	temperature_reg;					// the temperature sensor reading

#if defined(RFM22_DEBUG)
	volatile uint8_t	prev_device_status;				// just for debugging
	volatile uint8_t	prev_int_status1;				//  "          "
	volatile uint8_t	prev_int_status2;				//  "          "
	volatile uint8_t	prev_ezmac_status;				//  "          "

	bool debug_outputted;
#endif

volatile uint8_t	osc_load_cap;						// xtal frequency calibration value

volatile uint8_t	rssi;								// the current RSSI (register value)
volatile int16_t	rssi_dBm;							// dBm value

uint8_t			tx_power;							// the transmit power to use for data transmissions
volatile uint8_t	tx_pwr;								// the tx power register read back

volatile uint8_t	rx_buffer_current;									// the current receive buffer in use (double buffer)
volatile uint8_t	rx_buffer[256] __attribute__ ((aligned(4)));		// the receive buffer .. received packet data is saved here
volatile uint16_t	rx_buffer_wr;										// the receive buffer write index

volatile uint8_t	rx_packet_buf[256] __attribute__ ((aligned(4)));	// the received packet
volatile uint16_t	rx_packet_wr;										// the receive packet write index
volatile int16_t	rx_packet_start_rssi_dBm;							//
volatile int32_t	rx_packet_start_afc_Hz;								//
volatile int16_t	rx_packet_rssi_dBm;									// the received packet signal strength
volatile int32_t	rx_packet_afc_Hz;									// the receive packet frequency offset

volatile uint8_t	*tx_data_addr;						// the address of the data we send in the transmitted packets
volatile uint16_t	tx_data_rd;							// the tx data read index
volatile uint16_t	tx_data_wr;							// the tx data write index

int					lookup_index;

volatile bool		power_on_reset;						// set if the RF module has reset itself

volatile uint16_t	rfm22_int_timer;					// used to detect if the RF module stops responding. thus act accordingly if it does stop responding.
volatile uint16_t	rfm22_int_time_outs;				// counter
volatile uint16_t	prev_rfm22_int_time_outs;			//

uint32_t			clear_channel_count = (TX_PREAMBLE_NIBBLES + 4) * 2;	// minimum clear channel time before allowing transmit

uint16_t			timeout_ms = 20000;					//
uint16_t			timeout_sync_ms = 3;				//
uint16_t			timeout_data_ms = 20;				//

// ************************************
// SPI read/write

void rfm22_startBurstWrite(uint8_t addr)
{
	// wait 1us .. so we don't toggle the CS line to quickly
	PIOS_DELAY_WaituS(1);

	// chip select line LOW
	PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 0);

	PIOS_SPI_TransferByte(RFM22_PIOS_SPI, 0x80 | addr);
}

inline void rfm22_burstWrite(uint8_t data)
{
	PIOS_SPI_TransferByte(RFM22_PIOS_SPI, data);
}

void rfm22_endBurstWrite(void)
{
	// chip select line HIGH
	PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 1);
}

void rfm22_write(uint8_t addr, uint8_t data)
{
	// wait 1us .. so we don't toggle the CS line to quickly
	PIOS_DELAY_WaituS(1);

	// chip select line LOW
	PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 0);

	PIOS_SPI_TransferByte(RFM22_PIOS_SPI, 0x80 | addr);
	PIOS_SPI_TransferByte(RFM22_PIOS_SPI, data);

	// chip select line HIGH
	PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 1);
}

void rfm22_startBurstRead(uint8_t addr)
{
	// wait 1us .. so we don't toggle the CS line to quickly
	PIOS_DELAY_WaituS(1);

	// chip select line LOW
	PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 0);

	PIOS_SPI_TransferByte(RFM22_PIOS_SPI, addr & 0x7f);
}

inline uint8_t rfm22_burstRead(void)
{
	return PIOS_SPI_TransferByte(RFM22_PIOS_SPI, 0xff);
}

void rfm22_endBurstRead(void)
{
	// chip select line HIGH
	PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 1);
}

uint8_t rfm22_read(uint8_t addr)
{
	uint8_t rdata;

	// wait 1us .. so we don't toggle the CS line to quickly
	PIOS_DELAY_WaituS(1);

	// chip select line LOW
	PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 0);

	PIOS_SPI_TransferByte(RFM22_PIOS_SPI, addr & 0x7f);
	rdata = PIOS_SPI_TransferByte(RFM22_PIOS_SPI, 0xff);

	// chip select line HIGH
	PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 1);

	return rdata;
}

// ************************************
// external interrupt

#if defined(RFM22_EXT_INT_USE)

	void RFM22_EXT_INT_FUNC(void)
	{
		inside_ext_int = TRUE;

		if (EXTI_GetITStatus(RFM22_EXT_INT_LINE) != RESET)
		{
			// Clear the EXTI line pending bit
			EXTI_ClearITPendingBit(RFM22_EXT_INT_LINE);

//			USB_LED_TOGGLE;	// TEST ONLY

			if (!booting && !exec_using_spi)
			{
//				while (!GPIO_IN(RF_INT_PIN) && !exec_using_spi)
				{	// stay here until the interrupt line returns HIGH
					rfm22_processInt();
				}
			}
		}

		inside_ext_int = FALSE;
	}

	void rfm22_disableExtInt(void)
	{
		// Configure the external interrupt
		GPIO_EXTILineConfig(RFM22_EXT_INT_PORT_SOURCE, RFM22_EXT_INT_PIN_SOURCE);
		EXTI_InitTypeDef EXTI_InitStructure;
		EXTI_InitStructure.EXTI_Line = RFM22_EXT_INT_LINE;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
		EXTI_Init(&EXTI_InitStructure);

		EXTI_ClearFlag(RFM22_EXT_INT_LINE);
	}

	void rfm22_enableExtInt(void)
	{
		// Configure the external interrupt
		GPIO_EXTILineConfig(RFM22_EXT_INT_PORT_SOURCE, RFM22_EXT_INT_PIN_SOURCE);
		EXTI_InitTypeDef EXTI_InitStructure;
		EXTI_InitStructure.EXTI_Line = RFM22_EXT_INT_LINE;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		EXTI_ClearFlag(RFM22_EXT_INT_LINE);

		// Enable and set the external interrupt
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = RFM22_EXT_INT_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RFM22_EXT_INT_PRIORITY;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}

#endif

// ************************************
// set/get the frequency calibration value

void rfm22_setFreqCalibration(uint8_t value)
{
	osc_load_cap = value;

	if (!initialized || power_on_reset)
		return;				// we haven't yet been initialized

	#if defined(RFM22_EXT_INT_USE)
		exec_using_spi = TRUE;
	#endif

	rfm22_write(rfm22_xtal_osc_load_cap, osc_load_cap);

	#if defined(RFM22_EXT_INT_USE)
		exec_using_spi = FALSE;
	#endif
}

uint8_t rfm22_getFreqCalibration(void)
{
	return osc_load_cap;
}

// ************************************
// radio datarate about 19200 Baud
// radio frequency deviation 45kHz
// radio receiver bandwidth 67kHz.
//
// Carson's rule:
//  The signal bandwidth is about 2(Delta-f + fm) ..
//
// Delta-f = frequency deviation
// fm = maximum frequency of the signal
//
// This gives 2(45 + 9.6) = 109.2kHz.

void rfm22_setDatarate(uint32_t datarate_bps)
{

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = TRUE;
#endif
	// *******

	lookup_index = 0;
	while (lookup_index < (LOOKUP_SIZE - 1) && data_rate[lookup_index] < datarate_bps)
		lookup_index++;

	carrier_datarate_bps = datarate_bps = data_rate[lookup_index];

	// ********************************

#if defined(RFM22_DEBUG)
	uint32_t frequency_deviation = freq_deviation[lookup_index];	// Hz
	uint32_t modulation_bandwidth = datarate_bps + (2 * frequency_deviation);
#endif






	rfm22_write(0x1C, reg_1C[lookup_index]);	// rfm22_if_filter_bandwidth
	rfm22_write(0x1D, reg_1D[lookup_index]);	// rfm22_afc_loop_gearshift_override

	rfm22_write(0x20, reg_20[lookup_index]);	// rfm22_clk_recovery_oversampling_ratio
	rfm22_write(0x21, reg_21[lookup_index]);	// rfm22_clk_recovery_offset2
	rfm22_write(0x22, reg_22[lookup_index]);	// rfm22_clk_recovery_offset1
	rfm22_write(0x23, reg_23[lookup_index]);	// rfm22_clk_recovery_offset0
	rfm22_write(0x24, reg_24[lookup_index]);	// rfm22_clk_recovery_timing_loop_gain1
	rfm22_write(0x25, reg_25[lookup_index]);	// rfm22_clk_recovery_timing_loop_gain0

	rfm22_write(0x2A, reg_2A[lookup_index]);	// rfm22_afc_limiter

	if (carrier_datarate_bps < 100000)
		rfm22_write(0x58, 0x80);				// rfm22_chargepump_current_trimming_override
	else
		rfm22_write(0x58, 0xC0);				// rfm22_chargepump_current_trimming_override

	rfm22_write(0x6E, reg_6E[lookup_index]);	// rfm22_tx_data_rate1
	rfm22_write(0x6F, reg_6F[lookup_index]);	// rfm22_tx_data_rate0

	rfm22_write(0x70, reg_70[lookup_index]);	// rfm22_modulation_mode_control1
	rfm22_write(0x71, reg_71[lookup_index]);	// rfm22_modulation_mode_control2

	rfm22_write(0x72, reg_72[lookup_index]);	// rfm22_frequency_deviation

	rfm22_write(rfm22_ook_counter_value1, 0x00);
	rfm22_write(rfm22_ook_counter_value2, 0x00);

	// ********************************
	// calculate the TX register values
/*
	uint16_t fd = frequency_deviation / 625;

	uint8_t mmc1 = rfm22_mmc1_enphpwdn | rfm22_mmc1_manppol;
	uint16_t txdr;
	if (datarate_bps < 30000)
	{
		txdr = (datarate_bps * 20972) / 10000;
		mmc1 |= rfm22_mmc1_txdtrtscale;
	}
	else
		txdr = (datarate_bps * 6553) / 100000;

	uint8_t mmc2 = rfm22_mmc2_dtmod_fifo | rfm22_mmc2_modtyp_gfsk;		// FIFO mode, GFSK
//	uint8_t mmc2 = rfm22_mmc2_dtmod_pn9 | rfm22_mmc2_modtyp_gfsk;		// PN9 mode, GFSK .. TX TEST MODE
	if (fd & 0x100) mmc2 |= rfm22_mmc2_fd;

	rfm22_write(rfm22_frequency_deviation, fd);							// set the TX peak frequency deviation

	rfm22_write(rfm22_modulation_mode_control1, mmc1);
	rfm22_write(rfm22_modulation_mode_control2, mmc2);

	rfm22_write(rfm22_tx_data_rate1, txdr >> 8);						// set the TX data rate
	rfm22_write(rfm22_tx_data_rate0, txdr);								//   "         "
*/
	// ********************************
	// determine a clear channel time

	// initialise the stopwatch with a suitable resolution for the datarate
	STOPWATCH_init(4000000ul / carrier_datarate_bps);					// set resolution to the time for 1 nibble (4-bits) at rf datarate

	// ********************************
	// determine suitable time-out periods

	timeout_sync_ms = (8000ul * 16) / carrier_datarate_bps;				// milliseconds
	if (timeout_sync_ms < 3)
		timeout_sync_ms = 3;											// because out timer resolution is only 1ms

	timeout_data_ms = (8000ul * 100) / carrier_datarate_bps;			// milliseconds
	if (timeout_data_ms < 3)
		timeout_data_ms = 3;											// because out timer resolution is only 1ms

	// ********************************

#if defined(RFM22_DEBUG)
	DEBUG_PRINTF("rf datarate_bps: %d\r\n", datarate_bps);
	DEBUG_PRINTF("rf frequency_deviation: %d\r\n", frequency_deviation);
	DEBUG_PRINTF("rf modulation_bandwidth: %u\r\n", modulation_bandwidth);
	DEBUG_PRINTF("rf_rx_bandwidth[%u]: %u\r\n", lookup_index, rx_bandwidth[lookup_index]);
	DEBUG_PRINTF("rf est rx sensitivity[%u]: %ddBm\r\n", lookup_index, est_rx_sens_dBm[lookup_index]);
#endif

	// *******

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = FALSE;
#endif

}

uint32_t rfm22_getDatarate(void)
{
    return carrier_datarate_bps;
}

// ************************************
// set/get the current tx power setting

void rfm22_setTxPower(uint8_t tx_pwr)
{
    switch (tx_pwr)
    {
      case 0: tx_power = rfm22_tx_pwr_txpow_0; break;    // +1dBm ... 1.25mW
      case 1: tx_power = rfm22_tx_pwr_txpow_1; break;    // +2dBm ... 1.6mW
      case 2: tx_power = rfm22_tx_pwr_txpow_2; break;    // +5dBm ... 3.16mW
      case 3: tx_power = rfm22_tx_pwr_txpow_3; break;    // +8dBm ... 6.3mW
      case 4: tx_power = rfm22_tx_pwr_txpow_4; break;    // +11dBm .. 12.6mW
      case 5: tx_power = rfm22_tx_pwr_txpow_5; break;    // +14dBm .. 25mW
      case 6: tx_power = rfm22_tx_pwr_txpow_6; break;    // +17dBm .. 50mW
      case 7: tx_power = rfm22_tx_pwr_txpow_7; break;    // +20dBm .. 100mW
      default: break;
    }
}

uint8_t rfm22_getTxPower(void)
{
    return tx_power;
}

// ************************************

void rfm22_setNominalCarrierFrequency(uint32_t frequency_hz)
{

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = TRUE;
#endif

	// *******

	if (frequency_hz < lower_carrier_frequency_limit_Hz) frequency_hz = lower_carrier_frequency_limit_Hz;
	else
	if (frequency_hz > upper_carrier_frequency_limit_Hz) frequency_hz = upper_carrier_frequency_limit_Hz;

	carrier_frequency_hz = frequency_hz;

	if (frequency_hz < 480000000)
		hbsel = 1;
	else
		hbsel = 2;
	uint8_t fb = (uint8_t)(frequency_hz / (10000000 * hbsel));

	uint32_t fc = (uint32_t)(frequency_hz - (10000000 * hbsel * fb));

	fc = (fc * 64u) / (10000ul * hbsel);
	fb -= 24;

	if (hbsel > 1)
		fb |= rfm22_fbs_hbsel;

	fb |= rfm22_fbs_sbse;	// is this the RX LO polarity?

	frequency_step_size = 156.25f * hbsel;

	rfm22_write(rfm22_frequency_hopping_channel_select, frequency_hop_channel);	// frequency hoppping channel (0-255)

	rfm22_write(rfm22_frequency_offset1, 0);						// no frequency offset
	rfm22_write(rfm22_frequency_offset2, 0);						// no frequency offset

	rfm22_write(rfm22_frequency_band_select, fb);					// set the carrier frequency
	rfm22_write(rfm22_nominal_carrier_frequency1, fc >> 8);			//    "            "
	rfm22_write(rfm22_nominal_carrier_frequency0, fc & 0xff);		//    "            "

	// *******

#if defined(RFM22_DEBUG)
	DEBUG_PRINTF("rf setFreq: %0.2f\r\n", carrier_frequency_hz);
#endif

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = FALSE;
#endif

}

uint32_t rfm22_getNominalCarrierFrequency(void)
{
    return carrier_frequency_hz;
}

float rfm22_getFrequencyStepSize(void)
{
	return frequency_step_size;
}

void rfm22_setFreqHopChannel(uint8_t channel)
{	// set the frequency hopping channel
	frequency_hop_channel = channel;
	rfm22_write(rfm22_frequency_hopping_channel_select, frequency_hop_channel);
}

uint8_t rfm22_freqHopChannel(void)
{	// return the current frequency hopping channel
	return frequency_hop_channel;
}

// ************************************

void rfm22_reenableRx(void)
{
#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = TRUE;
#endif

	RX_LED_OFF;

	// disable the receiver
//	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_xton);								// READY mode
	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_pllon);							// TUNE mode

	// clear FIFOs
	rfm22_write(rfm22_op_and_func_ctrl2, rfm22_opfc2_ffclrrx | rfm22_opfc2_ffclrtx);
	rfm22_write(rfm22_op_and_func_ctrl2, 0x00);

	rx_buffer_wr = 0;		// empty the rx buffer

	rfm22_int_timer = 0;	// reset the timer

	STOPWATCH_reset();		// reset clear channel detect timer

	rf_mode = RX_WAIT_PREAMBLE_MODE;

	// enable the receiver
//	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_xton | rfm22_opfc1_rxon);
	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_pllon | rfm22_opfc1_rxon);

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = FALSE;
#endif
}

void rfm22_setRxMode(void)
{
#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = TRUE;
#endif

	// disable interrupts
	rfm22_write(rfm22_interrupt_enable1, 0x00);
	rfm22_write(rfm22_interrupt_enable2, 0x00);

	// disable the receiver and transmitter
//	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_xton);								// READY mode
	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_pllon);							// TUNE mode

	RX_LED_OFF;
	TX_LED_OFF;

//	rfm22_write(rfm22_rx_fifo_control, RX_FIFO_HI_WATERMARK);				// RX FIFO Almost Full Threshold (0 - 63)

	if (rf_mode == TX_CARRIER_MODE || rf_mode == TX_PN_MODE)
	{	// FIFO mode, GFSK modulation
		uint8_t fd_bit = rfm22_read(rfm22_modulation_mode_control2) & rfm22_mmc2_fd;
		rfm22_write(rfm22_modulation_mode_control2, fd_bit | rfm22_mmc2_dtmod_fifo | rfm22_mmc2_modtyp_gfsk);
	}

	rx_buffer_wr = 0;		// empty the rx buffer

	rfm22_int_timer = 0;	// reset the timer

	STOPWATCH_reset();		// reset clear channel detect timer

	rf_mode = RX_WAIT_PREAMBLE_MODE;

	// enable RX interrupts
	rfm22_write(rfm22_interrupt_enable1, rfm22_ie1_encrcerror | rfm22_ie1_enpkvalid | rfm22_ie1_enrxffafull | rfm22_ie1_enfferr);
	rfm22_write(rfm22_interrupt_enable2, rfm22_ie2_enpreainval | rfm22_ie2_enpreaval | rfm22_ie2_enswdet);

	// read interrupt status - clear interrupts
	rfm22_read(rfm22_interrupt_status1);
	rfm22_read(rfm22_interrupt_status2);

	// clear FIFOs
	rfm22_write(rfm22_op_and_func_ctrl2, rfm22_opfc2_ffclrrx | rfm22_opfc2_ffclrtx);
	rfm22_write(rfm22_op_and_func_ctrl2, 0x00);

	// enable the receiver
//	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_xton | rfm22_opfc1_rxon);
	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_pllon | rfm22_opfc1_rxon);

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = FALSE;
#endif

#if defined(RFM22_DEBUG)
	DEBUG_PRINTF(" RX Mode\r\n");
#endif
}

// ************************************

void rfm22_setTxMode(uint8_t mode)
{
	if (mode != TX_DATA_MODE && mode != TX_CARRIER_MODE && rf_mode != TX_PN_MODE)
		return;		// invalid mode

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = TRUE;
#endif

	// *******************

	// disable interrupts
	rfm22_write(rfm22_interrupt_enable1, 0x00);
	rfm22_write(rfm22_interrupt_enable2, 0x00);

//	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_xton);								// READY mode
	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_pllon);							// TUNE mode

	RX_LED_OFF;

	uint8_t fd_bit = rfm22_read(rfm22_modulation_mode_control2) & rfm22_mmc2_fd;
	if (mode == TX_CARRIER_MODE)
	{	// blank carrier mode -  for testing
		rfm22_write(rfm22_tx_power, rfm22_tx_pwr_papeaken | rfm22_tx_pwr_papeaklvl_0 | rfm22_tx_pwr_lna_sw | rfm22_tx_pwr_txpow_0);		// tx power +1dBm ... 1.25mW
		rfm22_write(rfm22_modulation_mode_control2, fd_bit | rfm22_mmc2_dtmod_pn9 | rfm22_mmc2_modtyp_none);	// FIFO mode, Blank carrier
	}
	else
	if (mode == TX_PN_MODE)
	{	// psuedo random data carrier mode - for testing
		rfm22_write(rfm22_tx_power, rfm22_tx_pwr_papeaken | rfm22_tx_pwr_papeaklvl_0 | rfm22_tx_pwr_lna_sw | rfm22_tx_pwr_txpow_0);		// tx power +1dBm ... 1.25mW
		rfm22_write(rfm22_modulation_mode_control2, fd_bit | rfm22_mmc2_dtmod_pn9 | rfm22_mmc2_modtyp_gfsk);	// FIFO mode, PN9 carrier
	}
	else
	{	// data transmission
//		rfm22_write(rfm22_tx_power, rfm22_tx_pwr_lna_sw | tx_power);											// set the tx power
		rfm22_write(rfm22_tx_power, rfm22_tx_pwr_papeaken | rfm22_tx_pwr_papeaklvl_0 | rfm22_tx_pwr_lna_sw | tx_power);					// set the tx power
		rfm22_write(rfm22_modulation_mode_control2, fd_bit | rfm22_mmc2_dtmod_fifo | rfm22_mmc2_modtyp_gfsk);	// FIFO mode, GFSK modulation
	}

//	rfm22_write(0x72, reg_72[lookup_index]);	// rfm22_frequency_deviation

	// clear FIFOs
	rfm22_write(rfm22_op_and_func_ctrl2, rfm22_opfc2_ffclrrx | rfm22_opfc2_ffclrtx);
	rfm22_write(rfm22_op_and_func_ctrl2, 0x00);

	// *******************
	// add some data to the chips TX FIFO before enabling the transmitter

	if (mode == TX_DATA_MODE)
	{
		tx_data_rd = 0;

		register uint16_t rd = tx_data_rd;

		// set the total number of data bytes we are going to transmit
		rfm22_write(rfm22_transmit_packet_length, tx_data_wr);

		register uint16_t num = tx_data_wr - rd;
		if (num > (FIFO_SIZE - 1)) num = FIFO_SIZE - 1;

		// add some data
		rfm22_startBurstWrite(rfm22_fifo_access);
		for (register uint16_t i = num; i > 0; i--)
			rfm22_burstWrite(tx_data_addr[rd++]);
		rfm22_endBurstWrite();

		tx_data_rd += num;

		#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
//			DEBUG_PRINTF(" added_%d_bytes", num);
//			debug_outputted = true;
		#endif
	}

	// *******************

	rfm22_int_timer = 0;	// reset the timer

	rf_mode = mode;

	// enable TX interrupts
//	rfm22_write(rfm22_interrupt_enable1, rfm22_ie1_enpksent | rfm22_ie1_entxffaem | rfm22_ie1_enfferr);
	rfm22_write(rfm22_interrupt_enable1, rfm22_ie1_enpksent | rfm22_ie1_entxffaem);

	// read interrupt status - clear interrupts
	rfm22_read(rfm22_interrupt_status1);
	rfm22_read(rfm22_interrupt_status2);

	// enable the transmitter
//	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_xton | rfm22_opfc1_txon);
	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_pllon | rfm22_opfc1_txon);

	TX_LED_ON;

	// *******************
	// create new slightly random clear channel detector count value

	uint32_t ccc = (TX_PREAMBLE_NIBBLES + 8) + 4;			// minimum clear channel time before allowing transmit
	clear_channel_count = ccc + (random32 % (ccc * 2));		// plus a some randomness

	// *******************

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = FALSE;
#endif

#if defined(RFM22_DEBUG)
	if (rf_mode == TX_DATA_MODE) DEBUG_PRINTF(" TX_Data_Mode\r\n");
	else
	if (rf_mode == TX_CARRIER_MODE) DEBUG_PRINTF(" TX_Carrier_Mode\r\n");
	else
	if (rf_mode == TX_PN_MODE) DEBUG_PRINTF(" TX_PN_Mode\r\n");
#endif
}

// ************************************
// external interrupt line triggered (or polled) from the rf chip

void rfm22_processRxInt(void)
{
	register uint8_t int_stat1 = int_status1;
	register uint8_t int_stat2 = int_status2;

	if (int_stat2 & rfm22_is2_ipreaval)
	{	// Valid preamble detected

		if (rf_mode == RX_WAIT_PREAMBLE_MODE)
		{
			rfm22_int_timer = 0;	// reset the timer
			rf_mode = RX_WAIT_SYNC_MODE;
			RX_LED_ON;

			#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
				DEBUG_PRINTF(" pream_det");
				debug_outputted = true;
			#endif
		}
	}
/*	else
	if (int_stat2 & rfm22_is2_ipreainval)
	{	// Invalid preamble detected

		if (rf_mode == RX_WAIT_SYNC_MODE)
		{
			#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
				DEBUG_PRINTF(" invalid_preamble");
				debug_outputted = true;
			#endif

//			rfm22_reenableRx();				// re-enable the receiver
			rfm22_setRxMode();
			return;
		}
		else
		{
		}
	}
*/
	if (int_stat2 & rfm22_is2_iswdet)
	{	// Sync word detected

		STOPWATCH_reset();						// reset timer

		if (rf_mode == RX_WAIT_PREAMBLE_MODE || rf_mode == RX_WAIT_SYNC_MODE)
		{
			rfm22_int_timer = 0;	// reset the timer
			rf_mode = RX_DATA_MODE;
			RX_LED_ON;

			// read the 10-bit signed afc correction value
			afc_correction = (uint16_t)rfm22_read(rfm22_afc_correction_read) << 8;		// bits 9 to 2
			afc_correction |= (uint16_t)rfm22_read(rfm22_ook_counter_value1) & 0x00c0;	// bits 1 & 0
			afc_correction >>= 6;
			afc_correction_Hz = (int32_t)(frequency_step_size * afc_correction + 0.5f);	// convert the afc value to Hz

			rx_packet_start_rssi_dBm = rssi_dBm;			// remember the rssi for this packet
			rx_packet_start_afc_Hz = afc_correction_Hz;		// remember the afc value for this packet

			#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
				DEBUG_PRINTF(" sync_det");
				DEBUG_PRINTF(" AFC_%d_%dHz", afc_correction, afc_correction_Hz);
				debug_outputted = true;
			#endif
		}
	}

	if (int_stat1 & rfm22_is1_irxffafull)
	{	// RX FIFO almost full, it needs emptying

		if (rf_mode == RX_DATA_MODE)
		{	// read data from the rf chips FIFO buffer
			rfm22_int_timer = 0;	// reset the timer

			register uint16_t len = rfm22_read(rfm22_received_packet_length);		// read the total length of the packet data

			register uint16_t wr = rx_buffer_wr;

			if ((wr + RX_FIFO_HI_WATERMARK) > len)
			{	// some kind of error in the RF module
				#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
					DEBUG_PRINTF(" r_size_error1");
					debug_outputted = true;
				#endif

//				rfm22_reenableRx();				// re-enable the receiver
				rfm22_setRxMode();
				return;
			}

			if (((wr + RX_FIFO_HI_WATERMARK) >= len) && !(int_stat1 & rfm22_is1_ipkvalid))
			{	// some kind of error in the RF module
				#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
					DEBUG_PRINTF(" r_size_error2");
					debug_outputted = true;
				#endif

//				rfm22_reenableRx();				// re-enable the receiver
				rfm22_setRxMode();
				return;
			}

			// fetch the rx'ed data from the rf chips RX FIFO
			rfm22_startBurstRead(rfm22_fifo_access);
			for (register uint16_t i = RX_FIFO_HI_WATERMARK; i > 0; i--)
			{
				register uint8_t b = rfm22_burstRead();	// read a byte from the rf modules RX FIFO buffer
				if (wr < sizeof(rx_buffer))
					rx_buffer[wr++] = b;				// save the byte into our rx buffer
			}
			rfm22_endBurstRead();

			rx_buffer_wr = wr;

			#if defined(RFM22_DEBUG) &&  !defined(RFM22_EXT_INT_USE)
//				DEBUG_PRINTF(" r_data_%u/%u", rx_buffer_wr, len);
//				debug_outputted = true;
			#endif
		}
		else
		{	// just clear the RX FIFO
			rfm22_startBurstRead(rfm22_fifo_access);
			for (register uint16_t i = RX_FIFO_HI_WATERMARK; i > 0; i--)
				rfm22_burstRead();	// read a byte from the rf modules RX FIFO buffer
			rfm22_endBurstRead();
		}
	}

	if (int_stat1 & rfm22_is1_icrerror)
	{	// CRC error .. discard the received data

		if (rf_mode == RX_DATA_MODE)
		{
			rfm22_int_timer = 0;	// reset the timer

			#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
				DEBUG_PRINTF(" CRC_ERR");
				debug_outputted = true;
			#endif

//			rfm22_reenableRx();				// re-enable the receiver
			rfm22_setRxMode();					// reset the receiver
			return;
		}
	}

//	if (int_stat2 & rfm22_is2_irssi)
//	{	// RSSI level is >= the set threshold
//	}

//	if (device_status & rfm22_ds_rxffem)
//	{	// RX FIFO empty
//	}

//	if (device_status & rfm22_ds_headerr)
//	{	// Header check error
//	}

	if (int_stat1 & rfm22_is1_ipkvalid)
	{	// Valid packet received

		if (rf_mode == RX_DATA_MODE)
		{
			rfm22_int_timer = 0;	// reset the timer

			// disable the receiver
//			rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_xton);		// READY mode
			rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_pllon);	// TUNE mode

			register uint16_t len = rfm22_read(rfm22_received_packet_length);		// read the total length of the packet data

			register uint16_t wr = rx_buffer_wr;

			if (wr < len)
			{	// their must still be data in the RX FIFO we need to get

				rfm22_startBurstRead(rfm22_fifo_access);
				while (wr < len)
				{
					if (wr >= sizeof(rx_buffer)) break;
					rx_buffer[wr++] = rfm22_burstRead();
				}
				rfm22_endBurstRead();

				rx_buffer_wr = wr;
			}

//			rfm22_reenableRx();					// re-enable the receiver
			rfm22_setRxMode();					// reset the receiver

			if (wr != len)
			{	// we have a packet length error .. discard the packet
				#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
					DEBUG_PRINTF(" r_pack_len_error_%u_%u", len, wr);
					debug_outputted = true;
				#endif

				return;
			}

			// we have a valid received packet
			#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
				DEBUG_PRINTF(" VALID_R_PACKET_%u", wr);
				debug_outputted = true;
			#endif

			if (rx_packet_wr == 0)
			{	// save the received packet for further processing
				rx_packet_rssi_dBm = rx_packet_start_rssi_dBm;			// remember the rssi for this packet
				rx_packet_afc_Hz = rx_packet_start_afc_Hz;				// remember the afc offset for this packet
				memmove((void *)rx_packet_buf, (void *)rx_buffer, wr);	// copy the packet data
				rx_packet_wr = wr;										// save the length of the data
			}
			else
			{	// the save buffer is still in use .. nothing we can do but to drop the packet
			}

//			return;
		}
		else
		{
////		rfm22_reenableRx();				// re-enable the receiver
//			rfm22_setRxMode();				// reset the receiver
//			return;
		}
	}
}

void rfm22_processTxInt(void)
{
	register uint8_t int_stat1 = int_status1;
//	register uint8_t int_stat2 = int_status2;

	/*
		if (int_stat1 & rfm22_is1_ifferr)
		{	// FIFO underflow/overflow error
			rfm22_setRxMode();
			tx_data_addr = NULL;
			tx_data_rd = tx_data_wr = 0;
			return;
		}
	*/

	if (int_stat1 & rfm22_is1_ixtffaem)
	{	// TX FIFO almost empty, it needs filling up

		#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
//			DEBUG_PRINTF(" T_FIFO_AE");
//			debug_outputted = true;
		#endif

		if (rf_mode == TX_DATA_MODE)
		{
			if ((tx_data_wr > 0) && (tx_data_rd < tx_data_wr))
			{	// we have more data to send
				rfm22_int_timer = 0;	// reset the timer

				register uint16_t rd = tx_data_rd;

				register uint16_t num = tx_data_wr - rd;
				if (num > (FIFO_SIZE - TX_FIFO_LO_WATERMARK - 1))
					num = FIFO_SIZE - TX_FIFO_LO_WATERMARK - 1;

				// top-up the rf chips TX FIFO buffer
				rfm22_startBurstWrite(rfm22_fifo_access);
				for (register uint16_t i = num; i > 0; i--)
					rfm22_burstWrite(tx_data_addr[rd++]);
				rfm22_endBurstWrite();

				tx_data_rd = rd;

				#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
//					DEBUG_PRINTF(" added_%d_bytes", num);
//					debug_outputted = true;
				#endif

			}

//			return;
		}
	}

	if (int_stat1 & rfm22_is1_ipksent)
	{	// Packet has been sent
		#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
			DEBUG_PRINTF(" T_Sent");
			debug_outputted = true;
		#endif

		if (rf_mode == TX_DATA_MODE)
		{
			rfm22_setRxMode();					// back to receive mode

			tx_data_addr = NULL;
			tx_data_rd = tx_data_wr = 0;
			return;
		}
	}

//	if (int_stat1 & rfm22_is1_itxffafull)
//	{	// TX FIFO almost full, it needs to be transmitted
//	}
}

void rfm22_processInt(void)
{	// this is called from the external interrupt handler

	#if !defined(RFM22_EXT_INT_USE)
		if (GPIO_IN(RF_INT_PIN))
			return;			// the external int line is high (no signalled interrupt)
	#endif

	if (!initialized || power_on_reset)
		return;				// we haven't yet been initialized

	#if defined(RFM22_DEBUG)
		debug_outputted = false;
	#endif

	// ********************************
	// read the RF modules current status registers

	// read device status register
	device_status = rfm22_read(rfm22_device_status);

	// read ezmac status register
	ezmac_status = rfm22_read(rfm22_ezmac_status);

	// read interrupt status registers - clears the interrupt line
	int_status1 = rfm22_read(rfm22_interrupt_status1);
	int_status2 = rfm22_read(rfm22_interrupt_status2);

	if (rf_mode != TX_DATA_MODE && rf_mode != TX_CARRIER_MODE && rf_mode != TX_PN_MODE)
	{
		rssi = rfm22_read(rfm22_rssi);			// read rx signal strength .. 45 = -100dBm, 205 = -20dBm
		rssi_dBm = ((int16_t)rssi / 2) - 122;	// convert to dBm
	}
	else
	{
		tx_pwr = rfm22_read(rfm22_tx_power);	// read the tx power register
	}

	if (int_status2 & rfm22_is2_ipor)
	{	// the RF module has gone and done a reset - we need to re-initialize the rf module
		initialized = FALSE;
		power_on_reset = TRUE;
		return;
	}

	// ********************************
	// debug stuff

	#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
		if (prev_device_status != device_status || prev_int_status1 != int_status1 || prev_int_status2 != int_status2 || prev_ezmac_status != ezmac_status)
		{
			DEBUG_PRINTF("%02x %02x %02x %02x %dC", device_status, int_status1, int_status2, ezmac_status, temperature_reg);

			if ((device_status & rfm22_ds_cps_mask) == rfm22_ds_cps_rx)
				DEBUG_PRINTF(" %ddBm", rssi_dBm);	// rx mode
			else
			if ((device_status & rfm22_ds_cps_mask) == rfm22_ds_cps_tx)
				DEBUG_PRINTF(" %s", (tx_pwr & rfm22_tx_pwr_papeakval) ? "ANT_MISMATCH" : "ant_ok");	// tx mode

			debug_outputted = true;

			prev_device_status = device_status;
			prev_int_status1 = int_status1;
			prev_int_status2 = int_status2;
			prev_ezmac_status = ezmac_status;
		}
	#endif

	// ********************************
	// read the ADC - temperature sensor .. this can only be used in IDLE mode
/*
	if (!(rfm22_read(rfm22_adc_config) & rfm22_ac_adcstartbusy))
	{	// the ADC has completed it's conversion

		// read the ADC sample
		temperature_reg = (int16_t)rfm22_read(rfm22_adc_value) * 0.5f - 64;

		// start a new ADC conversion
		rfm22_write(rfm22_adc_config, adc_config | rfm22_ac_adcstartbusy);

		#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
			DEBUG_PRINTF(", %dC", temperature_reg);
			debug_outputted = true;
		#endif
	}
*/
	// ********************************

	register uint16_t timer_ms = rfm22_int_timer;

	switch (rf_mode)
	{
		case RX_WAIT_PREAMBLE_MODE:
		case RX_WAIT_SYNC_MODE:
		case RX_DATA_MODE:

			if (device_status & (rfm22_ds_ffunfl | rfm22_ds_ffovfl))
			{	// FIFO under/over flow error

				#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
					DEBUG_PRINTF(" R_UNDER/OVERRUN");
					debug_outputted = true;
				#endif

				rfm22_setRxMode();								// reset the receiver
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			if (rf_mode == RX_WAIT_SYNC_MODE && timer_ms >= timeout_sync_ms)
			{
				rfm22_int_time_outs++;

				#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
					DEBUG_PRINTF(" R_SYNC_TIMEOUT");
					debug_outputted = true;
				#endif

				rfm22_setRxMode();								// reset the receiver
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			if (rf_mode == RX_DATA_MODE && timer_ms >= timeout_data_ms)
			{	// missing interrupts
				rfm22_int_time_outs++;
				rfm22_setRxMode();								// reset the receiver
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			if ((device_status & rfm22_ds_cps_mask) != rfm22_ds_cps_rx)
			{	// the rf module is not in rx mode
				if (timer_ms >= 100)
				{
					rfm22_int_time_outs++;

					#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
						DEBUG_PRINTF(" R_TIMEOUT");
						debug_outputted = true;
					#endif

					rfm22_setRxMode();							// reset the receiver
					tx_data_rd = tx_data_wr = 0;				// wipe TX buffer
					break;
				}
			}

			rfm22_processRxInt();								// process the interrupt
			break;

		case TX_DATA_MODE:

			if (device_status & (rfm22_ds_ffunfl | rfm22_ds_ffovfl))
			{	// FIFO under/over flow error

				#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
					DEBUG_PRINTF(" T_UNDER/OVERRUN");
					debug_outputted = true;
				#endif

				rfm22_setRxMode();								// back to rx mode
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			if (timer_ms >= timeout_data_ms)
			{
				rfm22_int_time_outs++;
				rfm22_setRxMode();								// back to rx mode
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			if ((device_status & rfm22_ds_cps_mask) != rfm22_ds_cps_tx)
			{	// the rf module is not in tx mode
				if (timer_ms >= 100)
				{
					rfm22_int_time_outs++;

					#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
						DEBUG_PRINTF(" T_TIMEOUT");
						debug_outputted = true;
					#endif

					rfm22_setRxMode();							// back to rx mode
					tx_data_rd = tx_data_wr = 0;				// wipe TX buffer
					break;
				}
			}

			rfm22_processTxInt();								// process the interrupt
			break;

		case TX_CARRIER_MODE:
		case TX_PN_MODE:

			if (timer_ms >= TX_TEST_MODE_TIMELIMIT_MS)			// 'nn'ms limit
			{
				rfm22_setRxMode();								// back to rx mode
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			break;

		default:	// unknown mode - this should NEVER happen, maybe we should do a complete CPU reset here
			rfm22_setRxMode();								// to rx mode
			tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
			break;
	}

	// ********************************

	#if defined(RFM22_DEBUG) && !defined(RFM22_EXT_INT_USE)
		if (debug_outputted)
		{
			switch (rf_mode)
			{
				case RX_WAIT_PREAMBLE_MODE:
					DEBUG_PRINTF(" R_WAIT_PREAMBLE\r\n");
					break;
				case RX_WAIT_SYNC_MODE:
					DEBUG_PRINTF(" R_WAIT_SYNC\r\n");
					break;
				case RX_DATA_MODE:
					DEBUG_PRINTF(" R_DATA\r\n");
					break;
				case TX_DATA_MODE:
					DEBUG_PRINTF(" T_DATA\r\n");
					break;
				case TX_CARRIER_MODE:
					DEBUG_PRINTF(" T_CARRIER\r\n");
					break;
				case TX_PN_MODE:
					DEBUG_PRINTF(" T_PN\r\n");
					break;
				default:
					DEBUG_PRINTF(" UNKNOWN_MODE\r\n");
					break;
			}
		}
	#endif

	// ********************************
}

// ************************************

int16_t rfm22_receivedRSSI(void)
{	// return the packets signal strength
	if (!initialized)
		return -200;
	else
		return rx_packet_rssi_dBm;
}

int32_t rfm22_receivedAFCHz(void)
{	// return the packets offset frequency
	if (!initialized)
		return 0;
	else
		return rx_packet_afc_Hz;
}

uint16_t rfm22_receivedLength(void)
{	// return the size of the data received
	if (!initialized)
		return 0;
	else
		return rx_packet_wr;
}

uint8_t * rfm22_receivedPointer(void)
{	// return the address of the data
	return (uint8_t *)&rx_packet_buf;
}

void rfm22_receivedDone(void)
{	// empty the rx packet buffer
	rx_packet_wr = 0;
}

// ************************************

int32_t rfm22_sendData(void *data, uint16_t length, bool send_immediately)
{
	if (!initialized)
		return -1;					// we are not yet initialized

	if (length == 0)
		return -2;					// no data to send

	if (length > 255)
		return -3;					// too much data

	if (tx_data_wr > 0)
		return -4;					// already have data to be sent

	if (rf_mode == TX_DATA_MODE || rf_mode == TX_CARRIER_MODE || rf_mode == TX_PN_MODE)
		return -5;					// we are currently transmitting

	tx_data_addr = data;
	tx_data_rd = 0;
	tx_data_wr = length;

	#if defined(RFM22_DEBUG)
		DEBUG_PRINTF("rf sendData(0x%08x %u)\r\n", (uint32_t)tx_data_addr, tx_data_wr);
	#endif

	if (send_immediately || rfm22_channelIsClear())	// is the channel clear to transmit on?
		rfm22_setTxMode(TX_DATA_MODE);				// transmit NOW

	return tx_data_wr;
}

// ************************************

// enable a blank tx carrier (for frequency alignment)
void rfm22_setTxCarrierMode(void)
{
	if (!initialized)
		return;

	if (rf_mode != TX_CARRIER_MODE)
		rfm22_setTxMode(TX_CARRIER_MODE);
}

// enable a psuedo random data tx carrier (for spectrum inspection)
void rfm22_setTxPNMode(void)
{
	if (!initialized)
		return;

	if (rf_mode != TX_PN_MODE)
		rfm22_setTxMode(TX_PN_MODE);
}

// ************************************

// return the current mode
int8_t rfm22_currentMode(void)
{
	return rf_mode;
}

// return TRUE if we are transmitting
bool rfm22_transmitting(void)
{
	return (rf_mode == TX_DATA_MODE || rf_mode == TX_CARRIER_MODE || rf_mode == TX_PN_MODE);
}

// return TRUE if the channel is clear to transmit on
bool rfm22_channelIsClear(void)
{
	if (!initialized)
		return FALSE;		// we haven't yet been initialized

	if (rf_mode != RX_WAIT_PREAMBLE_MODE && rf_mode != RX_WAIT_SYNC_MODE)
		return FALSE;		// we are receiving something or we are transmitting

	return TRUE;
//	return (STOPWATCH_get_count() > clear_channel_count);
}

// return TRUE if the transmiter is ready for use
bool rfm22_txReady(void)
{
	if (!initialized)
		return FALSE;		// we haven't yet been initialized

	return (tx_data_rd == 0 && tx_data_wr == 0 && rf_mode != TX_DATA_MODE && rf_mode != TX_CARRIER_MODE && rf_mode != TX_PN_MODE);
}

// ************************************
// can be called from an interrupt if you wish

void rfm22_1ms_tick(void)
{	// call this once every ms

	if (!initialized)
		return;				// we haven't yet been initialized

	if (rfm22_int_timer < 0xffff)
		rfm22_int_timer++;
}

// *****************************************************************************
// call this as often as possible - not from an interrupt

void rfm22_process(void)
{
	if (!initialized)
		return;				// we haven't yet been initialized

	#if !defined(RFM22_EXT_INT_USE)
		rfm22_processInt();	// manually poll the interrupt line routine
	#endif

	if (power_on_reset)
	{	// we need to re-initialize the RF module - it told us it's reset itself
		uint32_t current_freq = carrier_frequency_hz;										// fetch current rf nominal frequency
		uint32_t freq_hop_step_size = (uint32_t)frequency_hop_step_size_reg * 10000;	// fetch the frequency hoppping step size
		rfm22_init(lower_carrier_frequency_limit_Hz, upper_carrier_frequency_limit_Hz, freq_hop_step_size);
		rfm22_setNominalCarrierFrequency(current_freq);									// restore the nominal carrier frequency
		return;
	}

	switch (rf_mode)
	{
		case RX_WAIT_PREAMBLE_MODE:

			if (rfm22_int_timer >= timeout_ms)
			{	// assume somethings locked up
				rfm22_int_time_outs++;
				rfm22_setRxMode();								// reset the RF module to rx mode
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			// go to transmit mode if we have data to send and the channel is clear to transmit on
			if (tx_data_rd == 0 && tx_data_wr > 0 && rfm22_channelIsClear())
			{
				rfm22_setTxMode(TX_DATA_MODE);					// transmit packet NOW
				break;
			}

			break;

		case RX_WAIT_SYNC_MODE:

			if (rfm22_int_timer >= timeout_sync_ms)
			{	// assume somethings locked up
				rfm22_int_time_outs++;
				rfm22_setRxMode();								// reset the RF module to rx mode
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			// go to transmit mode if we have data to send and the channel is clear to transmit on
			if (tx_data_rd == 0 && tx_data_wr > 0 && rfm22_channelIsClear())
			{
				rfm22_setTxMode(TX_DATA_MODE);					// transmit packet NOW
				break;
			}

			break;

		case RX_DATA_MODE:
		case TX_DATA_MODE:

			if (rfm22_int_timer >= timeout_data_ms)
			{	// assume somethings locked up
				rfm22_int_time_outs++;
				rfm22_setRxMode();								// reset the RF module to rx mode
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			break;

		case TX_CARRIER_MODE:
		case TX_PN_MODE:

			if (rfm22_int_timer >= TX_TEST_MODE_TIMELIMIT_MS)
			{
				rfm22_setRxMode();								// back to rx mode
				tx_data_rd = tx_data_wr = 0;					// wipe TX buffer
				break;
			}

			break;

		default:
			// unknown mode - this should never happen, maybe we should do a complete CPU reset here?
			rfm22_setRxMode();									// to rx mode
			tx_data_rd = tx_data_wr = 0;						// wipe TX buffer
			break;
	}

	#if defined(RFM22_INT_TIMEOUT_DEBUG)
		if (prev_rfm22_int_time_outs != rfm22_int_time_outs)
		{
			prev_rfm22_int_time_outs = rfm22_int_time_outs;
			DEBUG_PRINTF("rf int timeouts %d\r\n", rfm22_int_time_outs);
		}
	#endif
}

// ************************************
// Initialize this hardware layer module and the rf module

int rfm22_init(uint32_t min_frequency_hz, uint32_t max_frequency_hz, uint32_t freq_hop_step_size)
{
	initialized = false;

#if defined(RFM22_EXT_INT_USE)
	rfm22_disableExtInt();
#endif

	power_on_reset = false;

	#if defined(RFM22_DEBUG)
		DEBUG_PRINTF("\r\nRF init\r\n");
	#endif

	// ****************

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = TRUE;
#endif

		// ****************
		// setup the SPI port

		// chip select line HIGH
		PIOS_SPI_RC_PinSet(RFM22_PIOS_SPI, 1);

		// set SPI port SCLK frequency .. 4.5MHz
		PIOS_SPI_SetClockSpeed(RFM22_PIOS_SPI, PIOS_SPI_PRESCALER_16);

		// ****************
		// software reset the RF chip .. following procedure according to Si4x3x Errata (rev. B)

		rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_swres);			// software reset the radio

		PIOS_DELAY_WaitmS(26);												// wait 26ms

		for (int i = 50; i > 0; i--)
		{
			PIOS_DELAY_WaitmS(1);											// wait 1ms

			// read the status registers
			int_status1 = rfm22_read(rfm22_interrupt_status1);
			int_status2 = rfm22_read(rfm22_interrupt_status2);
			if (int_status2 & rfm22_is2_ichiprdy) break;
		}

		// ****************

		// read status - clears interrupt
		device_status = rfm22_read(rfm22_device_status);
		int_status1 = rfm22_read(rfm22_interrupt_status1);
		int_status2 = rfm22_read(rfm22_interrupt_status2);
		ezmac_status = rfm22_read(rfm22_ezmac_status);

		// disable all interrupts
		rfm22_write(rfm22_interrupt_enable1, 0x00);
		rfm22_write(rfm22_interrupt_enable2, 0x00);

		// ****************

#if defined(RFM22_EXT_INT_USE)
	exec_using_spi = FALSE;
#endif

	// ****************

#if defined(RFM22_EXT_INT_USE)
	inside_ext_int = FALSE;
#endif

	rf_mode = RX_WAIT_PREAMBLE_MODE;

	device_status = int_status1 = int_status2 = ezmac_status = 0;

	rssi = 0;
	rssi_dBm = -200;

	rx_buffer_current = 0;
	rx_buffer_wr = 0;
	rx_packet_wr = 0;
	rx_packet_rssi_dBm = -200;
	rx_packet_afc_Hz = 0;

	tx_data_addr = NULL;
	tx_data_rd = tx_data_wr = 0;

	lookup_index = 0;

	rfm22_int_timer = 0;
	rfm22_int_time_outs = 0;
	prev_rfm22_int_time_outs = 0;

	hbsel = 0;
	frequency_step_size = 0.0f;

	frequency_hop_channel = 0;

	afc_correction = 0;
	afc_correction_Hz = 0;

	temperature_reg = 0;

	// set the TX power
	tx_power = RFM22_DEFAULT_RF_POWER;

	tx_pwr = 0;

	// ****************
	// set the minimum and maximum carrier frequency allowed

	if (min_frequency_hz < rfm22_min_carrier_frequency_Hz) min_frequency_hz = rfm22_min_carrier_frequency_Hz;
	else
	if (min_frequency_hz > rfm22_max_carrier_frequency_Hz) min_frequency_hz = rfm22_max_carrier_frequency_Hz;

	if (max_frequency_hz < rfm22_min_carrier_frequency_Hz) max_frequency_hz = rfm22_min_carrier_frequency_Hz;
	else
	if (max_frequency_hz > rfm22_max_carrier_frequency_Hz) max_frequency_hz = rfm22_max_carrier_frequency_Hz;

	if (min_frequency_hz > max_frequency_hz)
	{	// swap them over
		uint32_t tmp = min_frequency_hz;
		min_frequency_hz = max_frequency_hz;
		max_frequency_hz = tmp;
	}

	lower_carrier_frequency_limit_Hz = min_frequency_hz;
	upper_carrier_frequency_limit_Hz = max_frequency_hz;

	// ****************

	freq_hop_step_size /= 10000;	// in 10kHz increments
	if (freq_hop_step_size > 255) freq_hop_step_size = 255;

	frequency_hop_step_size_reg = freq_hop_step_size;

	// ****************
	// read the RF chip ID bytes

	device_type = rfm22_read(rfm22_device_type) & rfm22_dt_mask;		// read the device type
	#if defined(RFM22_DEBUG)
		DEBUG_PRINTF("rf device type: %d\r\n", device_type);
	#endif
	if (device_type != 0x08)
		return -1;	// incorrect RF module type

	device_version = rfm22_read(rfm22_device_version) & rfm22_dv_mask;	// read the device version
	#if defined(RFM22_DEBUG)
		DEBUG_PRINTF("rf device version: %d\r\n", device_version);
	#endif
//	if (device_version != RFM22_DEVICE_VERSION_V2)	// V2
//		return -2;	// incorrect RF module version
//	if (device_version != RFM22_DEVICE_VERSION_A0)	// A0
//		return -2;	// incorrect RF module version
	if (device_version != RFM22_DEVICE_VERSION_B1)	// B1
		return -2;	// incorrect RF module version

	// ****************

	// disable Low Duty Cycle Mode
	rfm22_write(rfm22_op_and_func_ctrl2, 0x00);

	// calibrate our RF module to be exactly on frequency .. different for every module
	osc_load_cap = OSC_LOAD_CAP;	// default
/*	if (serial_number_crc32 == 0x176C1EC6) osc_load_cap = OSC_LOAD_CAP_1;
	else
	if (serial_number_crc32 == 0xA524A3B0) osc_load_cap = OSC_LOAD_CAP_2;
	else
	if (serial_number_crc32 == 0x9F6393C1) osc_load_cap = OSC_LOAD_CAP_3;
	else
	if (serial_number_crc32 == 0x994ECD31) osc_load_cap = OSC_LOAD_CAP_4;
*/	rfm22_write(rfm22_xtal_osc_load_cap, osc_load_cap);

	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_xton);					// READY mode
//	rfm22_write(rfm22_op_and_func_ctrl1, rfm22_opfc1_pllon);				// TUNE mode

	// choose the 3 GPIO pin functions
	rfm22_write(rfm22_io_port_config, rfm22_io_port_default);								// GPIO port use default value
	rfm22_write(rfm22_gpio0_config, rfm22_gpio0_config_drv3 | rfm22_gpio0_config_txstate);	// GPIO0 = TX State (to control RF Switch)
	rfm22_write(rfm22_gpio1_config, rfm22_gpio1_config_drv3 | rfm22_gpio1_config_rxstate);	// GPIO1 = RX State (to control RF Switch)
	rfm22_write(rfm22_gpio2_config, rfm22_gpio2_config_drv3 | rfm22_gpio2_config_cca);		// GPIO2 = Clear Channel Assessment

        // set the RF datarate
	rfm22_setDatarate(RFM22_DEFAULT_RF_DATARATE);

	// Enable data whitening
//	uint8_t txdtrtscale_bit = rfm22_read(rfm22_modulation_mode_control1) & rfm22_mmc1_txdtrtscale;
//	rfm22_write(rfm22_modulation_mode_control1, txdtrtscale_bit | rfm22_mmc1_enwhite);

	// FIFO mode, GFSK modulation
	uint8_t fd_bit = rfm22_read(rfm22_modulation_mode_control2) & rfm22_mmc2_fd;
	rfm22_write(rfm22_modulation_mode_control2, rfm22_mmc2_trclk_clk_none | rfm22_mmc2_dtmod_fifo | fd_bit | rfm22_mmc2_modtyp_gfsk);

	rfm22_write(rfm22_cpu_output_clk, rfm22_coc_1MHz);						// 1MHz clock output

	// setup to read the internal temperature sensor
	adc_config = rfm22_ac_adcsel_temp_sensor | rfm22_ac_adcref_bg;					// ADC used to sample the temperature sensor
	rfm22_write(rfm22_adc_config, adc_config);										//
	rfm22_write(rfm22_adc_sensor_amp_offset, 0);									// adc offset
	rfm22_write(rfm22_temp_sensor_calib, rfm22_tsc_tsrange0 | rfm22_tsc_entsoffs);	// temp sensor calibration .. �40C to +64C 0.5C resolution
	rfm22_write(rfm22_temp_value_offset, 0);										// temp sensor offset
	rfm22_write(rfm22_adc_config, adc_config | rfm22_ac_adcstartbusy);				// start an ADC conversion

	rfm22_write(rfm22_rssi_threshold_clear_chan_indicator, (-80 + 122) * 2);		// set the RSSI threshold interrupt to about -80dBm

	// enable the internal Tx & Rx packet handlers (with CRC)
//	rfm22_write(rfm22_data_access_control, rfm22_dac_enpacrx | rfm22_dac_enpactx | rfm22_dac_encrc | rfm22_dac_crc_crc16);
	// enable the internal Tx & Rx packet handlers (without CRC)
	rfm22_write(rfm22_data_access_control, rfm22_dac_enpacrx | rfm22_dac_enpactx);

	rfm22_write(rfm22_preamble_length, TX_PREAMBLE_NIBBLES);				// x-nibbles tx preamble
	rfm22_write(rfm22_preamble_detection_ctrl1, RX_PREAMBLE_NIBBLES << 3);	// x-nibbles rx preamble detection

	rfm22_write(rfm22_header_control1, rfm22_header_cntl1_bcen_none | rfm22_header_cntl1_hdch_none);	// header control - we are not using the header
	rfm22_write(rfm22_header_control2, rfm22_header_cntl2_hdlen_none | rfm22_header_cntl2_synclen_3210 | ((TX_PREAMBLE_NIBBLES >> 8) & 0x01));	// no header bytes, synchronization word length 3, 2, 1 & 0 used, packet length included in header.

	rfm22_write(rfm22_sync_word3, SYNC_BYTE_1);								// sync word
	rfm22_write(rfm22_sync_word2, SYNC_BYTE_2);								//
	rfm22_write(rfm22_sync_word1, SYNC_BYTE_3);								//
	rfm22_write(rfm22_sync_word0, SYNC_BYTE_4);								//
/*
	rfm22_write(rfm22_transmit_header3, 'p');								// set tx header
	rfm22_write(rfm22_transmit_header2, 'i');								//
	rfm22_write(rfm22_transmit_header1, 'p');								//
	rfm22_write(rfm22_transmit_header0, ' ');								//

	rfm22_write(rfm22_check_header3, 'p');									// set expected rx header
	rfm22_write(rfm22_check_header2, 'i');									//
	rfm22_write(rfm22_check_header1, 'p');									//
	rfm22_write(rfm22_check_header0, ' ');									//

	// all the bits to be checked
	rfm22_write(rfm22_header_enable3, 0xff);
	rfm22_write(rfm22_header_enable2, 0xff);
	rfm22_write(rfm22_header_enable1, 0xff);
	rfm22_write(rfm22_header_enable0, 0xff);
*/	// no bits to be checked
	rfm22_write(rfm22_header_enable3, 0x00);
	rfm22_write(rfm22_header_enable2, 0x00);
	rfm22_write(rfm22_header_enable1, 0x00);
	rfm22_write(rfm22_header_enable0, 0x00);

//	rfm22_write(rfm22_modem_test, 0x01);

	rfm22_write(rfm22_agc_override1, rfm22_agc_ovr1_agcen);
//	rfm22_write(rfm22_agc_override1, rfm22_agc_ovr1_sgi | rfm22_agc_ovr1_agcen);

	rfm22_write(rfm22_frequency_hopping_step_size, frequency_hop_step_size_reg);	// set frequency hopping channel step size (multiples of 10kHz)

	rfm22_setNominalCarrierFrequency((min_frequency_hz + max_frequency_hz) / 2);	// set our nominal carrier frequency

	rfm22_write(rfm22_tx_power, rfm22_tx_pwr_papeaken | rfm22_tx_pwr_papeaklvl_0 | rfm22_tx_pwr_lna_sw | tx_power);	// set the tx power

//	rfm22_write(rfm22_vco_current_trimming, 0x7f);
//	rfm22_write(rfm22_vco_calibration_override, 0x40);
//	rfm22_write(rfm22_chargepump_current_trimming_override, 0x80);

	rfm22_write(rfm22_tx_fifo_control1, TX_FIFO_HI_WATERMARK);				// TX FIFO Almost Full Threshold (0 - 63)
	rfm22_write(rfm22_tx_fifo_control2, TX_FIFO_LO_WATERMARK);				// TX FIFO Almost Empty Threshold (0 - 63)

	rfm22_write(rfm22_rx_fifo_control, RX_FIFO_HI_WATERMARK);				// RX FIFO Almost Full Threshold (0 - 63)

#if defined(RFM22_EXT_INT_USE)
	// Enable RF module external interrupt
	rfm22_enableExtInt();
#endif

	rfm22_setRxMode();

	initialized = true;

	return 0;	// ok
}

// ************************************
