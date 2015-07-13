/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#include "ap_gui.h"
#include "string.h"


extern BOOL ui_auto_update;
extern BOOL ui_auto_select;

extern TCHAR ExtMusic[];
extern TCHAR ExtVoice[];

#define MAX_LIST_FILENAME           50

file_location_t g_list_location;
UINT32 g_list_folder_count;
UINT8 g_list_filename[MAX_LIST_FILENAME];
INT32 g_folder_last_index;
UINT8 g_folder_change = 0;
UINT8 g_folder_list_type;
TCHAR *g_folder_list_ext;

#if APP_SUPPORT_LCD==1
INT32 file_list_callback(INT32 type, INT32 value, INT8 **string)
{
	UINT32 LEntry;
	INT32 result = 0;
	INT i;
	if(LIST_CALLBACK_GET_ITEM == type)
	{
		if(g_folder_change == 2)
		{
			if(value == 0)
			{
				*string = "TFlashCard";
			}
#if APP_SUPPORT_USB
			else
			{
				*string = "USB Disk";
			}
#endif
			return 2;
		}
		
		if(value == 0)
		{
			*string = "Up ..";
			return 0;
		}
		else if(value <= g_list_folder_count)
		{
			if(value == 1)
			{
				FS_GetNextEntry(&LEntry, NULL, FS_DIR_DIR);
			}
			else if(value == g_folder_last_index + 1)
			{
				FS_GetNextEntry(&LEntry, NULL, FS_DIR_DIR_CONT);
			}
			else
			{
				FS_GetNextEntry(&LEntry, NULL, FS_DIR_DIR);
				for(i = 0; i < value - 1; i++)
				{
					FS_GetNextEntry(&LEntry, NULL, FS_DIR_DIR_CONT);
				}
			}
			result = 0;
		}
		else
		{
			if(value == g_list_folder_count + 1)
			{
				FS_GetNextEntry(&LEntry, g_folder_list_ext, FS_DIR_FILE);
			}
			else if(value == g_folder_last_index + 1)
			{
				FS_GetNextEntry(&LEntry, g_folder_list_ext, FS_DIR_FILE_CONT);
			}
			else
			{
				FS_GetNextEntry(&LEntry, g_folder_list_ext, FS_DIR_FILE);
				for(i = 0; i < value - g_list_folder_count - 1; i++)
				{
					FS_GetNextEntry(&LEntry, g_folder_list_ext, FS_DIR_FILE_CONT);
				}
			}
			if(g_folder_list_type == FSEL_TYPE_ALLFILE)
			{
				FS_GetShortName(LEntry, g_list_filename);
				if(MUSIC_CheckType(g_list_filename) == 0)
				{
					result = 4;
				}
				else
				{
					result = 3;
				}
			}
			else
			{
				result = 3;
			}
		}
		FS_GetEntryName(LEntry, MAX_LIST_FILENAME, g_list_filename);
		gui_unicode2local(g_list_filename, MAX_LIST_FILENAME - 2);
		g_folder_last_index = value;
		*string = g_list_filename;
		return result;
	}
	else if(LIST_CALLBACK_SELECT_ITEM == type)
	{
		if(DIRECTORY_ALL == g_folder_list_type && (g_folder_change == 2 || value > 0))
		{
			// menu
			result = GUI_Display_Menu(GUI_MENU_FOLDERLIST, NULL);
		}
		
		if(g_folder_change == 2)
		{
			if(FS_MountDevice(value + 1) == ERR_SUCCESS)
			{
				g_folder_change = 1;
				FS_ChangeDir(FS_CD_ROOT);
				g_list_location.subdir_entry = FS_ROOT_INO;
				if(result == 102) // select
				{
					g_folder_change = 0;
				}
				return 0;
			}
			else
			{
				GUI_DisplayMessage(0, GUI_STR_MREADERR, NULL, GUI_MSG_FLAG_WAIT);
				return 1;
			}
		}
		
		g_folder_change = 1;
		if(value == 0)
		{
			if(ERR_SUCCESS != FS_ChangeDir(FS_CD_PARENT))
			{
				g_folder_change = 2; // goto root
			}
			else
			{
				FS_GetSubDirEntry(&g_list_location.subdir_entry);
			}
		}
		else if(value <= g_list_folder_count)
		{
			FS_GetNextEntry(&LEntry, NULL, FS_DIR_DIR);
			for(i = 0; i < value - 1; i++)
			{
				FS_GetNextEntry(&LEntry, NULL, FS_DIR_DIR_CONT);
			}
			FS_ChangeSubDir(LEntry);
			FS_GetSubDirEntry(&g_list_location.subdir_entry);
			if(result == 102) // select
			{
				g_folder_change = 0;
			}
		}
		else
		{
			FS_GetNextEntry(&LEntry, g_folder_list_ext, FS_DIR_FILE);
			for(i = 0; i < value - g_list_folder_count - 1; i++)
			{
				FS_GetNextEntry(&LEntry, g_folder_list_ext, FS_DIR_FILE_CONT);
			}
			g_list_location.file_entry = LEntry;
			g_folder_change = 0;
		}
		return 0;
	}
}



/********************************************************************************
* Description : select the directory.
*
* Arguments  :
*            location: get thedisk type
*            type : show file kind
*            string_id: language type
* Returns     :
*            other:    Successful
*            0:    Fail, detail information in fselError
* Notes       :  only select the valid directory
*
********************************************************************************/
INT32 GUI_Directory(file_location_t *location, UINT8 type, UINT8 string_id)
{
	UINT32 count, curr;
	INT32 result;
	
	g_list_location = *location;
	
	if(ERR_FS_NOT_MOUNT == FS_GetEntryCount(&g_list_folder_count, NULL, FS_DIR_DIR))
	{
		// no disk mount, list disks
		curr = 0;
		g_folder_change = 2;
	}
	else
	{
		curr = g_list_folder_count;
		if(DIRECTORY_ALL != type)
		{
			curr += fselGetNo();
		}
	}
	g_folder_list_type = type;
	
	if(type == DIRECTORY_MUSIC)
	{
		g_folder_list_ext = ExtMusic;
	}
	else
	{
		g_folder_list_ext = NULL;
	}
	
	while(1)
	{
		if(g_folder_change == 2)
		{
#if APP_SUPPORT_USB
			g_list_folder_count = 1;
#else
			g_list_folder_count = 0;
#endif
			count = 0;
		}
		else
		{
			FS_GetEntryCount(&g_list_folder_count, NULL, FS_DIR_DIR);
			if(type == DIRECTORY_ALL)
			{
				count = 0;
			}
			else
			{
				FS_GetEntryCount(&count, g_folder_list_ext, FS_DIR_FILE);
			}
		}
		
		result = GUI_Display_List(LIST_TYPE_CUSTOM_IMAGE, GUI_IMG_FOLDER_LIST, count + g_list_folder_count + 1, curr, file_list_callback, FALSE);
		if(result == 0)
		{
			if(g_folder_change)
			{
				curr = 0;
				continue;
			}
			location->disk = g_list_location.disk;
			location->subdir_entry = g_list_location.subdir_entry;
			location->file_entry = g_list_location.file_entry;
			FS_GetShortName(location->subdir_entry, location->subdir_name);
			FS_GetShortName(location->file_entry, location->file_name);
		}
		return result;
	};
}
#endif


