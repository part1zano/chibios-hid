/* 
    USB-HID Device Example for ChibiOS/RT
    Copyright (C) 2012, +inf Roberto Marino.
    
    EMAIL: formica@member.fsf.org
    
    This piece of code is FREE SOFTWARE and is released under the terms
    of the GNU General Public License <http://www.gnu.org/licenses/>
*/

/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "usb_hid.h"
#include "hid_def.h"

/*===========================================================================*/
/* USB related stuff.                                                        */
/*===========================================================================*/

/* For a better understanding look at http://www.beyondlogic.org/usbnutshell/usb1.shtml */
/* Reference: [DCDHID] USB - Device Class Definition for Human Interface Devices (HID)  */
/*
 * USB Device Descriptor.
 */

static const uint8_t hid_device_descriptor_data[18] = {		
  USB_DESC_DEVICE       (0x0110,        /* bcdUSB (1.1).                    						*/
                         0x00,          /* bDeviceClass (in interface).     						*/
                         0x00,          /* bDeviceSubClass.                 						*/
                         0x00,          /* bDeviceProtocol.                 						*/
                         0x64,          /* bMaxPacketSize. Maximum Packet Size for Zero Endpoint (8,16,32,64) 		*/
                         0x0483,        /* idVendor (ST). Assigned by USB.org 						*/
                         0x2004,        /* idProduct. Assigned by the manufacture 					*/
                         0x0200,        /* bcdDevice. Device Version Number assigned by the developer 			*/
                         1,             /* iManufacturer.                   						*/
                         2,             /* iProduct.                        						*/
                         3,             /* iSerialNumber.                   						*/
                         1)             /* bNumConfigurations. The system has only one configuration             	*/
};

/*
 * Device Descriptor wrapper.
 */

static const USBDescriptor hid_device_descriptor = {
  sizeof hid_device_descriptor_data,
  hid_device_descriptor_data
};


/*
 * USB Configuration Descriptor.
 */

static const uint8_t hid_configuration_descriptor_data[32] = {
  /* Configuration Descriptor.*/
  USB_DESC_CONFIGURATION(32,            /* wTotalLength.                    */
                         0x01,          /* bNumInterfaces.                  */
                         0x01,          /* bConfigurationValue.             */
                         0,             /* iConfiguration.                  */
                         0xC0,          /* bmAttributes (self powered).     */
                         25),           /* bMaxPower (50mA).                */
  /* Interface Descriptor.*/
  USB_DESC_INTERFACE    (0x00,          /* bInterfaceNumber.                */
                         0x00,          /* bAlternateSetting.               */
                         0x02,          /* bNumEndpoints.                   */
                         0x03,          /* bInterfaceClass (Human Device Interface).   */
                         0x00,          /* bInterfaceSubClass  (DCDHID page 8)         */
                         0x00,          /* bInterfaceProtocol  (DCDHID page 9)         */
                         4),            /* iInterface.                                 */
  /* Endpoint 1 Descriptor INTERRUPT IN  */
  USB_DESC_ENDPOINT     (0x81,   	/* bEndpointAddress.   (0x80 = Direction IN) + (0x01 = Address 1)     */
                         0x03,          /* bmAttributes (Interrupt).             		 	      */
                         0x64,          /* wMaxPacketSize.     0x64 = 100 BYTES                               */
                         0x32),         /* bInterval (Polling every 50ms)                                     */
  /* Endpoint 2 Descriptor INTERRUPT OUT */
  USB_DESC_ENDPOINT     (0x02,       	/* bEndpointAddress.   (0x00 = Direction OUT) + (0x02 = Address 2)    */
                         0x03,          /* bmAttributes (Interrupt).             */
                         0x64,          /* wMaxPacketSize.     0x64 = 100 BYTES  */
                         0x32),         /* bInterval (Polling every 50ms)        */
  /* Specific Class HID Descriptor */
  USB_DESC_HID		(0x0101,	/* bcdHID 		*/
			 0x00,		/* bCountryCode 	*/
			 0x01,		/* bNumDescriptor 	*/
			 0x22,		/* bDescriptorType	*/
			 0xFF),		/* Report Descriptor Lenght. Compute it! */

  /* HID Report Descriptor */
  HID_USAGE_PAGE	(HID_USAGE_PAGE_GENERIC_DESKTOP),
  	HID_USAGE		(HID_USAGE_MULTIAXIS_CONTROLLER),
  		HID_COLLECTION		(HID_COLLECTION_PHYSICAL),
  		HID_USAGE		(HID_USAGE_VX),
  		HID_USAGE		(HID_USAGE_VY),
  		HID_USAGE		(HID_USAGE_VZ),
  		HID_USAGE		(HID_USAGE_VBRX),
  		HID_USAGE		(HID_USAGE_VBRY),
  		HID_USAGE		(HID_USAGE_VBRZ),
  		HID_USAGE		(HID_USAGE_RX),
  		HID_USAGE		(HID_USAGE_RY),
  		HID_USAGE		(HID_USAGE_RZ),
		HID_USAGE		(HID_USAGE_VNO),
		HID_LOGICAL_MINIMUM     (-32768),
		HID_LOGICAL_MAXIMUM	(32768),
		HID_REPORT_SIZE		(16),
		HID_REPORT_COUNT	(10),
		HID_INPUT		(HID_INPUT_DATA_VAR_REL),
		HID_END_COLLECTION  
};

/*
 * Configuration Descriptor wrapper.
 */
static const USBDescriptor hid_configuration_descriptor = {
  sizeof hid_configuration_descriptor_data,
  hid_configuration_descriptor_data
};

/*
 * U.S. English language identifier.
 */
static const uint8_t hid_string0[] = {
  USB_DESC_BYTE(4),                     /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  USB_DESC_WORD(0x0409)                 /* wLANGID (U.S. English).          */
};

/*
 * Vendor string.
 */
static const uint8_t hid_string1[] = {
  USB_DESC_BYTE(29),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'E', 0, 'A', 0, 'R', 0, 'T', 0, 'H', 0, '-', 0, 'R', 0, 'O', 0,
  'B', 0, 'O', 0, 'T', 0, 'I', 0, 'C', 0, 'S', 
};

/*
 * Device Description string.
 */
static const uint8_t hid_string2[] = {
  USB_DESC_BYTE(40),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'C', 0, 'h', 0, 'i', 0, 'b', 0, 'i', 0, 'O', 0, 'S', 0, '/', 0,
  'R', 0, 'T', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0, '-', 0, 'H', 0,
  'I', 0, 'D', 0,
};

/*
 * Serial Number string.
 */
static const uint8_t hid_string3[] = {
  USB_DESC_BYTE(8),                     /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  '0' + CH_KERNEL_MAJOR, 0,
  '0' + CH_KERNEL_MINOR, 0,
  '0' + CH_KERNEL_PATCH, 0
};

/*
 * Interface string.
 */
static const uint8_t hid_string4[] = {
  16,                               /* bLength.                             */
  USB_DESCRIPTOR_STRING,            /* bDescriptorType.                     */
  'U', 0, 'S', 0, 'B', 0, ' ', 0, 'H', 0, 'I', 0, 'D', 0
};

/*
 * Strings wrappers array.
 */
static const USBDescriptor hid_strings[] = {
  {sizeof hid_string0, hid_string0},
  {sizeof hid_string1, hid_string1},
  {sizeof hid_string2, hid_string2},
  {sizeof hid_string3, hid_string3},
  {sizeof hid_string4, hid_string4}
};

static const USBDescriptor *get_descriptor(USBDriver *usbp,
                                           uint8_t dtype,
                                           uint8_t dindex,
                                           uint16_t lang) {

  (void)usbp;
  (void)lang;
  switch (dtype) {
  case USB_DESCRIPTOR_DEVICE:
    return &hid_device_descriptor;
  case USB_DESCRIPTOR_CONFIGURATION:
    return &hid_configuration_descriptor;
  case USB_DESCRIPTOR_STRING:
    if (dindex < 5)
      return &hid_strings[dindex];
  }
  return NULL;
}

