/**
 ******************************************************************************
 *
 * @file       pios_com.c  
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * 	        Parts by Thorsten Klose (tk@midibox.org) (tk@midibox.org)
 * @brief      COM layer functions
 * @see        The GNU Public License (GPL) Version 3
 * @defgroup   PIOS_COM COM layer functions
 * @{
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


/* Project Includes */
#include "pios.h"


/* Private Function Prototypes */


/* Local Variables */
static int32_t (*receive_callback_func)(COMPortTypeDef port, char c);


/**
* Initializes COM layer
* \param[in] mode currently only mode 0 supported
* \return < 0 if initialisation failed
*/
int32_t PIOS_COM_Init(void)
{
	int32_t ret = 0;

	/* Disable callback by default */
	receive_callback_func = NULL;

	/* If any COM assignment: */
	PIOS_USART_Init();

	return ret;
}

/**
* Sends a package over given port
* \param[in] port COM port (COM_GPS_UART, COM_TELEM_UART, COM_AUX_UART)
* \param[in] buffer character buffer
* \param[in] len buffer length
* \return -1 if port not available
* \return -2 if non-blocking mode activated: buffer is full
*            caller should retry until buffer is free again
* \return 0 on success
*/
int32_t PIOS_COM_SendBufferNonBlocking(COMPortTypeDef port, uint8_t *buffer, uint16_t len)
{
	/* Branch depending on selected port */
	switch(port) {
		case 0:
			return PIOS_USART_TxBufferPutMoreNonBlocking(COM_DEBUG_PORT, buffer, len);
		case 1:
			return PIOS_USART_TxBufferPutMoreNonBlocking(GPS, buffer, len);
		case 2:
			return PIOS_USART_TxBufferPutMoreNonBlocking(TELEM, buffer, len);
		case 3:
			return PIOS_USART_TxBufferPutMoreNonBlocking(AUX, buffer, len);
		default:
			/* Invalid port */
			return -1;
	}
}



/**
* Sends a package over given port
* (blocking function)
* \param[in] port COM port (COM_GPS_UART, COM_TELEM_UART, COM_AUX_UART)
* \param[in] buffer character buffer
* \param[in] len buffer length
* \return -1 if port not available
* \return 0 on success
*/
int32_t PIOS_COM_SendBuffer(COMPortTypeDef port, uint8_t *buffer, uint16_t len)
{
	/* Branch depending on selected port */
	switch(port) {
		case 0:
			return PIOS_USART_TxBufferPutMore(COM_DEBUG_PORT, buffer, len);
		case 1:
			return PIOS_USART_TxBufferPutMore(GPS, buffer, len);
		case 2:
			return PIOS_USART_TxBufferPutMore(TELEM, buffer, len);
		case 3:
			return PIOS_USART_TxBufferPutMore(AUX, buffer, len);
		default:
			/* Invalid port */
			return -1;
	}
}


/**
* Sends a single character over given port
* \param[in] port COM port (COM_GPS_UART, COM_TELEM_UART, COM_AUX_UART)
* \param[in] c character
* \return -1 if port not available
* \return -2 buffer is full
*            caller should retry until buffer is free again
* \return 0 on success
*/
int32_t PIOS_COM_SendCharNonBlocking(COMPortTypeDef port, char c)
{
	return PIOS_COM_SendBufferNonBlocking(port, (uint8_t *)&c, 1);
}


/**
* Sends a single character over given port
* (blocking function)
* \param[in] port COM port (COM_GPS_UART, COM_TELEM_UART, COM_AUX_UART)
* \param[in] c character
* \return -1 if port not available
* \return 0 on success
*/
int32_t PIOS_COM_SendChar(COMPortTypeDef port, char c)
{
	return PIOS_COM_SendBuffer(port, (uint8_t *)&c, 1);
}


/**
* Sends a string over given port
* \param[in] port COM port (COM_GPS_UART, COM_TELEM_UART, COM_AUX_UART)
* \param[in] str zero-terminated string
* \return -1 if port not available
* \return -2 buffer is full
*         caller should retry until buffer is free again
* \return 0 on success
*/
int32_t PIOS_COM_SendStringNonBlocking(COMPortTypeDef port, char *str)
{
	return PIOS_COM_SendBufferNonBlocking(port, (uint8_t *)str, (uint16_t)strlen(str));
}


/**
* Sends a string over given port
* (blocking function)
* \param[in] port COM port (COM_GPS_UART, COM_TELEM_UART, COM_AUX_UART)
* \param[in] str zero-terminated string
* \return -1 if port not available
* \return 0 on success
*/
int32_t PIOS_COM_SendString(COMPortTypeDef port, char *str)
{
	return PIOS_COM_SendBuffer(port, (uint8_t *)str, strlen(str));
}


/**
* Sends a formatted string (-> printf) over given port
* \param[in] port COM port (COM_GPS_UART, COM_TELEM_UART, COM_AUX_UART)
* \param[in] *format zero-terminated format string - 128 characters supported maximum!
* \param[in] ... optional arguments,
*        128 characters supported maximum!
* \return -2 if non-blocking mode activated: buffer is full
*         caller should retry until buffer is free again
* \return 0 on success
*/
int32_t PIOS_COM_SendFormattedStringNonBlocking(COMPortTypeDef port, char *format, ...)
{
	uint8_t buffer[128]; // TODO: tmp!!! Provide a streamed COM method later!

	va_list args;

	va_start(args, format);
	vsprintf((char *)buffer, format, args);
	return PIOS_COM_SendBufferNonBlocking(port, buffer, (uint16_t)strlen((char *)buffer));
}


/**
* Sends a formatted string (-> printf) over given port
* (blocking function)
* \param[in] port COM port (COM_GPS_UART, COM_TELEM_UART, COM_AUX_UART)
* \param[in] *format zero-terminated format string - 128 characters supported maximum!
* \param[in] ... optional arguments,
* \return -1 if port not available
* \return 0 on success
*/
int32_t PIOS_COM_SendFormattedString(COMPortTypeDef port, char *format, ...)
{
	uint8_t buffer[128]; // TODO: tmp!!! Provide a streamed COM method later!
	va_list args;

	va_start(args, format);
	vsprintf((char *)buffer, format, args);
	return PIOS_COM_SendBuffer(port, buffer, (uint16_t)strlen((char *)buffer));
}



/**
* Checks for incoming COM messages, calls the callback function which has
* been installed via PIOS_COM_ReceiveCallbackInit()
* 
* Not for use in an application - this function is called by
* by a task in the programming model!
* 
* \return < 0 on errors
*/
int32_t PIOS_COM_ReceiveHandler(void)
{
	uint8_t port;

	/* Interface to be checked */
	uint8_t intf = 0;
	
	/* Number of forwards - stop after 10 forwards to yield some CPU time for other tasks */
	uint8_t total_bytes_forwarded = 0;
	
	uint8_t bytes_forwarded = 0;
	uint8_t again = 1;
	
	do {
		// Round Robin
		// TODO: maybe a list based approach would be better
		// it would allow to add/remove interfaces dynamically
		// this would also allow to give certain ports a higher priority (to add them multiple times to the list)
		// it would also improve this spagetthi code ;)
		int32_t status = -1;
		switch( intf++ ) {
			case 0: status = PIOS_USART_RxBufferGet(COM_DEBUG_PORT); port = COM_DEBUG_UART; break;
			case 1: status = PIOS_USART_RxBufferGet(GPS); port = COM_GPS_UART; break;
			case 2: status = PIOS_USART_RxBufferGet(TELEM); port = COM_TELEM_UART; break;
			case 3: status = PIOS_USART_RxBufferGet(AUX); port = COM_AUX_UART; break;
			default:
				// allow 64 forwards maximum to yield some CPU time for other tasks
				if(bytes_forwarded && total_bytes_forwarded < 64) {
					intf = 0; // restart with USB
					bytes_forwarded = 0; // for checking, if bytes still have been forwarded in next round
				} else {
					again = 0; // no more interfaces to be processed
				}
				status = -1; // empty round - no message
		}
		
		/* Message received? */
		if(status >= 0) {
		/* Notify that a package has been forwarded */
		++bytes_forwarded;
		++total_bytes_forwarded;
		
		/* Call function */
		if(receive_callback_func != NULL)
			receive_callback_func(port, (uint8_t)status);
		}
	} while(again);

	return 0;
}


/**
* Installs the callback function which is executed on incoming characters
* from a COM interface.
*
* Example:
* \code
* int32_t CONSOLE_Parse(COMPortTypeDef port, char c)
* {
* 	// Do Parsing here
*
* 	return 0; // no error
* }
* \endcode
*
* The callback function has been installed in an Init() function with:
* \code
*   PIOS_COM_ReceiveCallbackInit(CONSOLE_Parse);
* \endcode
* \param[in] callback_debug_command the callback function (NULL disables the callback)
* \return < 0 on errors
*/
int32_t PIOS_COM_ReceiveCallbackInit(void *callback_receive)
{
	receive_callback_func = callback_receive;

	/* No error */
	return 0;
}
