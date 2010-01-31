/**
 ******************************************************************************
 *
 * @file       pios_usb.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2009.
 * @brief      USB functions header.
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

#ifndef PIOS_USB_H
#define PIOS_USB_H

/* Local defines */
/* Following settings allow to customise the USB device descriptor */
#ifndef PIOS_USB_VENDOR_ID
#define PIOS_USB_VENDOR_ID    0x16c0        // sponsored by voti.nl! see http://www.voti.nl/pids
#endif

#ifndef PIOS_USB_VENDOR_STR
#define PIOS_USB_VENDOR_STR   "midibox.org" // you will see this in the USB device description
#endif

#ifndef PIOS_USB_PRODUCT_STR
#define PIOS_USB_PRODUCT_STR  "MIOS32"      /* You will see this in the USB device list */
#endif

#ifndef PIOS_USB_PRODUCT_ID
#define PIOS_USB_PRODUCT_ID   0x03ff        /* ==1023; 1020-1029 reserved for T.Klose, 1000 - 1009 free for lab use */
#endif

#ifndef PIOS_USB_VERSION_ID
#define PIOS_USB_VERSION_ID   0x0100        /* v1.00 */
#endif

/* Internal defines which are used by MIOS32 USB MIDI/COM (don't touch) */
#define PIOS_USB_EP_NUM   5

/* Buffer table base address */
#define PIOS_USB_BTABLE_ADDRESS      0x000

/* EP0 rx/tx buffer base address */
#define PIOS_USB_ENDP0_RXADDR        0x040
#define PIOS_USB_ENDP0_TXADDR        0x080

/* EP1 Rx/Tx buffer base address for MIDI driver */
#define PIOS_USB_ENDP1_TXADDR        0x0c0
#define PIOS_USB_ENDP1_RXADDR        0x100

/* EP2/3/4 buffer base addresses for COM driver */
#define PIOS_USB_ENDP2_TXADDR        0x140
#define PIOS_USB_ENDP3_RXADDR        0x180
#define PIOS_USB_ENDP4_TXADDR        0x1c0


/* Global Variables */
extern void (*pEpInt_IN[7])(void);
extern void (*pEpInt_OUT[7])(void);

/* Public Functions */
extern int32_t PIOS_USB_Init(uint32_t mode);
extern int32_t PIOS_USB_IsInitialized(void);

#endif /* PIOS_USB_H */
