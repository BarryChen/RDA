#ifndef __MMC_ERR_H__
#define __MMC_ERR_H__

#define MERR_SUCCESS		0
#define MERR_OK				0
#define MERR_NORMAL			-1//unclassified error
#define MERR_STATE			-2//state error
#define MERR_OPEN_FILE		-3//open file error
#define MERR_PARAMETER		-4//parameter error
#define MERR_MEMORY			-5//Malloc error
#define MERR_MOREDATA	    -6//No data for codec need read more
#define MERR_TOEND	        -7//Reach end of raw data
#define MERR_FORMAT	        -8//Raw data format error
#define MERR_UNSUPPORT      -9//Unsupport format error
#define MERR_MODE           -10//DSP working mode error
#define MERR_READFILE	    -11//Read file error
#define MERR_WRITEFILE      -12//Write file error
#define MERR_TIMEOUT		-13//Time out to start
#define MERR_DATAFULL		-14//Data is full
#define MERR_UNPREPARED		-15//unprepared
#define MERR_UNFINISHED		-16//unfinished

#endif//__MMC_ERR_H__

