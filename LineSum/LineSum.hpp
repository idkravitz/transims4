//*********************************************************
//	LineSum.hpp - Summarize Transit Line Performance Data
//*********************************************************

#ifndef LINESUM_HPP
#define LINESUM_HPP

#include "Demand_Service.hpp"
#include "Time_Range.hpp"
#include "Rider_Data.hpp"
#include "Data_Range.hpp"
#include "Offset_Data.hpp"
#include "Stop_Equiv.hpp"
#include "Line_Equiv.hpp"
#include "Link_Equiv.hpp"
#include "Db_Header.hpp"
#include "Veh_Type_Data.hpp"

#include "Data/Select_Set.hpp"
#include "Data/Route_Legs.hpp"

//---------------------------------------------------------
//	LineSum - execution class definition
//---------------------------------------------------------

class LineSum : public Demand_Service
{
public:
	LineSum (void);
	virtual ~LineSum (void);

	virtual void Execute (void);

protected:

	static char * SELECTION_DESCRIPTION;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * TIME_PERIOD_METHOD;
	static char * SELECT_SUBAREA_POLYGON;
	static char * SELECT_TRANSIT_MODES;
	static char * SELECT_TRANSIT_ROUTES;
	static char * SELECTION_DESCRIPTION_x;
	static char * SUMMARY_TIME_PERIODS_x;
	static char * SUMMARY_TIME_INCREMENT_x;
	static char * TIME_PERIOD_METHOD_x;
	static char * SELECT_SUBAREA_POLYGON_x;
	static char * SELECT_TRANSIT_MODES_x;
	static char * SELECT_TRANSIT_ROUTES_x;
	static char * STOP_EQUIVALENCE_FILE;
	static char * LINE_EQUIVALENCE_FILE;
	static char * LINK_EQUIVALENCE_FILE;
	static char * NEW_RUN_SCHEDULE_FILE;
	static char * NEW_RUN_CAPACITY_FILE;

	virtual void Program_Control (void);
	
	virtual bool Stop_Processing (Db_File *file);
	virtual void Page_Header (void);

private:
	enum LineSum_Reports { LINE_RIDER = 1, LINE_SUM, PERF_REPORT, CAP_REPORT, 
		LINE_RIDER_x, LINE_SUM_x, PERF_REPORT_x, CAP_REPORT_x, STOP_GROUP, STOP_DETAIL, 
		STOP_STOP, STOP_EQUIV, GROUP_SUM, GROUP_PERF, LINE_EQUIV, LINK_GROUP, LINK_EQUIV};

	enum Time_Period_Method { RUN_START = 1, RUN_MID, RUN_END, SCHED_START, SCHED_MID, SCHED_END };

	int group_num;
	bool offset_flag, distance_flag, run_flag, cap_flag;

	Select_Set_Array select_set;
	Select_Set *set_ptr;

	Stop_Equiv stop_equiv;	
	Line_Equiv line_equiv;
	Link_Equiv link_equiv;

	Db_Header run_file, cap_file;

	Offset_Array stop_offset;
	Point_Data points;

	//---- methods ----

	void Build_Legs (void);
	void Write_Capacity (void);

	void Line_Rider_Report (int number);
	void Line_Rider_Header (void);

	void Line_Sum_Report (int number);
	void Line_Sum_Header (void);

	void Performance_Report (int number);
	void Performance_Header (void);

	void Capacity_Report (int number);
	void Capacity_Header (void);

	void Stop_Group_Report (void);
	void Stop_Group_Header (void);

	void Stop_Detail_Report (void);
	void Stop_Detail_Header (void);

	void Stop_Stop_Report (void);
	void Stop_Stop_Header (void);

	void Line_Group_Report (void);
	void Line_Group_Header (void);

	void Group_Performance_Report (void);
	void Group_Performance_Header (void);

	void Link_Group_Report (void);
	void Link_Group_Header (void);

	void Run_Schedule (void);
};
#endif
