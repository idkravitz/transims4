//*********************************************************
//	Copy_Plans.cpp - copy plans to the new plan file
//*********************************************************

#include "AdjustPlans.hpp"

//---------------------------------------------------------
//	Copy_Plans
//---------------------------------------------------------

void AdjustPlans::Copy_Plans (void)
{
	int i, traveler, vehicle;
	bool trip_flag;
	char buffer [FIELD_BUFFER];

	Travel_Data *travel_ptr;
	Vehicle_Data vehicle_rec;

	//---- set the filename and extension ----

	if (plan_file.Extend ()) {
		Show_Message ("Copying Plan File %s -- Plan", plan_file.Extension ());
	} else {
		Show_Message ("Copying Plan File -- Plan");
	}
	Set_Progress (10000);

	//---- read each plan record ----

	while (plan_file.Read ()) {
		Show_Progress ();

		traveler = plan_file.Traveler ();
		trip_flag = (plan_file.Start_Type () == PARKING_ID && plan_file.End_Type () == PARKING_ID);

		travel_ptr = travel_data.Get (traveler);

		if (travel_ptr == NULL) {
			new_plan_file.Plan (plan_file.Plan ());	

			if (!new_plan_file.Write ()) goto write_error;

			//---- write the household records ----

			if (trip_flag && hhold_flag) {
				str_fmt (buffer, sizeof (buffer), "%d", (traveler / Traveler_Scale ()));

				if (!hhold_file.Write (buffer)) {
					Error ("Writing %s", hhold_file.File_Type ());
				}
			}
			continue;
		}

		//---- save the vehicle flag ----

		if (trip_flag) {
			vehicle = plan_file.Vehicle ();

			vehicle_rec.Vehicle (vehicle);
			vehicle_rec.Household (max_household);
			vehicle_rec.Location (max_vehicle);
			vehicle_rec.Passengers (travel_ptr->Weight ());

			if (!vehicle_data.Add (&vehicle_rec)) {
				Error ("Insufficient Memory for Vehicle Data");
			}
		}

		//---- delete flag ----

		if (travel_ptr->Weight () < 1) {
			if (trip_flag) num_deleted++;
			continue;
		}

		//---- save the plan ----

		new_plan_file.Plan (plan_file.Plan ());	

		if (!new_plan_file.Write ()) goto write_error;

		if (!trip_flag) continue;

		//---- write the household record ----

		if (hhold_flag) {
			str_fmt (buffer, sizeof (buffer), "%d", (traveler / Traveler_Scale ()));

			if (!hhold_file.Write (buffer)) {
				Error ("Writing %s", hhold_file.File_Type ());
			}
		}
		if (travel_ptr->Weight () == 1) continue;

		//---- replicated the trip plan ----

		num_replicated++;

		for (i=1; i < travel_ptr->Weight (); i++) {
			max_household++;
			max_vehicle++;

			new_plan_file.Traveler ((max_household * Traveler_Scale ()) + 1);
			new_plan_file.Vehicle (max_vehicle);

			if (!new_plan_file.Write ()) goto write_error;
			num_added++;

			//---- write the household record ----

			if (hhold_flag) {
				str_fmt (buffer, sizeof (buffer), "%d", (max_household));

				if (!hhold_file.Write (buffer)) {
					Error ("Writing %s", hhold_file.File_Type ());
				}
			}
		}
	}
	End_Progress ();

	plan_file.Close ();
	new_plan_file.Close ();
	return;

write_error:
	Error ("Writing New Plan File");
}
