//*********************************************************
//	CheckSurvey.hpp - Check the Household Travel Survey
//*********************************************************

#ifndef CHECKSURVEY_HPP
#define CHECKSURVEY_HPP

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
#include "Trip_Sum_Data.hpp"
#include "Veh_Use_Data.hpp"

//---------------------------------------------------------
//	CheckSurvey - execution class definition
//---------------------------------------------------------

class CheckSurvey : public Demand_Service, public Problem_Service
{
public:

	CheckSurvey (void);
	virtual ~CheckSurvey (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

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
	static char * NEW_SURVEY_ACTIVITY_FILE;
	static char * NEW_SURVEY_ACTIVITY_FORMAT;
	static char * NEW_PROBLEM_FILE;
	static char * NEW_PROBLEM_FORMAT;
	static char * PURPOSE_ANCHOR_ORDER;
	static char * AVERAGE_TRAVEL_SPEED;
	static char * ADDITIONAL_TRAVEL_TIME;
	static char * MAXIMUM_WALK_TIME;
	static char * MAXIMUM_TRIP_TIME;
	static char * RANDOM_NUMBER_SEED;

	virtual void Program_Control (void);

private:
	enum CheckSurvey_Reports { SURVEY_SCRIPT = 1, SURVEY_STACK, SURVEY_TYPE, 
		TRIP_LENGTH, TOUR_LENGTH, TRIP_PURPOSE, TOUR_PURPOSE, MODE_LENGTH, MODE_PURPOSE };

	int nhhold, nperson, nwork, max_walk, max_trip;
	int num_act_gen, num_problem, nproblem, num_new;
	int num_activity, num_tour, num_trip, nhh_list;
	bool survey_type_flag, weight_flag, problem_flag, hhlist_flag, new_act_flag;
	bool length_flag, tour_flag;
	bool trip_purpose_flag, tour_purpose_flag, mode_flag, mode_purpose_flag;

	int avg_speed [MAX_MODE], add_time [MAX_MODE];

	Household_File survey_hh_file;
	Population_File survey_pop_file;
	Activity_File survey_act_file, new_act_file;

	Db_File hhlist_file;
	Db_File survey_type_file;
	Db_File weight_file;
	
	Problem_File problem_file;

	Time_Step activity_time;
	Random random;

	User_Program survey_type;

	Integer_Sort hhold_list;
	Integer_List anchor_list;

	Household_Array survey_hh_data;
	Population_Array survey_pop_data;
	Activity_Array hhold_act_data;
	Veh_Use_Array veh_use_data;

	Trip_Sum_Array trip_length, tour_length, trip_purpose, tour_purpose, mode_length, mode_purpose;

	void Household_List (void);
	void Type_Script (void);
	void Survey_Households (void);
	void Survey_Weights (void);
	void Survey_Population (void);
	void Survey_Activity (void);
	void Assign_Vehicles (Household_Data *hh_ptr);
	void Reserve_Vehicle (int first, int last, int start, int end, int driver);
	void Locate_Activity (void);
	void Travel_Time (int weight);
	void Write_Activity (int weight);
	void Activity_Problem (Problem_Type problem, Activity_Data *act_ptr, int weight);

	void Type_Report (void);
};
#endif
