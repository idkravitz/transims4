//*********************************************************
//	Delay_Process.cpp - read the link delay file
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Delay_Processing
//---------------------------------------------------------

bool LinkDelay::Delay_Processing (Db_File *fh)
{
	int start, end, period, enter, exit, max_queue, num_fail, num;
	double density, max_density, queue, time, distance;

	TTime_Data *ttime_ptr;
	Delay_File *file;

	if (Demand_Service::Delay_Processing (fh)) {

		file = (Delay_File *) fh;

		//---- process a header line ----

		if (file->Nest ()) return (true);

		//---- get the time period and travel time ----

		if (file->Dbase_Format () == VERSION3) {
			end = file->Time ();
			enter = exit = file->Volume ();
			density = max_density = file->Vehicles ();
			queue = max_queue = num_fail = 0;
			time = distance = 0.0;
		} else {
			start = time_step.Step (file->Start_Time ());
			end = time_step.Step (file->End_Time ());
			enter = file->Enter ();
			exit = file->Exit ();
			density = file->Density ();
			max_density = file->Max_Density ();
			queue = file->Avg_Queue ();
			max_queue = file->Max_Queue ();
			num_fail = file->Cycle_Failure ();
			time = file->VHT ();
			distance = file->VMT ();
		}
		period = Time_Period (end - 1);		//---- subtract 1 to adjust end time ---

		ttime_ptr = ttime_data.Record ();

		//---- store the data ----

		if (ttime_ptr->Data (period, 3) == 0 && ttime_ptr->Data (period, 4) == 0) {
			ttime_ptr->Data (period, 3, enter);
			ttime_ptr->Data (period, 4, exit);
			ttime_ptr->Data (period, 5, Round (density * 10.0));
			ttime_ptr->Data (period, 6, Round (max_density * 10.0));
			ttime_ptr->Data (period, 7, Round (queue * 10.0));
			ttime_ptr->Data (period, 8, max_queue);
			ttime_ptr->Data (period, 9, num_fail);
			ttime_ptr->Data (period, 10, 1);
			ttime_ptr->Data (period, 11, Round (time));
			ttime_ptr->Data (period, 12, Round (distance));
		} else {
			ttime_ptr->Add_Data (period, 3, enter);
			ttime_ptr->Add_Data (period, 4, exit);
			ttime_ptr->Add_Data (period, 5, Round (density * 10.0));

			num = Round (max_density * 10.0);

			if (ttime_ptr->Data (period, 6) < num) {
				ttime_ptr->Data (period, 6, num);
			}
			ttime_ptr->Add_Data (period, 7, Round (queue * 10.0));

			if (ttime_ptr->Data (period, 8) < max_queue) {
				ttime_ptr->Data (period, 8, max_queue);
			}
			ttime_ptr->Add_Data (period, 9, num_fail);
			ttime_ptr->Add_Data (period, 10, 1);
			ttime_ptr->Add_Data (period, 11, Round (time));
			ttime_ptr->Add_Data (period, 12, Round (distance));
		}
		return (true);
	}
	return (false);
}
