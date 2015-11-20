//*********************************************************
//	Read_Veh_Type.cpp - Read the Vehicle Type File
//*********************************************************

#include "Demand_Service.hpp"

#include "Veh_Type_File.hpp"

//---------------------------------------------------------
//	Read_Veh_Type
//---------------------------------------------------------

void Demand_Service::Read_Veh_Type (void)
{
	Db_File *file = Demand_Db_File (VEHICLE_TYPE);

	//---- store the vehicle type data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();
	
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Veh_Type_Processing (file)) {
			if (!veh_type_data.Add ()) {
				Error ("Adding Record to the Vehicle Type Data Array");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = veh_type_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Vehicle Type Data Records = %d", num);
	}
	veh_type_data.Optimize ();

	if (num > 0) Demand_Data_True (VEHICLE_TYPE);
}

//---------------------------------------------------------
//	Veh_Type_Processing
//---------------------------------------------------------

bool Demand_Service::Veh_Type_Processing (Db_File *fh)
{
	int type, sub_type;
	double dvalue;
	char *str_ptr;

	Veh_Type_Data *veh_type_ptr;

	Veh_Type_File *file = (Veh_Type_File *) fh;

	//---- reserve memory ----

	if (veh_type_data.Max_Records () == 0) {
		if (!veh_type_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- check the vehicle type ----

	type = file->Type ();		
	if (type <= 0) return (false);

	//---- get the data record ----

	veh_type_ptr = veh_type_data.New_Record ();
	if (veh_type_ptr == NULL) goto mem_error;

	//---- set the record key ----

	veh_type_ptr->Type (type);

	sub_type = file->Sub_Type ();
	veh_type_ptr->Sub_Type (sub_type);

	//---- vehicle length ----

	dvalue = file->Length ();

	if (dvalue < 2.0) {
		Error ("Length %.1lf for Vehicle Type %d-%d is Out of Range", 
			dvalue, type, sub_type);
	}
	veh_type_ptr->Length (Round (dvalue));

	//---- maximum speed ----

	dvalue = file->Max_Speed ();

	if (dvalue < 2.0) {
		Error ("Maximum Speed %.1lf for Vehicle Type %d-%d is Out of Range",
			dvalue, type, sub_type);
	}
	veh_type_ptr->Max_Speed (Round (dvalue));

	//---- maximum acceleration ----

	dvalue = file->Max_Accel ();

	if (dvalue < 0.2) {
		Error ("Maximum Acceleration %.1lf for Vehicle Type %d-%d is Out of Range", 
			dvalue, type, sub_type);
	}
	veh_type_ptr->Max_Accel (Round (dvalue));

	//---- maximum deceleration ----

	dvalue = file->Max_Decel ();

	if (dvalue < 0.2) {
		veh_type_ptr->Max_Decel (veh_type_ptr->Max_Accel ());
	} else {
		veh_type_ptr->Max_Decel (Round (dvalue));
	}

	//---- vehicle use code ----

	str_ptr = file->Use ();

	if (*str_ptr == '\0') {
		veh_type_ptr->Use (Veh_Type_Map (type));
	} else {
		veh_type_ptr->Use (Veh_Use_Code (str_ptr));
	}
	if (veh_type_ptr->Use () == CAR) veh_type_ptr->Use (SOV);

	//---- vehicle capacity ----

	veh_type_ptr->Capacity (file->Capacity ());
	
	if (veh_type_ptr->Capacity () < 1) {
		Error ("Capacity %d for Vehicle Type %d-%d is Out of Range", 
			veh_type_ptr->Capacity (), type, sub_type);
	}

	//---- loading rate (seconds per passenger) ----

	dvalue = file->Loading ();

	if (dvalue <= 0.0) {
		dvalue = 3.0;
	}
	veh_type_ptr->Loading (dvalue * Resolution ());

	if (veh_type_ptr->Loading () == 0) {
		veh_type_ptr->Loading (1);
	}

	//---- unloading rate (seconds per passenger) ----

	dvalue = file->Unloading ();

	if (dvalue <= 0.0) {
		dvalue = 2.0;
	}
	veh_type_ptr->Unloading (dvalue * Resolution ());

	if (veh_type_ptr->Unloading () == 0) {
		veh_type_ptr->Unloading (1);
	}

	//---- loading method ----

	str_ptr = file->Method ();

	if (*str_ptr == '\0') {
		veh_type_ptr->Method (false);
	} else if (strcmp (str_ptr, "PARALLEL") == 0) {
		veh_type_ptr->Method (true);
	} else if (strcmp (str_ptr, "SERIAL") == 0) {
		veh_type_ptr->Method (false);
	} else {
		Error ("Loading Method \"%s\" was Not Recognized", str_ptr);
	}

	//---- dwell time ----

	veh_type_ptr->Min_Dwell (Round (file->Min_Dwell ()));
	veh_type_ptr->Max_Dwell (Round (file->Max_Dwell ()));

	return (true);

mem_error:
	Error ("Insufficient Memory for Vehicle Type Data");
	return (false);
}
