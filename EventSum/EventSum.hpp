//*********************************************************
//	EventSum.hpp - Summarize Event File Data
//*********************************************************

#ifndef EVENTSUM_HPP
#define EVENTSUM_HPP

#include "Demand_Service.hpp"
#include "Event_File.hpp"
#include "Time_Range.hpp"
#include "Link_Equiv.hpp"
#include "Link_Data_File.hpp"
#include "Data_Range.hpp"
#include "Ext_File.hpp"
#include "Class_Index.hpp"
#include "Class_Array.hpp"
#include "Plan_File.hpp"
#include "Time_Diff_Output.hpp"

#include "Data/Event_Data.hpp"
#include "Data/Event_Time.hpp"
#include "Data/HHList_Data.hpp"
#include "Data/Travel_Time_File.hpp"
#include "Data/Time_Sum_File.hpp"
#include "Data/Link_Event.hpp"

//---------------------------------------------------------
//	EventSum - execution class definition
//---------------------------------------------------------

class EventSum : public Demand_Service
{
public:
	EventSum (void);
	virtual ~EventSum (void);

	virtual void Execute (void);

protected:

	static char * EVENT_FILE;
	static char * EVENT_FORMAT;
	static char * HOUSEHOLD_LIST;
	static char * TRAVEL_TIME_FILE;
	static char * TRAVEL_TIME_FORMAT;
	static char * TIME_OF_DAY_FORMAT;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * SELECT_HOUSEHOLD_RANGE;
	static char * SELECT_TRIP_MODES;
	static char * SELECT_PLAN_MODES;
	static char * COMBINE_EVENT_LEGS;
	static char * NEW_HOUSEHOLD_LIST;
	static char * NEW_TRAVEL_TIME_FILE;
	static char * NEW_TRAVEL_TIME_FORMAT;
	static char * NEW_TIME_SUMMARY_FILE;
	static char * NEW_TIME_SUMMARY_FORMAT;
	static char * MINIMUM_SCHEDULE_VARIANCE;
	static char * NEW_DISTRIBUTION_FILE;
	static char * NEW_DIFFERENCE_FILE;
	static char * NEW_LINK_EVENT_FILE;
	static char * NEW_LINK_EVENT_FORMAT;
	static char * LINK_EQUIVALENCE_FILE;
	static char * PERCENT_TIME_DIFFERENCE;
	static char * MINIMUM_TIME_DIFFERENCE;
	static char * MAXIMUM_TIME_DIFFERENCE;
	static char * SELECTION_PERCENTAGE;
	static char * MAXIMUM_PERCENT_SELECTED;
	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NEW_PLAN_FILE;
	static char * NEW_PLAN_FORMAT;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum EventSum_Reports { TOP_100_TTIME = 1, TOTAL_DISTRIB, TTIME_DISTRIB, PERIOD_SUM, 
		TOP_100_LINK, TOP_100_PERIOD, 
		LINK_EVENT, GROUP_EVENT, LINK_EQUIV, TRAVELER_SUM, EVENT_SUM };

	bool hhlist_flag, link_flag, trip_flag, travel_flag, group_flag, time_in_flag;
	bool time_file_flag, time_sum_flag, compare_flag, trip_file_flag, activity_flag, diff_file_flag;
	bool distrib_flag, time_flag, select_flag, new_hhold_flag, update_flag, period_flag;
	bool trip_modes [MAX_MODE], plan_modes [DRIVER_MODE+1], combine_flag, range_flag;
	int increment, num_inc, max_hhold, min_time, max_time, nfile, min_variance;
	double percent_diff, percent, max_percent;

	Event_File event_file;
	Link_Data_File link_file;

	Plan_File plan_file, new_plan_file;

	Travel_Time_File travel_time_file, time_in_file;
	Time_Sum_File time_sum_file;
	Ext_File hhlist_file, new_hhlist_file;

	Data_Range hhold_range;
	Time_Range time_periods;
	Link_Equiv link_equiv;

	Time_Diff_Output time_diff;
	Db_File diff_file;


	HHList_Array hhlist_data;
	Event_Array event_data;
	Event_Time_Array event_time;
	Link_Event_Array link_event;
	Class2_Array difference_sort;

	//---- methods ----

	void Read_HHList (void);
	void Read_Events (void);
	void Read_Times (void);
	void Read_Trips (void);
	void Read_Activity (void);

	void Combine_Events (void);
	void Select_Travelers (void);

	void Write_Events (void);
	void Write_Times (void);
	void Write_Links (void);
	void Write_HHList (void);

	void Update_Plans (void);

	void Top_100_TTime_Report (void);
	void Top_100_TTime_Header (void);

	void Top_100_Link_Report (void);
	void Top_100_Link_Header (void);

	void Top_100_Period_Report (void);
	void Top_100_Period_Header (void);

	void Link_Event_Report (int filter);
	void Link_Event_Header (void);

	void Group_Event_Report (int filter);
	void Group_Event_Header (void);

	void Traveler_Sum_Report (void);
	void Traveler_Sum_Header (void);

	void Event_Sum_Report (void);
	void Event_Sum_Header (void);
};

#endif
