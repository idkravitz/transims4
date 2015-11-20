//*********************************************************
//	AdjustPlans.hpp - adjust plans based on counts
//*********************************************************

#ifndef ADJUSTPLANS_HPP
#define ADJUSTPLANS_HPP

#include "Demand_Service.hpp"
#include "Db_Header.hpp"
#include "Plan_File.hpp"
#include "Vehicle_File.hpp"
#include "Random.hpp"
#include "Ext_File.hpp"
#include "Link_Equiv.hpp"

#include "Data/Count_File.hpp"
#include "Data/Count_Data.hpp"
#include "Data/Travel_Data.hpp"

//---------------------------------------------------------
//	AdjustPlans - execution class definition
//---------------------------------------------------------

class AdjustPlans : public Demand_Service
{
public:
	AdjustPlans (void);
	virtual ~AdjustPlans (void);

	virtual void Execute (void);

protected:

	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * TRAFFIC_COUNT_FILE;
	static char * TRAFFIC_COUNT_FORMAT;
	static char * LINK_EQUIVALENCE_FILE;
	static char * COUNT_SCALING_FACTOR;
	static char * RANDOM_NUMBER_SEED;
	static char * MAXIMUM_PERCENT_ERROR;
	static char * MAXIMUM_ITERATIONS;
	static char * MAXIMUM_REPLICATION;
	static char * PROHIBIT_DELETIONS;
	static char * NEW_PLAN_FILE;
	static char * NEW_PLAN_FORMAT;
	static char * NEW_HOUSEHOLD_LIST;
	static char * CONVERGENCE_FILE;
	static char * CONVERGENCE_FORMAT;

	virtual void Program_Control (void);

private:
	enum AdjustPlans_Reports {LINK_EQUIV = 1};

	double max_error, factor;
	int max_iter, max_copy, num_deleted, num_replicated, num_added;
	int max_traveler, max_vehicle, max_household;
	bool delay_flag, turn_flag, type_flag, output_flag, converge_flag, hhold_flag, equiv_flag;
	bool delete_flag;

	Plan_File plan_file;
	Count_File count_file;

	Count_Array count_data;
	Travel_Array travel_data;

	Link_Equiv link_equiv;
	Random random;

	Plan_File new_plan_file;

	Ext_File hhold_file;
	Db_Header converge_file;

	//---- methods ----

	void Read_Counts (void);
	void Read_Plans (void);
	bool Adjust_Weights (void);
	void Write_Results (void);
	void Copy_Plans (void);
	void Copy_Vehicles (void);
};
#endif
