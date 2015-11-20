//*********************************************************
//	PlanPrep.hpp - Manipulate Plan Files
//*********************************************************

#ifndef PLANPREP_HPP
#define PLANPREP_HPP

#include "Demand_Service.hpp"
#include "Complex_Array.hpp"
#include "Plan_File.hpp"
#include "Random.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Db_Code.hpp"
#include "Index_Array.hpp"
#include "Ext_File.hpp"
#include "Integer_Array.hpp"

#define NUM_TIME_PERIOD	25
#define NUM_CHANGE_BINS	500
#define NUM_PERCENTILES	9

#define TOTAL_TIME_PERIOD	(NUM_TIME_PERIOD-1)

#define TEMP_LABEL		".temp"

//---------------------------------------------------------
//	PlanPrep - execution class definition
//---------------------------------------------------------

class PlanPrep : public Demand_Service
{
public:
	PlanPrep (void);
	virtual ~PlanPrep (void);

	virtual void Execute (void);

protected:

	static char * INPUT_PLAN_FILE;
	static char * INPUT_PLAN_FORMAT;
	static char * INPUT_PLAN_SORT;
	static char * MERGE_PLAN_FILE;
	static char * MERGE_PLAN_FORMAT;
	static char * SUBAREA_PLAN_FILE;
	static char * SUBAREA_PLAN_FORMAT;
	static char * OUTPUT_PLAN_FILE;
	static char * OUTPUT_PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * PLAN_SORT_OPTION;
	static char * PLAN_COMBINE_OPTION;
	static char * MAX_OUTPUT_PLAN_SIZE;
	static char * SELECT_TRAVELER_RANGE;
	static char * SELECT_TIME_PERIODS;
	static char * SELECTION_PERCENTAGE;
	static char * RANDOM_NUMBER_SEED;
	static char * CHECK_PLAN_PARTITIONS;
	static char * UPDATE_PLAN_PARTITIONS;
	static char * HOUSEHOLD_LIST;
	static char * DELETE_HOUSEHOLD_LIST;
	static char * DELETE_ONE_LEG_PLANS;
	static char * FIX_PLAN_FILE;
	static char * MERGE_BY_TRIP;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum PlanPrep_Reports { PATH_CHANGE = 1, TIME_CHANGE, DUMP_PATH_CHANGES, DUMP_TIME_CHANGES,
							FIX_PLAN };

	enum Combine_Type { FILE_COMBINE, MEMORY_COMBINE };

	Plan_File input_plans, merge_plans, subarea_plans, output_plans, **plan_file;
	Ext_File hhlist_file, delete_file;
	Db_File fix_file;

	Complex_Array plan_list;
	Time_Range time_period;
	Integer_Sort delete_list;

	Random random;

	char output_name [FILE_NAME_SIZE];

	bool merge_flag, output_flag, compare_flag, path_flag, sort_flag, combine_flag, trip_flag;
	bool input_sort_flag, select_flag, time_flag, check_flag, partition_flag, fix_flag;
	bool max_size_flag, size_flag, first_out, parts_flag, binary_flag, hhlist_flag;
	bool subarea_flag, subpart_flag, type_flag, hhold_flag, delete_flag, one_leg_flag;

	int replaced, updated, extend, nfile, nout, num_one_leg;
	off_t max_size;
	double percent;

	Sort_Type sort;
	Combine_Type combine;

	int increment;
	int path_changes [NUM_TIME_PERIOD] [NUM_CHANGE_BINS], dump_path;
	int time_changes [NUM_TIME_PERIOD] [NUM_CHANGE_BINS], dump_time;

	static int percent_break [NUM_PERCENTILES];

	typedef struct {
		int traveler;
		int partition;
	} Partition;

	Index_Array partition;
	Data_Range hhold_range;

	typedef struct {
		int number;
		double share;
		int num_old;
		int num_new;
		int *old_list;
		int *new_list;
		int num_in;
		int num_out;
	} Fix_List;

	Data_Array fix_list;

	//---- methods ----

	void Read_HHList (void);
	void Read_Deletes (void);
	void Read_Fix_Plans (void);
	void Sort_Plans (void);
	void Sort_Merge (void);
	void File_Merge (void);
	void Subarea_Merge (void);
	void Write_Plans (void);
	void Copy_Plans (void);
	void File_Combine (void);
	void Partition_Plans (void);
	void Output_Plan (Plan_Data *plan_ptr, bool label = false);
	void Compare (Plan_Data *plan, Plan_Data *base);
	Plan_Data * Fix_Plan (Plan_Data *plan_ptr);

	void Path_Change (void);
	void Path_Change_Header (void);

	void Travel_Time_Change (void);
	void Time_Change_Header (void);

	void Fix_Plan_Report (void);
	void Fix_Plan_Header (void);
};
#endif
