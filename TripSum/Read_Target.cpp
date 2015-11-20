//*********************************************************
//	Read_Target.cpp - Read the Zone Target File
//*********************************************************

#include "TripSum.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Target
//---------------------------------------------------------

void TripSum::Read_Target (void)
{
	int zone, nveh;
	double trips;
	char *str_ptr;

	Location_Data *loc_ptr;

	//---- find the highest zone number ----

	nzone = 0;

	for (loc_ptr = location_data.First (); loc_ptr; loc_ptr = location_data.Next ()) {
		if (loc_ptr->Zone () > nzone) nzone = loc_ptr->Zone ();
	}
	nzone++;

	//---- allocate memory ----

	synod = new double * [nzone];
	first_veh = new int * [nzone];
	veh_count = new int * [nzone];

	for (zone=1; zone < nzone; zone++) {
		synod [zone] = new double [nzone];
		first_veh [zone] = new int [nzone];
		veh_count [zone] = new int [nzone];

		if (synod [zone] == NULL || first_veh [zone] == NULL || veh_count [zone] == NULL) goto mem_error;

		memset (first_veh [zone], '\0', nzone * sizeof (int));
		memset (veh_count [zone], '\0', nzone * sizeof (int));
	}
	org_target = new double [nzone];
	des_target = new double [nzone];

	org_total = new double [nzone];
	des_total = new double [nzone];

	nveh = vehicle_data.Num_Records () + 1;
	next_veh = new int [nveh];

	if (org_target == NULL || des_target == NULL ||  
		org_total == NULL || des_total == NULL || next_veh == NULL) goto mem_error;

	memset (org_target, '\0', nzone * sizeof (double));
	memset (des_target, '\0', nzone * sizeof (double));
	memset (next_veh, '\0', nveh * sizeof (int));

	//---- read the zone target file ----

	Show_Message ("Reading %s -- Record", zone_file.File_Type ());
	Set_Progress ();

	//---- store the household person count ----

	while (zone_file.Read ()) {
		Show_Progress ();

		str_ptr = Get_Integer (zone_file.Record (), &zone);

		if (zone <= 0 || zone >= nzone) continue;

		str_ptr = Get_Double (str_ptr, &trips);

		org_target [zone] = trips;

		Get_Double (str_ptr, &trips);

		des_target [zone] = trips;
	}
	End_Progress ();

	zone_file.Close ();
	
	Print (2, "Number of %s Records = %d", zone_file.File_Type (), Progress_Count ());
	return;

mem_error:
	Error ("Insufficient Memory");
}
