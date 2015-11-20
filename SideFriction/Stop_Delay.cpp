//*********************************************************
//	Stop_Delay - estimate transit stop delay 
//*********************************************************

#include "SideFriction.hpp"

#include "In_Polygon.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Stop_Delay
//---------------------------------------------------------

void SideFriction::Stop_Delay (void)
{
	int stop, stops, run, runs, dir, route, period, lane, ln, time;
	int stop_id, offset, low, high, length, start, link_dir, num;
	char buffer [FIELD_BUFFER];

	Stop_Data *stop_ptr;
	Line_Data *line_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Offset_Data *offset_ptr;
	Link_Use_Data *use_ptr;

	Show_Message ("Estimate Transit Stop Delay -- Route");
	Set_Progress (100);

	//---- read each route ----

	for (line_ptr = line_data.First_Key (); line_ptr; line_ptr = line_data.Next_Key ()) {
		Show_Progress ();

		route = line_ptr->Route ();

		stops = line_ptr->Stops ();
		runs = line_ptr->Runs ();

		if (route_flag) {
			if (!select_routes.In_Range (route)) continue;
		}
		if (!select_modes [line_ptr->Mode ()]) continue;

		for (stop=1; stop <= stops; stop++) {

			stop_id = line_ptr->Stop (stop);

			//---- check for stop in the subarea ----

			if (subarea_flag) {
				offset_ptr = stop_offset.Get (stop_id);
				if (offset_ptr == NULL) continue;

				if (!In_Polygon (offset_ptr->X (), offset_ptr->Y (), &select_subarea.points)) continue;
			}

			//---- find the stop link ----

			stop_ptr = stop_data.Get (stop_id);

			if (stop_ptr == NULL) {
				Warning ("Route %d Stop %d was Not Found", route, line_ptr->Stop (stop));
				continue;
			}

			link_ptr = link_data.Get (stop_ptr->Link ());

			if (link_ptr == NULL) {
				Warning ("Stop Link %d was Not Found", stop_ptr->Link ());
				continue;
			}

			if (stop_ptr->Dir () == 0) {
				dir = link_ptr->AB_Dir ();
				offset = stop_ptr->Offset ();
				low = link_ptr->Aoffset ();
				high = link_ptr->Boffset ();
			} else {
				dir = link_ptr->BA_Dir ();
				offset = link_ptr->Length () - stop_ptr->Offset ();
				low = link_ptr->Boffset ();
				high = link_ptr->Aoffset ();
			}
			dir_ptr = dir_data [dir];

			if (dir_ptr == NULL) {
				Warning ("Stop Link %d Direction %d was Not Found", stop_ptr->Link (), stop_ptr->Dir ());
				continue;
			}
			link_dir = stop_ptr->Link_Dir ();
			lane = dir_ptr->Left () + dir_ptr->Thru ();
			high = link_ptr->Length () - high;

			length = stop_length;
			offset -= length;
			if (offset < low) {
				offset = low;
				if (offset + length > high) length = high - offset;
			} else if (offset + length > high) {
				offset = high - length;
				if (offset < low) {
					offset = low;
					length = high - low;
				}
			}

			//---- add a lane blockage for each run ----

			for (run=1; run < runs; run++) {

				time = Resolve (line_ptr->Schedule (run, stop));

				period = transit_periods.In_Index (time);

				if (period == 0) continue;

				start = time - Resolve (stop_times [period]);
				if (start < 0) start = 0;

				ln = lane;

				if (dir_ptr->TOD_List () > 0) {
					for (num = dir_ptr->TOD_List (); num; num = use_ptr->TOD_List ()) {
						use_ptr = link_use_data [num]; 

						if (use_ptr->Length () > 0) continue;
						if (use_ptr->Low_Lane () > lane || lane > use_ptr->High_Lane ()) continue;
						if (Use_Permission (use_ptr->Use (), BUS)) continue;
						if (use_ptr->End () < Round (start) || use_ptr->Start () > Round (time)) continue;
						ln = use_ptr->Low_Lane () - 1;
						break;
					}
				}
				if (ln <= dir_ptr->Left ()) continue;

				new_file->Link (stop_ptr->Link ());
				new_file->Dir (stop_ptr->Dir ());
				new_file->Lane (ln);
				new_file->Use (Use_Code (Use_Code ("BUS")));
				new_file->Type (Restrict_Code (ONLY));
				new_file->Start (time_step.Format_Step (start));
				new_file->End (time_step.Format_Step (time));
				new_file->Offset (UnRound (offset));
				new_file->Length (UnRound (length));

				str_fmt (buffer, sizeof (buffer), "Route %d Stop %d Run %d", route, stop_id, run); 

				new_file->Notes (buffer);

				if (!new_file->Write ()) {
					Error ("Writing %s", new_file->File_Type ());
				}
				new_use++;
			}
		}
	}
	End_Progress ();
}

