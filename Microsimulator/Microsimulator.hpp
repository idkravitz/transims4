//*********************************************************
//	Microsimulator.hpp - Network Microsimulator
//*********************************************************

#ifndef MICROSIMULATOR_HPP
#define MICROSIMULATOR_HPP

#include "Demand_Service.hpp"
#include "Problem_Service.hpp"
#include "Random.hpp"
#include "Data_Range.hpp"
#include "Time_Step.hpp"
#include "Plan_File.hpp"
#include "Problem_File.hpp"
#include "Index_Array.hpp"
#include "Integer_Array.hpp"
#include "Integer_Queue.hpp"
#include "Data_Queue.hpp"
#include "Vehicle_File.hpp"
#include "Traveler_Output.hpp"
#include "Snapshot_Output.hpp"
#include "Summary_Output.hpp"
#include "Problem_Output.hpp"
#include "System_Event_Output.hpp"
#include "Event_Output.hpp"
#include "Ridership_Output.hpp"
#include "Turn_Output.hpp"
#include "Speed_Bin_Output.hpp"

#include "Data/Grid_Data.hpp"
#include "Data/Control_Data.hpp"
#include "Data/Controller_Data.hpp"
#include "Data/Travel_Leg_Data.hpp"
#include "Data/Travel_Plan_Data.hpp"
#include "Data/Msim_Vehicle.hpp"
#include "Data/Traveler_Data.hpp"
#include "Data/Position_Data.hpp"
#include "Data/Occupancy_Output.hpp"

#define NUM_PRIORITY_LEVELS		5
#define MAX_FACILITY_TYPE		24

#define grid_data		(*((Grid_Array *) dir_array))
#undef  vehicle_data
#define vehicle_data	(*((Msim_Vehicle_Array *) vehicle_array))

//---------------------------------------------------------
//	Microsimulator - execution class definition
//---------------------------------------------------------

class Microsimulator : public Demand_Service, public Problem_Service
{
	friend class Occupancy_Output;

public:

	Microsimulator (void);
	virtual ~Microsimulator (void);

	virtual void Execute (void);

	virtual bool Veh_Type_Processing (Db_File *file);
	virtual bool Vehicle_Processing (Db_File *file);

protected:

	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;

	static char * CELL_SIZE;
	static char * TIME_STEPS_PER_SECOND;
	static char * TIME_OF_DAY_FORMAT;
	static char * SIMULATION_START_TIME;
	static char * SIMULATION_END_TIME;
	static char * SPEED_CALCULATION_METHOD;

	static char * PLAN_FOLLOWING_DISTANCE;
	static char * LOOK_AHEAD_DISTANCE;
	static char * LOOK_AHEAD_LANE_FACTOR;
	static char * LOOK_AHEAD_TIME_FACTOR;
	static char * MAXIMUM_SWAPPING_SPEED;
	static char * MAXIMUM_SPEED_DIFFERENCE;
	static char * ENFORCE_PARKING_LANES;
	static char * FIX_VEHICLE_LOCATIONS;

	static char * DRIVER_REACTION_TIME;
	static char * PERMISSION_PROBABILITY;
	static char * SLOW_DOWN_PROBABILITY;
	static char * SLOW_DOWN_PERCENTAGE;
	static char * RANDOM_NUMBER_SEED;

	static char * MINIMUM_WAITING_TIME;
	static char * MAXIMUM_WAITING_TIME;
	static char * MAX_ARRIVAL_TIME_VARIANCE;
	static char * MAX_DEPARTURE_TIME_VARIANCE;

	static char * NEW_PROBLEM_FILE;
	static char * NEW_PROBLEM_FORMAT;
	static char * MAX_SIMULATION_PROBLEMS;
	static char * PRINT_PROBLEM_MESSAGES;

	virtual void Program_Control (void);

private:
	enum Control_Type {RED_LIGHT = 3, YELLOW_LIGHT, PROTECTED_GREEN, PERMITTED_GREEN, STOP_GREEN};

	int start_time_step, end_time_step, steps_per_second, step_round;
	int min_wait_time, max_wait_time, max_start_variance, max_end_variance;
	int max_swap_speed, max_swap_diff, lane_factor, time_factor;
	int cell_size, cell_round, half_cell, look_ahead, plan_follow, slow_down_percent [MAX_FACILITY_TYPE];
	double slow_down_prob [MAX_FACILITY_TYPE], reaction_time [MAX_FACILITY_TYPE];
	double permission_prob [MAX_FACILITY_TYPE], tot_hours;

	int step, ncells, veh_cells, transit_id, next_vehicle, pce_cells;
	int ntrips, nstarted, ncompleted, nrequired, noptional, nswap, nreserve, nmove, npriority;
	int ntransit, nboard, nalight, nruns, nrun_start, nrun_end, ndiagonal;
	int num_veh_type, *veh_type_map, *num_veh, *num_per;
	bool step_flag, node_flag, problem_flag, error_flag, speed_method, tod_flag;
	bool parking_flag, look_ahead_flag, transit_flag, signal_flag, fix_flag, blocked_flag;
	bool slow_down_flag, reaction_flag, permit_none, permit_all, boundary_flag;

	int min_speed, max_speed, lane_change, problem_code, problem_dir, problem_lane, hold_time;
	int num_vehicles, max_vehicles, max_time, tot_complete;

	Random random;
	Time_Step time_step;
	Msim_Vehicle conflict_veh;

	Plan_File plan_file;

	Problem_File problem_file;

	Travel_Plan_Queue travel_queue;
	Travel_Plan_Queue network_traffic;
	Travel_Plan_Array route_plan;

	Traveler_Array traveler_data;
	Control_Array control_data;
	Controller_Array controller_data;
	Position_Data_Array position_data;

	Integer_Queue priority_queue;
	Integer_List lane_changes [NUM_PRIORITY_LEVELS];
	Integer_List node_in, dir_in;

	Traveler_Output traveler_output;
	Snapshot_Output snapshot_output;
	Summary_Output summary_output;
	Problem_Output problem_output;
	System_Event_Output system_event;
	Event_Output event_output;
	Ridership_Output ridership_output;
	Turn_Output turn_output;
	Occupancy_Output occupancy_output;
	Speed_Bin_Output speed_bin_output;

	void Data_Processing (void);
	void Traffic_Control (void);
	void Transit_Plan (void);
	void Update_Network (void);
	void Update_Signals (void);
	void Update_Transit (void);
	void Priority_Queue (void);
	void Change_Lanes (void);
	void Check_Queue (void);
	void Process_Plan (void);
	void Reposition_Plan (void);
	void Process_Step (void);
	bool Move_Vehicles (void);

	bool Process_Vehicle (Msim_Vehicle *vehicle_ptr);
	bool Lane_Change (Msim_Vehicle *vehicle_ptr);
	bool Check_Behind (Msim_Vehicle *vehicle_ptr, int seconds = 1);
	bool Check_Ahead (Msim_Vehicle *vehicle_ptr, bool save_flag = false);
	void Best_Lanes (Travel_Plan_Data *plan_ptr);
	bool Access_Lanes (Travel_Leg_Data *leg, Travel_Plan_Data *plan, bool start_flag);
	bool Load_Vehicle (Travel_Plan_Data *plan_ptr);
	void Park_Vehicle (Msim_Vehicle *vehicle_ptr, bool error_flag = false);
	void Look_Ahead (Msim_Vehicle *vehicle_ptr);
	int  Sum_Path (Msim_Vehicle *vehicle_ptr);
	bool Check_Move (Msim_Vehicle *vehicle_ptr);
	void Problem_Plan (int problem, Travel_Plan_Data *plan_ptr, int dir, int lane, int cell);
	void Start_Controller (Controller_Data *controller_ptr);
	bool Check_Detectors (int index);
	void Move_Veh_Cells (Msim_Vehicle *vehicle_ptr, bool in_place = false);
	int  Entry_Lane (int lane, Travel_Leg_Data *from_leg, Travel_Leg_Data *to_leg);
	bool Cell_Use (int cell, int lane, Grid_Data *grid_ptr, Use_Type use, bool use_flag = false);
	void Cycle_Failure (Control_Data *control_ptr);
	void Walk_Plan (Traveler_Data *traveler_ptr, Travel_Plan_Data *plan_ptr);
};
#endif
