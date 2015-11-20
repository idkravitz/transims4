//*********************************************************
//	Segment_Schedule.cpp - modify the transit schedules
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Segment_Schedule
//---------------------------------------------------------

void Reschedule::Segment_Schedule (void)
{
	int i, j, stop, stops, run, runs, link, links, route, count, num, lnk, dir;
	int shift, time1, time2, stop1, link_dir;
	bool first, select;

	num = link_equiv.Max_Group ();

	Driver_Data *driver_ptr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Integer_List *group;

	Schedule_File *new_schedule = (Schedule_File *) Network_Db_File (NEW_TRANSIT_SCHEDULE);

	Show_Message ("Writing %s -- Route", new_schedule->File_Type ());
	Set_Progress (100);
	count = 0;

	link_factor -= 1.0;

	//---- read each route ----

	for (line_ptr = line_data.First_Key (); line_ptr; line_ptr = line_data.Next_Key ()) {
		Show_Progress ();

		route = line_ptr->Route ();
		new_schedule->Route (route);

		stops = line_ptr->Stops ();
		runs = line_ptr->Runs ();

		//---- check the selection criteria ----

		if ((route_flag && !select_routes.In_Range (route)) || 
			!select_modes [line_ptr->Mode ()]) goto output;

		//---- scan for links in the link group ----

		driver_ptr = driver_data [line_ptr->Driver ()];

		if (driver_ptr == NULL) {
			Warning ("Route %d was Not Found in the Transit Driver File", route);
			continue;
		}
		links = driver_ptr->Links ();

		//---- find the first link ----

		stop1 = stop = 1;
		stop_ptr = stop_data.Get (line_ptr->Stop (stop));

		if (stop_ptr == NULL) {
			Warning ("Route %d Stop %d was Not Found", route, line_ptr->Stop (1));
			continue;
		}
		for (i=1; i <= links; i++) {
			if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (i)) break;
		}
		if (i > links) {
			Warning ("Route %d Stops and Driver Links are Incompatible", route);
			continue;
		}
		first = false;
		shift = 0;

		for (; i <= links; i++) {
			link = driver_ptr->Link (i);
			dir = driver_ptr->Dir (i);
			link_dir = driver_ptr->Link_Dir (i);

			//---- process each link gorup ----

			select = false;

			for (j=1; j <= num; j++) {
				group = link_equiv.Group_List (j);
				if (group == NULL) continue;

				for (lnk = group->First (); lnk; lnk = group->Next ()) {
					if (lnk < 0) {
						if (dir != 1 || link != abs (lnk)) continue;
					} else {
						if (dir != 0 || link != lnk) continue;
					}
					select = true;
					break;
				}
				if (select) break;
			}

			//---- processes the stops on the link

			while (stop_ptr->Link_Dir () == link_dir) {
				if (select) {
					if (!first) {
						first = true;
						stop1 = stop;
					} else {
						for (run = 1; run <= runs; run++) {
							time1 = line_ptr->Schedule (run, stop1);
							time2 = line_ptr->Schedule (run, stop);

							shift = (int) ((time2 - time1) * link_factor + 0.5);
							line_ptr->Penalty (run, stop, shift);
						}
					}
				} else {
					first = false;
				}

				//---- get the next stop ----

				if (++stop > stops) break;

				stop_ptr = stop_data.Get (line_ptr->Stop (stop));

				if (stop_ptr == NULL) {
					Warning ("Route %d Stop %d was Not Found", route, line_ptr->Stop (stop));
					break;
				}
			}
			if (select && !first) {
				stop1 = stop;
				first = true;
			}
		}

		//---- save the schedule records ----
output:
		for (run=1; run <= runs; run++) {
			shift = 0;

			for (stop=1; stop <= stops; stop++) {
				new_schedule->Stop (line_ptr->Stop (stop));
				time1 = line_ptr->Schedule (run, stop);
				time2 = line_ptr->Penalty (run, stop);

				if (time2 != 0) shift = time2;

				new_schedule->Time (Resolve (time1 + shift));

				if (!new_schedule->Write ()) {
					Error ("Writing %s", new_schedule->File_Type ());
				}
				count++;
			}
		}
	}
	End_Progress ();

	Write (2, "Number of %s Records = %d", new_schedule->File_Type (), count);
}
