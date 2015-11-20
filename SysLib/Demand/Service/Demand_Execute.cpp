//*********************************************************
//	Demand_Execute.cpp - main execution procedure
//*********************************************************

#include "Demand_Service.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Demand_Service::Execute (void)
{
	File_Data *file, *new_file;

	if (!control_flag) {
		Error ("Demand_Service::Program_Control has Not Been Called");
	}

	//---- read the network files ----

	Network_Service::Execute ();

	//---- household ----

	file = demand_file + HOUSEHOLD;
	new_file = demand_file + NEW_HOUSEHOLD;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (household_array == NULL) {
			household_array = new Household_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Household ();

	//---- population ----

	file = demand_file + POPULATION;
	new_file = demand_file + NEW_POPULATION;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (population_array == NULL) {
			population_array = new Population_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Population ();

	//---- link delay ----

	file = demand_file + LINK_DELAY;
	if (file->flag && file->access == READ) Read_Delay ();

	//---- performance ----

	file = demand_file + PERFORMANCE;
	if (file->flag && file->access == READ) Read_Performance ();

	//---- ridership ----

	file = demand_file + RIDERSHIP;
	if (file->flag && file->access == READ) Read_Ridership ();

	//---- vehicle type ----

	file = demand_file + VEHICLE_TYPE;
	new_file = demand_file + NEW_VEHICLE_TYPE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (veh_type_array == NULL) {
			veh_type_array = new Veh_Type_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Veh_Type ();

	//---- vehicle ----

	file = demand_file + VEHICLE;
	new_file = demand_file + NEW_VEHICLE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (vehicle_array == NULL) {
			vehicle_array = new Vehicle_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Vehicle ();

	//---- activity ----

	file = demand_file + ACTIVITY;
	new_file = demand_file + NEW_ACTIVITY;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (activity_array == NULL) {
			activity_array = new Activity_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Activity ();

	//---- trip ----

	file = demand_file + TRIP;
	new_file = demand_file + NEW_TRIP;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (trip_array == NULL) {
			trip_array = new Trip_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Trip ();

	//---- boundary ----

	file = demand_file + BOUNDARY_SPEED;
	new_file = demand_file + NEW_BOUNDARY_SPEED;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (boundary_array == NULL) {
			boundary_array = new Boundary_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Boundary ();
}
