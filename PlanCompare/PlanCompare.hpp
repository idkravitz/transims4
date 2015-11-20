//*********************************************************
//	PlanCompare.hpp - Compare Two Plan Files
//*********************************************************

#ifndef PLANCOMPARE_HPP
#define PLANCOMPARE_HPP

#include "Demand_Service.hpp"
#include "Integer_Array.hpp"
#include "Complex_Array.hpp"
#include "Plan_File.hpp"
#include "Location_File.hpp"
#include "Location_Data.hpp"
#include "Trip_File.hpp"
#include "Trip_Data.hpp"
#include "Random.hpp"
#include "Db_Code.hpp"
#include "Db_File.hpp"
#include "Time_Range.hpp"
#include "Data_Range.hpp"
#include "Time_Diff_Output.hpp"

#define NUM_TIME_PERIOD		25
#define NUM_CHANGE_BINS		500
#define NUM_PERCENTILES		9

#define TOTAL_TIME_PERIOD	(NUM_TIME_PERIOD-1)

//---------------------------------------------------------
//	PlanCompare - execution class definition
//---------------------------------------------------------

class PlanCompare : public Demand_Service
{
public:
	PlanCompare (void);
	virtual ~PlanCompare (void);

	virtual void Execute (void);

protected:

	static char * INPUT_PLAN_FILE;
	static char * INPUT_PLAN_FORMAT;
	static char * INPUT_TRIP_FILE;
	static char * COMPARE_PLAN_FILE;
	static char * COMPARE_PLAN_FORMAT;
	static char * COMPARE_LOCATION_FILE;
	static char * COMPARE_TRIP_FILE;
	static char * OUTPUT_PLAN_FILE;
	static char * OUTPUT_PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * HOUSEHOLD_LIST;
	static char * SELECT_TRAVELERS;
	static char * SELECT_TRIP_MODES;
	static char * IGNORE_LOCATION_DIFFERENCES;
	static char * INDEX_BY_COMPARE_LOCATION;
	static char * ONLY_COMPARE_DRIVE_LEGS;
	static char * COMPARE_GENERALIZED_COSTS;
	static char * PERCENT_TIME_DIFFERENCE;
	static char * MINIMUM_TIME_DIFFERENCE;
	static char * MAXIMUM_TIME_DIFFERENCE;
	static char * PERCENT_PATH_DIFFERENCE;
	static char * MINIMUM_PATH_DIFFERENCE;
	static char * MAXIMUM_PATH_DIFFERENCE;
	static char * SELECTION_PERCENTAGE;
	static char * MAXIMUM_PERCENT_SELECTED;
	static char * SELECT_BY_PERCENT_DIFFERENCE;
	static char * SELECT_BY_RELATIVE_GAP;
	static char * SELECT_TIME_PERIODS;
	static char * RANDOM_NUMBER_SEED;
	static char * TIME_OF_DAY_FORMAT;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * PERIOD_CONTROL_POINT;
	static char * HIGHEST_MATCH_DIFFERENCE;
	static char * LOWEST_MATCH_DIFFERENCE;
	static char * NEW_DISTRIBUTION_FILE;
	static char * NEW_COST_DISTRIBUTION_FILE;
	static char * NEW_MODE_DISTRIBUTION_FILE;
	static char * NEW_MODE_DISTRIBUTION_DATA_FILE;
	static char * NEW_DIFFERENCE_FILE;
	static char * NEW_START_DIFFERENCE_FILE;
	static char * NEW_END_DIFFERENCE_FILE;
	static char * NEW_TRIP_TIME_FILE;
	static char * NEW_TRAVELER_MATCH_FILE;
	static char * NEW_INCOMPLETE_TRIP_FILE;
	static char * NEW_PERIOD_DIFFERENCE_FILE;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum PlanCompare_Reports { PATH_CHANGE = 1, TIME_CHANGE, COST_CHANGE,
		DUMP_PATH_CHANGES, DUMP_TIME_CHANGES, DUMP_COST_CHANGES,
		TOTAL_DISTRIB, TTIME_DISTRIB, PERIOD_SUM, 
		TCOST_DISTRIB, COST_DISTRIB, COST_SUM, TRIP_GAP };

	enum Combine_Type { FILE_COMBINE, MEMORY_COMBINE };

	Plan_File input_plans, compare_plans, output_plans, input_temp, compare_temp;
	Trip_File input_trips, compare_trips;
	Location_File compare_loc_file;
	Ext_File hhlist_file;
	Db_File match_file;
	Db_File incomplete_file;

	Location_Array compare_loc_data;
	Trip_Array input_trip_data, compare_trip_data;

	Integer_Sort hhold_list;

	typedef struct {
		int traveler;
		int trip;
	} Select_Trips;

	Complex_Array select_trips;
	
	typedef struct {
		int location;
		int type;
		int count;
		double input;
		double compare;
		double diff;
	} Location_Diff;

	Complex_Array start_diff, end_diff;

	typedef struct {
		int percent;
		int traveler;
		int trip;
		int time_diff;
	} Travel_Diff;

	Complex_Array travel_diff;
	Travel_Diff min_key;

	typedef struct {
		int count;
		double input;
		double compare;
		double diff;
	} Period_Diff;

	Data_Array period_diff;

	Db_File diff_file, time_file, start_file, end_file, period_file;

	Data_Range travelers;
	Time_Range time_periods, select_time;
	Random random;

	bool output_flag, select_flag, diff_flag, hhlist_flag, path_flag, compare_flag, ignore_flag, drive_flag;
	bool distrib_flag, time_flag, time_diff_flag, path_diff_flag, diff_file_flag, period_flag, trip_flag;
	bool cost_flag, cost_data_flag, gap_flag, cost_distrib_flag, traveler_flag, start_flag, end_flag;
	bool loc_flag, match_out_flag, incomplete_flag, trip_mode [MAX_MODE], compare_loc_flag;
	bool mode_flag, mode_file_flag, drive_access_flag, mode_data_flag, period_diff_flag, gap_data_flag;

	int num_matched, num_complete, num_drive, num_compared, num_equal, in_only, compare_only, num_bad;
	int nsel, nout, min_time, max_time, min_path, max_path, max_diff, min_diff, high_diff, low_diff, period_point;
	double sig_diff, sig_tot;
	double percent, max_percent, percent_diff, path_diff, tot_select, tot_diff, abs_diff, tot_base;

	Time_Diff_Output time_diff, cost_diff, mode_diff, mode_data;
	
	int num_inc;

	typedef struct {
		int input_start;
		int compare_start;
		int start_diff;
		int input_end;
		int compare_end;
		int end_diff;
	} Trip_Time;

	Trip_Time trip_time [MIDNIGHT / 900];

	int increment;
	int path_changes [NUM_TIME_PERIOD] [NUM_CHANGE_BINS], dump_path;
	int time_changes [NUM_TIME_PERIOD] [NUM_CHANGE_BINS], dump_time;
	int cost_changes [NUM_TIME_PERIOD] [NUM_CHANGE_BINS], dump_cost;

	char *change_text [COST_CHANGE+1];

	static int percent_break [NUM_PERCENTILES];

	//---- methods ----

	void Read_HHList (void);
	void Compare_Plans (void);
	void Write_Plans (void);
	void Write_Trip_Times (void);

	void Read_Trips (Trip_File *file, Trip_Array *data);
	void Read_Location (void);

	void Change_Report (int type);
	void Change_Header (int type);

	void Trip_Gap_Report (void);
	void Trip_Gap_Header (void);
};
#endif

