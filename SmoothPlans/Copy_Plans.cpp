//*********************************************************
//	Copy_Plans.cpp - copy plans to the new plan file
//*********************************************************

#include "SmoothPlans.hpp"

//---------------------------------------------------------
//	Copy_Plans
//---------------------------------------------------------

void SmoothPlans::Copy_Plans (void)
{
	int traveler, change, time, hour;

	Travel_Data *travel_ptr;
	Period_Index *period_ptr;

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

		//---- copy the plan record ----

		new_plan_file.Plan (plan_file.Plan ());	

		traveler = plan_file.Traveler ();

		travel_ptr = travel_data.Get (traveler);

		if (travel_ptr != NULL && travel_ptr->Change () != 0) {
			change = travel_ptr->Change () * increment;

			time = plan_file.Time () + change;
			if (time < 0) time = 0;

			new_plan_file.Time (time);

			time = plan_file.Stop_Time () + change;
			if (time < 0) time = 0;

			new_plan_file.Stop_Time (time);

			if (plan_file.Start_Type () == PARKING_ID && plan_file.End_Type () == PARKING_ID) {
				new_time++;
			}
		}

		//---- gather change report data ----

		if (change_flag) {
			change = new_plan_file.Time () - plan_file.Time ();

			hour = plan_file.Time () / 3600 + 1;

			period_ptr = period_index [hour];

			period_ptr->Add_Smoothed ();

			hour = new_plan_file.Time () / 3600 + 1;

			period_ptr = period_index [hour];

			period_ptr->Add_Change (change);
		}

		//---- save the plan ----

		if (!new_plan_file.Write ()) goto write_error;
	}
	End_Progress ();

	plan_file.Close ();
	new_plan_file.Close ();
	return;

write_error:
	Error ("Writing New Plan File");
}
