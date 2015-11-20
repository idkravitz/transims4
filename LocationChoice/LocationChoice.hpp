//*********************************************************
//	LocationChoice.hpp - Activity Location Choice
//*********************************************************

#ifndef LOCATIONCHOICE_HPP
#define LOCATIONCHOICE_HPP

#include "Demand_Service.hpp"
#include "Problem_Service.hpp"
#include "Problem_File.hpp"
#include "Integer_Array.hpp"
#include "Household_File.hpp"
#include "Activity_File.hpp"
#include "User_Program.hpp"
#include "Time_Step.hpp"
#include "Random.hpp"
#include "Db_Array.hpp"
#include "Trip_Sum_Data.hpp"
#include "Trip_Length_Data.hpp"
#include "Act_Problem_Data.hpp"

#include "Purpose_Group.hpp"
#include "Choice_Data.hpp"
#include "Skim_Group.hpp"
#include "Zone_List.hpp"

#undef  location_data
#define location_data		(*((Choice_Array *) location_array))

#undef	zone_data
#define	zone_data			(*((Zone_List_Array *) zone_array))

#define MAX_PURPOSE		100
#define RANGE_SPEED		40
#define MIN_TTIME		300

//---------------------------------------------------------
//	LocationChoice - execution class definition
//---------------------------------------------------------

class LocationChoice : public Demand_Service, public Problem_Service
{
public:

	LocationChoice (void);
	virtual ~LocationChoice (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

	virtual bool Zone_Processing (Db_File *fh);
	virtual bool Location_Processing (Db_File *fh);
	virtual bool Household_Processing (Db_File *fh);

protected:

	static char * HOUSEHOLD_LIST;
	static char * ACTIVITY_PATTERN_FILE;
	static char * ACTIVITY_PATTERN_FORMAT;
	static char * TIME_OF_DAY_FORMAT;
	static char * NEW_PROBLEM_FILE;
	static char * NEW_PROBLEM_FORMAT;
	static char * NEW_TRIP_TIME_FILE;
	static char * NEW_TRIP_DISTANCE_FILE;
	static char * DISTANCE_CALCULATION;
	static char * MAXIMUM_WALK_DISTANCE;
	static char * AVERAGE_TRAVEL_SPEED;
	static char * ADDITIONAL_TRAVEL_TIME;
	static char * RANDOM_NUMBER_SEED;
	static char * ACTIVITY_PURPOSE_RANGE_x;
	static char * ACTIVITY_ANCHOR_FLAG_x;
	static char * SCHEDULE_CONSTRAINT_x;
	static char * ZONE_BASED_METHOD_x;
	static char * LOCATION_CHOICE_SCRIPT_x;
	static char * ZONE_WEIGHT_FIELD_x;
	static char * ZONE_WEIGHT_FACTOR_x;
	static char * BALANCING_FACTOR_FIELD_x;
	static char * LOCATION_WEIGHT_FIELD_x;
	static char * LOCATION_WEIGHT_FACTOR_x;
	static char * SKIM_TIME_FIELD_x;
	static char * MODE_DISTANCE_FACTORS_x;
	static char * MODE_TIME_FACTORS_x;
	static char * ZONE_SKIM_FILE_x;
	static char * ZONE_SKIM_FORMAT_x;
	static char * TIME_PERIOD_EQUIVALENCE_x;
	static char * SKIM_MEMORY_FACTOR_x;
	static char * TRAVEL_TIME_FIELD_MODE_x;
	static char * BALANCING_FACTOR_FILE;
	static char * BALANCING_FACTOR_FORMAT;

	virtual void Program_Control (void);

private:
	enum LocationChoice_Reports { LOCATION_SCRIPT = 1, LOCATION_STACK,
		LOCATION_DETAIL, TIME_EQUIV, TRIP_LENGTH, TOUR_LENGTH,
		TRIP_PURPOSE, TOUR_PURPOSE, MODE_LENGTH, MODE_PURPOSE, ACT_PROBLEM };

	Distance_Type distance_method;

	int num_act_gen, num_problem, zero_zone, warning_count, max_walk;
	int num_activity, num_tour, num_trip, purpose_number, num_db_files, max_purpose, nhh_list;
	int start_field, end_field, duration_field, purpose_field, mode_field, subtours_field, utility_field;
	int stops1_field, stops2_field, distance1_field, distance2_field, budget1_field, budget2_field;
	int min_fac1_field, max_fac1_field, min_fac2_field, max_fac2_field;
	int zone1_field, zone2_field, loc1_field, loc2_field, purp1_field, purp2_field;

	bool choice_flag, problem_flag, hhlist_flag, hhold_flag;
	bool zone_flag, skim_flag, length_flag, tour_flag, warning_flag, dump_flag, dump_list_flag;
	bool trip_purpose_flag, tour_purpose_flag, mode_flag, mode_purpose_flag, act_problem_flag;
	bool time_length_flag, distance_length_flag, update_flag, balance_flag;

	int purpose_map [MAX_PURPOSE];
	int avg_speed [MAX_MODE], add_time [MAX_MODE], time_field [MAX_MODE];

	Db_Base **db_files;
	Db_Base tour_info;
	Db_Header balance_file;

	Db_Data_Array location_db;
	Db_Data_Array household_db;
	Db_Data_Array zone_db;

	Db_File hhlist_file;
	Activity_File pattern_file;
	
	Problem_File problem_file;

	Time_Step activity_time;
	Random random;

	Integer_Sort hhold_list, dump_list;

	Activity_Array hhold_act_data;

	Purpose_Group_Array purpose_group;
	Skim_Group_Array skim_group;

	Trip_Sum_Array trip_length, tour_length, trip_purpose, tour_purpose, mode_length, mode_purpose;
	Trip_Length_Array trip_time_length, trip_distance_length;
	Activity_Problem_Array act_problem_data;

	void Read_Scripts (void);
	void Read_Factors (void);
	void Household_List (void);
	void Read_Activity (void);
	void Locate_Activity (void);
	void Set_Location (int start, int arrive, int leave, int end);
	int  Location_Method (Purpose_Group *group_ptr, int loc1, int start, int budget1, int loc2, int end, int budget2, int mode);
	int  Zone_Method (Purpose_Group *group_ptr, int loc1, int start, int budget1, int loc2, int end, int budget2, int mode);
	void Travel_Time (void);
	void Schedule_Activity (void);
	void Write_Activity (void);
	void Activity_Problem (Problem_Type problem, Activity_Data *act_ptr);

	double Exp (double value);
};
#endif
