//*********************************************************
//	Run_Schedule.cpp - Write the Run Schedule File
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Run_Schedule
//---------------------------------------------------------

void LineSum::Run_Schedule (void)
{
	int run, runs, stops, period, per, num, time;
	int run_start, run_end, run_mid, sched_start, sched_end, sched_mid;

	Rider_Data *rider_ptr;

	Show_Message ("Writing %s -- Record", run_file.File_Type ());
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set [0];
	if (set_ptr == NULL) return;

	//---- initialize the report data ----

	num = set_ptr->Num_Periods ();

	//---- process each time period ----

	for (per=1; per <= num; per++) {

		run_start = run_end = run_mid = sched_start = sched_end = sched_mid = 0;

		//---- process each route ----

		for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
			Show_Progress ();

			//---- check the selection criteria ----

			if (!set_ptr->Select_Modes (rider_ptr->Mode ())) continue;
			if (!set_ptr->Select_Routes (rider_ptr->Route ())) continue;

			stops = rider_ptr->Stops ();
			runs = rider_ptr->Runs ();

			//---- process each run ----

			for (run=1; run <= runs; run++) {

				//---- run start ----

				time = rider_ptr->Time (run, 1);
				period = set_ptr->Time_Period (Resolve (time));
				if (period == per) run_start++;

				//---- run end ----

				time = rider_ptr->Time (run, stops);
				period = set_ptr->Time_Period (Resolve (time));
				if (period == per) run_end++;

				//---- run mid ----

				time = (rider_ptr->Time (run, 1) + rider_ptr->Time (run, stops)) / 2;
				period = set_ptr->Time_Period (Resolve (time));
				if (period == per) run_mid++;

				//---- schedule start ----

				time = rider_ptr->Schedule (run, 1);
				period = set_ptr->Time_Period (Resolve (time));
				if (period == per) sched_start++;

				//---- schedule end ----

				time = rider_ptr->Schedule (run, stops);
				period = set_ptr->Time_Period (Resolve (time));
				if (period == per) sched_end++;

				//---- schedule mid ----

				time = (rider_ptr->Schedule (run, 1) + rider_ptr->Schedule (run, stops)) / 2;
				period = set_ptr->Time_Period (Resolve (time));
				if (period == per) sched_mid++;
			}
		}

		//----- save the data ----

		run_file.Put_Field (1, (char *) (set_ptr->Time_Format (per)));
		run_file.Put_Field (2, run_start);
		run_file.Put_Field (3, run_end);
		run_file.Put_Field (4, run_mid);
		run_file.Put_Field (5, sched_start);
		run_file.Put_Field (6, sched_end);
		run_file.Put_Field (7, sched_mid);

		if (!run_file.Write ()) {
			Error ("Writing %s", run_file.File_Type ());
		}
	}
	End_Progress ();
}
