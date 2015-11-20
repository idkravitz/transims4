//*********************************************************
//	Microsimulator.cpp - Network Microsimulator
//*********************************************************

#include "Microsimulator.hpp"

char * Microsimulator::PLAN_FILE						= "PLAN_FILE";
char * Microsimulator::PLAN_FORMAT						= "PLAN_FORMAT";
char * Microsimulator::NODE_LIST_PATHS					= "NODE_LIST_PATHS";

char * Microsimulator::CELL_SIZE						= "CELL_SIZE";
char * Microsimulator::TIME_STEPS_PER_SECOND			= "TIME_STEPS_PER_SECOND";
char * Microsimulator::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * Microsimulator::SIMULATION_START_TIME			= "SIMULATION_START_TIME";
char * Microsimulator::SIMULATION_END_TIME				= "SIMULATION_END_TIME";
char * Microsimulator::SPEED_CALCULATION_METHOD			= "SPEED_CALCULATION_METHOD";

char * Microsimulator::PLAN_FOLLOWING_DISTANCE			= "PLAN_FOLLOWING_DISTANCE";
char * Microsimulator::LOOK_AHEAD_DISTANCE				= "LOOK_AHEAD_DISTANCE";
char * Microsimulator::LOOK_AHEAD_LANE_FACTOR			= "LOOK_AHEAD_LANE_FACTOR";
char * Microsimulator::LOOK_AHEAD_TIME_FACTOR			= "LOOK_AHEAD_TIME_FACTOR";
char * Microsimulator::MAXIMUM_SWAPPING_SPEED			= "MAXIMUM_SWAPPING_SPEED";
char * Microsimulator::MAXIMUM_SPEED_DIFFERENCE			= "MAXIMUM_SPEED_DIFFERENCE";
char * Microsimulator::ENFORCE_PARKING_LANES			= "ENFORCE_PARKING_LANES";
char * Microsimulator::FIX_VEHICLE_LOCATIONS			= "FIX_VEHICLE_LOCATIONS";

char * Microsimulator::DRIVER_REACTION_TIME				= "DRIVER_REACTION_TIME";
char * Microsimulator::PERMISSION_PROBABILITY			= "PERMISSION_PROBABILITY";
char * Microsimulator::SLOW_DOWN_PROBABILITY			= "SLOW_DOWN_PROBABILITY";
char * Microsimulator::SLOW_DOWN_PERCENTAGE				= "SLOW_DOWN_PERCENTAGE";
char * Microsimulator::RANDOM_NUMBER_SEED				= "RANDOM_NUMBER_SEED";

char * Microsimulator::MINIMUM_WAITING_TIME				= "MINIMUM_WAITING_TIME";
char * Microsimulator::MAXIMUM_WAITING_TIME				= "MAXIMUM_WAITING_TIME";
char * Microsimulator::MAX_ARRIVAL_TIME_VARIANCE		= "MAX_ARRIVAL_TIME_VARIANCE";
char * Microsimulator::MAX_DEPARTURE_TIME_VARIANCE      = "MAX_DEPARTURE_TIME_VARIANCE";

char * Microsimulator::NEW_PROBLEM_FILE					= "NEW_PROBLEM_FILE";
char * Microsimulator::NEW_PROBLEM_FORMAT				= "NEW_PROBLEM_FORMAT";
char * Microsimulator::MAX_SIMULATION_PROBLEMS			= "MAX_SIMULATION_PROBLEMS";
char * Microsimulator::PRINT_PROBLEM_MESSAGES			= "PRINT_PROBLEM_MESSAGES";

//---------------------------------------------------------
//	Microsimulator constructor
//---------------------------------------------------------

Microsimulator::Microsimulator (void) : Demand_Service (), Problem_Service ()
{
	Program ("Microsimulator");
	Version ("4.0.79");
	Title ("Network Microsimulator");

	Network_File required_network [] = {
		NODE, LINK, POCKET_LANE, LANE_CONNECTIVITY, PARKING, ACTIVITY_LOCATION, PROCESS_LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, LANE_USE, TURN_PROHIBITION, 
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		UNSIGNALIZED_NODE, SIGNALIZED_NODE, TIMING_PLAN, PHASING_PLAN, DETECTOR, SIGNAL_COORDINATOR, 
		END_NETWORK
	};

	Demand_File required_demand [] = {
		VEHICLE_TYPE, VEHICLE, END_DEMAND
	};

	Demand_File optional_demand [] = {
		BOUNDARY_SPEED, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,

		CELL_SIZE,
		TIME_STEPS_PER_SECOND,
		TIME_OF_DAY_FORMAT,
		SIMULATION_START_TIME,
		SIMULATION_END_TIME,
		SPEED_CALCULATION_METHOD,

		PLAN_FOLLOWING_DISTANCE,
		LOOK_AHEAD_DISTANCE,
		LOOK_AHEAD_LANE_FACTOR,
		LOOK_AHEAD_TIME_FACTOR,
		MAXIMUM_SWAPPING_SPEED,
		MAXIMUM_SPEED_DIFFERENCE,
		ENFORCE_PARKING_LANES,
		FIX_VEHICLE_LOCATIONS,

		DRIVER_REACTION_TIME,
		PERMISSION_PROBABILITY,
		SLOW_DOWN_PROBABILITY,
		SLOW_DOWN_PERCENTAGE,
		RANDOM_NUMBER_SEED,

		MINIMUM_WAITING_TIME,
		MAXIMUM_WAITING_TIME,
		MAX_ARRIVAL_TIME_VARIANCE,
		MAX_DEPARTURE_TIME_VARIANCE,

		NEW_PROBLEM_FILE,
		NEW_PROBLEM_FORMAT,
		MAX_SIMULATION_PROBLEMS,
		PRINT_PROBLEM_MESSAGES,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);

	Check_Data (true);
	Renumber (true);

	Lane_Use (true);

	step = 0;
	start_time_step = 0;
	end_time_step = MIDNIGHT;
	steps_per_second = 1;							//---- 1 time step per second ----

	min_speed = max_speed = lane_change = problem_code = problem_dir = problem_lane = 0;
	num_vehicles = max_vehicles = max_time = tot_complete = 0;
	tot_hours = 0.0;

	ncells = veh_cells = transit_id = 0;
	ntrips = nstarted = ncompleted = nrequired = noptional = nswap = nreserve = nmove = npriority = 0;
	ntransit = nboard = nalight = nruns = nrun_start = nrun_end = ndiagonal = 0;

	conflict_veh.Num_Cells (1);

	pce_cells = 1;
	cell_size = Round (7.5);						//---- 7.5 meters ----
	min_wait_time = 180;							//---- 3 minutes ----
	max_wait_time = 3600;							//---- 1 hour ----
	max_start_variance = max_end_variance = 60;		//---- 1 hour ----
	max_swap_speed = Round (37.5);					//---- 37.5 meters/second ----
	max_swap_diff = Round (7.5);					//---- 7.5 meters/second ----
	lane_factor = Round (4.0);						//---- lane change weight ----
	time_factor = Round (1.0);						//---- travel time weight ----
	look_ahead = 260;								//---- 260 meters ----
	plan_follow = 525;								//---- 525 meters ----
	slow_down_prob [0] = 0.0;						//---- 0 percent ----
	slow_down_percent [0] = 0;						//---- 0 percent ----
	reaction_time [0] = 1.0;						//---- 1 second ----
	permission_prob [0] = 50.0;						//---- 50 percent ----

	num_veh_type = 0;
	veh_type_map = num_veh = num_per = NULL;

	node_flag = true;
	step_flag = problem_flag = error_flag = speed_method = tod_flag = false;
	parking_flag = look_ahead_flag = transit_flag = signal_flag = fix_flag = blocked_flag = false;
	slow_down_flag = reaction_flag = permit_all = permit_none = boundary_flag = false;

	traveler_output.Add_Keys ();
	snapshot_output.Add_Keys ();
	summary_output.Add_Keys ();
	occupancy_output.Add_Keys ();
	problem_output.Add_Keys ();
	system_event.Add_Keys ();
	event_output.Add_Keys ();
	turn_output.Add_Keys ();
	ridership_output.Add_Keys ();
	speed_bin_output.Add_Keys ();

	conflict_veh.Initialize (1);

	dir_array = (Dir_Array *) new Grid_Array ();
	vehicle_array = (Vehicle_Array *) new Msim_Vehicle_Array ();
	line_array = (Line_Array *) new Rider_Array ();
}

//---------------------------------------------------------
//	Microsimulator destructor
//---------------------------------------------------------

Microsimulator::~Microsimulator (void)
{
	if (veh_type_map != NULL) {
		delete [] veh_type_map;
		veh_type_map = NULL;
	}
	if (num_veh != NULL) {
		delete [] num_veh;
		num_veh = NULL;
	}
	if (num_per != NULL) {
		delete [] num_per;
		num_per = NULL;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Microsimulator *exe = new Microsimulator ();

	return (exe->Start_Execution (commands, control));
}

