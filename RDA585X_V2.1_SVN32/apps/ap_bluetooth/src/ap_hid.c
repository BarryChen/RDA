
/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_common.h"
#include "ap_gui.h"


#include "bt.h"
#include "hid.h"
#include "ap_bluetooth.h"

#if APP_SUPPORT_BTHID==1

extern UINT8 g_bt_msg[32];
extern BOOL ui_auto_update;
extern bt_state_t g_bt_state;
extern UINT8 g_bt_need_draw;
extern UINT8 g_bt_device_change;
extern INT8  g_bt_cur_device;
extern bt_vars_t *g_pBT_vars;
extern UINT8 g_bt_connect_pending;
extern INT32 g_current_module;
extern UINT32 g_bt_call_time;
extern UINT32 g_bt_ops_state;

UINT8 g_hid_connect;


t_bdaddr g_bt_hid_bdaddr;



INT32 BT_HID(void)
{
    UINT32 key;
    INT32  result;
    INT32 wait_volume = 0;
    INT8 data[10], c, mouse=0;

    app_trace(APP_BT_TRC, "enter BT_HID");

    g_bt_need_draw = TRUE;
    MESSAGE_Initial(g_comval);
    while(1)
    {
#if (APP_SUPPORT_RGBLCD==1) || (APP_SUPPORT_LCD==1)
        if(g_bt_need_draw)
        {
            GUI_ClearScreen(NULL);
            BT_DisplayIcon(0,0);
            GUI_DisplayBattaryLever();
            GUI_DisplayTextCenter(32, gui_get_string(GUI_STR_BTHIDCONTROL));
			GUI_UpdateScreen(NULL);
            g_bt_need_draw = FALSE;
        }
#endif
        if(g_hid_connect == 0)
            return 0;

        key = MESSAGE_Wait();
        memset(data, 0, 10);
        c = 0; 
        data[0] = 1; // id = keyboard

        if((key&0xffff) == AP_KEY_DOWN)
        {
            switch(key&0xffff0000)
            {
                case AP_KEY_MODE:
                    break;
                case AP_KEY_PLAY:
                    c = 0x28; // enter
                    break;
                case AP_KEY_REC:
                    data[0] = 2; // id = mounse
                    data[1] = 2; // button: right button
                    data[2] = 0; // x
                    data[3] = 0; // y
                    data[4] = 0; // wheel
                    data[5] = 0; // acdata
                    break;
                case AP_KEY_EQ:
                    c = 0x50;  // left key
                    break;
                
                case AP_KEY_LOOP:
                    c = 0x4f;  // right key
                    break;
                
                case AP_KEY_POWER:
                    c = 0x29;  // esc
                    break;
                case AP_KEY_VOLADD:
                    data[0] = 2; // id = mounse
                    data[1] = 0; // button
                    data[2] = 0; // x
                    data[3] = -10; // y
                    data[4] = 0; // wheel
                    data[5] = 0; // acdata
                    c = -10;
                    break;
                case AP_KEY_VOLSUB:
                    data[0] = 2; // id = mounse
                    data[1] = 0; // button
                    data[2] = 0; // x
                    data[3] = 10; // y
                    data[4] = 0; // wheel
                    data[5] = 0; // acdata
                    c = 10;
                    break;
                case AP_KEY_PREV:
                    data[0] = 2; // id = mounse
                    data[1] = 0; // button
                    data[2] = -10; // x
                    data[3] = 0; // y
                    data[4] = 0; // wheel
                    data[5] = 0; // acdata
                    break;
                case AP_KEY_NEXT:
                    data[0] = 2; // id = mounse
                    data[1] = 0; // button
                    data[2] = 10; // x
                    data[3] = 0; // y
                    data[4] = 0; // wheel
                    data[5] = 0; // acdata
                    break;
                default:
                    if((key&0xffff0000)<=AP_KEY_NUM0 && (key&0xffff0000)>=AP_KEY_NUM1)
                        c = ((key&0xffff0000)>>16)+0x1d;
                    break;
            }
            if(data[0] == 1)
            {
                data[3] = c; // key1
                HID_Packet_Send_Interrupt(0xa1, data, 9,0);
                mouse = 0;
            }
            else
            {
                HID_Packet_Send_Interrupt(0xa1, data, 6,0);
                mouse = 1;
            }
        }
        else if((key&0xffff) == AP_KEY_UP)
        {
            if(!mouse)
            {
                data[0] = 1; // id = keyboard
                data[1] = 0; // flag
                data[2] = 0; // pad
                data[3] = 0; // key1
                data[4] = 0; // key2
                data[5] = 0; // key3
                HID_Packet_Send_Interrupt((HID_DATA << 4)|Hid_report_input, data, 9,0);
            }
            else
            {
                mouse = 0;
                data[0] = 2; // id = mounse
                data[1] = 0; // button
                data[2] = 0; // x
                data[3] = 0; // y
                data[4] = 0; // wheel
                data[5] = 0; // acdata
                HID_Packet_Send_Interrupt(0xa1, data, 6,0);
            }
        }
        else if((key&0xffff) == AP_KEY_PRESS || (key&0xffff) == AP_KEY_HOLD)
        {
            if(key==(AP_KEY_MODE|AP_KEY_PRESS))
                return 0;
        }
        else //ÈÈ¼ü´¦Àí
        {
            result = BT_HandleKey(key);
            if(result != 0)
            {
                return result;
            }
        }
    };

    return 0;
}
void BT_HandleHIDMessage(u_int16 msg_id, void* msg_data)
{
    INT32 i;
    switch(msg_id)
    {
    case HID_CONNECT_CNF:
    case HID_CONNECT_IND:
        {
            u_int16 handle;
            t_hid_connect *msg = (t_hid_connect*)msg_data;
            if(msg->result != RDABT_NOERROR)
                break;
            g_bt_hid_bdaddr = msg->addr;
            g_bt_cur_device = BT_Find_Device(msg->addr);
            L2_RetrieveHandle(msg->cid, &handle);
            //MGR_Set_Sniff_Interval(handle, 0x80,0,0,0);
            if(g_hid_connect == 0)
                BT_Send_Msg_Up(EV_BT_MESSAGE_IND, GUI_STR_BTHIDCONNECT);
            g_bt_state     = BT_CONNECTED;
            g_hid_connect = 1;
        }
        break;
    case HID_DISCONNECT_CNF:
    case HID_DISCONNECT_IND:
        g_hid_connect = 0;
        g_bt_state = BT_IDLE;
		g_bt_ops_state = 0;
        break;
    case HID_INTERRUPT_DATA_IND:
        {
            t_hid_input_data *msg = (t_hid_input_data*)msg_data;

            if(msg == NULL)
                break;
            if(msg->ReportType == Hid_report_input)
            {
                if(msg->ReportID == 1) // keyboard
                {
                    UINT8 key_flags = msg->data[2];// key flag
                }
            }
        }
        break;
     default:
         break;
     }
}


void hid_packet_sends(UINT8 *data, UINT8 length)
{
	HID_Packet_Send_Interrupt(0xa1, data, length,0);
}



static const u_int8 hid_description[] = {
//Keyboard
	0x05, 0x01,  		//Usage Page (Generic Desktop)
	0x09, 0x06,         //Usage (Keyboard)
	0xA1, 0x01,         //Collection (Application)

	0x85, 0x01,			//report ID
	0x05, 0x07,         //    Usage Page (Keyboard/Keypad)
	0x19, 0xE0,         //    Usage Minimum (Keyboard Left Control)
	0x29, 0xE7,         //    Usage Maximum (Keyboard Right GUI)
	0x15, 0x00,         //    Logical Minimum (0)
	0x25, 0x01,         //    Logical Maximum (1)
	0x75, 0x01,         //    Report Size (1)
	0x95, 0x08,         //    Report Count (8)
	0x81, 0x02,         //    Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
	0x95, 0x03,         //    Report Count (3)
	0x75, 0x08,         //    Report Size (8)
	0x15, 0x00,         //    Logical Minimum (0)
	0x25, 0x7f,         //    Logical Maximum (101)
	0x05, 0x07,         //    Usage Page (Keyboard/Keypad)
	0x19, 0x00,         //    Usage Minimum (Undefined)
	0x29, 0x7f,         //    Usage Maximum (Keyboard Application)
	0x81, 0x00,         //    Input (Data,Ary,Abs)


	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)

	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x95, 0x01,         //    Report Count (1)
	0x75, 0x01,         //    Report Size (1)
	0x91, 0x01,         //    Output (Cnst,Ary,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)

	0xC0,              	//End Collection


	/*add for iphone*/
	0x05, 0x0c, 
	0x09, 0x01, 
	0xa1, 0x01,
	0x85, 0x04,
	0x15, 0x00, 
	0x25, 0x01, 
	0x75, 0x01, 
	0x95, 0x18,
	0x09, 0xb5, 
	0x09, 0xb6,
	0x09, 0xb7, 
	0x09, 0xcd,
	0x09, 0xe2,
	0x09, 0xe5,
	0x09, 0xe7,
	0x09, 0xe9,
	0x09, 0xea,
	0x0a, 0x52,
	0x01, 0x0a,
	0x53, 0x01, 
	0x0a, 0x54, 
	0x01, 0x0a, 
	0x55, 0x01, 
	0x0a, 0x83, 
	0x01, 0x0a,
	0x8a, 0x01, 
	0x0a, 0x92,
	0x01, 0x0a, 
	0x94, 0x01,
	0x0a, 0x21,
	0x02, 0x0a,
	0x23, 0x02, 
	0x0a, 0x24, 
	0x02, 0x0a,
	0x25, 0x02, 
	0x0a, 0x26,
	0x02, 0x0a, 
	0x27, 0x02, 
	0x0a, 0x2a, 
	0x02, 0x81, 
	0x02, 0xc0,


//Mouse
	0x05, 0x01, 		//Usage Page (Generic Desktop)
	0x09, 0x02,       	//Usage (Mouse)
	0xA1, 0x01,       	//Collection (Application)

	0x85, 0x02,			//report ID
	0x09, 0x01,       	//    Usage (Pointer)
	0xA1, 0x00,       	//    Collection (Physical)
	0x05, 0x09,       	//        Usage Page (Button)
	0x19, 0x01,       	//        Usage Minimum (Button 1)
	0x29, 0x03,       	//        Usage Maximum (Button 3)
	0x15, 0x00,       	//        Logical Minimum (0)
	0x25, 0x01,       	//        Logical Maximum (1)
	0x95, 0x03,       	//        Report Count (3)
	0x75, 0x01,       	//        Report Size (1)
	0x81, 0x02,       	//        Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
	0x95, 0x05,       	//        Report Count (1)
	0x75, 0x01,       	//        Report Size (1)
	0x81, 0x01,       	//        Input (Cnst,Ary,Abs)
	0x05, 0x01,       	//        Usage Page (Generic Desktop)
	0x09, 0x30,       	//        Usage (X)
	0x09, 0x31,       	//        Usage (Y)
	0x09, 0x38,       	//        Usage (Wheel)
	0x15, 0x81,       	//        Logical Minimum (-127)
	0x25, 0x7F,       	//        Logical Maximum (127)
	0x75, 0x08,       	//        Report Size (8)
	0x95, 0x03,       	//        Report Count (3)
	0x81, 0x06,       	//        Input (Data,Var,Rel,NWrp,Lin,Pref,NNul,Bit)
	0xC0,             	//    End Collection
	0xC0,              	//End Collection


//Multimedia
	0x05, 0x0C, 			//Usage Page (Consumer Devices)                  
	0x09, 0x01,             //Usage (Consumer Control)                       
	0xA1, 0x01,             //Collection (Application)
	
	0x85, 0x03,				//report ID
	0x75, 0x10,         //    Report Size (16)
	0x95, 0x01,         //    Report Count (1)
	0x15, 0x00,    		//    Logical Minimum
	0x26, 0x3C,0x04,    //    Logical Maximum
	0x19, 0x00,			//    Usage Minimum (Undefined)
	0x2a, 0x3C,0x04,	//    Usage Maximum (Keyboard Application)
	0x81, 0x00,         //    Input (Data,Ary,Abs)
	0xC0,                    //End Collection 


//system control	                       
	0x05, 0x01,             //Usage Page (Generic Desktop)                   
	0x09, 0x80,             //Usage (System Control)                         
	0xA1, 0x01,             //Collection (Application)
	0x85, 0x04,				//report ID
	
	0x75, 0x01,         	//    Report Size (1)
	0x95, 0x08,         	//    Report Count (8)
	0x15, 0x00,             //    Logical Minimum (0)                        
	0x25, 0x01,             //    Logical Maximum (1)                        
	0x09, 0x81,             //    Usage (System Power Down)                  
	0x09, 0x82,             //    Usage (System Sleep)                       
	0x09, 0x83,             //    Usage (System Wake Up)                     
	0x09, 0x84,             //    Usage (System Context Menu)                
	0x09, 0x85,             //    Usage (System Main Menu)                   
	0x09, 0x86,             //    Usage (System App Menu)                    
	0x09, 0x87,             //    Usage (System Menu Help)                   
	0x09, 0x88,             //    Usage (System Menu Exit)                   
	0x81, 0x02,             //    Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
	
	0x75, 0x01,             //    Report Size (1)                            
	0x95, 0x08,             //    Report Count (8)                           
	0x81, 0x03,             //    Input (constant)
	0xC0,                    //End Collection 

};

static const t_hid_attributes hid_attribute = {
    0x0100, //u_int16 version;
    HID_VIRTUAL_CABLE|HID_RECONNECT_INITIATE|HID_BATTER_POWER|HID_REMOTE_WAKE|HID_NORMALLY_CONNECTABLE|HID_BOOT_DEVICE, //u_int16 feature;
    0xc0, //u_int8  subClass;
    0x21, //u_int8  countryCode;
    0x656e, //u_int16 language;
    0x006a, //u_int16 encoding;
    0x0409, //u_int16 langID;
    0x1f40, //u_int16 supervisionTimeout;
    sizeof(hid_description),//u_int16 desc_length;
    hid_description,//u_int8 *descriptor;
    "Human Interface Device"//u_int8 *serviceName;
};

#endif

void BT_RegisterHIDService(void)
{
#if APP_SUPPORT_BTHID==1

    HID_RegisterService(HID_OPCODE_KEYBOARD, &hid_attribute);

#endif
}




