//*********************************************************
//	Build_Legs.cpp - Create the Transit Ridership File
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Build_Legs
//---------------------------------------------------------

void LineSum::Build_Legs (void)
{
	int stop, stops, link, links, route;
	int from_offset, to_offset, link_dir, distance;

	Link_Data *link_ptr;
	Driver_Data *driver_ptr;
	Stop_Data *stop_ptr;
	Rider_Data *rider_ptr;

	Show_Message ("Building Route Legs -- Route");
	Set_Progress (100);

	//---- process each line ----

	for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {
		Show_Progress ();

		route = rider_ptr->Route ();
		stops = rider_ptr->Stops ();

		//---- get the driver data ----

		driver_ptr = driver_data [rider_ptr->Driver ()];

		if (driver_ptr == NULL) {
			Warning ("Route %d was Not Found in the Transit Driver File", route);
			continue;
		}
		links = driver_ptr->Links ();

		//---- find the first link ----

		stop = 1;
		stop_ptr = stop_data.Get (rider_ptr->Stop (stop));

		if (stop_ptr == NULL) {
			Warning ("Route %d Stop %d was Not Found", route, rider_ptr->Stop (stop));
			continue;
		}
		distance = 0;
		rider_ptr->Distance (1, distance);

		//---- accumulate distance between stops ----

		for (link=1; link <= links && stop_ptr; link++) {

			link_dir = driver_ptr->Link_Dir (link);
			from_offset = 0;

			//---- get the link and travel time ----

			link_ptr = link_data.Get (driver_ptr->Link (link));

			if (link_ptr == NULL) {
				Warning ("Route %d Link %d was Not Found", route, driver_ptr->Link (link));
				continue;
			}

			while (stop_ptr->Link_Dir () == link_dir) {

				//---- calculate the stop offset ----

				if (stop_ptr->Dir () == 1) {
					to_offset = link_ptr->Length () - stop_ptr->Offset ();
				} else {
					to_offset = stop_ptr->Offset ();
				}

				if (stop > 1) {
					if (to_offset <= from_offset) {
						Warning ("Route %d Stops are Out of Order", route);
						break;
					}
					distance += to_offset - from_offset;

					rider_ptr->Distance (stop, distance);
				}

				//---- get the next stop ----

				if (++stop > stops) break;

				from_offset = to_offset;

				stop_ptr = stop_data.Get (rider_ptr->Stop (stop));

				if (stop_ptr == NULL) {
					Warning ("Route %d Stop %d was Not Found", route, rider_ptr->Stop (stop));
					break;
				}
			}

			//---- distance to the end of the link ----

			if (stop > 1) {
				distance += link_ptr->Length () - from_offset;
			}
		}
	}
	End_Progress ();
}
