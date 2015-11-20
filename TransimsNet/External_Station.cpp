//*********************************************************
//	Externals.cpp - Process External Stations
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	External_Station
//---------------------------------------------------------

void TransimsNet::External_Station (Link_Name *link_ptr)
{
	int zone, dir, best_lanes, use_code;

	Link_Data *lnk_ptr;
	Dir_Connect *dir_ptr, *best_ptr, *zone_ptr;
	Connection *connect_ptr;

	if (link_ptr->Anode () > high_zone) {

		//---- flip the link orientation ----

		dir = link_ptr->AB_Dir ();
		link_ptr->AB_Dir (link_ptr->BA_Dir ());
		link_ptr->BA_Dir (dir);

        zone = link_ptr->Anode ();
		link_ptr->Anode (link_ptr->Bnode ());
		link_ptr->Bnode (zone);
	}
	use_code = 0;

	//---- get the AB link direction ----

	dir = link_ptr->AB_Dir ();

	if (dir > 0) {
		zone_ptr = dir_data [dir];

		connect_ptr = zone_ptr->To_Thru ();

		if (connect_ptr == NULL) {
			best_ptr = NULL;
			best_lanes = 0;

			for (connect_ptr = zone_ptr->To_First (); connect_ptr; connect_ptr = zone_ptr->To_Next ()) {
				if (connect_ptr->Dir () != dir) {
					dir_ptr = dir_data [connect_ptr->Dir ()];

					if (dir_ptr->Thru () > best_lanes) {
						best_ptr = dir_ptr;
						best_lanes = dir_ptr->Thru ();
					}
				}
			}
			dir_ptr = best_ptr;
		} else {
			dir_ptr = dir_data [connect_ptr->Dir ()];
		}

		//---- copy the attributes ----

		if (dir_ptr != NULL) {
			zone_ptr->Thru (dir_ptr->Thru ());
			zone_ptr->Speed (dir_ptr->Speed ());
			zone_ptr->Capacity (dir_ptr->Capacity ());

			lnk_ptr = link_data.Get (dir_ptr->Link ());
			use_code = use_code | lnk_ptr->Use ();
		}
	}

	//---- get the BA link direction ----

	dir = link_ptr->BA_Dir ();

	if (dir > 0) {
		zone_ptr = dir_data [dir];

		connect_ptr = zone_ptr->From_Thru ();

		if (connect_ptr == NULL) {
			best_ptr = NULL;
			best_lanes = 0;

			for (connect_ptr = zone_ptr->From_First (); connect_ptr; connect_ptr = zone_ptr->From_Next ()) {
				if (connect_ptr->Dir () != dir) {
					dir_ptr = dir_data [connect_ptr->Dir ()];

					if (dir_ptr->Thru () > best_lanes) {
						best_ptr = dir_ptr;
						best_lanes = dir_ptr->Thru ();
					}
				}
			}
			dir_ptr = best_ptr;
		} else {
			dir_ptr = dir_data [connect_ptr->Dir ()];
		}

		//---- copy the attributes ----

		if (dir_ptr != NULL) {
			zone_ptr->Thru (dir_ptr->Thru ());
			zone_ptr->Speed (dir_ptr->Speed ());
			zone_ptr->Capacity (dir_ptr->Capacity ());

			lnk_ptr = link_data.Get (dir_ptr->Link ());
			use_code = use_code | lnk_ptr->Use ();
		}
	}

	//---- check for transit usage ----

	if (!Use_Permission (use_code, WALK) && (Use_Permission (use_code, BUS) || Use_Permission (use_code, RAIL))) {
		use_code = use_code | Use_Code ("WALK");
	}
	link_ptr->Use (use_code);

	//---- reestablish positioning ----

	link_data.Get (link_ptr->Link ());
}
