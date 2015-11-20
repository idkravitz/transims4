//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ConvertTours.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ConvertTours::Execute (void)
{
	int hhold1, vehicle1;

	File_Group *group;

	//---- compile the tour group script ----

	if (script_flag) {
		Show_Message (1);

		script_base.File_ID ("GROUP");
		script_base.Dbase_Format (BINARY);
		script_base.File_Access (MODIFY);

		script_base.Add_Field ("GROUP_OUT", INTEGER, 2, 0, -1, true);
		script_base.Add_Field ("GROUP_IN", INTEGER, 2, 0, -1, true);
		script_base.Add_Field ("DIURNAL_OUT", INTEGER, 2, 0, -1, true);
		script_base.Add_Field ("DIURNAL_IN", INTEGER, 2, 0, -1, true);
		script_base.Add_Field ("MODE_OUT", INTEGER, 2, 0, -1, true);
		script_base.Add_Field ("MODE_IN", INTEGER, 2, 0, -1, true);
		script_base.Add_Field ("WORK_CODE", INTEGER, 2, 0, -1, true);

		if (!group_script.Set_Files (2, &tour_file_db, &script_base)) {
			Error ("Initializing Tour Group Script Files");
		}
		group_script.Random_Seed (random_org.Seed () + 4);

		if (Report_Flag (SCRIPT_REPORT)) {
			Header_Number (SCRIPT_REPORT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		if (!group_script.Compile (&script_file, Report_Flag (SCRIPT_REPORT))) {
			Error ("Compiling Tour Group Script");
		}
		if (Report_Flag (STACK_REPORT)) {
			Header_Number (STACK_REPORT);

			group_script.Print_Commands (Report_Flag (STACK_REPORT));
		}
		Header_Number (0);
	}
	Show_Message (1);

	for (group = file_group.First (); group != NULL; group = file_group.Next ()) {
		Time_Processing (group);
	}

	//---- read the network ----

	Demand_Service::Execute ();

	//---- generate the zone map ----

	Zone_Location ();

	//---- check the parking lots ----

	Parking_Check ();

	//---- check the vehicle types ----

	if (Demand_File_Flag (VEHICLE_TYPE)) {
		for (group = file_group.First (); group != NULL; group = file_group.Next ()) {
			if (veh_type_data.Get (group->Type (), group->SubType ()) == NULL) {
				Error ("Vehicle Type %d-%d was Not Found in the Vehicle Type Flle", group->Type (), group->SubType ());
			}
		}
	}

	//---- read the household list ----

	if (hhlist_flag) {
		Read_HHList ();
	}

	//---- process each trip table -----

	if (hhold_id <= max_hh_in) hhold_id = 100 * ((max_hh_in / 100) + 1);
	if (vehicle_id <= max_veh_in) vehicle_id = 100 * ((max_veh_in / 100) + 1);

	hhold1 = hhold_id;
	vehicle1 = vehicle_id;

	//---- travel time skim ----

	if (skim_flag) {
		Skim_Processing ();
	}

	//---- read the tour file ----

	Tour_Processing ();

	//---- diurnal allocation file ----

	if (diurnal_flag) {
		Diurnal_Results ();
	}

	//---- summarize the results ----

	if (trip_copy > 0 || hhold_copy > 0 || pop_copy > 0 || veh_copy > 0) {
		Break_Check (5);

		Write (2, "Number of Trip Records Copied      = %d", trip_copy);
		Write (1, "Number of Household Records Copied = %d", hhold_copy);
		Write (1, "Number of Person Records Copied    = %d", pop_copy);
		Write (1, "Number of Vehicle Records Copied   = %d", veh_copy);
	}

	Break_Check (10);

	Write (2, "Total Number of Tours Read     = %d", tot_trips);
	Write (1, "Total Number of Trips Written  = %d", num_trip);
	if (tot_add > 0.5) {
		Write (1, "Total Number of Trips Added    = %.0lf", tot_add);
	}
	if (tot_del > 0.5) {
		Write (1, "Total Number of Trips Deleted  = %.0lf", tot_del);
	}
	Write (1, "Number of Trips Not Allocated  = %d", tot_errors);

	Write (2, "Number of Households Generated = %d", nhhold);
	Write (1, "Number of Persons Generated    = %d", nperson);
	Write (1, "Number of Vehicles Generated   = %d", (vehicle_id - vehicle1));

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ConvertTours::Page_Header (void)
{
	switch (Header_Number ()) {
		case SCRIPT_REPORT:		//---- Conversion Script ----
			Print (1, "Tour Group Script");
			Print (1);
			break;
		case STACK_REPORT:		//---- Conversion Stack ----
			Print (1, "Tour Group Stack");
			Print (1);
			break;
		default:
			break;
	}
}
