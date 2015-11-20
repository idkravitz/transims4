//*********************************************************
//	Read_Schedule.cpp - read the transit schedule file
//*********************************************************

#include "Network_Service.hpp"

#include "Schedule_File.hpp"

//---------------------------------------------------------
//	Read_Schedule
//---------------------------------------------------------

void Network_Service::Read_Schedule (void)
{
	int i, route, time, stop, last_route, first_stop, nrun, nstop, index, npoints, tim;
	int num_runs, max_runs, num;
	bool stop_flag;

	Line_Data *line_ptr = NULL;

	Schedule_File *file = (Schedule_File *) Network_Db_File (TRANSIT_SCHEDULE);

	if (!Network_Data_Flag (TRANSIT_ROUTE)) {
		Error (file_error, "Transit Route", file->File_ID ());
	}
	stop_flag = Network_Data_Flag (TRANSIT_STOP);

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (TRANSIT_STOP) && Network_Option_Flag (TRANSIT_STOP)) {
			Error (file_error, "Transit Stop", file->File_ID ());
		}
	}

	Show_Message ("Reading %s -- Record", file->File_Type ());

	//---- scan for number of runs per route ----

	last_route = first_stop = nrun = nstop = npoints = num_runs = max_runs = 0;

	while (file->Read ()) {
		route = file->Route ();
		if (route == 0) continue;
			
		//---- check the stop data ----

		stop = file->Stop ();

		if (stop_flag) {
			index = stop_data.Get_Index (stop);

			if (index == 0) {
				Check_Message ("Stop %d on Route %d was Not Found in the Stop File", stop, route);
				continue;
			}
			if (Renumber ()) stop = index;
		}

		if (route != last_route) {
			if (last_route > 0) {
				if (!line_ptr->Runs (nrun)) goto mem_error;
				if (nrun > max_runs) max_runs = nrun;
				num_runs += nrun;
			}
			first_stop = stop;
			last_route = route;
			nrun = 1;
			nstop = 1;

			line_ptr = line_data.Get (route);

			if (line_ptr == NULL) {
				Check_Message ("Transit Route %d was Not Found in the Route File", route);
				continue;
			}
			stop = line_ptr->Stop (1);

			if (first_stop != stop) {
				if (Renumber ()) stop = stop_data [stop]->ID ();
				Check_Message ("Transit Route %d First Stop %d Schedule was Not Found", route, stop);
				continue;
			}
		} else {

			for (nstop++; nstop <= line_ptr->Stops (); nstop++) {
				if (stop == line_ptr->Stop (nstop)) break;
			}
			if (nstop > line_ptr->Stops ()) {
				nstop = 1;
				if (stop == first_stop) {
					nrun++;
				} else {
					Check_Message ("Stop %d was Not Found on Route %d", file->Stop (), route);
					continue;
				}
				nstop = i = 1;
			}
		}
	}
	if (last_route > 0) {
		if (!line_ptr->Runs (nrun)) goto mem_error;
		if (nrun > max_runs) max_runs = nrun;
		num_runs += nrun;
	}
	file->Rewind ();

	line_data.Num_Runs (num_runs);
	line_data.Max_Runs (max_runs);

	//---- store the transit schedule data ----

	Set_Progress ();
	last_route = first_stop = num = 0;
	nrun = nstop = 1;

	while (file->Read ()) {
		Show_Progress ();

		route = file->Route ();
		if (route == 0) continue;

		//---- check the stop id ----

		stop = file->Stop ();

		if (Renumber ()) {
			stop = stop_data.Get_Index (stop);
		}
		time = Round (file->Time ());

		//---- check for a new route ----

		if (route != last_route) {
			first_stop = stop;
			last_route = route;
			nrun = nstop = i = 1;

			line_ptr = line_data.Get (route);

		} else {

			//---- find the stop id on the route ----

			for (i=nstop; i <= line_ptr->Stops (); i++) {
				if (line_ptr->Stop (i) == stop) break;
			}
			if (i > line_ptr->Stops ()) {
				if (stop == first_stop) nrun++;
				nstop = i = 1;
			}
		}

		//---- add the time point ----

		line_ptr->Schedule (nrun, i, time);
		num++;

		//---- interpolate time points ----

		if (i > nstop) {
			if (nstop == 1) {
				Check_Message ("Stop %d is Not the First Stop for Route %d", file->Stop (), route);
				continue;
			}
			index = i;
			tim = line_ptr->Schedule (nrun, nstop-1);
			time = (time - tim) / (index - nstop + 1);

			for (i=nstop; i < index; i++) {
				tim += time;
				line_ptr->Schedule (nrun, i, tim);
				npoints++;
			}
			nstop = index;
		}
		nstop++;
	}
	End_Progress ();

	file->Close ();

	line_data.Schedule_Records (Progress_Count ());

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Transit Schedule Data Records = %d", num);
	}
	if (npoints > 0) {
		Print (1, "Number of Interpolated Time Points = %d", npoints);
	}
	if (num > 0) Network_Data_True (TRANSIT_SCHEDULE);
	return;

mem_error:
	Error ("Insufficient Memory for %d Runs on Route %d", nrun, last_route);
}
