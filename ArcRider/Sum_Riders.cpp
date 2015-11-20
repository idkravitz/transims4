//*********************************************************
//	Sum_Riders - sum the transit route between stops
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Sum_Riders
//---------------------------------------------------------

void ArcRider::Sum_Riders (void)
{
	int i, *board, *alight, *start, *end, stop, run, index, index1, index2, schedule;
	int num, link, dir, link_dir, num_links, load, runs;
	bool route_flag, tod_flag, link_flag, select;

	Link_Data *link_ptr;
	Rider_Data *rider_ptr;
	Offset_Data *stop_ptr;
	Driver_Data *driver_ptr;
	Data_Range *range;

	tod_flag = (times.Num_Ranges () > 0);	
	route_flag = (transit_routes.Num_Ranges () > 0);
	link_flag = (links.Num_Records () > 0);

	//---- process each route ----

	Show_Message ("Summing Ridership Data -- Record");
	Set_Progress (1000);

	for (rider_ptr = rider_data.First (); rider_ptr; rider_ptr = rider_data.Next ()) {
		Show_Progress ();
			
		if (mode_flag && !travel_modes [rider_ptr->Mode ()]) continue;
		if (route_flag && !transit_routes.In_Range (rider_ptr->Route ())) continue;

		//---- sum the ridership by link ----

		driver_ptr = driver_data [rider_ptr->Driver ()];

		num_links = driver_ptr->Links ();

		if (link_flag) {

			//---- check the link selection criteria ----

			select = false;

			for (range = (Data_Range *) links.First (); range && !select; range = (Data_Range *) links.Next ()) {

				num = range->Num_Ranges ();
				index1 = index2 = 0;

				for (i=1; i <= num_links; i++) {
					link = driver_ptr->Link (i);

					index2 = range->In_Index (link);
					if (!index2) continue;

					if (index2 != index1 + 1) {
						index2 = 0;
						break;
					}
					if (index2 == num) {
						select = true;
						break;
					}
					index1 = index2;
				}
			}
			if (!select) continue;
		}

		//---- save the route ridership by link ----

		stop = 1;
		stop_ptr = stop_offset.Get (rider_ptr->Stop (stop));
		index = stop_offset.Record_Index ();

		board = board_data + index;
		alight = alight_data + index;
		start = start_data + index;
		end = end_data + index;
		load = runs = 0;

		for (i=1; i <= num_links; i++) {
			link = driver_ptr->Link (i);
			link_dir = driver_ptr->Link_Dir (i);

			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) continue;

			dir = driver_ptr->Dir (i);

			if (dir == 0) {
				dir = link_ptr->AB_Dir ();
			} else {
				dir = link_ptr->BA_Dir ();
			}
			if (dir == 0) continue;

			if (rider_flag) {
				load_data [dir] += load;
				enter_data [dir] += runs;
			}
			while (stop_ptr->Link_Dir () == link_dir) {

				load = runs = 0;

				for (run=1; run <= rider_ptr->Runs (); run++) {
					if (tod_flag) {
						schedule = Resolve (rider_ptr->Schedule (run, stop));
						if (!times.In_Range (schedule)) continue;
					}
					load += rider_ptr->Riders (run, stop);
					*board += rider_ptr->Board (run, stop);
					*alight += rider_ptr->Alight (run, stop);
					runs += 1;
					if (stop == 1) *start += 1;
					if (stop == rider_ptr->Stops ()) *end += 1;
				}
				if (++stop > rider_ptr->Stops ()) break;

				stop_ptr = stop_offset.Get (rider_ptr->Stop (stop));
				index = stop_offset.Record_Index ();

				board = board_data + index;
				alight = alight_data + index;
				start = start_data + index;
				end = end_data + index;
			}
		}
	}
	End_Progress ();
}
