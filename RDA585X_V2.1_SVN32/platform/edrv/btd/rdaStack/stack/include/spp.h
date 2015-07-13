/***********************************************************************
 *
 * MODULE NAME:    spp_dun.c
 * PROJECT CODE:   host side stack
 * DESCRIPTION:    interface of SPP, DUN and FAX
 * MAINTAINER:     Mark Donnelly
 * CREATION DATE:  08 March 2000
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2011 RDA Microelectronics
 *     All rights reserved.
 *
 * REVISION HISTORY:
 * 
 *
 * ISSUES:
 * 
 ***********************************************************************/

#ifndef __SPP_DUN_H__
#define __SPP_DUN_H__

// struct for message from stack to app
typedef struct {
    t_bdaddr address;
    u_int8 port;
    u_int16 uuid16;
    u_int8 *uuid128;
    u_int8 dlci;
    u_int8 result;
    u_int16 max_frame_size;
    t_DataBuf *data;
} t_spp_msg;

// struct for message from app to profile
typedef struct {
    u_int8 port;
    u_int16 length;
    u_int8 *data;
} t_spp_data_msg;

#ifdef __cplusplus
extern "C" {
#endif

APIDECL1 t_api APIDECL2 SPP_Active_Req(u_int16 uuid16, u_int8 *uuid128, u_int16 version, u_int8 *name);
APIDECL1 t_api APIDECL2 SPP_Disconnect_Req(u_int16 spp_port);

APIDECL1 t_api APIDECL2 SPP_Connect_Req(t_bdaddr device_addr, u_int16 uuid16, u_int8 *uuid128);
APIDECL1 t_api APIDECL2 SPP_Send_Data(u_int8* data, u_int16 length, u_int16 spp_port);

#define RDABT_SPP_Active_Req   					        SPP_Active_Req                      
#define RDABT_SPP_Disconnect_Req(spp_port)              SPP_Disconnect_Req(spp_port)        
                                                       
APIDECL1 t_api APIDECL2 RDABT_SPP_Send_Data_Req(u_int8 *buffer,u_int16 length,u_int8 port);

                                                       
#ifdef __cplusplus
} /* extern "C" { */
#endif                                                      

#endif


