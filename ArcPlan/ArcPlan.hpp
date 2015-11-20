//*********************************************************
//	ArcPlan.hpp - Plan File Display Utility
//*********************************************************

#ifndef ARCPLAN_HPP
#define ARCPLAN_HPP

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
#include "Random.hpp"

//---------------------------------------------------------
//	ArcPlan - execution class definition
//---------------------------------------------------------

class ArcPlan : public Demand_Service
{
public:
	ArcPlan (void);
	virtual ~ArcPlan (void);

	virtual void Execute (void);

	virtual bool Parking_Processing (Db_File *file);
	virtual bool Location_Processing (Db_File *file);
	virtual bool Stop_Processing (Db_File *file);

protected:
	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * PROBLEM_FILE;
	static char * PROBLEM_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * HOUSEHOLD_LIST;
	static char * ARCVIEW_PLAN_FILE;
	static char * ARCVIEW_PROBLEM_FILE;
	static char * ARCVIEW_BANDWIDTH_FILE;
	static char * ARCVIEW_TIME_CONTOUR;
	static char * ARCVIEW_DISTANCE_CONTOUR;
	static char * ARCVIEW_ACCESSIBILITY_FILE;
	static char * ARCVIEW_RIDERSHIP_FILE;
	static char * ARCVIEW_STOP_DEMAND_FILE;
	static char * ARCVIEW_STOP_GROUP_FILE;
	static char * ARCVIEW_PARKING_DEMAND_FILE;
	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_TRAVELERS;
	static char * SELECT_NODES_x;
	static char * SELECT_LINKS_x;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_LOCATIONS;
	static char * SELECT_PARKING_LOTS;
	static char * SELECT_TRANSIT_STOPS;
	static char * SELECT_TRANSIT_ROUTES;
	static char * SELECT_TRANSIT_MODES;
	static char * SELECT_PROBLEM_TYPES;
	static char * SELECT_RANDOM_PERCENTAGE;
	static char * RANDOM_NUMBER_SEED;
	static char * LINK_DIRECTION_OFFSET;
	static char * PARKING_SIDE_OFFSET;
	static char * ACTIVITY_LOCATION_SIDE_OFFSET;
	static char * TRANSIT_STOP_SIDE_OFFSET;
	static char * TRANSIT_DIRECTION_OFFSET;
	static char * BANDWIDTH_SCALING_FACTOR;
	static char * MINIMUM_BANDWIDTH_VALUE;
	static char * MINIMUM_BANDWIDTH_SIZE;
	static char * MAXIMUM_BANDWIDTH_SIZE;
	static char * MAXIMUM_SHAPE_ANGLE;
	static char * MINIMUM_SHAPE_LENGTH;
	static char * CONTOUR_TIME_INCREMENTS;
	static char * CONTOUR_DISTANCE_INCREMENTS;
	static char * RIDERSHIP_SCALING_FACTOR;
	static char * MINIMUM_RIDERSHIP_VALUE;
	static char * MINIMUM_RIDERSHIP_SIZE;
	static char * MAXIMUM_RIDERSHIP_SIZE;
	static char * STOP_EQUIVALENCE_FILE;

	virtual void Program_Control (void);

private:
	Db_Header route_header, route_nodes;

	Arcview_File arcview_plan, arcview_problem, arcview_access;
	Arcview_File arcview_width, arcview_time, arcview_distance;
	Arcview_File arcview_rider, arcview_stop, arcview_group, arcview_parking;

	Plan_File plan_file;
	Problem_File problem_file;
	Ext_File hhlist_file;

	Integer_Sort problem_list, hhold_list;

	Time_Step trip_time;	
	Time_Range times;
	Data_Range travelers, locations, parking_lots, transit_stops, transit_routes;
	Data_Array nodes, links;
	Integer_Sort time_points, distance_points;

	Offset_Array parking_offset;
	Offset_Array location_offset;
	Offset_Array stop_offset;
	Stop_Equiv stop_equiv;	

	Random random;

	Point_Data points;

	int num_out, num_problem, max_problem, *width_data, num_width, num_time, num_distance, num_access;
	int num_rider, num_stop, num_group, max_angle, min_length, min_value, min_load, num_parking;
	int *load_data, *board_data, *alight_data, *stop_list, *parking_out, *parking_in;
	int traveler_field, trip_field, leg_field, time_field, duration_field;
	int cost_field, impedance_field, mode_field, mode_id_field;

	double link_offset, parking_side, location_side, stop_side, route_offset;
	double width_factor, min_width, max_width, load_factor, min_rider, max_rider, random_share;

	bool route_flag, driver_flag, stop_flag, type_flag, problem_flag, *problem_type, problem_out;
	bool plan_flag, hhlist_flag, width_flag, time_flag, distance_flag, *link_flag, turn_flag;
	bool access_flag, rider_flag, load_flag, on_off_flag, demand_flag, group_flag, parking_flag;
	bool mode_flag, travel_modes [REGIONRAIL+1], random_flag;

	Projection_Service projection;	

	//---- methods ----

	void Read_HHList (void);
	void Read_Problem (void);
	void Read_Plan (void);
	void Write_Route (Offset_Data *org, Offset_Data *des);
	void Write_Node_Path (Offset_Data *org, Offset_Data *des, bool veh_flag = true);
	void Write_Link_Path (Offset_Data *org, Offset_Data *des, bool veh_flag = true);
	void Sum_Volume (Offset_Data *org, Offset_Data *des);
	void Write_Contour (Offset_Data *org);
	void Sum_Access (Offset_Data *org, Offset_Data *des, int *ttime, int *distance);
	void Write_Bandwidth (void);
	void Setup_Riders (void);
	void Sum_Riders (Offset_Data *org, Offset_Data *des);
	void Write_Riders (void);
	void Write_Stops (void);
	void Write_Group (void);
	void Write_Parking (void);
};
#endif


