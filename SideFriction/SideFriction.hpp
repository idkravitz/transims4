//*********************************************************
//	SideFriction.hpp - Estimate Side Friction Delays
//*********************************************************

#ifndef SIDEFRICTION_HPP
#define SIDEFRICTION_HPP

#include "Demand_Service.hpp"
#include "Time_Range.hpp"
#include "Arcview_File.hpp"
#include "Data_Range.hpp"
#include "Lane_Use_File.hpp"
#include "Offset_Data.hpp"

//---------------------------------------------------------
//	SideFriction - execution class definition
//---------------------------------------------------------

class SideFriction : public Demand_Service
{
public:
	SideFriction (void);
	virtual ~SideFriction (void);

	virtual void Execute (void);

protected:

	static char * TIME_OF_DAY_FORMAT;
	static char * TRANSIT_TIME_PERIODS;
	static char * TRANSIT_LANE_BLOCK_TIMES;
	static char * TRANSIT_LANE_BLOCK_LENGTH;
	static char * SELECT_TRANSIT_MODES;
	static char * SELECT_TRANSIT_ROUTES;
	static char * SELECT_SUBAREA_POLYGON;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

	virtual bool Lane_Use_Processing (Db_File *file);
	virtual bool Stop_Processing (Db_File *file);

private:
	enum SideFriction_Reports { DELAY_SUM = 1 };

	int num_use, new_use, stop_length, *stop_times;
	bool subarea_flag, route_flag, select_modes [REGIONRAIL+1];

	Arcview_File select_subarea;
	Time_Range transit_periods;
	Data_Range select_routes;

	Time_Step time_step;

	Offset_Array stop_offset;
	Point_Data points;
	
	Lane_Use_File *new_file;

	//---- methods ----

	void Stop_Delay (void);

	//void Delay_Sum_Report (void);
	//void Delay_Sum_Header (void);
};
#endif
