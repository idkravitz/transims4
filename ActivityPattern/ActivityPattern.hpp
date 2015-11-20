//*********************************************************
//	ActivityPattern.hpp - Activity Pattern Generation
//*********************************************************

#ifndef ACTIVITYPATTERN_HPP
#define ACTIVITYPATTERN_HPP

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
#include "Veh_Use_Data.hpp"

#include "HHold_Data.hpp"
#include "Survey_HHold.hpp"
#include "Survey_Person.hpp"
#include "Type_Data.hpp"
#include "Weight_Data.hpp"

#undef  household_data
#define household_data		(*((HHold_Array *) household_array))

//---------------------------------------------------------
//	ActivityPattern - execution class definition
//---------------------------------------------------------

class ActivityPattern : public Demand_Service, public Problem_Service
{
public:

	ActivityPattern (void);
	virtual ~ActivityPattern (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

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
	static char * ACTIVITY_PATTERN_FILE;
	static char * ACTIVITY_PATTERN_FORMAT;
	static char * NEW_PROBLEM_FILE;
	static char * NEW_PROBLEM_FORMAT;
	static char * NEW_HOUSEHOLD_MATCH_FILE;
	static char * NEW_HOUSEHOLD_PERSON_COUNT;
	static char * NEW_ACTIVITY_PATTERN_FILE;
	static char * NEW_ACTIVITY_PATTERN_FORMAT;
	static char * TIME_SCHEDULE_VARIANCE;
	static char * RANDOM_NUMBER_SEED;

	virtual void Program_Control (void);

private:
	enum ActivityPattern_Reports { HOUSEHOLD_SCRIPT = 1, HOUSEHOLD_STACK, 
		HOUSEHOLD_TYPE,	SURVEY_SCRIPT, SURVEY_STACK, SURVEY_TYPE, HOUSEHOLD_MATCH };

	int hh_match, per_match, age_match, work_match, gender_match, nhhold, nperson, nwork;
	int num_act_gen, num_problem, survey_hhold, warning_count, max_variance, time_variance;
	int num_activity, num_tour, num_trip, num_db_files, nhh_list;

	bool hhold_type_flag, survey_type_flag, weight_flag, problem_flag, hhlist_flag;
	bool trip_purpose_flag, tour_purpose_flag, update_flag, match_flag, count_flag;

	Household_File survey_hh_file;
	Population_File survey_pop_file;
	Activity_File survey_act_file;

	Db_File hhlist_file;
	Db_File hhold_type_file;
	Db_File survey_type_file;
	Db_File weight_file;
	Db_File match_file;
	Db_File count_file;

	Activity_File pattern_file, new_pattern_file;
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

	void Household_List (void);
	void Read_Scripts (void);
	void Read_Survey (void);
	void Survey_Weights (void);
	void Survey_Households (void);
	void Survey_Population (void);
	void Survey_Activity (void);
	void Generate_Activity (void);
	void Household_Match (HHold_Data *hh_ptr);
	void Save_Activity (HHold_Data *hh_ptr, Population_Data *pop_ptr, int hhold, int person);
	void Assign_Vehicles (void);
	void Reserve_Vehicle (int first, int last, int start, int end, int driver);
	void Write_Pattern (void);
	void Write_Count (void);
	void Activity_Problem (Problem_Type problem, Activity_Data *act_ptr);
};
#endif
