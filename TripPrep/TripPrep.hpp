//*********************************************************
//	TripPrep.hpp - Prepare Trip and Activity Files
//*********************************************************

#ifndef TRIPPREP_HPP
#define TRIPPREP_HPP

#include "Demand_Service.hpp"
#include "Trip_File.hpp"
#include "Activity_File.hpp"
#include "Vehicle_File.hpp"
#include "Integer_Array.hpp"
#include "Time_Range.hpp"
#include "Data_Range.hpp"
#include "Ext_File.hpp"
#include "Arcview_File.hpp"
#include "Factor_File.hpp"
#include "Factor_Data.hpp"
#include "Zone_Equiv.hpp"
#include "Time_Equiv.hpp"
#include "Random.hpp"
#include "User_Program.hpp"

//---------------------------------------------------------
//	TripPrep - execution class definition
//---------------------------------------------------------

class TripPrep : public Demand_Service
{
public:
	TripPrep (void);
	virtual ~TripPrep (void);

	virtual void Execute (void);

protected:

	static char * HOUSEHOLD_LIST;
	static char * MERGE_TRIP_FILE;
	static char * MERGE_TRIP_FORMAT;
	static char * MERGE_ACTIVITY_FILE;
	static char * MERGE_ACTIVITY_FORMAT;
	static char * MERGE_VEHICLE_FILE;
	static char * MERGE_VEHICLE_FORMAT;
	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_HOUSEHOLDS;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_TRIP_MODES;
	static char * SELECT_TRIP_PURPOSES;
	static char * SELECT_ORIGIN_ZONES;
	static char * SELECT_DESTINATION_ZONES;
	static char * SELECT_ORIGIN_POLYGON;
	static char * SELECT_DESTINATION_POLYGON;
	static char * SELECT_PROBABILITY_FILE;
	static char * SELECT_PROBABILITY_FORMAT;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * TIME_PERIOD_EQUIVALENCE;
	static char * SELECTION_PERCENTAGE;
	static char * RANDOM_NUMBER_SEED;
	static char * START_TIME_SHIFT;
	static char * DATA_PROCESSING_SCRIPT;
	static char * OUTPUT_ALL_RECORDS;
	static char * CREATE_INDEPENDENT_TRIPS;
	static char * NEW_HOUSEHOLD_LIST;
	static char * STARTING_HOUSEHOLD_ID;
	static char * STARTING_VEHICLE_ID;
	static char * ZONE_TARGET_FILE;
	static char * TARGET_HOUSEHOLD_RANGE;
	static char * MOVE_ORIGIN_FILE;
	static char * MOVE_DESTINATION_FILE;
	static char * CONVERT_ACTIVITIES_TO_TRIPS;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum TripPrep_Reports { ZONE_EQUIV = 1, TIME_EQUIV, PRINT_SCRIPT, PRINT_STACK  };

	bool select_org_flag, select_des_flag, org_zone_flag, des_zone_flag, vehicle_flag;
	bool activity_flag, purpose_flag, zone_equiv_flag, prob_flag, synod_flag, all_flag;
	bool trip_mode [MAX_MODE], hhlist_flag, output_flag, split_flag, shift_flag, convert_flag;
	bool location_flag, factor_flag, create_flag, newhh_flag, script_flag, traveler_flag;
	bool merge_trip_flag, merge_act_flag, merge_veh_flag, trip_flag, move_org_flag, move_des_flag;
	int total_in, total_out, total_used, total_hhold, hhold_id, veh_id, shift_size;
	double probability;

	Arcview_File select_origin, select_destination;

	Ext_File hhlist_file;
	Ext_File newhh_file;

	Trip_File merge_trip_file;
	Activity_File merge_act_file;
	Vehicle_File merge_veh_file;

	Db_File zone_file, move_org_file, move_des_file;
	int nzone, **first_veh, *next_veh, **veh_count;
	double **synod, *org_target, *des_target, *zone_total, *org_total, *des_total;
	Data_Range hhold_range;

	Time_Range time_periods;
	Data_Range purpose_range, org_zone_range, des_zone_range, traveler_range;

	Integer_Sort hhold_list, vehicle_list;
	Index_Array move_org_map, move_des_map, move_veh_map;
	
	Db_File program_file;
	User_Program program;

	Factor_File factor_file;
	Factor_Array factor_data;
	Time_Equiv time_equiv;
	Zone_Equiv zone_equiv;
	Random random, random2;

	//---- methods ----

	void Set_Files (void);
	bool Household_List (int file_num);
	void Read_Target (void);
	bool Read_Trip (int file_num);
	bool Read_Activity (int file_num);
	bool Trip_Check (int hholdr, int origin, int destination, int start, int end, int mode, int purpose, int vehicle);

	void Synthetic_OD (void);

	void Read_Factor (void);
	void Read_Vehicle (void);
};

#endif
