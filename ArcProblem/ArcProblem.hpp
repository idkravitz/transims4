//*********************************************************
//	ArcProblem.hpp - ArcView Problem Files
//*********************************************************

#ifndef ARCPROBLEM_HPP
#define ARCPROBLEM_HPP

#include "Demand_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Time_Range.hpp"
#include "Utility.hpp"
#include "Problem_Service.hpp"
#include "Integer_Array.hpp"
#include "Problem_File.hpp"

//---------------------------------------------------------
//	ArcProblem - execution class definition
//---------------------------------------------------------

class ArcProblem : public Demand_Service, public Problem_Service
{
public:
	ArcProblem (void);
	virtual ~ArcProblem (void);

	virtual void Execute (void);

protected:
	static char * PROBLEM_FILE;
	static char * PROBLEM_FORMAT;
	static char * ARCVIEW_PROBLEM_FILE;
	static char * LANE_WIDTH;
	static char * CENTER_ONEWAY_LINKS;
	static char * DRAW_VEHICLE_SHAPES;
	static char * DRAW_TRIP_OD_LINKS;
	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_TIME_INCREMENT;
	static char * SELECT_LINKS;
	static char * SELECT_PROBLEM_TYPES;
	static char * SELECT_SUBAREA_POLYGON;

	virtual void Program_Control (void);

private:
	Problem_File problem_file;

	Arcview_File arcview_problem;
	Arcview_File select_subarea;

	Point_Data points;

	double lane_width;

	bool center_flag, link_flag, type_flag, time_flag, shape_flag, subarea_flag, location_flag, od_flag;

	Data_Range links, types;

	Time_Range times;

	Projection_Service projection;	

	char shapename [FILE_NAME_SIZE];

	//---- methods ----

	void Read_Problem (void);
};
#endif


