//*********************************************************
//	ConvertTrips.hpp - Trip Table Conversion Utility
//*********************************************************

#ifndef CONVERTTRIPS_HPP
#define CONVERTTRIPS_HPP

#include "Demand_Service.hpp"
#include "Random.hpp"
#include "Time_Step.hpp"
#include "Integer_Array.hpp"
#include "Trip_File.hpp"
#include "Matrix_Data.hpp"
#include "Skim_File.hpp"
#include "Factor_Data.hpp"
#include "Factor_File.hpp"
#include "Household_File.hpp"
#include "Population_File.hpp"
#include "Vehicle_File.hpp"
#include "Zone_Equiv.hpp"
#include "Zone_Location.hpp"

#include "Data/File_Group.hpp"
#include "Data/Diurnal_Target.hpp"
#include "Data/Convert_Location.hpp"

//---------------------------------------------------------
//	ConvertTrips - execution class definition
//---------------------------------------------------------

class ConvertTrips : public Demand_Service
{
public:
	ConvertTrips (void);
	virtual ~ConvertTrips (void);

	virtual void Execute (void);
	
	virtual bool Location_Processing (Db_File *file);
	virtual bool Access_Processing (Db_File *file);

	virtual bool Trip_Processing (Db_File *file);
	virtual bool Household_Processing (Db_File *file);
	virtual bool Population_Processing (Db_File *file);
	virtual bool Vehicle_Processing (Db_File *file);

protected:

	static char * STARTING_HOUSEHOLD_ID;
	static char * STARTING_VEHICLE_ID;
	static char * TIME_OF_DAY_FORMAT;
	static char * ADDITIONAL_TRAVEL_TIME;
	static char * ZONE_LOCATION_MAP_FILE;
	static char * RANDOM_NUMBER_SEED;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * TRAVEL_TIME_FILE;
	static char * TRIP_TABLE_FILE_x;
	static char * TRIP_TABLE_FORMAT_x;
	static char * TRIP_SCALING_FACTOR_x;
	static char * TRIP_TIME_FILE_x;
	static char * TRIP_TIME_FORMAT_x;
	static char * TRIP_TIME_SCRIPT_x;
	static char * TRIP_TIME_FIELD_x;
	static char * TIME_PERIOD_RANGE_x;
	static char * TIME_CONTROL_POINT_x;
	static char * RETURN_TRIP_OFFSET_x;
	static char * ORIGIN_WEIGHT_FIELD_x;
	static char * DESTINATION_WEIGHT_FIELD_x;
	static char * DISTANCE_WEIGHT_FLAG_x;
	static char * TRIP_PURPOSE_CODE_x;
	static char * TRAVEL_MODE_CODE_x;
	static char * AVERAGE_TRAVEL_SPEED_x;
	static char * VEHICLE_TYPE_x;
	static char * VEHICLE_SUBTYPE_x;
	static char * TRIP_ADJUSTMENT_FACTORS_x;
	static char * TRIP_ADJUSTMENT_FORMAT_x;
	static char * TIME_PERIOD_EQUIVALENCE_x;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum Convert_Reports { SCRIPT_REPORT = 1, STACK_REPORT, ZONE_EQUIV, TIME_EQUIV };

	bool equiv_flag, skim_flag, zone_flag, zone_loc_flag;
	int num_zone, num_trip, tot_trips, tot_errors, group_number;
	int hhold_id, vehicle_id, additional_time, nperson;
	int trip_copy, hhold_copy, pop_copy, veh_copy, max_hh_in, max_veh_in;
	double tot_add, tot_del;

	Trip_File *trip_file;
	Vehicle_File *vehicle_file;
	Household_File *household_file;
	Population_File *population_file;

	Random random_org, random_des, random_tod;
	Time_Step trip_time;
	Integer_List zone_loc;
	Integer_Sort field_map;

	Zone_Equiv zone_equiv;
	File_Group_Array file_group;
	Diurnal_Target_Array diurnal_data;
	Factor_Array factor_data;
	Factor_Data default_factor;

	Matrix_Array ttime_skim;
	Skim_File skim_file;
	Matrix_Data *skim_ptr;

	Zone_Location_Map zone_loc_map;

	//---- methods ----

	void Zone_Location (void);
	void Parking_Check (void);
	void Skim_Processing (void);
	void Factor_Processing (File_Group *group);
	void Time_Processing (File_Group *group);
	void Table_Processing (File_Group *group);
	int  Set_Trips (File_Group *group, int org, int des, int trips, int num, int t1, int t2, int duration);

};
#endif
