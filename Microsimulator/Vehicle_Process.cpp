//*********************************************************
//	Vehicle_Process.cpp - Read the Vehicle File
//*********************************************************

#include "Microsimulator.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Vehicle_Processing
//---------------------------------------------------------

bool Microsimulator::Vehicle_Processing (Db_File *fh)
{
	int num;

	if (First_Record ()) {
		if (veh_type_array == NULL) {
			Error ("Vehicle Type Data are Required for Vehicle Processing");
		}
		Vehicle_File *file = (Vehicle_File *) fh;

		num = file->Estimate_Records ();

		if (transit_flag) {
			num += rider_data.Num_Runs ();

			Compress_Vehicles (false);
		}
		if (!vehicle_data.Max_Records (num)) {
			Error ("Insufficient Memory for Vehicle Data");
		}
	}

	if (Demand_Service::Vehicle_Processing (fh)) {

		Msim_Vehicle *vehicle_ptr;
		Veh_Type_Data *veh_type_ptr;

		vehicle_ptr = vehicle_data.New_Record ();
		
		//vehicle_ptr->Driver (vehicle_ptr->Household ());
		vehicle_ptr->Driver (0);

		//---- convert length to cells ----

		veh_type_ptr = veh_type_data.Get (vehicle_ptr->Type (), vehicle_ptr->Sub_Type ());
		
		if (veh_type_ptr == NULL) {
			Error ("Vehicle %d Type %d-%d was Not Found in the Vehicle Type File", 
				vehicle_ptr->Vehicle (), vehicle_ptr->Type (), vehicle_ptr->Sub_Type ());
		}
		vehicle_ptr->Type (veh_type_data.Record_Index ());
		vehicle_ptr->Sub_Type (0);

		if (speed_method) {
			num = (veh_type_ptr->Length () + half_cell) / cell_size;
			if (num < 1) num = 1;
		} else {
			num = veh_type_ptr->Length ();
		}
		veh_cells += num;

		if (!vehicle_ptr->Num_Cells (num)) {
			Error ("Adding Vehicle %d Cell Data", vehicle_ptr->Vehicle ());
		}
		return (true);
	}
	return (false);
}
