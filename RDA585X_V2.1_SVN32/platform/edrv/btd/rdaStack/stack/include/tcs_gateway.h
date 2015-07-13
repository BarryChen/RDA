/***********************************************************************
 *
 * MODULE NAME:    tcs_gateway.h
 * DESCRIPTION:    Bluetooth Host Stack TCS Gateway API Header File
 * AUTHOR:         Thomas Howley
 *
 * SOURCE CONTROL: $Id: tcs_gateway.h,v 1.1.1.1 2007/12/18 05:32:27 tianwq Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2002 RDA Microelectronics.
 *     All rights reserved.
 *
 * REMARKS:
 *     This file should be incuded via bt.h only.
 *
 ***********************************************************************/


#ifndef BLUETOOTH_TCSGATEWAY_API_DECLARED
#define BLUETOOTH_TCSGATEWAY_API_DECLARED


APIDECL1 int APIDECL2 TCS_GWRegister(u_int8 flag, t_TCS_callbacks *callbacks);
APIDECL1 int APIDECL2 TCS_GWCordlessSetup(u_int16 *callHandle, t_bdaddr *TLbdAddress, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_GWIntercomSetup(u_int16 *callHandle, t_bdaddr TLbdAddress, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_GWConnect(u_int16 callHandle, t_TCS_callParams *callParams);
APIDECL1 int APIDECL2 TCS_GWReleaseComplete(u_int16 callHandle, t_TCS_callParams *callParams);

#define TCS_GWSetupAcknowledge      TCS_SetupAcknowledge
#define TCS_GWAlerting              TCS_Alerting
#define TCS_GWInformation			TCS_Information
#define TCS_GWCallProceeding		TCS_CallProceeding
#define TCS_GWConnectAcknowledge	TCS_ConnectAcknowledge
#define TCS_GWDisconnect			TCS_Disconnect
#define TCS_GWRelease				TCS_Release
#define TCS_GWStartDTMF				TCS_StartDTMF
#define TCS_GWStartDTMFAcknowledge	TCS_StartDTMFAcknowledge
#define TCS_GWStartDTMFReject		TCS_StartDTMFReject
#define	TCS_GWStopDTMF				TCS_StopDTMF
#define TCS_GWStopDTMFAcknowledge	TCS_StopDTMFAcknowledge
#define TCS_GWProgress				TCS_Progress
APIDECL1 int APIDECL2 TCS_GWDisconnectFromTL(t_bdaddr TLAddress);

#endif /* BLUETOOTH_TCSTERMINAL_API_DECLARED */

