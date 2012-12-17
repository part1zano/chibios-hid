#include "ch.h"
#include "hal.h"
#include "usb_hid.h"
#include "hid_def.h"


/* CHECK IT!!! */
extern hid_data hid_in_data;
extern hid_data hid_out_data;
//-----------------------------

hid_usb_state_t hid_state;

static void hid_reset(USBDriver *usbp) {
	
	hid_state = HID_BUSY;
	chSysLockFromIsr();
	usbStartReceiveI(usbp, HID_OUT_EP_ADDRESS, (uint8_t *)&hid_in_data, sizeof hid_data);
	chSysUnlockFromIsr();
}

static void hid_transmit(USBDriver *usbp, /*****/, size_t n) {

	chSysLockFromIsr();
	usbStartTransmitI(usbp,HID_IN_EP_ADDRESS, /*****/, sizeof /*****/);
	chSysUnlockFromIsr();

}





bool_t hidRequestsHook(USBDriver *usbp){
//usbSetupTransfer()
 if ((usbp->setup[0] & (USB_RTYPE_TYPE_MASK | USB_RTYPE_RECIPIENT_MASK)) ==
       (USB_RTYPE_TYPE_CLASS | USB_RTYPE_RECIPIENT_INTERFACE)) {
    switch (usbp->setup[1]) {
    case HID_GET_REPORT_REQUEST:
      return TRUE;
    case HID_GET_IDLE_REQUEST:
      return TRUE;
    case HID_GET_PROTOCOL_REQUEST:
      return TRUE;
    case HID_SET_REPORT_REQUEST:
      return TRUE;
    case HID_SET_IDLE_REQUEST:
      return TRUE;
    case HID_SET_PROTOCOL_REQUEST:
      return TRUE;
    default:
      return FALSE;
    }
  }
  return FALSE;
}



// Callback for ENDPOINT


void hidDataTransmitted(USBDriver *usbp, usbep_t ep){

}



void hidDataReceived(USBDriver *usbp, usbep_t ep){

}




