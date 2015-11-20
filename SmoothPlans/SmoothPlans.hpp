//*********************************************************
//	SmoothPlans.hpp - smooth plans by time of day
//*********************************************************

#ifndef SMOOTHPLANS_HPP
#define SMOOTHPLANS_HPP

#include "Demand_Service.hpp"
#include "Db_Header.hpp"
#include "Plan_File.hpp"
#include "Integer_Array.hpp"
#include "Time_Step.hpp"
#include "Random.hpp"

#include "Data/Travel_Data.hpp"
#include "Data/Period_Index.hpp"

//---------------------------------------------------------
//	SmoothPlans - execution class definition
//---------------------------------------------------------

class SmoothPlans : public Demand_Service
{
public:
	SmoothPlans (void);
	virtual ~SmoothPlans (void);

	virtual void Execute (void);

protected:

	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * LINK_LIST;
	static char * RANDOM_NUMBER_SEED;
	static char * TIME_OF_DAY_FORMAT;
	static char * TIME_INCREMENT;
	static char * MAXIMUM_TIME_SHIFT;
	static char * SMOOTH_GROUP_SIZE;
	static char * PERCENT_MOVED_FORWARD;
	static char * PERCENT_MOVED_BACKWARD;
	static char * NUMBER_OF_ITERATIONS;
	static char * NEW_PLAN_FILE;
	static char * NEW_PLAN_FORMAT;
	static char * NEW_LINK_SUMMARY_FILE;

	virtual void Program_Control (void);

private:
	enum SmoothPlans_Reports { TIME_CHANGE = 1 };

	int naverage, nperiod, increment, max_change, niter, new_time;
	double *distribution, forward, backward;
	bool delay_flag, type_flag, output_flag, summary_flag, turn_flag, change_flag, link_flag;

	Plan_File plan_file;
	Db_File list_file;
	Db_File summary_file;

	Integer_Sort link_list;

	Period_Index_Array period_index;
	Travel_Array travel_data;

	Time_Step time_step;
	Random random;

	Plan_File new_plan_file;

	//---- methods ----

	void Read_List (void);
	void Read_Plans (void);
	void Smooth_Link (int iter, int link);
	void Write_Results (void);
	void Copy_Plans (void);
	void Change_Report (void);
};
#endif
