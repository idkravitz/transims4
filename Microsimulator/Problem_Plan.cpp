//*********************************************************
//	Problem_Plan.cpp - Write a Plan to the Problem File
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Problem_Plan
//---------------------------------------------------------

void Microsimulator::Problem_Plan (int problem, Travel_Plan_Data *plan_ptr, int dir, int lane, int cell)
{
	int traveler;

	if (plan_ptr->Mode () != DRIVER_MODE) {
		traveler = plan_ptr->Traveler ();

		Traveler_Data *traveler_ptr = traveler_data [traveler];

		traveler_ptr->Problem (true);

		traveler = traveler_ptr->Traveler ();
	} else {
		traveler = plan_ptr->Vehicle ();
	}

	if (problem_flag) {
		int link = 0;
		int direction = 0;
		Rider_Data *rider_ptr = NULL;
		
		problem_file.Mode (plan_ptr->Mode ());		
		problem_file.Trip (plan_ptr->Trip ());
		problem_file.Leg (plan_ptr->Leg ());
		problem_file.Problem (problem);
		problem_file.Time (time_step.Format_Step (step));

		if (dir > 0) {
			Grid_Data *grid_ptr = grid_data [dir];
			if (grid_ptr != NULL) {
				Link_Data *link_ptr = link_data [grid_ptr->Link ()];

				if (link_ptr != NULL) {
					link = link_ptr->Link ();
				}
				direction = grid_ptr->Dir ();
			}
		}
		problem_file.Link (link);
		problem_file.Dir (direction);
		problem_file.Lane (lane);
		problem_file.Offset (UnRound (cell * cell_size));
		problem_file.Route (0);

		if (plan_ptr->Mode () == DRIVER_MODE) {
			Msim_Vehicle *vehicle_ptr = vehicle_data [traveler];

			if (vehicle_ptr != NULL) {
				traveler = vehicle_ptr->Vehicle ();

				problem_file.Household (rider_data.Route (traveler));
				problem_file.Person (rider_data.Run (traveler));
				problem_file.Route (rider_data.Route (traveler));
			} else {
				problem_file.Household (traveler);
				problem_file.Person (0);
			}
		} else {
			problem_file.Household (traveler / Traveler_Scale ());
			problem_file.Person (traveler % Traveler_Scale ());

			if (plan_ptr->Mode () == TRANSIT_MODE) {
				rider_ptr = rider_data [plan_ptr->Vehicle ()];

				if (rider_ptr != NULL) {
					problem_file.Route (rider_ptr->Route ());
				}
			}
		}
		problem_file.Start (time_step.Format_Step (plan_ptr->Start_Time ()));
		problem_file.Origin (0);

		if (plan_ptr->Start_Type () == PARKING_ID) {
			Parking_Data *parking_ptr = parking_data [plan_ptr->Start_Index ()];
			if (parking_ptr != NULL) {
				problem_file.Origin (parking_ptr->ID ());
			} else {
				problem_file.Origin (plan_file.Start_ID ());
			}
		} else if (plan_ptr->Start_Type () == STOP_ID) {
			int index = plan_ptr->Start_Index ();

			if (rider_ptr != NULL) {
				index = rider_ptr->Stop (index);
			}
			Stop_Data *stop_ptr = stop_data [index];
			if (stop_ptr != NULL) {
				problem_file.Origin (stop_ptr->ID ());
			} else {
				problem_file.Origin (plan_file.Start_ID ());
			}
		}
		problem_file.Arrive (time_step.Format_Step (plan_ptr->End_Time ()));
		problem_file.Destination (0);

		if (plan_ptr->End_Type () == PARKING_ID) {
			Parking_Data *parking_ptr = parking_data [plan_ptr->End_Index ()];
			if (parking_ptr != NULL) {
				problem_file.Destination (parking_ptr->ID ());
			} else {
				problem_file.Destination (plan_file.Start_ID ());
			}
		} else if (plan_ptr->End_Type () == STOP_ID) {
			int index = plan_ptr->End_Index ();

			if (rider_ptr != NULL) {
				index = rider_ptr->Stop (index);
			}
			Stop_Data *stop_ptr = stop_data [index];
			if (stop_ptr != NULL) {
				problem_file.Destination (stop_ptr->ID ());
			} else {
				problem_file.Destination (plan_file.End_ID ());
			}
		}
		if (!problem_file.Write ()) {
			Error ("Writing %s", problem_file.File_Type ());
		}
	}

	if (dir > 0 && problem_output.In_Range (step, dir)) {
		problem_output.Problem (problem);
		problem_output.Link_Dir (dir);

		problem_output.Summarize (step);
	}
}
