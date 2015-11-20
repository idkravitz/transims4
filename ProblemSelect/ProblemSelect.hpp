//*********************************************************
//	ProblemSelect.hpp - Select Households for Re-Routing
//*********************************************************

#ifndef PROBLEMSELECT_HPP
#define PROBLEMSELECT_HPP

#include "Demand_Service.hpp"
#include "Problem_Service.hpp"
#include "Arcview_File.hpp"
#include "Integer_Array.hpp"
#include "Problem_File.hpp"
#include "Random.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Ext_File.hpp"

//---------------------------------------------------------
//	ProblemSelect - execution class definition
//---------------------------------------------------------

class ProblemSelect : public Demand_Service, public Problem_Service
{
public:
	ProblemSelect (void);
	virtual ~ProblemSelect (void);

	virtual void Execute (void);

protected:

	static char * PROBLEM_FILE;
	static char * PROBLEM_FORMAT;
	static char * HOUSEHOLD_LIST;
	static char * NEW_HOUSEHOLD_LIST;
	static char * SELECT_LINKS;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_PROBLEM_TYPES;
	static char * SELECT_SUBAREA_POLYGON;
	static char * SELECTION_PERCENTAGE;
	static char * MAXIMUM_PERCENT_SELECTED;
	static char * RANDOM_NUMBER_SEED;
	static char * TIME_OF_DAY_FORMAT;

	virtual void Program_Control (void);

private:
	int nproblem, num_select, nhhold, nselect;
	double percent, max_percent;
	bool select_flag, time_flag, link_flag, type_flag, subarea_flag, hh_flag;

	char hhold_name [FILE_NAME_SIZE];

	Ext_File hhold_file, new_hhold_file;
	Problem_File problem_file;
	
	Arcview_File select_subarea;

	Random random;
	Time_Range times;
	Data_Range links, types;
	Integer_Sort hhold_list, hh_list;

	//---- methods ----

	void Read_Problem (void);
	void Read_Household (void);

};
#endif
