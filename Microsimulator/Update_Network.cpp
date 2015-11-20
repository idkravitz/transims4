//*********************************************************
//	Update_Network.cpp - update time of day restrictions
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Update_Network
//---------------------------------------------------------

void Microsimulator::Update_Network (void)
{
	int i, j, k, dir, time, use_code;

	Link_Data *link_ptr;
	Grid_Data *grid_ptr;
	Link_Use_Data *use_ptr;

	//---- set the access restrictions ----

	time = Round (step / steps_per_second);

	use_code = Use_Code ("CAR/TRUCK");

	for (link_ptr = link_data.First (); link_ptr != NULL; link_ptr = link_data.Next ()) {

		//---- car/truck links only ----

		if ((link_ptr->Use () & use_code) == 0) continue;

		//---- process each direction ----

		for (i=0; i < 2; i++) {
			dir = (i) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
			if (dir == 0) continue;

			grid_ptr = grid_data [dir];

			//---- check access restrictions by time of day ----

			if (grid_ptr->TOD_List () > 0) {
				grid_ptr = grid_data [dir];

				j = grid_ptr->Lanes ();

				for (k=1; k <= j; k++) {
					grid_ptr->Lane_Use (k, link_ptr->Use ());
					grid_ptr->TOD_Use (k, 0);
				}

				for (j = grid_ptr->TOD_List (); j; j = use_ptr->TOD_List ()) {
					use_ptr = link_use_data [j]; 

					if (use_ptr->Start () <= time && time < use_ptr->End ()) {
						for (k=use_ptr->Low_Lane (); k <= use_ptr->High_Lane (); k++) {
							if (use_ptr->High_Cell () > 0) {
								use_ptr->TOD_Use (grid_ptr->TOD_Use (k));
								grid_ptr->TOD_Use (k, j);
							} else {
								grid_ptr->Lane_Use (k, use_ptr->Use ());
							}
						}
					}
				}
			}
		}
	}
}
