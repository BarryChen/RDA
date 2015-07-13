#ifndef AMP_TEST_H
#define AMP_TEST_H

#define MAX_PHYLINK_NUM	5
#define MAX_CONTROLLER 	MAX_PHYLINK_NUM

typedef struct st_ampTestRemoteInfo
{
	u_int8		controllerNum;
	struct
	{
		u_int8	controlID;
		u_int8	controlType;
		u_int8	controlStatus;
	}controlInfo[MAX_CONTROLLER];

}ampTestRemoteInfo;

int AMPTestGetRemotePeerAddress(t_bdaddr *pAddr);

int AMPTestGetRemoteControlInfo(t_bdaddr address, ampTestRemoteInfo *pInfo);

int AMPTestWriteRemoteAMPInfo(t_bdaddr address, u_int8 controlID);

#endif