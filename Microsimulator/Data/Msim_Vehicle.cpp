//*********************************************************
//	Msim_Vehicle.cpp - microsimulator vehicle data classes
//*********************************************************

#include "Msim_Vehicle.hpp"

//---------------------------------------------------------
//	Msim_Veh_Data constructor
//---------------------------------------------------------

Msim_Veh_Data::Msim_Veh_Data (void) : Position_Data ()
{
	Travel_Plan (NULL);
	Status (0);
	Move_Flag (false);
	Priority (false);
	Blocked (false);
	Lane_Change (0);
	Hold_Flag (false);
	Hold (0);
	Waiting (0);
	Reserve (0);
	Cycle_Dir (0);
	Offset (0);
	Speed (0);
	Leg (0);
	To_Offset (0);
	To_Speed (0);
	To_Leg (0);
}

//---------------------------------------------------------
//	Msim_Vehicle constructor
//---------------------------------------------------------

Msim_Vehicle::Msim_Vehicle (int vehicle) : Vehicle_Data (vehicle)
{
	data = NULL;
	num_cells = 0;
}

bool Msim_Vehicle::Initialize (int ncells)
{
	if (ncells != 0) {
		if (ncells != num_cells && num_cells > 0) {
			Clear ();
		}
		Num_Cells (ncells);
	}
	if (Num_Cells () == 0) return (false);

	if (Check ()) return (true);

	data = new Msim_Veh_Data ();
	if (data == NULL) return (false);

	if (!data->Num_Cells (Num_Cells ())) {
		Clear ();
		return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Msim_Vehicle_Array constructor
//---------------------------------------------------------

Msim_Vehicle_Array::Msim_Vehicle_Array (int max_records) : 
	Vehicle_Array ()
{
	Initialize (sizeof (Msim_Vehicle), max_records);
}
