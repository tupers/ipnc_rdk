/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file Dmva_scheduler.h
* @brief Dmva scheduler structures and #defines.
*/
#ifndef __DMVA_SCHEDULER_H__
#define __DMVA_SCHEDULER_H__

#include <sem_util.h>

// to print DMVA scheduler messages
//#define DMVA_SCH_DBG

#define DMVA_SCH_MAX_STRING_LENGTH          64
#define DMVA_SCH_MAX_NUM_SCHEDULE           10

// schedule days
#define DMVA_SCH_DAY_MON                    1
#define DMVA_SCH_DAY_TUE                    2
#define DMVA_SCH_DAY_WED                    3
#define DMVA_SCH_DAY_THU                    4
#define DMVA_SCH_DAY_FRI                    5
#define DMVA_SCH_DAY_SAT                    6
#define DMVA_SCH_DAY_SUN                    7
#define DMVA_SCH_DAY_ALL                    8
#define DMVA_SCH_DAY_WEEKDAYS               9
#define DMVA_SCH_DAY_WEEKEND                10

// schedule rule type
#define DMVA_SCH_RULETYPE_TZ                0
#define DMVA_SCH_RULETYPE_IMD               1
#define DMVA_SCH_RULETYPE_OC                2
#define DMVA_SCH_RULETYPE_SMETA             3

// schedule result
#define DMVA_SCH_RESULT_ADDDONE             1
#define DMVA_SCH_RESULT_CONFLICT            2
#define DMVA_SCH_RESULT_LISTFULL            3
#define DMVA_SCH_RESULT_INVALIDLOADSETTINGS 4
#define DMVA_SCH_RESULT_INCORRECTTIME		5
#define DMVA_SCH_RESULT_INCORRECTDAY 		6
#define DMVA_SCH_RESULT_DELDONE             7
#define DMVA_SCH_RESULT_DELFAIL             8


extern SemHandl_t gSemDmvaSchedule;

// DMVA Schedule Data structures

/* structure for the each schedule */
typedef struct
{
    int  loadSetting;                              /* name of the rule to be loaded */
    int  ruleType;                                 /* TRIP/IMD/OC/SMETA */
    char ruleLabel[DMVA_SCH_MAX_STRING_LENGTH];    /* Label for the schedule rule */
    int  startDay;                                 /* start day - everyday/weekdays/weekend/sunday/../saturday */
    int  startTimeInMin;                           /* start time in minutes */
    int  endDay;                                   /* end day */
    int  endTimeInMin;                             /* end time in minutes */
    int  isActive;                                 /* Is the schedule active/deleted */
    int  indexNo;                                  /* Index in the schedule list */

}DMVA_schedule_t;


/* structure for the schedule manager */
typedef struct
{
    int enable;                                    /* DMVA scheduler is enabled/disabled */
    int curSchedule;                               /* current schedule */
    int curRuleType;                               /* current rule type */
    int curLoadSetting;                            /* current load setting */
    int result;                                    /* result of schedule add */
    DMVA_schedule_t dmvaSchedule[DMVA_SCH_MAX_NUM_SCHEDULE];
    DMVA_schedule_t curDmvaSchedule;

}DMVA_scheduleMgr_t;

#endif /* __DMVA_SCHEDULER_H__ */
