//*********************************************************
//	VehGen.cpp - generate vehicles for households
//*********************************************************

#include "VehGen.hpp"

//---------------------------------------------------------
//	VehGen constructor
//---------------------------------------------------------

VehGen::VehGen (void) : Demand_Service ()
{
	Program ("VehGen");
	Version ("4.0.1");
	Title ("Vehicle Generation");

	Network_File required_network [] = {
		PROCESS_LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, END_NETWORK
	};

	Demand_File required_demand [] = {
		HOUSEHOLD, NEW_VEHICLE, END_DEMAND
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);

	vehicle_flag = false;
	num_vehicle = num_hhold = num_problem = 0;

	parking_lot.Data_Size (sizeof (Parking_Lot));
}

//---------------------------------------------------------
//	VehGen destructor
//---------------------------------------------------------

VehGen::~VehGen (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	VehGen *exe = new VehGen ();

	return (exe->Start_Execution (commands, control));
}

