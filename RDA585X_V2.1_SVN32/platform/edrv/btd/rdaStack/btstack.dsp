# Microsoft Developer Studio Project File - Name="btstack" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=btstack - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "btstack.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "btstack.mak" CFG="btstack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "btstack - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "btstack - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "Desktop"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "btstack - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\..\include" /I "..\..\arch\win32\include" /I "..\..\hci\include" /I "..\..\l2cap\include" /I "..\..\manager\include" /I "..\..\rfcomm\include" /I "..\..\api\include" /I "..\..\sdp\include" /I "..\..\obex\include" /I "..\..\tcs\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\libstack.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\libstack.lib ..\..\..\..\HC_Data_Test\Release	del ..\..\..\..\HC_Data_Test\Release\HC_Data_Test.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\arch\common\include" /I "..\..\arch\mips-mp3\include" /I "..\..\hci\include" /I "..\..\l2cap\include" /I "..\..\manager\include" /I "..\..\rfcomm\include" /I "..\..\api\include" /I "..\..\sdp\include" /I "..\..\obex\include" /I "..\..\hfp\include" /I "..\..\a2dp\include" /I "..\..\pal\include" /I "..\..\spp\include" /I "..\..\tcs\include" /I "..\..\hid\include" /I "..\..\amp\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D APIDECL1= /FR /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\libstack.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\libstack.lib ..\..\..\..\simulator
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "btstack - Win32 Release"
# Name "btstack - Win32 Debug"
# Begin Group "HCI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\hci\hci_bufalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\hci\hci_callbacks.c
# End Source File
# Begin Source File

SOURCE=..\..\hci\include\hci_callbacks.h
# End Source File
# Begin Source File

SOURCE=..\..\hci\include\hci_constants.h
# End Source File
# Begin Source File

SOURCE=..\..\hci\hci_debug.c
# End Source File
# Begin Source File

SOURCE=..\..\hci\include\hci_debug.h
# End Source File
# Begin Source File

SOURCE=..\..\hci\hci_encoder.c
# End Source File
# Begin Source File

SOURCE=..\..\hci\include\hci_encoder.h
# End Source File
# Begin Source File

SOURCE=..\..\hci\hci_flow_ctrl.c
# End Source File
# Begin Source File

SOURCE=..\..\hci\include\hci_flow_ctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\hci\hci_primitives.c
# End Source File
# Begin Source File

SOURCE=..\..\hci\include\hci_primitives.h
# End Source File
# End Group
# Begin Group "RFCOMM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\rfcomm\port_ent_api.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_bufalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\rfcomm_buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_core.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\rfcomm_core.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_credit.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_ctrl_chan.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_ctrl_frames.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\rfcomm_debug.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_dlc_state.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_entity_core.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\rfcomm_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_frames.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_l2auto_state.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_l2cap.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_pe.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\rfcomm_pe.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\rfcomm_proto.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\rfcomm_types.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\rfcomm_ue_api.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\rfcomm_ue_api.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\stack_ent.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\stack_ent_api.c
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\stack_ent_api.h
# End Source File
# Begin Source File

SOURCE=..\..\rfcomm\include\stack_ent_core.h
# End Source File
# End Group
# Begin Group "L2CAP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\l2cap\bd_addr_list.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\bd_addr_list.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_acl_link.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_acl_link.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_bufalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_channel.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_channel.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_config_helper.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_config_sm.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_connect_sm.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_connect_sm.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_const.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_enc_dec.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_enc_dec.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_groups.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_groups.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_layer_config.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_pkt_handler.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_pkt_handler.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_psm.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_psm.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_sar.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_sar.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_types.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2_utils.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2_utils.h
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\l2cap_primitives.c
# End Source File
# Begin Source File

SOURCE=..\..\l2cap\include\l2cap_primitives.h
# End Source File
# End Group
# Begin Group "Generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\generic\host_buf.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host_buf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\host_config.h
# End Source File
# Begin Source File

SOURCE=..\..\generic\host_ctrl.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host_ctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\generic\host_ll.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host_ll.h
# End Source File
# Begin Source File

SOURCE=..\..\generic\host_mem.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host_mem.h
# End Source File
# Begin Source File

SOURCE=..\..\generic\host_msg.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host_sched.h
# End Source File
# Begin Source File

SOURCE=..\..\generic\host_timer.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host_timer.h
# End Source File
# End Group
# Begin Group "Platform Specific"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\papi_impl.h
# End Source File
# Begin Source File

SOURCE=.\include\platform_config.h
# End Source File
# End Group
# Begin Group "Manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\manager\mgr_core.c
# End Source File
# Begin Source File

SOURCE=..\..\manager\include\mgr_core.h
# End Source File
# Begin Source File

SOURCE=..\..\manager\mgr_devdb.c
# End Source File
# Begin Source File

SOURCE=..\..\manager\include\mgr_devdb.h
# End Source File
# Begin Source File

SOURCE=..\..\manager\mgr_inqdb.c
# End Source File
# Begin Source File

SOURCE=..\..\manager\mgr_link.c
# End Source File
# Begin Source File

SOURCE=..\..\manager\include\mgr_link.h
# End Source File
# Begin Source File

SOURCE=..\..\manager\mgr_secdb.c
# End Source File
# Begin Source File

SOURCE=..\..\manager\include\mgr_secdb.h
# End Source File
# Begin Source File

SOURCE=..\..\manager\mgr_utils.c
# End Source File
# Begin Source File

SOURCE=..\..\manager\include\mgr_utils.h
# End Source File
# End Group
# Begin Group "SDP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sdp\sdp_callbacks.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_callbacks.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_constants.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_db.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_db.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_error.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_le.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_le_common.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_le_request_decoder.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_le_request_decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_le_request_encoder.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_le_request_encoder.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_le_response_decoder.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_le_response_decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_le_response_encoder.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_le_response_encoder.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_server.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_server.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_state_machine.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_state_machine.h
# End Source File
# Begin Source File

SOURCE=..\..\sdp\sdp_ue.c
# End Source File
# Begin Source File

SOURCE=..\..\sdp\include\sdp_ue.h
# End Source File
# End Group
# Begin Group "User API Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\include\a2dp.h
# End Source File
# Begin Source File

SOURCE=..\common\include\avdtp.h
# End Source File
# Begin Source File

SOURCE=..\common\include\avrcp.h
# End Source File
# Begin Source File

SOURCE=..\common\include\bt.h
# End Source File
# Begin Source File

SOURCE=..\common\include\bt_ctrl.h
# End Source File
# Begin Source File

SOURCE=..\common\include\bt_sys.h
# End Source File
# Begin Source File

SOURCE=..\common\include\hci.h
# End Source File
# Begin Source File

SOURCE=..\common\include\host_msg.h
# End Source File
# Begin Source File

SOURCE=..\common\include\host_types.h
# End Source File
# Begin Source File

SOURCE=..\common\include\host_utils.h
# End Source File
# Begin Source File

SOURCE=..\common\include\host_version.h
# End Source File
# Begin Source File

SOURCE=..\common\include\manager.h
# End Source File
# Begin Source File

SOURCE=..\common\include\obex.h
# End Source File
# Begin Source File

SOURCE=..\common\include\papi.h
# End Source File
# Begin Source File

SOURCE=..\common\include\rfcomm.h
# End Source File
# Begin Source File

SOURCE=..\common\include\sdp.h
# End Source File
# Begin Source File

SOURCE=..\common\include\tcs.h
# End Source File
# Begin Source File

SOURCE=..\common\include\tcs_gateway.h
# End Source File
# Begin Source File

SOURCE=..\common\include\tcs_terminal.h
# End Source File
# End Group
# Begin Group "Examples"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\user\examples\ex1.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex10.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex11.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex2.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex3.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex4.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex5.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex6.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex7.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex8.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\user\examples\ex9.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Obex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\obex\obex_client_interface.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_client_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_client_types.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_defines.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_ftp_client.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_ftp_client.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_ftp_server.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_ftp_server.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_map_client.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_map_client.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_ob_push.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_ob_push.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_pbap_client.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_sec.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_sec.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_sec_wrap.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_sec_wrap.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_server_interface.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_server_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_server_types.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_sync_client.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_sync_client.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_sync_server.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_sync_server.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_tal.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_tal.h
# End Source File
# Begin Source File

SOURCE=..\..\obex\obex_utils.c
# End Source File
# Begin Source File

SOURCE=..\..\obex\include\obex_utils.h
# End Source File
# End Group
# Begin Group "TCS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\tcs\tcs_cc_callbacks.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_callbacks.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_const.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_cc_core.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_core.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_cc_dec.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_dec.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_cc_enc.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_enc.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_cc_gateway.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_gateway.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_cc_peer.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_peer.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_cc_terminal.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_terminal.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_types.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_cc_upper.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_cc_upper.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_const.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_core.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_gm_api.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_gm_api.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_gm_clock.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_gm_decode.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_gm_encode.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_gm_keydb.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_gm_types.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_gm_wug.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_gm_wugdb.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_gm_wuglisten.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\tcs_l2cap.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_l2cap.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_lower_layer.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tcs\include\tcs_wug.h

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "HFP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Hfp\hfp_ag.c
# End Source File
# Begin Source File

SOURCE=..\..\Hfp\hfp_at.c
# End Source File
# Begin Source File

SOURCE=..\..\Hfp\hfp_core.c
# End Source File
# Begin Source File

SOURCE=..\..\Hfp\hfp_hs.c
# End Source File
# End Group
# Begin Group "A2DP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\a2dp\avdtp_callbacks.c
# End Source File
# Begin Source File

SOURCE=..\..\a2dp\avdtp_core.c
# End Source File
# Begin Source File

SOURCE=..\..\a2dp\avdtp_handle_req.c
# End Source File
# Begin Source File

SOURCE=..\..\a2dp\avdtp_handle_res.c
# End Source File
# Begin Source File

SOURCE=..\..\a2dp\avdtp_send_req.c
# End Source File
# Begin Source File

SOURCE=..\..\a2dp\avdtp_send_res.c
# End Source File
# Begin Source File

SOURCE=..\..\a2dp\avrcp_callbacks.c
# End Source File
# Begin Source File

SOURCE=..\..\a2dp\avrcp_core.c
# End Source File
# Begin Source File

SOURCE=..\..\a2dp\headset_sched.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "AMP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\amp\amp_core.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\amp\amp_core.h
# End Source File
# Begin Source File

SOURCE=..\..\amp\amp_test.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\amp\amp_test.h
# End Source File
# End Group
# Begin Group "PAL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\pal\pal_buf.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\pal\include\pal_buf.h
# End Source File
# Begin Source File

SOURCE=..\..\pal\pal_cmd_disp.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\pal\include\pal_cmd_disp.h
# End Source File
# Begin Source File

SOURCE=..\..\pal\pal_core.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\pal\include\pal_core.h
# End Source File
# Begin Source File

SOURCE=..\..\pal\pal_event_gen.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\pal\include\pal_event_gen.h
# End Source File
# Begin Source File

SOURCE=..\..\pal\pal_link.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\pal\include\pal_link.h
# End Source File
# End Group
# Begin Group "SPP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\spp\dun.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\spp\spp.c
# End Source File
# Begin Source File

SOURCE=..\..\spp\include\spp.h
# End Source File
# End Group
# Begin Group "HID"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\hid\hid_callback.c
# End Source File
# Begin Source File

SOURCE=..\..\hid\include\hid_callback.h
# End Source File
# Begin Source File

SOURCE=..\..\hid\hid_core.c
# End Source File
# Begin Source File

SOURCE=..\..\hid\include\hid_core.h
# End Source File
# Begin Source File

SOURCE=..\..\hid\hid_wii.c

!IF  "$(CFG)" == "btstack - Win32 Release"

!ELSEIF  "$(CFG)" == "btstack - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\hid\include\hid_wii.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\a2dp\include\headset_sched.h
# End Source File
# Begin Source File

SOURCE=..\..\Hfp\include\hfp_const.h
# End Source File
# End Target
# End Project
