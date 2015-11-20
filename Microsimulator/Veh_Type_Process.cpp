//*********************************************************
//	Veh_Type_Process.cpp - Read the Vehicle Type File
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Veh_Type_Processing
//---------------------------------------------------------

bool Microsimulator::Veh_Type_Processing (Db_File *fh)
{
	if (Demand_Service::Veh_Type_Processing (fh)) {

		if (speed_method) return (true);

		//---- convert to cell-based sizes ----

		int lvalue;
		Veh_Type_Data *veh_type_ptr;

		veh_type_ptr = veh_type_data.New_Record ();

		lvalue = (veh_type_ptr->Length () + half_cell) / cell_size;
		if (lvalue < 1) lvalue = 1;

		veh_type_ptr->Length (lvalue);

		if (veh_type_ptr->Use () == CAR || veh_type_ptr->Use () == SOV) {
			pce_cells = lvalue;
		}

		lvalue = (veh_type_ptr->Max_Speed () + cell_round) / cell_size;
		if (lvalue < 1) lvalue = 1;

		veh_type_ptr->Max_Speed (lvalue);

		lvalue = (veh_type_ptr->Max_Accel () + cell_round) / cell_size;
		if (lvalue < 1) lvalue = 1;

		veh_type_ptr->Max_Accel (lvalue);

		lvalue = (veh_type_ptr->Max_Decel () + cell_round) / cell_size;
		if (lvalue < 1) lvalue = 1;

		veh_type_ptr->Max_Decel (lvalue);

		//---- convert transit boarding times to rounded steps ----

		veh_type_ptr->Loading (steps_per_second * veh_type_ptr->Loading ());
		veh_type_ptr->Unloading (steps_per_second * veh_type_ptr->Unloading ());
		veh_type_ptr->Min_Dwell (steps_per_second * veh_type_ptr->Min_Dwell ());
		veh_type_ptr->Max_Dwell (steps_per_second * veh_type_ptr->Max_Dwell ());

		return (true);
	}
	return (false);
}
