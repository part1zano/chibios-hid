#include "ch.h"
#include "hal.h"
#include "usb_hid.h"
#include "hid_def.h"


/* CHECK IT!!! */
hid_data hid_in_data;
hid_data hid_out_data;
//-----------------------------

hid_usb_state_t hid_state;

static void hid_reset(USBDriver *usbp) {
	
	hid_state = HID_STATE_BUSY;
	chSysLockFromIsr();
	usbPrepareReceive(usbp, HID_OUT_EP_ADDRESS, (uint8_t *)&hid_in_data, sizeof hid_in_data);
	chSysUnlockFromIsr();
}

static void hid_transmit(USBDriver *usbp, hid_data *packet) {

	hid_state = HID_STATE_BUSY;
	chSysLockFromIsr();
	usbPrepareTransmit(usbp, HID_IN_EP_ADDRESS, (uint8_t *)packet, sizeof *packet);
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



// Callback for THE IN ENDPOINT (INTERRUPT). Device -> HOST
void hidDataTransmitted(USBDriver *usbp, usbep_t ep){
	switch(hid_state){
	case HID_STATE_FREE:
		hid_transmit(usbp, &hid_in_data);
	default:
		return;
	}		
}


// Callback for THE OUT ENDPOINT (INTERRUPT)
void hidDataReceived(USBDriver *usbp, usbep_t ep){

}




