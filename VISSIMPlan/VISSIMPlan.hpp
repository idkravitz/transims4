//*********************************************************
//	VISSIMPlan.hpp - Convert Plans to VISSIM XML Format
//*********************************************************

#ifndef VISSIMPLAN_HPP
#define VISSIMPLAN_HPP

#include "Demand_Service.hpp"
#include "Plan_File.hpp"
#include "Time_Range.hpp"
#include "Random.hpp"

//---------------------------------------------------------
//	VISSIMPlan - execution class definition
//---------------------------------------------------------

class VISSIMPlan : public Demand_Service
{
public:
	VISSIMPlan (void);
	virtual ~VISSIMPlan (void);

	virtual void Execute (void);

protected:

	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * SELECT_TIME_PERIOD;
	static char * OUTPUT_TIME_INCREMENT;
	static char * SELECTION_PERCENTAGE;
	static char * RANDOM_NUMBER_SEED;
	static char * NEW_VISSIM_XML_FILE;

	virtual void Program_Control (void);

private:

	int num_out;
	bool type_flag, select_flag;
	double percent;

	Plan_File plan_file, drive_plan;
	Db_File xml_file;
	
	FILE *file;

	Random random;
	Time_Range time_range;

	//---- methods ----

	void Read_Plan (void);
};
#endif
