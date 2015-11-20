//*********************************************************
//	LinkSum.hpp - Summarize Link Performance Data
//*********************************************************

#ifndef LINKSUM_HPP
#define LINKSUM_HPP

#include "Demand_Service.hpp"
#include "Time_Range.hpp"
#include "Link_Equiv.hpp"
#include "Link_Data_File.hpp"
#include "Link_Dir_File.hpp"
#include "Delay_File.hpp"
#include "Arcview_File.hpp"
#include "Integer_Array.hpp"
#include "Db_Header.hpp"
#include "Db_Array.hpp"
#include "Complex_Array.hpp"
#include "Zone_Equiv.hpp"

#define NUM_SUM_BINS	1000
#define NUM_PERCENTILES	9
#define RESOLUTION		100

//---------------------------------------------------------
//	LinkSum - execution class definition
//---------------------------------------------------------

class LinkSum : public Demand_Service
{
public:
	LinkSum (void);
	virtual ~LinkSum (void);

	virtual void Execute (void);

	virtual bool Location_Processing (Db_File *fh);

protected:

	static char * PREVIOUS_LINK_DELAY_FILE;
	static char * PREVIOUS_LINK_DELAY_FORMAT;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * MINIMUM_LINK_VOLUME;
	static char * CONGESTED_TIME_RATIO;
	static char * SELECT_FACILITY_TYPES;
	static char * SELECT_SUBAREA_POLYGON;
	static char * SELECT_BY_LINK_GROUP;
	static char * LINK_EQUIVALENCE_FILE;
	static char * DEFAULT_OUTPUT_FORMAT;
	static char * NEW_LINK_ACTIVITY_FILE;
	static char * NEW_LINK_ACTIVITY_FORMAT;
	static char * ACTIVITY_LOCATION_FIELDS;
	static char * NEW_ZONE_TRAVEL_FILE;
	static char * NEW_ZONE_TRAVEL_FORMAT;
	static char * NEW_GROUP_TRAVEL_FILE;
	static char * NEW_GROUP_TRAVEL_FORMAT;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * NEW_LINK_DIRECTION_FILE_x;
	static char * NEW_LINK_DIRECTION_FORMAT_x;
	static char * NEW_LINK_DIRECTION_FIELD_x;
	static char * NEW_LINK_DIRECTION_INDEX_x;
	static char * NEW_LINK_DATA_FILE_x;
	static char * NEW_LINK_DATA_FORMAT_x;
	static char * NEW_LINK_DATA_FIELD_x;
	static char * NEW_PERFORMANCE_DATA_FILE;
	static char * NEW_PERFORMANCE_DATA_FORMAT;
	//----- the following are for backward compartibility only ----
	static char * NEW_LINK_VOLUME_FILE;
	static char * NEW_LINK_VOLUME_FORMAT;
	static char * NEW_LINK_SPEED_FILE;
	static char * NEW_LINK_SPEED_FORMAT;
	static char * NEW_LINK_TRAVEL_TIME_FILE;
	static char * NEW_LINK_TRAVEL_TIME_FORMAT;
	static char * NEW_LINK_VC_RATIO_FILE;
	static char * NEW_LINK_VC_RATIO_FORMAT;
	static char * NEW_LINK_TIME_RATIO_FILE;
	static char * NEW_LINK_TIME_RATIO_FORMAT;

	virtual void Program_Control (void);

	virtual void Page_Header (void);
	virtual int  Time_Period (int tod)		{ return (time_periods.Period (tod)); }

private:
	enum LinkSum_Reports { TOP_LINK_VOL = 1, TOP_LANE_VOL, TOP_PERIOD, TOP_SPEED, 
		TOP_TIME_RATIO, TOP_VC_RATIO, TOP_TIME_CHANGE, TOP_VOL_CHANGE, 
		LINK_REPORT, LINK_GROUP, LINK_EQUIV, ZONE_EQUIV,
		TIME_DISTRIB, VC_RATIOS, TIME_CHANGE, VOLUME_CHANGE, TRAVEL_TIME,
		PERF_REPORT, PERF_DETAIL, RELATIVE_GAP };

	bool funcl_flag [EXTERNAL+1], subarea_flag, select_flag, activity_flag, zone_flag, group_flag;
	bool previous_flag, group_select, detail_flag;
	int increment, num_inc, min_volume, nerror;
	double header_value, cap_factor, congested_ratio, **sum_bin, **zone_vmt, **zone_vht;

	Delay_File previous_delay;

	Db_Index_Array link_db;
	Db_Header activity_file, zone_file, group_file, detail_file;
	Integer_List field_list;

	Arcview_File select_subarea;
	Time_Range time_periods;
	Link_Equiv link_equiv;
	Zone_Equiv zone_equiv;
	Integer_Sort zone_list;

	typedef struct {
		Link_Data_File *file;
		int field;
	} Data_Group;

	Data_Array data_group;
	
	typedef struct {
		Link_Direction_File *file;
		int field;
		bool index;
	} Dir_Group;

	Data_Array dir_group;

	typedef struct {
		int link;
		int location;
		int zone;
		int count;
	} Link_Location;

	Complex_Array link_location;

	static int percent_break [NUM_PERCENTILES];

	//---- methods ----

	void Link_Dir_Control (char *name, char *file, char *format, char *field, bool index);
	void Link_Data_Control (char *name, char *file, char *format, char *field);
	void Select_Links (void);
	void Previous_Delay (void);
	void Error_Check (void);
	void Write_Activity (void);
	void Zone_Summary (void);
	void Write_Zone (void);
	void Write_Group (void);

	void Write_Link_Data (Data_Group *group);
	void Write_Link_Dir (Dir_Group *group);

	void Top_100_Report (int number);
	void Top_100_Link_Header (void);
	void Top_100_Lane_Header (void);
	void Top_100_Time_Header (void);

	void Top_100_Ratios (int number);
	void Top_100_Speed_Header (void);
	void Top_100_Travel_Time_Header (void);
	void Top_100_VC_Header (void);
	void Top_100_Time_Change_Header (void);
	void Top_100_Volume_Change_Header (void);

	void Link_Report (double limit);
	void Link_Report_Header (void);

	void Link_Group (double limit);
	void Link_Group_Header (void);

	void Travel_Time_Report (void);
	void Travel_Time_Header (void);

	void Volume_Capacity_Report (void);
	void Volume_Capacity_Header (void);

	void Travel_Time_Change (void);
	void Time_Change_Header (void);

	void Volume_Change (void);
	void Volume_Change_Header (void);

	void Group_Time_Report (void);
	void Group_Time_Header (void);

	void Performance_Report (void);
	void Performance_Header (void);

	void Perf_Detail_Report (void);
	void Perf_Detail_Header (void);

	void Relative_Gap_Report (void);
	void Relative_Gap_Header (void);

	void Perf_Detail_File (void);
};
#endif
