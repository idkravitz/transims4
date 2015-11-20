//*********************************************************
//	ArcRider.hpp - Ridership File Display Utility
//*********************************************************

#ifndef ARCRIDER_HPP
#define ARCRIDER_HPP

#include "Demand_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Plan_File.hpp"
#include "Problem_File.hpp"
#include "Offset_Data.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Integer_Array.hpp"
#include "Pointer_Array.hpp"
#include "Stop_Equiv.hpp"
#include "Line_Equiv.hpp"
#include "Veh_Type_Data.hpp"

//---------------------------------------------------------
//	ArcRider - execution class definition
//---------------------------------------------------------

class ArcRider : public Demand_Service
{
public:
	ArcRider (void);
	virtual ~ArcRider (void);

	virtual void Execute (void);

	virtual bool Stop_Processing (Db_File *file);

protected:

	static char * ARCVIEW_LINE_DEMAND_FILE;
	static char * ARCVIEW_LINE_GROUP_FILE;
	static char * ARCVIEW_RIDERSHIP_FILE;
	static char * ARCVIEW_STOP_DEMAND_FILE;
	static char * ARCVIEW_STOP_GROUP_FILE;
	static char * ARCVIEW_RUN_CAPACITY_FILE;
	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_LINKS_x;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_TIME_INCREMENT;
	static char * SELECT_TRANSIT_STOPS;
	static char * SELECT_TRANSIT_ROUTES;
	static char * SELECT_TRANSIT_MODES;
	static char * TRANSIT_STOP_SIDE_OFFSET;
	static char * TRANSIT_DIRECTION_OFFSET;
	static char * MAXIMUM_SHAPE_ANGLE;
	static char * MINIMUM_SHAPE_LENGTH;
	static char * RIDERSHIP_SCALING_FACTOR;
	static char * MINIMUM_RIDERSHIP_VALUE;
	static char * MINIMUM_RIDERSHIP_SIZE;
	static char * MAXIMUM_RIDERSHIP_SIZE;
	static char * SET_WIDTH_USING_RUNS;
	static char * SET_WIDTH_USING_LOAD_FACTOR;
	static char * STOP_EQUIVALENCE_FILE;
	static char * LINE_EQUIVALENCE_FILE;

	virtual void Program_Control (void);

private:

	Arcview_File arcview_line, arcview_sum, arcview_rider, arcview_stop, arcview_group, arcview_cap;

	Time_Step trip_time;	
	Time_Range times;
	Data_Range transit_stops, transit_routes;
	Data_Array links;

	Offset_Array stop_offset;
	Stop_Equiv stop_equiv;	
	Line_Equiv line_equiv;

	Point_Data points;

	int *width_data, num_width, num_time, num_distance, num_access, num_cap;
	int num_line, num_sum, num_rider, num_stop, num_group, max_angle, min_length, min_value, min_load;
	int *load_data, *board_data, *alight_data, *stop_list, *enter_data, *end_data, *start_data;

	double stop_side, route_offset;
	double width_factor, min_width, max_width, load_factor, min_rider, max_rider;

	bool type_flag, line_flag, sum_flag, runs_flag, factor_flag;
	bool rider_flag, load_flag, on_off_flag, demand_flag, group_flag, cap_flag;
	bool mode_flag, travel_modes [REGIONRAIL+1];

	Projection_Service projection;	

	//---- methods ----

	void Setup_Riders (void);
	void Sum_Riders (void);
	void Write_Route (void);
	void Write_Sum (void);
	void Write_Riders (void);
	void Write_Stops (void);
	void Write_Group (void);
	void Write_Capacity (void);
};
#endif


