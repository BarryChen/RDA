/***************************************************************************** 
* Include
*****************************************************************************/
#include "project_config.h"
#include "cswtype.h"
#include "cos.h"
#include "pmd_m.h"
#include "hal_sys.h"
#include "hal_uart.h"
#include "hal_host.h"
#include "hal_mem_map.h"
#include "hal_timers.h"

#include "bt.h"
#include "bt_ctrl.h"
#include "bt_config.h"
#include "hfp.h"
#include "papi.h"
#include "rdabt_main.h"
#include "rdabt_hcit.h"
#include "rdabt_uart.h"
#include "rdabt_drv.h"
#include "manager.h"

// host debug command/result
enum {
    HOST_CMD_NONE=0,
    HOST_CMD_START=1,
    HOST_CMD_END=2,
    HOST_RESULT_OK=8,
    HOST_RESULT_DATA=9,
    HOST_RESULT_CLOSED=10,
    HOST_RESULT_PENDING=11,
    HOST_RESULT_ERROR=12,
};

u_int8 bt_pcmin_mode = 0; // for bt_common_cfg.c used

rdabt_context_struct rdabt_cntx;
rdabt_context_struct* rdabt_p = &rdabt_cntx;
extern const unsigned char bt_gpio_setting[16]; 
#include "btd_config.h"
#include "tgt_btd_cfg.h"
#include "pmd_m.h"
#include "hal_gpio.h"
CONST TGT_BTD_CONFIG_T* g_btdConfig = NULL;

typedef struct{
    UINT8 cmd;
    UINT8 result;
    UINT8 length;
    UINT8 calib_flag; // 0: no calib, 1: calib clock, 2: calib finish
    union{
        UINT32 data;
        UINT8 buff[32];
    }data;
} host_debug_t;

VOLATILE host_debug_t HAL_UNCACHED_BSS host_debug_buff;
static u_int8 host_cmd_pending = 0;
static u_int32 host_cmd_time = 0;

static u_int8 rdabt_wakeup_data[] = {0x01,0xc0,0xfc,0x00};
static u_int8 rdabt_change_baudrate[] = 
	#if 0
	{0x01,0x02,0xfd,0x0a,0x00,0x01,0x60,0x00,0x00,0x80,0x00,0x10,0x0e,0x00,
	0x01,0x02 ,0xfd ,0x0a ,0x00 ,0x01 ,0x40 ,0x00 ,0x00 ,0x80 ,0x00 ,0x01 ,0x00 ,0x00};
	#else
{0x01, 0x34, 0xfc, 0x04, 0x00, 0x10, 0x0e, 0x00};//{0x01, 0x34, 0xfc, 0x04, 0x00, 0x10, 0x0e, 0x00};//{0x01,0x77,0xfc,0x01,0x00};
#endif

static u_int8 rdabt_wake_chip_data[] = {0xff};
static u_int8 warm_reset[] = {0x01,0x00,0xfd,0x00}; // for fpga test
static u_int8 sniff_window[] = {0x01, 0x43, 0xfc, 0x01, 0x0b};
BOOL  reset_in_process=0;

/* Set the device Address                       */
//t_bdaddr local_addr;

#define RDABT_WAKE_UP_TIME       					5000      /* host wake up time 2s */

extern int 	bonding_role; 	
extern char *  deviceName;
extern u_int8  nameLen;
extern BOOL g_test_mode;
u_int8  rdabt_send_notify_pending = 0;
u_int8  rdabt_send_data_ready = 0;
u_int8  rdabt_send_timer_expiry_pending = 0;
BOOL  g_CalbClock = 0;
BOOL  IsFlowCtrlStart = 0;/*added by laixf for Lps to remember flow control*/

static const bt_config_t host_config =
{  
	//.host_ctrl_config
	{
		10,//.max_timers
		20,//.max_buff
		30,//.max_message
		512,//.message_length
		0,//
		256,//host_mem_size
	},
	//.hci_config
		{
			1, //flow_control_support
			1, //flow_control_timeout
			2, //flow_control_threshold
			7, //max_acl_number
			508, // max_acl_size  
			4, // max_sco_number
			60, // max_sco_size
			10240, //max_acl_queue_size
		},
	//.manager_config

		{
			3,//max_acl_links
			10,//max_device_list
			1,//multi_connect_support
#if APP_SUPPORT_BTHID==1			
			0x2540,//0x242404,//0x240404,//0x2540,////0x7e041c cod of obex transfer file ,cod device_class // RDABT_CODSERVICEBIT_AUDIO|RDABT_CODSERVICEBIT_RENDERING|RDABT_CODMAJORDEVICE_AUDIO|RDABT_CODMINORDEVICE_AUDIO_HEADSETPROFILE
#else
			0x240404,
#endif
			1,//security_support
			0x0C,//sco_packets    //0x4c->0x0c;
#ifdef USE_VOC_CVSD
			0xc040,//voice_setting
#else
			0x8060,//voice_setting
#endif
			0,//sco_via_hci
			1,//ssp_support
			3,//io_capability
			100,//sniff_timeout 100ms
			0x320,//sniff_interval 500ms
			3,//unsniff_packets
			0,//link time out support
			20000,//link time out value (ms)
		},
	//.l2cap_config
	{
		1,    //auto_config
		1,//fragment_acl_support
		0,   //config_to_core
		10,  //max_channel
		0x06, //   max_links    
		1021, //  max_in_mtu
		1021,  //  max_out_mtu
		0,//use_piconet_broadcast
		0x04ED,//max_broadcast_mtu
		0xef1, //broadcast_handle
		0//RDABT_L2CAP_EXTENDED_FEATURE_FLOWCONTROL_MODE|RDABT_L2CAP_EXTENDED_FEATURE_RETRANSMISSION_MODE,
	},
	//sdp_config
	{
		0x80,//mtu,
	},
	// obex_config
	{   
		0x3800, // local_mtu
              3,      // server_transport
	},
	//.rfcomm_config
	{
		7,//init_credit
		255,//max_credit
		3,//min_credit
		7,//credit_issue
		128, // max_frame_size
	},
	//.hfp_config
	{
		HF_FEATURE_CLIP|HF_FEATURE_REMOTE_VOL,//.hf_feature
		AG_FEATURE_IN_BAND_RING,//.ag_feature
		1,// .handle_at_cmd
	},
	//.a2dp_config
	{
		768,//.media_mtu
		48,//.signal_mtu
		1,// .accetp_connect
	},
	// spp_config
    {
        6, // max_ports
        2, // server_num
        128, // max_data_len
    },      
};

extern void BT_Callback(rdabt_msg_t *message);
extern void BT_RegisterHeadsetService(void);
extern void BT_RegisterObexService(void);
void BT_Register_Modules(void);

/* functions on rdabt driver */
void rdabt_poweron_init(void);
void RDA_bt_Power_Off(void);

void BtRadio_PowerOn(u_int8 polBT, u_int8 numBT);
void BtRadio_PowerOff(u_int8 polBT, u_int8 numBT);

void RDABT_Platform_Init(void);
void rdabt_timer_callback(void *param);
void rdabt_uart_eint_hdlr(void);

void rdabt_antenna_on()
{
	u_int16 num_send;
	u_int32 ii=0;

#ifdef BT_LDO_WORKAROUND
    pmd_EnableDcdcPower(FALSE);
#endif

    rdabt_adp_uart_start();
    //I2C_Open();
    rdabt_adp_uart_configure(115200, FALSE);
#ifndef BT_UART_BREAK_INT_WAKEUP
    GPIO_INT_Registration(g_btdConfig->pinSleep,0,rdabt_uart_eint_hdlr);
#endif

#if 0 // for BQB RF test
    rda_bt_poweron_for_test();
    rda_bt_test_enable_dut();
    return;
#endif
    rdabt_poweron_init();
    rdabt_uart_register_cb(); 
    rdabt_baudrate_ctrl();
	RDABT_Tranport_Init();

       rdabt_p->timer_id = COS_SetTimer(100, rdabt_timer_callback, NULL, COS_TIMER_MODE_PERIODIC);

	if(RDABT_PENDING != RDABT_Add_Transport(TRANSPORT_UART_ID, RDABT_CORE_TYPE_BR_EDR))
    {
        hal_HstSendEvent(SYS_EVENT, 0x13062508);
        BT_Send_Msg_Up(EV_BT_READY_IND, 1);
        rdabt_antenna_off();
        return;
    }   
    I2C_Close();
    // Disable 26M AUX clock

//	L1SM_SleepDisable(rdabt_cntx.l1_handle);
    rdabt_cntx.tx_busy = 0;
	rdabt_send_notify_pending = 0;
}

extern INT32 h_Log_File;
void rdabt_antenna_off(void)
{
	
    COS_EVENT ev;
   u_int32 msg_count;

   RDABT_Remove_Transport(TRANSPORT_UART_ID);

   RDA_bt_Power_Off();

   /* remove all external messages */
//   while(1)
   {      
//      COS_WaitEvent(MOD_BT, &ev, COS_WAIT_FOREVER);
   }
    COS_KillTimer(rdabt_p->timer_id);
    rdabt_p->timer_id = 0;

   /* reset global context */
//   L1SM_SleepEnable(rdabt_p->l1_handle);
   rdabt_p->host_wake_up = FALSE;
   rdabt_p->chip_knocked = FALSE;

   //RDABT_Tranport_Tx_Data(warm_reset, sizeof(warm_reset));
   RDABT_Tranport_Shutdown();

	rdabt_p->state = RDABT_STATE_POWER_OFF;
    rdabt_adp_uart_stop();
    
#ifdef BT_LDO_WORKAROUND
    pmd_EnableDcdcPower(TRUE);
#endif
    
    //BT_Send_Msg_Up(EV_BT_POWEROFF_CNF, NULL);	
#if pDEBUG
    if(h_Log_File>=0)
    {
        FS_Close(h_Log_File);
        h_Log_File = -1;
    }
#endif
}


/*****************************************************************************
* FUNCTION
*   rdabt_timer_callback
* DESCRIPTION
*   General callback function for event scheduler to execute when time out.
* PARAMETERS
*   param IN data from event scheduler
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_timer_callback(void *param)
{
    COS_EVENT ev = {0};
    if(rdabt_send_timer_expiry_pending == 0)
    {
        rdabt_send_timer_expiry_pending = 1;
        ev.nEventId = EV_BT_TIMER_EXPIRY_IND;
        COS_SendEvent(MOD_BT, &ev , COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    }
}


/*****************************************************************************
* FUNCTION
*   rdabt_init
* DESCRIPTION
*   Init function if bt task
* PARAMETERS
*   task_index  IN   index of the taks
* RETURNS
*   TRUE if reset successfully
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
boolean rdabt_init(void)
{
    EXPORT HAL_HOST_CONFIG_t g_halHostConfig;
    
    g_halHostConfig.bt_debug = &host_debug_buff;
    RDABT_Platform_Init();
	
    /* init stack */
    RDABT_Initialise(&host_config);

    BT_Register_Modules();

   rdabt_p->state = RDABT_STATE_POWER_OFF;

   return TRUE;
}

/*****************************************************************************
* FUNCTION
*   rdabt_power_off_sched
* DESCRIPTION
*   Main loop when in power off state
* PARAMETERS
*  queue_id               IN     queue index of bt
* RETURNS
*   None.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
void rdabt_power_off_sched(void)
{
    COS_EVENT ev;

   while(1) /* main root for message processing */
   {
        if(host_debug_buff.result == HOST_RESULT_PENDING)
            host_debug_buff.result = HOST_RESULT_CLOSED;
        
        COS_WaitEvent(MOD_BT, &ev, COS_WAIT_FOREVER);
        hal_HstSendEvent(SYS_EVENT,0x10140001);
      switch(ev.nEventId)
      {
         case EV_BT_POWERON_REQ:
         {
            /* ====================== state change ====================== */
            /* send response back to MMI */
            //local_addr = *((t_bdaddr*)(ev.nParam1));
            g_CalbClock = host_debug_buff.calib_flag;
            rdabt_antenna_on();
            
            rdabt_p->state = RDABT_STATE_POWER_ON;
            rdabt_p->original_boot_state = RDABT_STATE_POWER_ON;

            return;
         }   
      } /* end of switch */
   }  /* end of while(1) */
}

void rdabt_main_msg_hdler(COS_EVENT *event)
{
    switch(event->nEventId)
    {
        case EV_BT_TIMER_EXPIRY_IND:   /* timer routine */
        {
            rdabt_send_timer_expiry_pending = 0;
      		RDABT_Send_Message(ADP_TIMER_IND, RDABT_ADP, RDABT_HOST_CORE, 0, NULL);
            break;
        }
        case EV_BT_NOTIFY_IND:
		if(rdabt_send_notify_pending)
			rdabt_send_notify_pending--;
            break;
        
        case EV_BT_READY_TO_READ_IND:
            {
                UINT32 status;
                //hal_HstSendEvent(SYS_EVENT,0x04070120);
                //status= hal_SysEnterCriticalSection();
                if(rdabt_send_data_ready)
                    rdabt_send_data_ready--;
                rdabt_adp_uart_rx();
                host_cmd_time = hal_TimGetUpTime();
                //hal_SysExitCriticalSection(status);
           }
        break;
        
        
        case EV_BT_WAKE_UP_REQ:
        {
            RDABT_Tranport_Tx_Data(rdabt_wake_chip_data,sizeof(rdabt_wake_chip_data));
            break;
        }
        
        case EV_BT_POWEROFF_REQ:
            RDABT_Shut_Down();
        break;
        
        default:
            break;				
    }
}

/*****************************************************************************
* FUNCTION
*   rdabt_power_on_sched
* DESCRIPTION
*   Main loop when in power on state
* PARAMETERS
*  queue_id               IN     queue index of bt
*   taks_entry_ptr      IN    task entry
* RETURNS
*   None.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
void rdabt_power_on_sched(void)
{
    COS_EVENT ev;
   int status;

    while (1)
    {
         /* proecess external queue */
         if(rdabt_cntx.tx_busy == 0)
        {
            if(host_cmd_pending == 0)
            {
                 do 
                {
        	      status=RDABT_Execute(2); 
                 } while (0);//status==RDABT_RETRY);	 
                 //hal_HstSendEvent(SYS_EVENT,0x10140003);
                 if(rdabt_p->state == RDABT_STATE_POWER_OFF)
                     break;
            }

            if(host_debug_buff.result == HOST_RESULT_PENDING)
            {
                if(host_debug_buff.calib_flag == 2) // calib end
                {
                    // write calib to flash
                    UINT32 system_flag;
                    UINT32 s;
                    EXPORT HAL_HOST_CONFIG_t g_halHostConfig;
                    EXPORT INT32 g_CDAC;
                    UINT32 csStatus = hal_SysEnterCriticalSection();

                    system_flag = *(((U32 *)(&_flash_start))+1);
                    system_flag = system_flag&~4;
                    memd_FlashWrite((UINT8*)0x04,sizeof(UINT32), &s,(CHAR*)(&system_flag));
                    system_flag = (g_halHostConfig.spi_data>>10) - g_CDAC;
                    memd_FlashWrite((UINT8*)0x08,sizeof(UINT32), &s,(CHAR*)(&system_flag));
                    
                    host_debug_buff.result = HOST_RESULT_OK;
                    hal_SysExitCriticalSection(csStatus);
                    BT_Send_Msg_Up(EV_BT_READY_IND, 0);
                }
                else
                {
                    if(hal_TimGetUpTime() > host_cmd_time+ 1 SECOND)
                    {
                        RDABT_Tranport_Tx_Data(rdabt_wake_chip_data,sizeof(rdabt_wake_chip_data));
                        COS_Sleep(500);
                    }
                    hal_HstSendEvent(SYS_EVENT, 0x14012601);
                    hal_HstSendEvent(SYS_EVENT, host_debug_buff.length);
                    RDABT_Tranport_Tx_Data(host_debug_buff.data.buff,host_debug_buff.length);
                    host_cmd_time = hal_TimGetUpTime();
                    host_debug_buff.result = HOST_RESULT_OK;
                    if(host_debug_buff.cmd == HOST_CMD_END)
                        host_cmd_pending = 0;
                    else
                        host_cmd_pending = 1;
                }
            }
        }
        COS_WaitEvent(MOD_BT, &ev, COS_WAIT_FOREVER);
        rdabt_main_msg_hdler(&ev);
//        hal_HstSendEvent(SYS_EVENT,0x10140005);

    }
}

/*****************************************************************************
* FUNCTION
*   rdabt_adp_msg_dispatch
* DESCRIPTION
*   This function handle of internal message
* PARAMETERS
*   Message
* RETURNS
*   None
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
t_api rdabt_adp_msg_dispatch(rdabt_msg_t *message)
{
        switch(message->msg_id)
        {
            case ADP_DATA_REQ:
                {
                    adp_data_msg_t *data_msg = (adp_data_msg_t*)message->data;
                    //COS_Sleep(10);
                    RDABT_Tranport_Tx_PDU(data_msg->buff,data_msg->type,data_msg->flags,data_msg->hci_handle_flags);
                    host_cmd_time = hal_TimGetUpTime();
                }
                break;
            case ADP_TRANSPORT_CNF:
                {
                    u_int32 status = ((u_int32)(message->data));
                    u_int8 transport_id;
                    transport_id = status&0xf;
                    status = status >> 8;

                    //RDAbt_enable_dut_write();
                    //break;

                    if(transport_id != TRANSPORT_UART_ID || status != RDABT_NOERROR)
                     {
                        // error
                        BT_Send_Msg_Up(EV_BT_READY_IND, 1);
                        hal_HstSendEvent(SYS_EVENT, 0x1306250a);
                        rdabt_antenna_off();
                    }
        			else if(g_test_mode)
                    {
                        struct st_t_dataBuf * cmd = NULL;
                        if(g_CalbClock)
                        {
                            // notify host to start clock calib
                            hal_HstSendEvent(BOOT_EVENT, 0x7e57ca1b); 
                        }
                        else
                        {
                            HCI_Inquiry(&cmd, 0x9e8b33, 10, 0);
                            HCI_SendRawPDU(TRANSPORT_UART_ID, 1, cmd);
                        }
                    }
                    else
                    {
                            RDABT_Start_Up(TRANSPORT_UART_ID);
                            BT_RegisterHeadsetService();
                            BT_RegisterObexService();
                            BT_RegisterHIDService();
							BT_RegisterSPPService();
                    }
                }
                break;
            case HOST_INIT_REQ:
               if(rdabt_p->original_boot_state)
               {
                    t_MGR_HCVersion version;
                    u_int16 num_send;
#ifdef BT_UART_BREAK_INT_WAKEUP
                    u_int32 value = 0x00007fff; // set hostwake  gpio to	input
                    //MGR_WritePHYRegister(0, 0x40200010, 1, &value);
#endif
                     /* send to MMI */
                    BT_Send_Msg_Up(EV_BT_READY_IND, NULL);
                    hal_HstSendEvent(SYS_EVENT, 0x1306250b);
                    // MGR_ChangeLocalName("RDA MP3 BT");
                    MGR_GetDeviceVersion(&version);
			RDABT_Tranport_Tx_Data(sniff_window, sizeof(sniff_window));		
               }
                break;
            case HOST_SHUTDOWN_CNF:
                {
                    //RDABT_Terminate(0);
                    rdabt_antenna_off();
                    BT_Send_Msg_Up(EV_BT_POWEROFF_CNF, NULL);	
			rdabt_send_notify_pending = 0;
			rdabt_send_notify();
			break;
                }
            case ADP_WAKE_IND: // wake up bluetooth bt chip
                {
			//L1SM_SleepDisable(rdabt_cntx.l1_handle);
                    //rdabt_hcit_sharemem_tx_data(rdabt_wake_chip_data,sizeof(rdabt_wake_chip_data));
                    //RDABT_Send_Message(ADP_WAKE_IND, RDABT_ADP, RDABT_HCI, 0, message->data);
                    RDABT_Tranport_Tx_Data(rdabt_wake_chip_data,sizeof(rdabt_wake_chip_data));
                    hal_UartAllowSleep(TRANSPORT_UART_ID,FALSE);
                }
                break;
            case ADP_SLEEP_IND:
                {
                    app_trace(1, "Recieve ADP_SLEEP_IND, data=%d", message->data);
                    if(message->data)  // host stack sleep
                    {
                        COS_KillTimer(rdabt_p->timer_id);
                        rdabt_p->timer_id = 0;
                    }
                    else
                    {
                        if(rdabt_p->timer_id == 0)
                            rdabt_p->timer_id = COS_SetTimer(100, rdabt_timer_callback, NULL, COS_TIMER_MODE_PERIODIC);
                    }
                }
                break;
            case MGR_RSSI_IND:
                host_debug_buff.data.data = (u_int32)(message->data);
                host_debug_buff.result = HOST_RESULT_DATA;
                break;
            case MGR_READ_REGISTER_CNF:
                host_debug_buff.data.data = (u_int32)(message->data);
                host_debug_buff.result = HOST_RESULT_DATA;
                break;
            default:
                BT_Callback(message);
                break;
        }
    return RDABT_NOERROR;
}


#ifdef FCC_TEST_OPEN
extern void rda_bt_test_enable_dut(void);
extern void Rdabt_manual_tx_test(UINT32 type,UINT32 package,UINT32 freq,UINT32 pwrLevel,UINT32 msgPattern);

UINT32* BT_Test_Mast_Ptr = 0;
UINT32 volatile BT_Test_Mask=0x1;
static uint32 pre_BT_Test_Mask=0;

UINT32 type, package, freq, pwrLevel, msgPattern;
UINT32 bt_test_timer_id = 0;
UINT32 volatile mask_tmp = 1;
#endif

/*****************************************************************************
* FUNCTION
*   rdabt_main
* DESCRIPTION
*   This function is the main function of bt task
* PARAMETERS
*   task_entry_ptr  IN   taks entry of bt
* RETURNS
*   None.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
void rdabt_main(void)
{
    memset(rdabt_p,0,sizeof(rdabt_cntx));

    g_btdConfig = tgt_GetBtdConfig();

	if (g_btdConfig->pinReset.type == HAL_GPIO_TYPE_IO)
	{
		hal_GpioSetOut(g_btdConfig->pinReset.gpioId);
	}
	if (g_btdConfig->pinWakeUp.type == HAL_GPIO_TYPE_IO)
	{
		hal_GpioSetOut(g_btdConfig->pinWakeUp.gpioId);
	}
	if (g_btdConfig->pinSleep.type == HAL_GPIO_TYPE_IO)
	{
		hal_GpioSetIn(g_btdConfig->pinSleep.gpioId);
	}

 	rdabt_poweronphone_init();

#ifdef FCC_TEST_OPEN
rda_bt_poweron_for_test();
BT_Test_Mask=0x0;
while(1)
{    
            hal_HstSendEvent(SYS_EVENT, 0x88888888);
            BT_Test_Mask = *((UINT32*)(((UINT32)(&mask_tmp))|0x20000000));
            if(BT_Test_Mask == 0x01)
            {
                hal_HstSendEvent(SYS_EVENT, 0x11111111);
                RDAbt_send_vco_test();
                app_trace(1, "Vco Test end, clear mask...");
                mask_tmp = 0;
            }
         
            if(BT_Test_Mask == 0x02)
            {
                hal_HstSendEvent(SYS_EVENT, 0x22222222);
                RDAbt_send_bbtest_singlehop_inquiry();
                app_trace(1, "SingleHop Test end, clear mask...");
                mask_tmp = 0;
            }
        
            if(BT_Test_Mask == 0x04)
            {
                hal_HstSendEvent(SYS_EVENT, 0x33333333);
                rda_bt_test_enable_dut();
                app_trace(1, "Enable Dut Test end, clear mask...");
                mask_tmp = 0;
            }
    
            if(BT_Test_Mask == 0x08)
            {
                hal_HstSendEvent(SYS_EVENT, 0x44444444);
                Rdabt_manual_tx_test( type, package, freq, 2, msgPattern);
                app_trace(1, "Manual Tx Test end, clear mask...");
                mask_tmp = 0;
            }
 
             sxr_Sleep(16384);
}    
    
#endif        

   /* main loop of bchs_main for all kinds of states */
   while(1)
   {
      switch(rdabt_p->state)
      {
         case RDABT_STATE_POWER_OFF:
            rdabt_power_off_sched();
            break;
         case RDABT_STATE_POWER_ON:
            rdabt_power_on_sched();
            break;
         default:
            ;//ASSERT(0);
      }
   }
}

u_int8 get_rdabt_state()
{
	return rdabt_p->state;
}

/*****************************************************************************
* FUNCTION
*   bt_create
* DESCRIPTION
*   This function creats bt task
* PARAMETERS
*   handle  OUT   pointer to the table of bt task structure.
* RETURNS
*   Always TRUE since the creation shall not be failed.
* GLOBALS AFFECTED
*   external_global
*****************************************************************************/
TASK_ENTRY BAL_BTTaskEntry (void *pData)
{
        rdabt_init();
        rdabt_main();
}


/*****************************************************************************
* FUNCTION
*   rdabt_host_wake_up_timeout
* DESCRIPTION
*   Timeout function for host wake up -- enable sleep mode again
* PARAMETERS
*   mi   IN  no use
*   mv  IN  no use
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_host_wake_up_timeout(void *dummy)
{
   if(rdabt_cntx.chip_knocked == FALSE)
   {
      /* disable sleep mode */
//      L1SM_SleepEnable(rdabt_cntx.l1_handle);
      hal_UartAllowSleep(TRANSPORT_UART_ID,TRUE);
      rdabt_cntx.host_wake_up = FALSE;
   }
   else
   {
      rdabt_cntx.chip_knocked = FALSE;
      COS_SetTimer(RDABT_WAKE_UP_TIME, rdabt_host_wake_up_timeout, 0, COS_TIMER_MODE_SINGLE);
   }
    rdabt_send_notify();
}

/*****************************************************************************
* FUNCTION
*   rdabt_uart_host_wake_up
* DESCRIPTION
*   Handle of host wake up routine
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_uart_host_wake_up(void)
{
   if(!rdabt_cntx.host_wake_up)
   {
      /* disable sleep mode */
//      L1SM_SleepDisable(rdabt_cntx.l1_handle);
      hal_UartAllowSleep(TRANSPORT_UART_ID,FALSE);
      rdabt_cntx.host_wake_up = TRUE;
      COS_SetTimer(RDABT_WAKE_UP_TIME, rdabt_host_wake_up_timeout, 0, COS_TIMER_MODE_SINGLE);
   }
   else
   {
      rdabt_cntx.chip_knocked = TRUE;
   }
}

/*****************************************************************************
* FUNCTION
*   rdabt_uart_unsleep
* DESCRIPTION
*   Disable UART sleep mode
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_uart_unsleep(void)
{
   if(!rdabt_cntx.host_wake_up)
   {
      /* disable sleep mode */
//      L1SM_SleepDisable(rdabt_cntx.l1_handle);
      hal_UartAllowSleep(TRANSPORT_UART_ID,FALSE);
      rdabt_cntx.host_wake_up = TRUE;
      COS_SetTimer(RDABT_WAKE_UP_TIME, rdabt_host_wake_up_timeout, 0, COS_TIMER_MODE_SINGLE);
   }
   else
    {
        rdabt_cntx.chip_knocked = TRUE;
   }
}

void rdabt_uart_eint_hdlr(void)
{
    COS_EVENT event = {0};
    event.nEventId = EV_BT_WAKE_UP_REQ;
    hal_HstSendEvent(SYS_EVENT,0x77778888);
    hal_UartAllowSleep(TRANSPORT_UART_ID,FALSE);
    COS_SendEvent(MOD_BT, &event, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

void BT_Register_Modules(void)
{
    // base module
    RDABT_AddModule(RDABT_ADP, NULL, rdabt_adp_msg_dispatch);
    RDABT_AddModule(RDABT_HCI, rdabt_hci_layer_ctrl, rdabt_hci_msg_dispatch);
    RDABT_AddModule(RDABT_MANAGER, rdabt_manager_layer_ctrl, rdabt_manager_msg_dispatch);
    RDABT_AddModule(RDABT_L2CAP, rdabt_l2cap_layer_ctrl, rdabt_l2cap_msg_dispatch);
    // modules for 3.0
    //RDABT_AddModule(RDABT_PAL, rdabt_pal_layer_ctrl, rdabt_pal_msg_dispatch);
    //RDABT_AddModule(RDABT_AMP, rdabt_amp_layer_ctrl, rdabt_amp_msg_dispatch);

    // stack protocol
    RDABT_AddModule(RDABT_SDP_CLI, rdabt_sdp_cli_layer_ctrl, rdabt_sdp_cli_msg_dispatch);
    RDABT_AddModule(RDABT_SDP_SRV, rdabt_sdp_srv_layer_ctrl, rdabt_sdp_srv_msg_dispatch);
    RDABT_AddModule(RDABT_RFCOMM, rdabt_rfcomm_layer_ctrl, rdabt_rfcomm_msg_dispatch);
#if APP_SUPPORT_PBAP==1
    RDABT_AddModule(RDABT_OBEX_CLI, rdabt_OBEX_CLI_layer_ctrl, rdabt_OBEX_CLI_msg_dispatch);
#endif
#if APP_SUPPORT_OPP==1
    RDABT_AddModule(RDABT_OBEX_SRV, rdabt_OBEX_SRV_layer_ctrl, rdabt_OBEX_SRV_msg_dispatch);
#endif    
    // profiles
    RDABT_AddModule(RDABT_A2DP, rdabt_avdtp_layer_ctrl, rdabt_avdtp_msg_dispatch);
    RDABT_AddModule(RDABT_AVRCP, rdabt_avctp_layer_ctrl, rdabt_avctp_msg_dispatch);
    RDABT_AddModule(RDABT_HFP, rdabt_hfp_layer_ctrl, rdabt_hfp_msg_dispatch);
    RDABT_AddModule(RDABT_HID, rdabt_hid_layer_ctrl, rdabt_hid_msg_dispatch);
    RDABT_AddModule(RDABT_SPP, rdabt_spp_layer_ctrl, rdabt_spp_msg_dispatch);
    //RDABT_AddModule(RDABT_HCRP, rdabt_hcrp_layer_ctrl, rdabt_hcrp_msg_dispatch);
}

