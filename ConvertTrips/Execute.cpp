//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ConvertTrips::Execute (void)
{
	int hhold1, vehicle1;

	File_Group *group;

	//---- compile the trip time scripts ----

	for (group = file_group.First (); group != NULL; group = file_group.Next ()) {
		group_number = group->Group ();

		if (group->Script_File () != NULL) {
			if (Report_Flag (SCRIPT_REPORT)) {
				Header_Number (SCRIPT_REPORT);

				if (!Break_Check (10)) {
					Print (1);
					Page_Header ();
				}
			}
			if (!group->Compile (Report_Flag (SCRIPT_REPORT))) {
				Error ("Compiling Trip Time Script");
			}
			if (Report_Flag (STACK_REPORT)) {
				Header_Number (STACK_REPORT);

				group->Stack_Report (false);
			}
			Header_Number (0);
		}
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

	//---- travel time skim ----

	if (skim_flag) {
		Skim_Processing ();
	}

	//---- process each trip table -----

	if (hhold_id <= max_hh_in) hhold_id = 100 * ((max_hh_in / 100) + 1);
	if (vehicle_id <= max_veh_in) vehicle_id = 100 * ((max_veh_in / 100) + 1);

	hhold1 = hhold_id;
	vehicle1 = vehicle_id;

	for (group = file_group.First (); group != NULL; group = file_group.Next ()) {
		Write (2, "Processing %s", group->Trip_File ()->File_Type ());

		default_factor.Factor (1.0);
		default_factor.Bucket (0.45);

		group->Factor_Periods (Report_Flag (TIME_EQUIV));
		if (Report_Flag (TIME_EQUIV)) Print (1);

		if (group->Trip_Factor () != NULL) {
			Factor_Processing (group);
		} else {
			Show_Message (1);
		}
		Time_Processing (group);
		Table_Processing (group);
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

	Write (2, "Total Number of Trips Read     = %d", tot_trips);
	Write (1, "Total Number of Trips Written  = %d", num_trip);
	if (tot_add > 0.5) {
		Write (1, "Total Number of Trips Added    = %.0lf", tot_add);
	}
	if (tot_del > 0.5) {
		Write (1, "Total Number of Trips Deleted  = %.0lf", tot_del);
	}
	Write (1, "Number of Trips Not Allocated  = %d", tot_errors);

	Write (2, "Number of Households Generated = %d", (hhold_id - hhold1));
	Write (1, "Number of Persons Generated    = %d", nperson);
	Write (1, "Number of Vehicles Generated   = %d", (vehicle_id - vehicle1));

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ConvertTrips::Page_Header (void)
{
	switch (Header_Number ()) {
		case SCRIPT_REPORT:		//---- Conversion Script ----
			Print (1, "Trip Time Script #%d", group_number);
			Print (1);
			break;
		case STACK_REPORT:		//---- Conversion Stack ----
			Print (1, "Trip Time Stack #%d", group_number);
			Print (1);
			break;
		default:
			break;
	}
}
