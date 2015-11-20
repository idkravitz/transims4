//*********************************************************
//	ActGen.hpp - Activity Generator
//*********************************************************

#ifndef ACTGEN_HPP
#define ACTGEN_HPP

#include "Demand_Service.hpp"
#include "Problem_Service.hpp"
#include "Problem_File.hpp"
#include "Integer_Array.hpp"
#include "Household_File.hpp"
#include "Population_File.hpp"
#include "Activity_File.hpp"
#include "User_Program.hpp"
#include "Time_Step.hpp"
#include "Random.hpp"
#include "Db_Array.hpp"
#include "Trip_Sum_Data.hpp"
#include "Veh_Use_Data.hpp"
#include "Trip_Length_Data.hpp"
#include "Act_Problem_Data.hpp"

#include "HHold_Data.hpp"
#include "Survey_HHold.hpp"
#include "Survey_Person.hpp"
#include "Type_Data.hpp"
#include "Weight_Data.hpp"
#include "Purpose_Group.hpp"
#include "Choice_Data.hpp"
#include "Skim_Group.hpp"
#include "Zone_List.hpp"

#undef  household_data
#define household_data		(*((HHold_Array *) household_array))

#undef  location_data
#define location_data		(*((Choice_Array *) location_array))

#undef	zone_data
#define	zone_data			(*((Zone_List_Array *) zone_array))

#define MAX_PURPOSE		100
#define RANGE_SPEED		40
#define MIN_TTIME		300

//---------------------------------------------------------
//	ActGen - execution class definition
//---------------------------------------------------------

class ActGen : public Demand_Service, public Problem_Service
{
public:

	ActGen (void);
	virtual ~ActGen (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

	virtual bool Zone_Processing (Db_File *fh);
	virtual bool Location_Processing (Db_File *fh);
	virtual bool Household_Processing (Db_File *fh);
	virtual bool Population_Processing (Db_File *fh);
	virtual bool Vehicle_Processing (Db_File *fh);

protected:

	static char * HOUSEHOLD_LIST;
	static char * TIME_OF_DAY_FORMAT;
	static char * HOUSEHOLD_TYPE_SCRIPT;
	static char * SURVEY_HOUSEHOLD_FILE;
	static char * SURVEY_HOUSEHOLD_FORMAT;
	static char * SURVEY_HOUSEHOLD_WEIGHTS;
	static char * SURVEY_TYPE_SCRIPT;
	static char * SURVEY_POPULATION_FILE;
	static char * SURVEY_POPULATION_FORMAT;
	static char * SURVEY_ACTIVITY_FILE;
	static char * SURVEY_ACTIVITY_FORMAT;
	static char * NEW_PROBLEM_FILE;
	static char * NEW_PROBLEM_FORMAT;
	static char * NEW_HOUSEHOLD_MATCH_FILE;
	static char * NEW_HOUSEHOLD_PERSON_COUNT;
	static char * NEW_ACTIVITY_PATTERN_FILE;
	static char * NEW_ACTIVITY_PATTERN_FORMAT;
	static char * NEW_TRIP_TIME_FILE;
	static char * NEW_TRIP_DISTANCE_FILE;
	static char * DISTANCE_CALCULATION;
	static char * MAXIMUM_WALK_DISTANCE;
	static char * AVERAGE_TRAVEL_SPEED;
	static char * ADDITIONAL_TRAVEL_TIME;
	static char * TIME_SCHEDULE_VARIANCE;
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
	enum ActGen_Reports { HOUSEHOLD_SCRIPT = 1, HOUSEHOLD_STACK, HOUSEHOLD_TYPE,
		SURVEY_SCRIPT, SURVEY_STACK, SURVEY_TYPE, LOCATION_SCRIPT, LOCATION_STACK,
		LOCATION_DETAIL, TIME_EQUIV, HOUSEHOLD_MATCH, TRIP_LENGTH, TOUR_LENGTH,
		TRIP_PURPOSE, TOUR_PURPOSE, MODE_LENGTH, MODE_PURPOSE, ACT_PROBLEM };

	Distance_Type distance_method;

	int hh_match, per_match, age_match, work_match, gender_match, nhhold, nperson, nwork;
	int num_act_gen, num_problem, survey_hhold, zero_zone, warning_count, num_pattern, max_walk;
	int num_activity, num_tour, num_trip, purpose_number, num_db_files, max_purpose, nhh_list;
	int start_field, end_field, duration_field, purpose_field, mode_field, subtours_field, utility_field;
	int stops1_field, stops2_field, distance1_field, distance2_field, budget1_field, budget2_field;
	int min_fac1_field, max_fac1_field, min_fac2_field, max_fac2_field, max_variance, time_variance;
	int zone1_field, zone2_field, loc1_field, loc2_field, purp1_field, purp2_field;

	bool hhold_type_flag, survey_type_flag, choice_flag, weight_flag, problem_flag, hhlist_flag;
	bool zone_flag, skim_flag, length_flag, tour_flag, warning_flag, dump_flag, dump_list_flag;
	bool trip_purpose_flag, tour_purpose_flag, update_flag, match_flag, mode_flag, mode_purpose_flag;
	bool time_length_flag, distance_length_flag, new_pattern_flag, balance_flag, count_flag;
	bool act_problem_flag;

	int purpose_map [MAX_PURPOSE];
	int avg_speed [MAX_MODE], add_time [MAX_MODE], time_field [MAX_MODE];

	Db_Base **db_files;
	Db_Base tour_info;

	Db_Data_Array location_db;
	Db_Data_Array household_db;
	Db_Data_Array zone_db;

	Household_File survey_hh_file;
	Population_File survey_pop_file;
	Activity_File survey_act_file;
	Activity_File new_pattern_file;

	Db_File hhlist_file;
	Db_File hhold_type_file;
	Db_File survey_type_file;
	Db_File weight_file;
	Db_File match_file;
	Db_File count_file;
	Db_Header balance_file;
	
	Problem_File problem_file;

	Time_Step activity_time;
	Random random, random_time;

	User_Program hhold_type;
	User_Program survey_type;

	Integer_Sort hhold_list, dump_list;

	Type_Array type_data;
	Weight_Array weight_data;
	Survey_HHold_Array survey_hh_data;
	Survey_Person_Array survey_pop_data;
	Activity_Array survey_act_data;

	Activity_Array hhold_act_data;
	Veh_Use_Array veh_use_data;

	Purpose_Group_Array purpose_group;
	Skim_Group_Array skim_group;

	Trip_Sum_Array trip_length, tour_length, trip_purpose, tour_purpose, mode_length, mode_purpose;
	Trip_Length_Array trip_time_length, trip_distance_length;
	Activity_Problem_Array act_problem_data;

	void Household_List (void);
	void Read_Scripts (void);
	void Read_Survey (void);
	void Read_Factors (void);
	void Survey_Weights (void);
	void Survey_Households (void);
	void Survey_Population (void);
	void Survey_Activity (void);
	void Generate_Activity (void);
	void Household_Match (HHold_Data *hh_ptr);
	void Save_Activity (HHold_Data *hh_ptr, Population_Data *pop_ptr, int hhold, int person);
	void Assign_Vehicles (void);
	void Reserve_Vehicle (int first, int last, int start, int end, int driver);
	void Locate_Activity (void);
	void Location_Choice (int start, int arrive, int leave, int end);
	int  Location_Method (Purpose_Group *group_ptr, int loc1, int start, int budget1, int loc2, int end, int budget2, int mode);
	int  Zone_Method (Purpose_Group *group_ptr, int loc1, int start, int budget1, int loc2, int end, int budget2, int mode);
	void Travel_Time (void);
	void Schedule_Activity (void);
	void Write_Activity (void);
	void Write_Pattern (void);
	void Write_Count (void);
	void Activity_Problem (Problem_Type problem, Activity_Data *act_ptr);

	double Exp (double value);
};
#endif
