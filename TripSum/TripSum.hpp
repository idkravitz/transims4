//*********************************************************
//	TripSum.hpp - Summarize Trip File
//*********************************************************

#ifndef TRIPSUM_HPP
#define TRIPSUM_HPP

#include "Demand_Service.hpp"
#include "Integer_Array.hpp"
#include "Time_Range.hpp"
#include "Zone_Equiv.hpp"
#include "Link_Data_File.hpp"
#include "Matrix_Data.hpp"
#include "Matrix_File.hpp"
#include "Data_Range.hpp"
#include "Trip_Sum_Data.hpp"
#include "Trip_Length_Data.hpp"
#include "Ext_File.hpp"
#include "Arcview_File.hpp"
#include "Factor_File.hpp"
#include "Factor_Data.hpp"
#include "Time_Equiv.hpp"
#include "Random.hpp"
#include "Survey_Person.hpp"
#include "Point_Time_Data.hpp"
#include "Point_Time_File.hpp"

//---------------------------------------------------------
//	TripSum - execution class definition
//---------------------------------------------------------

class TripSum : public Demand_Service
{
public:
	TripSum (void);
	virtual ~TripSum (void);

	virtual void Execute (void);

protected:

	static char * HOUSEHOLD_LIST;
	static char * HOUSEHOLD_PERSON_COUNT;
	static char * TIME_OF_DAY_FORMAT;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * SUMMARY_LENGTH_INCREMENT;
	static char * SELECT_TRIP_MODES;
	static char * SELECT_TRIP_PURPOSES;
	static char * SELECT_ORIGIN_ZONES;
	static char * SELECT_DESTINATION_ZONES;
	static char * SELECT_ORIGIN_POLYGON;
	static char * SELECT_DESTINATION_POLYGON;
	static char * SELECT_PROBABILITY_FILE;
	static char * SELECT_PROBABILITY_FORMAT;
	static char * TIME_PERIOD_EQUIVALENCE;
	static char * RANDOM_NUMBER_SEED;
	static char * NEW_LINK_TRIP_END_FILE;
	static char * NEW_LINK_TRIP_END_FORMAT;
	static char * NEW_TIME_DISTRIBUTION;
	static char * NEW_TRIP_TABLE_FILE;
	static char * NEW_TRIP_TABLE_FORMAT;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * NEW_TRIP_TIME_FILE;
	static char * NEW_TRIP_DISTANCE_FILE;
	static char * DISTANCE_CALCULATION;
	static char * NEW_LOCATION_TRIP_END_FILE;
	static char * NEW_LOCATION_TRIP_END_FORMAT;
	static char * NEW_ZONE_TRIP_END_FILE;
	static char * NEW_ZONE_TRIP_END_FORMAT;
	static char * CREATE_INDEPENDENT_TRIPS;
	static char * NEW_HOUSEHOLD_LIST;
	static char * STARTING_HOUSEHOLD_ID;
	static char * STARTING_VEHICLE_ID;
	static char * ZONE_TARGET_FILE;
	static char * TARGET_HOUSEHOLD_RANGE;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum TripSum_Reports { TOP_100_LINKS = 1, TOP_100_LANES, TOP_100_VC_RATIOS, ZONE_EQUIV, 
		TRIP_TIME, TRIP_LENGTH, TRIP_PURPOSE, MODE_LENGTH, MODE_PURPOSE, TIME_EQUIV};

	bool select_org_flag, select_des_flag, org_zone_flag, des_zone_flag, vehicle_flag;
	bool trip_flag, activity_flag, volume_flag, link_flag, time_flag, table_flag, zone_flag, purpose_flag;
	bool trip_mode [MAX_MODE], hhlist_flag, output_flag, distance_length_flag, time_length_flag;
	bool length_flag, trip_purpose_flag, mode_flag, mode_purpose_flag, split_flag, count_flag;
	bool location_flag, factor_flag, loc_end_flag, zone_end_flag, create_flag, newhh_flag;
	int increment, num_inc, total_in, total_out, total_used, total_hhold, hhold_id, veh_id;
	
	Distance_Type distance_method;

	Arcview_File select_origin, select_destination;

	Link_Data_File volume_file;
	Matrix_File trip_file;
	Db_File time_file;
	Ext_File count_file;
	Ext_File hhlist_file;
	Ext_File newhh_file;

	bool synod_flag;
	Db_File zone_file;
	int nzone, **first_veh, *next_veh, **veh_count;
	double **synod, *org_target, *des_target, *zone_total, *org_total, *des_total;
	Data_Range hhold_range;

	Point_Time_File loc_end_file, zone_end_file;
	Point_Time_Array loc_end_data, zone_end_data;

	Time_Range time_periods;
	Data_Range purpose_range, org_zone_range, des_zone_range;

	Zone_Equiv zone_equiv;

	Integer_List start_time, end_time, mid_time;
	Integer_Sort hhold_list, vehicle_list;

	Matrix_Array trip_table;
	
	Trip_Sum_Array trip_length, trip_purpose, mode_length, mode_purpose;
	Trip_Length_Array trip_time_length, trip_distance_length;

	Factor_File factor_file;
	Factor_Array factor_data;
	Time_Equiv time_equiv;
	Random random;

	Survey_Person_Array person_count;

	//---- methods ----

	bool Household_List (int file_num);
	void Read_Count (void);
	void Read_Target (void);
	bool Read_Trip (int file_num);
	bool Read_Activity (int file_num);
	bool Trip_Process (int origin, int destination, int start, int end, int mode, int purpose, int prev_purp, int vehicle, int count = 1);

	void Synthetic_OD (void);
	void Write_Volume (void);
	void Diurnal_Distribution (void);
	void Write_Trips (void);
	void Write_Locations (void);
	void Write_Zones (void);

	void Read_Factor (void);
	void Read_Vehicle (void);

	void Top_100_Report (int report_num);
	void Top_100_Header (int report_num);

	void Trip_Time (void);
	void Trip_Time_Header (void);
};

#endif
