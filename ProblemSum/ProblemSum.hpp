//*********************************************************
//	ProblemSum.hpp - summarize problem files
//*********************************************************

#ifndef PROBLEMSUM_HPP
#define PROBLEMSUM_HPP

#include "Demand_Service.hpp"
#include "Problem_Service.hpp"
#include "Arcview_File.hpp"
#include "Problem_File.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Db_Array.hpp"
#include "Problem_Link_File.hpp"

//---------------------------------------------------------
//	ProblemSum - execution class definition
//---------------------------------------------------------

class ProblemSum : public Demand_Service, public Problem_Service
{
public:
	ProblemSum (void);
	virtual ~ProblemSum (void);

	virtual void Execute (void);

protected:

	static char * PROBLEM_FILE;
	static char * PROBLEM_FORMAT;
	static char * TIME_OF_DAY_FORMAT;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * SELECT_LINKS;
	static char * SELECT_PROBLEM_TYPES;
	static char * SELECT_SUBAREA_POLYGON;
	static char * NEW_PROBLEM_TIME_FILE;
	static char * NEW_PROBLEM_TIME_FORMAT;
	static char * NEW_PROBLEM_LINK_FILE;
	static char * NEW_PROBLEM_LINK_FORMAT;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum ProblemSum_Reports { TYPE_REPORT = 1, TIME_REPORT, DENSITY_REPORT };

	int  num_types, num_problems [MAX_PROBLEM], header_code, nproblem;
	bool time_flag, link_flag, type_flag, subarea_flag, location_flag, net_flag;
	bool problem_time_flag, problem_link_flag;

	Problem_File problem_file;

	Problem_Link_File link_file;
	Db_Header time_file;
	
	Arcview_File select_subarea;

	Time_Range times;
	Data_Range links, types;
	
	Db_Data_Array problem_db;

	//---- methods ----

	void Read_Problem (int nfile);

	void Problem_Time (void);
	void Problem_Link (void);

	void Type_Report (void);
	void Type_Header (void);

	void Time_Report (void);
	void Time_Header (void);

	void Density_Report (double min_density);
	void Density_Header (void);
};
#endif
