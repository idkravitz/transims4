//*********************************************************
//	Router.hpp - Network Path Builder
//*********************************************************

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Demand_Service.hpp"
#include "Problem_Service.hpp"
#include "Random.hpp"
#include "Time_Step.hpp"
#include "Integer_Array.hpp"
#include "Equation.hpp"
#include "User_Program.hpp"
#include "List_Data.hpp"
#include "Trip_Data.hpp"

#include "Plan_File.hpp"
#include "Activity_File.hpp"
#include "Trip_File.hpp"
#include "Problem_File.hpp"
#include "HHRec_File.hpp"
#include "Db_File.hpp"

#include "Arcview_File.hpp"

#include "Time_Range.hpp"
#include "Data_Range.hpp"

#include "Data/Path_Data.hpp"
#include "Data/Trip_End_Data.hpp"
#include "Data/TOD_Turn_Data.hpp"
#include "Data/Park_Ride_Data.hpp"
#include "Data/Route_Stop_Data.hpp"
#include "Data/Household_Type.hpp"

#define MAX_PATHS	10

//---------------------------------------------------------
//	Router - execution class definition
//---------------------------------------------------------

class Router : public Demand_Service, public Problem_Service
{
public:

	Router (void);
	virtual ~Router (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:

	static char * HOUSEHOLD_LIST;
	static char * HOUSEHOLD_RECORD_FILE;
	static char * PARKING_PENALTY_FILE;
	static char * TRANSIT_PENALTY_FILE;
	static char * TIME_OF_DAY_FORMAT;
	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NEW_PLAN_FILE;
	static char * NEW_PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * ROUTE_SELECTED_MODES;
	static char * ROUTE_SELECTED_PURPOSES;
	static char * ROUTE_WITH_SPECIFIED_MODE;
	static char * ROUTE_FROM_SPECIFIED_LOCATIONS;
	static char * ROUTE_TO_SPECIFIED_LOCATIONS;
	static char * ROUTE_AT_SPECIFIED_TIMES;
	static char * ROUTE_BY_TIME_INCREMENT;
	static char * UPDATE_PLAN_RECORDS;
	static char * PRINT_UPDATE_WARNINGS;
	static char * IGNORE_VEHICLE_ID;
	static char * LIMIT_PARKING_ACCESS;
	static char * WALK_PATH_DETAILS;
	static char * ADJUST_ACTIVITY_SCHEDULE;
	static char * IGNORE_ACTIVITY_DURATIONS;
	static char * IGNORE_TIME_CONSTRAINTS;
	static char * END_TIME_CONSTRAINT;
	static char * IGNORE_ROUTING_PROBLEMS;
	static char * INTERPOLATE_LINK_DELAYS;
	static char * PERCENT_RANDOM_IMPEDANCE;
	static char * RANDOM_NUMBER_SEED;
	static char * HOUSEHOLD_TYPE_SCRIPT;
	static char * WALK_SPEED;
	static char * BICYCLE_SPEED;
	static char * WALK_TIME_VALUE;
	static char * BICYCLE_TIME_VALUE;
	static char * FIRST_WAIT_VALUE;
	static char * TRANSFER_WAIT_VALUE;
	static char * VEHICLE_TIME_VALUE;
	static char * DISTANCE_VALUE;
	static char * COST_VALUE;
	static char * LEFT_TURN_PENALTY;
	static char * RIGHT_TURN_PENALTY;
	static char * U_TURN_PENALTY;
	static char * TRANSFER_PENALTY;
	static char * STOP_WAITING_PENALTY;
	static char * STATION_WAITING_PENALTY;
	static char * BUS_BIAS_FACTOR;
	static char * BUS_BIAS_CONSTANT;
	static char * RAIL_BIAS_FACTOR;
	static char * RAIL_BIAS_CONSTANT;
	static char * MAX_WALK_DISTANCE;
	static char * MAX_BICYCLE_DISTANCE;
	static char * MAX_WAIT_TIME;
	static char * MIN_WAIT_TIME;
	static char * MAX_NUMBER_OF_TRANSFERS;
	static char * MAX_NUMBER_OF_PATHS;
	static char * MAX_PARK_RIDE_PERCENTAGE;
	static char * MAX_KISS_RIDE_PERCENTAGE;
	static char * KISS_RIDE_TIME_FACTOR;
	static char * KISS_RIDE_STOP_TYPES;
	static char * MAX_KISS_RIDE_DROPOFF_WALK;
	static char * MAX_LEGS_PER_PATH;
	static char * ADD_WAIT_TO_TRANSIT_LEG;
	static char * FARE_CLASS_DISTRIBUTION;
	static char * PARKING_HOURS_BY_PURPOSE;
	static char * LOCAL_ACCESS_DISTANCE;
	static char * LOCAL_FACILITY_TYPE;
	static char * MAX_CIRCUITY_RATIO;
	static char * MIN_CIRCUITY_DISTANCE;
	static char * MAX_CIRCUITY_DISTANCE;
	static char * LINK_DELAY_UPDATE_RATE;
	static char * LINK_DELAY_VOL_FACTOR;
	static char * LINK_DELAY_START_TIME;
	static char * LINK_DELAY_END_TIME;
	static char * EQUATION_PARAMETERS_x;
	static char * NEW_PROBLEM_FILE;
	static char * NEW_PROBLEM_FORMAT;
	static char * MAX_ROUTING_PROBLEMS;	
	static char * ARCVIEW_PROBLEM_DUMP;
	static char * PROBLEM_DUMP_TYPE;
	static char * PERCENT_PROBLEMS_DUMPED;
	static char * NEW_PATH_ATTRIBUTE_FILE;

	virtual void Program_Control (void);
	virtual bool Vehicle_Processing (Db_File *fh);

private:
	enum Router_Reports { HOUSEHOLD_SCRIPT = 1, HOUSEHOLD_STACK, FARE_DATA };

	int nhh_list, nhh_proc, nrecord, nprocess, ntrips, ntransit, nmagic, nroute, tot_trips;
	int random_imped, max_tod, max_walk, max_bike, update_rate, vol_factor, nupdates, num_trips;
	int last_traveler, last_trip, last_time, last_xfer, last_mode, last_wait, last_acc;
	int nlegs, nlegs_transit, nlegs_auto;

	bool purpose_flag, veh_type_flag, ignore_veh_flag, reset_veh_flag, stop_imp_flag, skim_flag, warn_flag;
	bool walk_net, bike_net, drive_net, loc_net, transit_net, fare_flag, toll_flag, sel_dump_flag;
	bool trip_flag, node_flag, delay_flag, zero_flag, wait_flag, update_flag, tod_flag, dump_flag;
	bool random_flag, walk_flag, bike_flag, hhlist_flag, hhold_flag, hhrec_flag, limit_access;
	bool save_plans, problem_flag, walk_detail, walk_active, distance_flag, wait_leg_flag, veh_flag;
	bool select_mode [MAX_MODE], mode_flag, park_ride_flag, kiss_ride_flag, ignore_time, ignore_errors;
	bool time_flag, dist_flag, length_flag, wait_time_flag, use_flag, turn_flag, old_plan_flag;
	bool activity_flag, select_od, select_org, select_des, select_time, park_imp_flag, local_flag;
	bool rail_bias_flag, bus_bias_flag, park_hours_flag, fare_class_flag, kissride_type [EXTLOAD+1];
	bool attribute_flag, interpolate_flag, adjust_flag, duration_flag;

	int rail_bias, rail_constant, bus_bias, bus_constant, new_mode, leg_check, end_time, park_hours;
	int xfer_imped, stop_imped, station_imped, left_imped, right_imped, uturn_imped, traveler_class;
	int value_round, value_roll, offset_round, offset_roll, time_period, local_distance, local_type;
	int max_distance, min_distance, max_wait, min_wait, max_transfers, max_paths;
	int max_parkride, max_kissride, kissride_factor, kissride_walk, max_ratio, dump_type, percent_dump;
	int value_walk, value_bike, value_wait, value_xfer, value_time, value_distance, value_cost;
	double walk_speed, bike_speed, fare_class [SPECIAL+1];

	Random random;
	Time_Step trip_time;
	Equation_Array equation;
	User_Program hhold_type;

	Time_Range time_range;
	Data_Range org_range, des_range, purpose_range;

	Db_File hhlist_file;
	HHRec_File hhrec_file;
	Db_File hhold_type_file;
	Household_Type_Array household_type;

	Db_File attribute_file;

	typedef struct {
		int traveler;
		int trip;
		int impedance;
		int walk;
		int first_wait;
		int xfer_wait;
		int num_xfer;
		int penalty;
		int transit;
		int drive;
		int cost;
	} Attribute_Data;

	Attribute_Data attribute_data;

	Db_Header park_imp_file;
	Db_Header stop_imp_file;
	Activity_File *activity_file;
	Trip_File *trip_file;
	Problem_File problem_file;
	Plan_File plan_file, old_plan_file, temp_plan;
	Arcview_File dump_file;
	Vehicle_Data default_vehicle;
	Integer_List vehicle_home;

	Integer_List walk_value, bike_value, wait_value, xfer_value, time_value, distance_value, cost_value;
	Integer_List imped_left, imped_right, imped_uturn, imped_xfer, imped_stop, imped_station;
	Integer_List parkride_max, kissride_max, factor_kissride, walk_max, bike_max;
	Integer_List wait_max, wait_min, xfer_max, path_max, bias_rail, add_rail, bias_bus, add_bus;
	Integer_List walk_list, bike_list, drive_list, stop_list, link_list, loc_list, hours_park;
	List_Array walk_link, bike_link;
	TOD_Turn_Array tod_turn;
	List_Array access_list, parking_access, parking_egress, transit_access, transit_egress;
	Route_Stop_Array route_stop;
	Park_Ride_Array park_ride, kiss_ride;

	Trip_End_Data trip_org, trip_des, walk_org;
	Trip_End_Array org_array, des_array, park_array;

	Path_Array link_path, node_path [MAX_PATHS+1];
	Path_Array loc_path [MAX_PATHS+1], board_path [MAX_PATHS], alight_path [MAX_PATHS];
	Path_Data *first_ptr, *last_ptr;

	void Data_Processing (void);
	void Read_Household (void);
	void Read_Activity (void);
	void Read_Trip (void);
	void Plan_Build (Trip_Data *trip_ptr, int type = 1, int duration = 0);

	int  Walk_to_Vehicle (Vehicle_Data *veh_ptr);
	bool Destination_Parking (void);
	int  Parking_to_Activity (Trip_End_Data *org, Trip_End_Data *lot, Trip_End_Data *des);
	int  Best_Combination (void);
	int  Set_Drive_Error (void);
	int  Set_Transit_Error (void);

	int  Drive_Plan (Use_Type type, Vehicle_Data *veh_ptr);
	int  Drive_Path (Use_Type type, bool best_flag = true);
	int  Node_Plan (Use_Type type);
	int  Node_Path (Use_Type type, bool best_flag = true);
	int  Transit_Plan (void);
	void Transit_Path (int board, int xfer, int mode);
	void Location_Access (int origin, int xfer, int mode);
	void Walk_Access (int anode, int xfer, int mode);
	int  Park_Ride_Plan (Vehicle_Data *veh_ptr);
	int  Park_Ride_Lots (void);
	int  Park_Ride_Return (Vehicle_Data *veh_ptr);
	int  Build_Transit_Legs (void);
	int  Set_Leg_Trip_Ends (void);
	int  Save_Transit_Legs (void);
	int  Save_Plan (Trip_End_Data *org, Trip_End_Data *des, int mode = Plan_File::WALK_MODE, int mode_id = 1, int passengers = 0, int wait = 0, int penalty = 0);
	int  Magic_Move (int end_time, int type);
	void Drive_Vines (void);
	void Build_Paths (void);
	int  Kiss_Ride_Plan (Vehicle_Data *veh_ptr);
	int  Kiss_Ride_Lots (void);
	int  Kiss_Ride_Return (Vehicle_Data *veh_ptr);

	bool Update_Plan (void);
	void Update_Travel_Times (void);
	void Write_Attribute (void);

	void Fare_Report (void);
	void Fare_Header (void);
};
#endif
