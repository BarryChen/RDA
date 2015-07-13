/********************************************************************************
*                RDA API FOR MP3HOST
*
*        (c) Copyright, RDA Co,Ld.
*             All Right Reserved
*
********************************************************************************/
#ifndef _AP_CALENDAR_H_
#define _AP_CALENDAR_H_


/* struct clndr_lnuar_date_info_struct defines all data needed for lunar calendar */
typedef struct
{
	UINT16 first_year;
	UINT16 last_year;
	struct
	{
		UINT8 BaseDays;    /* different days from solar Jan 1 to lunar Jan 1. */
		UINT8 BaseWeekday; /* week day of Jan 1 this year. 0=Sunday */
		UINT8 EasterSunday; /* easter sunday from March 1st */
		UINT8 Intercalary; /* an intercalary month in the lunar calendar. 0==no intercalary month in this year. */
		UINT16 MonthDays;  /* the number of days in lunar month, bit-level representation 0=29, 1=30. */
		UINT8 solar_term_table[24];
	} data[1];
} clndr_table_struct;



INT32 CALENDAR_Entry(INT32 param);//进入日历的主要函数
U16 IsLeapYear(U16 year);

#endif /*_AP_CALENDAR_H_*/




