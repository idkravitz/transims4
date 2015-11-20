//*********************************************************
//	NewFormat.hpp - Convert Version 3 Files to Version 4
//*********************************************************

#ifndef NEWFORMAT_HPP
#define NEWFORMAT_HPP

#include "Demand_Service.hpp"
#include "Index_Array.hpp"
#include "Activity_File.hpp"
#include "Plan_File.hpp"
#include "Db_Header.hpp"
#include "Time_Step.hpp"
#include "Projection.hpp"
#include "Projection_Service.hpp"
#include "System_Defines.hpp"

//---------------------------------------------------------
//	NewFormat - execution class definition
//---------------------------------------------------------

class NewFormat : public Demand_Service
{
public:

	NewFormat (void);
	virtual ~NewFormat (void);

	virtual void Execute (void);

protected:

	static char * VERSION3_DRIVER_PLANS;
	static char * VERSION3_POPULATION_FILE;
	static char * VERSION3_ACTIVITY_FILE;
	static char * VERSION3_SURVEY_ACTIVITY;
	static char * TIME_OF_DAY_FORMAT;

	virtual void Program_Control (void);

private:
	bool driver_flag, population_flag, survey_flag, activity_flag, xy_flag, loc_flag;

	Plan_File driver_plan;
	Db_Base pop_file;
	Activity_File activity_file;
	Db_Header survey_file;

	typedef struct {
		int person_id;
		int person_num;
	} Person_Number;

	Index_Array person_number;
	
	Time_Step activity_time;
	
	Projection projection;
	Projection_Service proj_service;

	void Driver_Plan (void);
	void Population (void);
	void Pop_Header (void);
	void Activity (void);
	void Survey_Activity (void);
	int  Closest_Location (XY_Point *xy);
};
#endif
