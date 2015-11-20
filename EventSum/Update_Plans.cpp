//*********************************************************
//	Update_Plans.cpp - update plan travel time data
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Update_Plans
//---------------------------------------------------------

void EventSum::Update_Plans (void)
{
	int nfile, diff, time, last_traveler, last_trip, last_leg, duration, cost, gcf;
	
	Event_Data *event_ptr, event_rec;

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

		if (!new_plan_file.Open (nfile)) {
			Error ("Opening %s", new_plan_file.File_Type ());
		}
		if (plan_file.Extend ()) {
			Show_Message ("Updating Plan File %s -- Plan", plan_file.Extension ());
		} else {
			Show_Message ("Updating Plan File -- Plan");
		}
		Set_Progress (10000);

		time = last_traveler = last_trip = last_leg = 0;
		event_ptr = NULL;

		//---- read each plan ----

		while (plan_file.Read ()) {
			Show_Progress ();

			event_rec.Traveler (plan_file.Traveler ());
			event_rec.Trip (plan_file.Trip ());
			event_rec.Leg (plan_file.Leg ());

			event_ptr = event_data.Get (&event_rec);

			if (event_ptr != NULL && event_ptr->Started () > event_ptr->Ended ()) {
				event_ptr = NULL;
			}
			if (event_ptr == NULL && event_rec.Leg () == 2) {
				HHList_Data *hhlist_ptr = hhlist_data.Get (event_rec.Household ());
				if (hhlist_ptr != 0) hhlist_ptr->Keep (true);
				continue;
			}

			//---- check for a new trip ----

			if (plan_file.Traveler () != last_traveler ||
				plan_file.Trip () != last_trip) {

				last_traveler = plan_file.Traveler ();
				last_trip = plan_file.Trip ();

				time = plan_file.Time ();

				if (event_ptr != NULL) {
					diff = event_ptr->Base_Start_Diff ();

					if (diff != 0) {
						time += diff;
						if (time < 0) time = 0;
					}
				}
			}
			plan_file.Time (time);

			//---- check for a travel legs ----

			if (event_ptr != NULL) {
				diff = event_ptr->Travel_Time () - event_ptr->Base_Time ();

				if (diff != 0) {
					duration = plan_file.Duration ();
					cost = plan_file.Cost ();
					gcf = plan_file.GCF ();

					time = plan_file.Duration () + diff;
					if (time < 0) time = 0;

					duration += cost;
					if (duration == 0) duration = 1;

					gcf = (int) ((double) gcf * (time + cost) / duration + 0.5);

					plan_file.Duration (time);
					plan_file.GCF (gcf);
				}
			}

			//---- update the arrival time ----

			time = plan_file.Time () + plan_file.Duration ();

			plan_file.Stop_Time (time);

			//---- write the updated plan to the new plan file ----

			if (!new_plan_file.Write (plan_file.Plan ())) {
				Error ("Writing %s", new_plan_file.File_Type ());
			}
		}
		End_Progress ();

		plan_file.Close ();
		new_plan_file.Close ();
	}

	//---- write summary statistics ----

	Break_Check (6);

	Write (2, "Number of Plan Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", plan_file.Num_Plans ());
	Write (1, "Number of Input Records = %d", plan_file.Num_Records ());
	Write (1, "Number of Input Travelers = %d", plan_file.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", plan_file.Num_Trips ());

	Break_Check (5);

	Write (2, "Number of Output Plans = %d", new_plan_file.Num_Plans ());
	Write (1, "Number of Output Records = %d", new_plan_file.Num_Records ());
	Write (1, "Number of Output Travelers = %d", new_plan_file.Num_Travelers ());
	Write (1, "Number of Output Trips = %d", new_plan_file.Num_Trips ());
	Print (1);
}
