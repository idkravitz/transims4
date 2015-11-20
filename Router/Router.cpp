//*********************************************************
//	Router.cpp - Network Path Builder
//*********************************************************

#include "Router.hpp"

char * Router::HOUSEHOLD_LIST					= "HOUSEHOLD_LIST";
char * Router::HOUSEHOLD_RECORD_FILE			= "HOUSEHOLD_RECORD_FILE";
char * Router::PARKING_PENALTY_FILE				= "PARKING_PENALTY_FILE";
char * Router::TRANSIT_PENALTY_FILE				= "TRANSIT_PENALTY_FILE";
char * Router::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * Router::PLAN_FILE						= "PLAN_FILE";
char * Router::PLAN_FORMAT						= "PLAN_FORMAT";
char * Router::NEW_PLAN_FILE					= "NEW_PLAN_FILE";
char * Router::NEW_PLAN_FORMAT					= "NEW_PLAN_FORMAT";
char * Router::NODE_LIST_PATHS					= "NODE_LIST_PATHS";
char * Router::ROUTE_SELECTED_MODES				= "ROUTE_SELECTED_MODES";
char * Router::ROUTE_SELECTED_PURPOSES			= "ROUTE_SELECTED_PURPOSES";
char * Router::ROUTE_WITH_SPECIFIED_MODE		= "ROUTE_WITH_SPECIFIED_MODE";
char * Router::ROUTE_FROM_SPECIFIED_LOCATIONS	= "ROUTE_FROM_SPECIFIED_LOCATIONS";
char * Router::ROUTE_TO_SPECIFIED_LOCATIONS		= "ROUTE_TO_SPECIFIED_LOCATIONS";
char * Router::ROUTE_AT_SPECIFIED_TIMES			= "ROUTE_AT_SPECIFIED_TIMES";
char * Router::ROUTE_BY_TIME_INCREMENT			= "ROUTE_BY_TIME_INCREMENT";
char * Router::UPDATE_PLAN_RECORDS				= "UPDATE_PLAN_RECORDS";
char * Router::PRINT_UPDATE_WARNINGS			= "PRINT_UPDATE_WARNINGS";
char * Router::IGNORE_VEHICLE_ID				= "IGNORE_VEHICLE_ID";
char * Router::LIMIT_PARKING_ACCESS				= "LIMIT_PARKING_ACCESS";
char * Router::WALK_PATH_DETAILS				= "WALK_PATH_DETAILS";
char * Router::ADJUST_ACTIVITY_SCHEDULE			= "ADJUST_ACTIVITY_SCHEDULE";
char * Router::IGNORE_ACTIVITY_DURATIONS		= "IGNORE_ACTIVITY_DURATIONS";
char * Router::IGNORE_TIME_CONSTRAINTS			= "IGNORE_TIME_CONSTRAINTS";
char * Router::END_TIME_CONSTRAINT				= "END_TIME_CONSTRAINT";
char * Router::IGNORE_ROUTING_PROBLEMS			= "IGNORE_ROUTING_PROBLEMS";
char * Router::INTERPOLATE_LINK_DELAYS			= "INTERPOLATE_LINK_DELAYS";
char * Router::PERCENT_RANDOM_IMPEDANCE			= "PERCENT_RANDOM_IMPEDANCE";
char * Router::RANDOM_NUMBER_SEED				= "RANDOM_NUMBER_SEED";
char * Router::HOUSEHOLD_TYPE_SCRIPT			= "HOUSEHOLD_TYPE_SCRIPT";
char * Router::WALK_SPEED						= "WALK_SPEED";
char * Router::BICYCLE_SPEED					= "BICYCLE_SPEED";
char * Router::WALK_TIME_VALUE					= "WALK_TIME_VALUE";
char * Router::BICYCLE_TIME_VALUE				= "BICYCLE_TIME_VALUE";
char * Router::FIRST_WAIT_VALUE					= "FIRST_WAIT_VALUE";
char * Router::TRANSFER_WAIT_VALUE				= "TRANSFER_WAIT_VALUE";
char * Router::VEHICLE_TIME_VALUE				= "VEHICLE_TIME_VALUE";
char * Router::DISTANCE_VALUE					= "DISTANCE_VALUE";
char * Router::COST_VALUE						= "COST_VALUE";
char * Router::LEFT_TURN_PENALTY				= "LEFT_TURN_PENALTY";
char * Router::RIGHT_TURN_PENALTY				= "RIGHT_TURN_PENALTY";
char * Router::U_TURN_PENALTY					= "U_TURN_PENALTY";
char * Router::TRANSFER_PENALTY					= "TRANSFER_PENALTY";
char * Router::STOP_WAITING_PENALTY				= "STOP_WAITING_PENALTY";
char * Router::STATION_WAITING_PENALTY			= "STATION_WAITING_PENALTY";
char * Router::BUS_BIAS_FACTOR					= "BUS_BIAS_FACTOR";
char * Router::BUS_BIAS_CONSTANT				= "BUS_BIAS_CONSTANT";
char * Router::RAIL_BIAS_FACTOR					= "RAIL_BIAS_FACTOR";
char * Router::RAIL_BIAS_CONSTANT				= "RAIL_BIAS_CONSTANT";
char * Router::MAX_WALK_DISTANCE				= "MAX_WALK_DISTANCE";
char * Router::MAX_BICYCLE_DISTANCE				= "MAX_BICYCLE_DISTANCE";
char * Router::MAX_WAIT_TIME					= "MAX_WAIT_TIME";
char * Router::MIN_WAIT_TIME					= "MIN_WAIT_TIME";
char * Router::MAX_NUMBER_OF_TRANSFERS			= "MAX_NUMBER_OF_TRANSFERS";
char * Router::MAX_NUMBER_OF_PATHS				= "MAX_NUMBER_OF_PATHS";
char * Router::MAX_PARK_RIDE_PERCENTAGE			= "MAX_PARK_RIDE_PERCENTAGE";
char * Router::MAX_KISS_RIDE_PERCENTAGE			= "MAX_KISS_RIDE_PERCENTAGE";
char * Router::KISS_RIDE_TIME_FACTOR			= "KISS_RIDE_TIME_FACTOR";
char * Router::KISS_RIDE_STOP_TYPES				= "KISS_RIDE_STOP_TYPES";
char * Router::MAX_KISS_RIDE_DROPOFF_WALK		= "MAX_KISS_RIDE_DROPOFF_WALK";
char * Router::MAX_LEGS_PER_PATH				= "MAX_LEGS_PER_PATH";
char * Router::ADD_WAIT_TO_TRANSIT_LEG			= "ADD_WAIT_TO_TRANSIT_LEG";
char * Router::FARE_CLASS_DISTRIBUTION			= "FARE_CLASS_DISTRIBUTION";
char * Router::PARKING_HOURS_BY_PURPOSE			= "PARKING_HOURS_BY_PURPOSE";
char * Router::LOCAL_ACCESS_DISTANCE			= "LOCAL_ACCESS_DISTANCE";
char * Router::LOCAL_FACILITY_TYPE				= "LOCAL_FACILITY_TYPE";
char * Router::MAX_CIRCUITY_RATIO				= "MAX_CIRCUITY_RATIO";
char * Router::MIN_CIRCUITY_DISTANCE			= "MIN_CIRCUITY_DISTANCE";
char * Router::MAX_CIRCUITY_DISTANCE			= "MAX_CIRCUITY_DISTANCE";
char * Router::LINK_DELAY_UPDATE_RATE			= "LINK_DELAY_UPDATE_RATE";
char * Router::LINK_DELAY_VOL_FACTOR			= "LINK_DELAY_VOL_FACTOR";
char * Router::LINK_DELAY_START_TIME			= "LINK_DELAY_START_TIME";
char * Router::LINK_DELAY_END_TIME				= "LINK_DELAY_END_TIME";
char * Router::EQUATION_PARAMETERS_x			= "EQUATION_PARAMETERS_*";
char * Router::NEW_PROBLEM_FILE					= "NEW_PROBLEM_FILE";
char * Router::NEW_PROBLEM_FORMAT				= "NEW_PROBLEM_FORMAT";
char * Router::MAX_ROUTING_PROBLEMS				= "MAX_ROUTING_PROBLEMS";
char * Router::ARCVIEW_PROBLEM_DUMP				= "ARCVIEW_PROBLEM_DUMP";
char * Router::PROBLEM_DUMP_TYPE				= "PROBLEM_DUMP_TYPE";
char * Router::PERCENT_PROBLEMS_DUMPED			= "PERCENT_PROBLEMS_DUMPED";
char * Router::NEW_PATH_ATTRIBUTE_FILE          = "NEW_PATH_ATTRIBUTE_FILE";

//---------------------------------------------------------
//	Router constructor
//---------------------------------------------------------

Router::Router (void) : Demand_Service (), Problem_Service ()
{
	Program ("Router");
	Version ("4.0.71");
	Title ("Network Path Builder");

	Network_File required_network [] = {
		NODE, LINK, LANE_CONNECTIVITY, PARKING, ACTIVITY_LOCATION, PROCESS_LINK, END_NETWORK
	};
	Network_File optional_network [] = {
		DIRECTORY, LANE_USE, TOLL, TURN_PROHIBITION, 
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, TRANSIT_SCHEDULE, 
		END_NETWORK
	};
	Demand_File optional_demand [] = {
		HOUSEHOLD, TRIP, ACTIVITY, LINK_DELAY, VEHICLE, VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		HOUSEHOLD_LIST,
		HOUSEHOLD_RECORD_FILE,
		PARKING_PENALTY_FILE,
		TRANSIT_PENALTY_FILE,
		TIME_OF_DAY_FORMAT,
		PLAN_FILE,
		PLAN_FORMAT,
		NEW_PLAN_FILE,
		NEW_PLAN_FORMAT,
		NODE_LIST_PATHS,
		ROUTE_SELECTED_MODES,
		ROUTE_SELECTED_PURPOSES,
		ROUTE_WITH_SPECIFIED_MODE,
		ROUTE_FROM_SPECIFIED_LOCATIONS,
		ROUTE_TO_SPECIFIED_LOCATIONS,
		ROUTE_AT_SPECIFIED_TIMES,
		ROUTE_BY_TIME_INCREMENT,
		UPDATE_PLAN_RECORDS,
		PRINT_UPDATE_WARNINGS,
		IGNORE_VEHICLE_ID,
		LIMIT_PARKING_ACCESS,
		WALK_PATH_DETAILS,
		ADJUST_ACTIVITY_SCHEDULE,
		IGNORE_ACTIVITY_DURATIONS,
		IGNORE_TIME_CONSTRAINTS,
		END_TIME_CONSTRAINT,
		IGNORE_ROUTING_PROBLEMS,
		INTERPOLATE_LINK_DELAYS,
		PERCENT_RANDOM_IMPEDANCE,
		RANDOM_NUMBER_SEED,
		HOUSEHOLD_TYPE_SCRIPT,
		WALK_SPEED,
		BICYCLE_SPEED,
		WALK_TIME_VALUE,
		BICYCLE_TIME_VALUE,
		FIRST_WAIT_VALUE,
		TRANSFER_WAIT_VALUE,
		VEHICLE_TIME_VALUE,
		DISTANCE_VALUE,
		COST_VALUE,
		LEFT_TURN_PENALTY,
		RIGHT_TURN_PENALTY,
		U_TURN_PENALTY,
		TRANSFER_PENALTY,
		STOP_WAITING_PENALTY,
		STATION_WAITING_PENALTY,
		BUS_BIAS_FACTOR,
		BUS_BIAS_CONSTANT,
		RAIL_BIAS_FACTOR,
		RAIL_BIAS_CONSTANT,
		MAX_WALK_DISTANCE,
		MAX_BICYCLE_DISTANCE,
		MAX_WAIT_TIME,
		MIN_WAIT_TIME,
		MAX_NUMBER_OF_TRANSFERS,
		MAX_NUMBER_OF_PATHS,
		MAX_PARK_RIDE_PERCENTAGE,
		MAX_KISS_RIDE_PERCENTAGE,
		KISS_RIDE_TIME_FACTOR,
		KISS_RIDE_STOP_TYPES,
		MAX_KISS_RIDE_DROPOFF_WALK,
		MAX_LEGS_PER_PATH,
		ADD_WAIT_TO_TRANSIT_LEG,
		FARE_CLASS_DISTRIBUTION,
		PARKING_HOURS_BY_PURPOSE,
		LOCAL_ACCESS_DISTANCE,
		LOCAL_FACILITY_TYPE,
		MAX_CIRCUITY_RATIO,
		MIN_CIRCUITY_DISTANCE,
		MAX_CIRCUITY_DISTANCE,
		MAX_LINK_DELAY_ERRORS,
		LINK_DELAY_UPDATE_RATE,
		LINK_DELAY_VOL_FACTOR,
		LINK_DELAY_START_TIME,
		LINK_DELAY_END_TIME,
		EQUATION_PARAMETERS_x,
		NEW_PROBLEM_FILE,
		NEW_PROBLEM_FORMAT,
		MAX_ROUTING_PROBLEMS,
		ARCVIEW_PROBLEM_DUMP,
		PROBLEM_DUMP_TYPE,
		PERCENT_PROBLEMS_DUMPED,
		NEW_PATH_ATTRIBUTE_FILE,
		NULL
	};

	char *reports [] = {
		"HOUSEHOLD_TYPE_SCRIPT",
		"HOUSEHOLD_TYPE_STACK",
		"FARE_DATA_REPORT",
		NULL
	};
	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	Check_Data (true);
	Renumber (true);
	Max_Problems (100000);

	nhh_list = nhh_proc = nrecord = nprocess = ntrips = num_trips = update_rate = nupdates = 0;
	random_imped = max_tod = new_mode = nmagic = ntransit = nroute = tot_trips = 0;
	last_traveler = last_trip = last_time = last_xfer = last_wait = last_acc = 0;
	nlegs = nlegs_transit = nlegs_auto = 0;
	vol_factor = 1;

	limit_access = node_flag = true;
	purpose_flag = ignore_veh_flag = reset_veh_flag = stop_imp_flag = skim_flag = warn_flag = false;
	walk_net = bike_net = drive_net = loc_net = transit_net = fare_flag = wait_leg_flag = false;
	hhlist_flag = hhold_flag = hhrec_flag = trip_flag = ignore_time = update_flag = tod_flag = false;
	random_flag = walk_flag = bike_flag = walk_detail = walk_active = local_flag = false;
	save_plans = problem_flag = ignore_errors = delay_flag = zero_flag = old_plan_flag = false;
	mode_flag = park_ride_flag = distance_flag = wait_flag = toll_flag = turn_flag = false;
	time_flag = dist_flag = length_flag = wait_time_flag = use_flag = dump_flag = veh_flag = false;
	activity_flag = select_od = select_org = select_des = select_time = park_imp_flag = false;
	rail_bias_flag = bus_bias_flag = park_hours_flag = fare_class_flag = sel_dump_flag = false;
	attribute_flag = interpolate_flag = adjust_flag = duration_flag = false;

	first_ptr = last_ptr = NULL;

	walk_speed = 1.0;
	bike_speed = 4.0;

	value_walk = 20;
	value_bike = 15;
	value_wait = 20;
	value_xfer = 20;
	value_time = 10;
	value_distance = 0;
	value_cost = 0;

	value_roll = 3;		//---- 1/8 resolution ----
	value_round = (1 << (value_roll-1));

	offset_roll = 11;	//---- 1/2048 resolution ----
	offset_round = (1 << (offset_roll-1));
	
	time_period = 900;		//---- 15 minutes ----

	max_walk = 2000;
	max_bike = 10000;
	max_wait = 60;
	min_wait = 0;
	max_transfers = 3;
	max_paths = 4;
	max_parkride = 50;
	max_kissride = 35;
	kissride_factor = 250;
	kissride_walk = 100;
	rail_bias = 100;
	rail_constant = 0;
	bus_bias = 100;
	bus_constant = 0;
	park_hours = 0;

	xfer_imped = 0;
	stop_imped = 0;
	station_imped = 0;
	left_imped = 0;
	right_imped = 0;
	uturn_imped = 0;
	traveler_class = CASH;

	local_type = EXTERNAL;
	local_distance = 2000;

	end_time = 0;			//---- 0 minutes ----
	leg_check = 1000;
	min_distance = 2000;
	max_distance = 20000;
	max_ratio = 0;

	dump_type = 0;
	percent_dump = Round (100);
		
	memset (&attribute_data, '\0', sizeof (attribute_data));

	default_vehicle.ID (1);
	default_vehicle.Type (CAR);
	default_vehicle.Sub_Type (0);
	default_vehicle.Passengers (0);
	default_vehicle.Household (1);
	default_vehicle.Location (1);
}

//---------------------------------------------------------
//	Router destructor
//---------------------------------------------------------

Router::~Router (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Router *exe = new Router ();

	return (exe->Start_Execution (commands, control));
}
