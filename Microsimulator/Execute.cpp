//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Microsimulator::Execute (void)
{
	bool stat, first;
	int nfile, plan_step, nproblem;
	
	Traveler_Data *traveler_ptr;

	//---- read the network ----

	Demand_Service::Execute ();

	if (veh_cells > 0) {
		Print (1, "Number of Vehicle Cells = %d", veh_cells);
	}

	//---- prepare simulation data ----

	Data_Processing ();

	//---- read the plan file ----

	step = start_time_step;
	Show_Message ("Initializing First Time Step -- Plan");
	first = true;
	Set_Progress (50000);

	Break_Check (3);
	Print (2, "Simulation Started at Time %s", time_step.Format_Step (step));

	nfile = 0;
	if (!plan_file.Open (nfile++)) {
		File_Error ("No Plan Files were Found", plan_file.Filename ());
	}
	stat = plan_file.Read ();

	//---- process each time step ----

	for (; step <= end_time_step; step++) {
		step_flag = ((step % steps_per_second) == 0);

		summary_output.Output_Check (step);
		problem_output.Output_Check (step);
		turn_output.Output_Check (step);
		occupancy_output.Output_Check ();
		speed_bin_output.Output_Check (step);

		if (step_flag) {
			if (tod_flag) {
				Update_Network ();
			}
			if (signal_flag) {
				Update_Signals ();
			}
		}

		//---- reserve priority queue locations ----

		Priority_Queue ();
		
		//---- consider lane changes ----

		Change_Lanes ();

		//---- load vehicles in the travel queue ----

		Check_Queue ();

		//---- processing for each second ----

		if (step_flag) {
			while (stat) {
				plan_step = time_step.Step (plan_file.Time ());

				if (first) Show_Progress ();

				if (plan_step > end_time_step) {
					stat = false;
					break;
				}
				if (plan_step > step) break;

				if (plan_step >= start_time_step) {

					//---- process a new travel plan ----

					Process_Plan ();

				} else {

					//---- update the vehicle/traveler location ----

					Reposition_Plan ();

				}
				stat = plan_file.Read ();

				if (!stat) {
					if (plan_file.Open (nfile)) {
						nfile++;
						stat = plan_file.Read ();
					}
				}
			}
			if (first) {
				End_Progress ();
				Show_Message ("Processing Time of Day");
				Set_Progress (60);
				first = false;
			}
			Show_Progress (time_step.Format_Step (step));

			if (transit_flag) {
				Update_Transit ();
			}
		}

		//---- process the network traffic ----

		Process_Step ();

		if (num_vehicles > max_vehicles) {
			max_vehicles = num_vehicles;
			max_time = step;
		}

		//---- reposition the vehicles ----

		if (!Move_Vehicles ()) {
			if (stat == 0) break;
		}
	}
	End_Progress (time_step.Format_Step (step));

	snapshot_output.Close ();
	summary_output.Close ();
	occupancy_output.Close ();
	problem_output.Close ();
	turn_output.Close ();
	speed_bin_output.Close ();

	//---- write the ridership data ----

	ridership_output.Output ();
	ridership_output.Close ();

	Print (1, "Simulation Ended at Time %s", time_step.Format_Step (step));

	Break_Check (6);
	Print (2, "Number of Plan Files = %d", nfile);
	Print (1, "Number of Input Plans = %d", plan_file.Num_Plans ());
	Print (1, "Number of Input Records = %d", plan_file.Num_Records ());
	Print (1, "Number of Input Travelers = %d", plan_file.Num_Travelers ());
	Print (1, "Number of Input Trips = %d", plan_file.Num_Trips ());

	Break_Check (7);
	Write (2, "Number of Vehicle Trips Processed = %d", ntrips);
	Write (1, "Number of Vehicle Trips Started   = %d", nstarted);
	if (ntrips > 0) Write (0, " (%.1lf%%)", 100.0 * nstarted / ntrips);
	Write (1, "Number of Vehicle Trips Completed = %d", ncompleted);
	if (ntrips > 0) Write (0, " (%.1lf%%)", 100.0 * ncompleted / ntrips);

	if (tot_complete > 0) {
		Print (2, "Total Hours for Completed Vehicle Trips = %.1lf hours", tot_hours / 3600.0);
		Print (1, "Average Travel Time for Completed Trips = %.2lf minutes", tot_hours / (tot_complete * 60.0));
	}
	if (transit_flag) {
		Break_Check (4);
		Write (2, "Number of Transit Runs Processed = %d", nruns);
		Write (1, "Number of Transit Runs Started   = %d", nrun_start);
		if (nruns > 0) Write (0, " (%.1lf%%)", 100.0 * nrun_start / nruns);
		Write (1, "Number of Transit Runs Completed = %d", nrun_end);
		if (nruns > 0) Write (0, " (%.1lf%%)", 100.0 * nrun_end / nruns);

		Break_Check (4);
		Write (2, "Number of Transit Legs Processed = %d", ntransit);
		Write (1, "Number of Transit Legs Started   = %d", nboard);
		if (ntransit > 0) Write (0, " (%.1lf%%)", 100.0 * nboard / ntransit);
		Write (1, "Number of Transit Legs Completed = %d", nalight);
		if (ntransit > 0) Write (0, " (%.1lf%%)", 100.0 * nalight / ntransit);
	}

	Break_Check (7);
	Print (2, "Number of Priority Movements = %d", npriority);
	Print (1, "Number of Reserved Movements = %d", nmove);
	Print (1, "Number of Required Lane Changes = %d", nrequired);
	Print (1, "Number of Swapping Lane Changes = %d", nswap);
	Print (1, "Number of Long Diagonal Changes = %d", ndiagonal);
	Print (1, "Number of Reserved Lane Changes = %d", nreserve);
	Print (1, "Number of Look-Ahead Lane Changes = %d", noptional);

	Print (2, "Maximum Number of Vehicles on the Network = %d at %s", max_vehicles, time_step.Format_Step (max_time));
	
	//---- print the problem report ----

	nproblem = 0;

	for (traveler_ptr = traveler_data.First (); traveler_ptr != NULL; traveler_ptr = traveler_data.Next ()) {
		if (traveler_ptr->Problem ()) nproblem++;
	}

	if (nproblem) {
		Write (2, "Number of Travelers with Problems = %d (%.1lf%%)", nproblem,
			(100.0 * nproblem / traveler_data.Num_Records ()));
	}
	Report_Problems ();

	Exit_Stat (DONE);
}
