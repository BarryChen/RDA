/***********************************************************************
 *
 * MODULE NAME:    hcrp.h
 * DESCRIPTION:    hcrp profile platform interface
 * MAINTAINER:      Zhu Jianguo
 *
 * 
 * LICENSE:
 *     This source code is copyright (c) 2011 RDA Ltd.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 * 
 *
 * ISSUES:
 * 
 ***********************************************************************/
#ifndef __RDABT_HCRP_H__
#define __RDABT_HCRP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_HCRP_BUFFER_SIZE 4096
#define PJL_HEAD_LENGTH    1200


typedef struct 
{
    u_int16 cid;
    u_int16 result;
    t_bdaddr bdaddr;
}hcrp_connect_cnf_msg_t;

typedef struct 
{
    u_int16 result;
    u_int32 credit_num;
}hcrp_credit_request_rsp_t;

int Hcrp_Start_Signal_Connect(t_bdaddr address);
u_int8 Hcrp_Start_Data_Connect(void);
int Hcrp_Disconnect_Signal_Channel(void);
void Hcrp_Disconnect_Data_Channel(void);

APIDECL1 t_api APIDECL2  Hcrp_CR_DataChannelCreditGrant(void);
APIDECL1 t_api APIDECL2  Hcrp_CR_DataChannelCreditRequest(void);
APIDECL1 t_api APIDECL2  Hcrp_CR_GetLPTStatus(void);


int Hcrp_Send_Data(u_int32 length, u_int8* p_data);


#ifdef __cplusplus
}
#endif

#endif //__RDABT_HCRP_H__

