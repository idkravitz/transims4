//*********************************************************
//	DynusTPlan.hpp - Convert Plans to DynusT Format
//*********************************************************

#ifndef DYNUSTPLAN_HPP
#define DYNUSTPLAN_HPP

#include "Demand_Service.hpp"
#include "Plan_File.hpp"
#include "Time_Range.hpp"
#include "Integer_Array.hpp"
#include "Random.hpp"

//---------------------------------------------------------
//	DynusTPlan - execution class definition
//---------------------------------------------------------

class DynusTPlan : public Demand_Service
{
public:
	DynusTPlan (void);
	virtual ~DynusTPlan (void);

	virtual void Execute (void);

protected:

	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * SELECT_TIME_PERIOD;
	static char * OUTPUT_TIME_INCREMENT;
	static char * SELECTION_PERCENTAGE;
	static char * RANDOM_NUMBER_SEED;
	static char * DYNUST_PLAN_DIRECTORY;
	static char * DYNUST_USER_CLASS;

	virtual void Program_Control (void);

private:

	int num_out, start_time, user_class;
	bool type_flag, select_flag;
	double percent;
	char plan_dir [FILE_NAME_SIZE];

	Plan_File plan_file;
	Db_File path_file;
	Db_File veh_file;

	Random random;
	Time_Range time_range;

	Integer_List node_zone;

	//---- methods ----

	void Set_Zone (void);
	void Read_Plan (void);
};
#endif
