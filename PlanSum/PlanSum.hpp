//*********************************************************
//	PlanSum.hpp - Summarize Plan File Volumes
//*********************************************************

#ifndef PLANSUM_HPP
#define PLANSUM_HPP

#include "Demand_Service.hpp"
#include "Integer_Array.hpp"
#include "Plan_File.hpp"
#include "Random.hpp"
#include "Time_Range.hpp"
#include "Equation.hpp"
#include "Link_Equiv.hpp"
#include "Zone_Equiv.hpp"
#include "Stop_Equiv.hpp"
#include "Link_Data_File.hpp"
#include "Skim_Data.hpp"
#include "Skim_File.hpp"
#include "Matrix_Data.hpp"
#include "Matrix_File.hpp"
#include "Movement_Data.hpp"
#include "Movement_File.hpp"
#include "Data_Range.hpp"
#include "Transfer_Data.hpp"
#include "Trip_Sum_Data.hpp"
#include "Time_Skim_Data.hpp"
#include "TLen_Skim_Data.hpp"
#include "Survey_Person.hpp"

#define  MAX_BOARD	10

#define transfer_data		(*((Transfer_Array *) stop_array))

//---------------------------------------------------------
//	PlanSum - execution class definition
//---------------------------------------------------------

class PlanSum : public Demand_Service
{
public:
	PlanSum (void);
	virtual ~PlanSum (void);

	virtual void Execute (void);

protected:

	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * HOUSEHOLD_LIST;
	static char * HOUSEHOLD_PERSON_COUNT;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * NEW_PLAN_FILE;
	static char * NEW_LINK_VOLUME_FILE;
	static char * NEW_LINK_VOLUME_FORMAT;
	static char * KEEP_INPUT_TRAVEL_TIMES;
	static char * EQUATION_PARAMETERS_x;
	static char * LINK_EQUIVALENCE_FILE;
	static char * NEW_TRIP_TIME_FILE;
	static char * NEW_ZONE_SKIM_FILE;
	static char * NEW_ZONE_SKIM_FORMAT;
	static char * SKIM_MODE_SELECTION;
	static char * SKIM_TOTAL_TIME;
	static char * SKIM_TRIP_LENGTH;
	static char * SKIM_TIME_CONTROL_POINT;
	static char * NEAREST_NEIGHBOR_FACTOR;
	static char * NEW_TRIP_TABLE_FILE;
	static char * NEW_TRIP_TABLE_FORMAT;
	static char * TRIP_MODE_SELECTION;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * NEW_TURN_MOVEMENT_FILE;
	static char * NEW_TURN_MOVEMENT_FORMAT;
	static char * TURN_NODE_RANGE;
	static char * STOP_EQUIVALENCE_FILE;
	static char * OUTPUT_ALL_TRANSIT_STOPS;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum PlanSum_Reports { TOP_100 = 1, VC_RATIO, LINK_GROUP, LINK_EQUIV, ZONE_EQUIV, STOP_EQUIV, 
		SUM_RIDERS, SUM_STOPS, SUM_TRANSFERS, XFER_DETAILS, STOP_GROUP, SUM_PASSENGERS, RIDER_GROUP, 
		TRIP_TIME, SUM_TRAVEL };

	bool delay_in, delay_out, travel_flag, update_flag, passenger_flag, time_skim_flag;
	bool volume_flag, time_flag, skim_flag, type_flag, trip_flag, zone_flag, transit_flag;
	bool skim_mode [MAX_MODE], trip_mode [MAX_MODE], turn_flag, turn_delay, hhlist_flag;
	bool xfer_flag, xfer_detail, neighbor_flag, length_flag, count_flag, stops_flag, vc_flag;
	int increment, num_inc, **boardings, max_rail, layer, skim_point;
	char xfer_label [FIELD_BUFFER];
	double minimum_vc, cap_factor, neighbor_factor;

	Plan_File plan_file, new_plan_file;
	Link_Data_File volume_file;
	Skim_File skim_file;
	Matrix_File trip_file;
	Db_File time_file;
	Ext_File count_file;
	Ext_File hhlist_file;

	Random random;
	Time_Range time_periods;

	Link_Equiv link_equiv;
	Zone_Equiv zone_equiv;
	Equation_Array equation;
	Stop_Equiv stop_equiv;

	Integer_Sort hhold_list;
	Integer_List start_time, end_time, mid_time, park_location, stop_location;

	Survey_Person_Array person_count;
	Skim_Array skim_data;
	Matrix_Array trip_table;
	Time_Skim_Array time_skim_data;
	Time_Len_Skim_Array time_len_skim_data;

	Trip_Sum_Data trip_sum_total;
	Trip_Sum_Array trip_sum_data;
	Trip_Sum_Data pass_sum_total;
	Trip_Sum_Array pass_sum_data;

	Movement_File turn_file;
	Movement_Array **movement_data;
	Data_Range *node_range;

	//---- methods ----

	void Data_Preparation (void);
	void Read_HHList (void);
	void Read_Count (void);
	void Read_Plans (void);
	void Write_Volume (void);
	void Link_Delay (void);
	void Write_Riders (void);
	void Diurnal_Distribution (void);
	void Write_Skim (void);
	void Write_Time (void);
	void Write_Time_Length (void);
	void Write_Trips (void);
	void Write_Turns (void);

	void Top_100_Report (void);
	void Top_100_Header (void);

	void VC_Ratio (double min_vc);
	void VC_Ratio_Header (void);

	void Link_Group (double min_vc);
	void Link_Group_Header (void);

	void Trip_Time (void);
	void Trip_Time_Header (void);

	void Transit_Report (void);
	void Transit_Header (void);

	void Stop_Report (void);
	void Stop_Header (void);

	void Stop_Group_Report (void);
	void Stop_Group_Header (void);

	void Transfer_Report (int num);
	void Transfer_Header (void);

	void Travel_Report (void);
	void Travel_Header (void);

	void Passenger_Report (void);
	void Passenger_Header (void);

	void Rider_Group (void);
	void Rider_Header (void);
};

#endif
