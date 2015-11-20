//*********************************************************
//	Setup_Riders.cpp - setup the transit ridership data
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Setup_Riders
//---------------------------------------------------------

void ArcRider::Setup_Riders (void)
{
	int ndir, nstop, *list;

	Dir_Data *dir_ptr;
	Offset_Data *stop_ptr;
	
	ndir = dir_data.Num_Records () + 1;
	nstop = stop_offset.Num_Records () + 1;

	//---- allocate ridership memory ----

	if (rider_flag) {
		load_data = new int [ndir];
		enter_data = new int [ndir];
		stop_list = new int [nstop];

		if (load_data == NULL || enter_data == NULL || stop_list == NULL) {
			Error ("Insufficient Memory for Ridership Data");
		}
		memset (load_data, '\0', ndir * sizeof (int));
		memset (enter_data, '\0', ndir * sizeof (int));
		memset (stop_list, '\0', nstop * sizeof (int));

		//---- initialize the link direction data ----

		for (dir_ptr = dir_data.First (); dir_ptr; dir_ptr = dir_data.Next ()) {
			dir_ptr->Stop_List (0);
		}

		//---- process each stop ----

		list = stop_list;

		for (stop_ptr = stop_offset.First (); stop_ptr; stop_ptr = stop_offset.Next ()) {
			list++;

			dir_ptr = dir_data.Get (stop_ptr->Link_Dir ());

			if (dir_ptr != NULL) {
				*list = dir_ptr->Stop_List ();
				dir_ptr->Stop_List (stop_offset.Record_Index ());
			}
		}
	}

	//---- allocate boarding and alighting memory ----

	if (rider_flag || on_off_flag) {
		board_data = new int [nstop];
		alight_data = new int [nstop];
		start_data = new int [nstop];
		end_data = new int [nstop];


		if (board_data == NULL || alight_data == NULL || start_data == NULL || end_data == NULL) {
			Error ("Insufficient Memory for On-Off Data");
		}
		memset (board_data, '\0', nstop * sizeof (int));
		memset (alight_data, '\0', nstop * sizeof (int));
		memset (start_data, '\0', nstop * sizeof (int));
		memset (end_data, '\0', nstop * sizeof (int));
	}

	//---- update the leg load ----

	rider_data.Summarize_Riders ();
}

