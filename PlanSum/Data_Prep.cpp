//*********************************************************
//	Data_Prep.cpp - prepare data items
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Data_Preparation
//---------------------------------------------------------

void PlanSum::Data_Preparation (void)
{
	//---- initialize stop data ----

	if (Report_Flag (SUM_STOPS) || Report_Flag (STOP_GROUP)) {
		Transfer_Data *transfer_ptr;

		for (transfer_ptr = transfer_data.First (); transfer_ptr; transfer_ptr = transfer_data.Next ()) {
			transfer_ptr->Walk_Board (0);
			transfer_ptr->Bus_Board (0);
			transfer_ptr->Rail_Board (0);
			transfer_ptr->Drive_Board (0);
			transfer_ptr->Walk_Alight (0);
			transfer_ptr->Bus_Alight (0);
			transfer_ptr->Rail_Alight (0);
			transfer_ptr->Drive_Alight (0);
		}
	}

	//---- build parking and stop location lists ----

	if (skim_flag || trip_flag) {
		int *id, index;
		bool transit_flag = false;

		Access_Data *access_ptr;

		if (!park_location.Num_Records (parking_data.Num_Records ())) {
			Error ("Insufficient Memory for Parking Location Map");
		}
		if (stop_data.Num_Records () > 0) {
			if (!stop_location.Num_Records (stop_data.Num_Records ())) {
				Error ("Insufficient Memory for Stop Location Map");
			}
			transit_flag = true;
		}
		if (access_data.Num_Records () == 0) {
			Warning ("Process Link Data was Not Available for Location Maps");
		} else {
			for (access_ptr = access_data.First (); access_ptr; access_ptr = access_data.Next ()) {
				if (access_ptr->From_Type () == LOCATION_ID) {
					if (access_ptr->To_Type () == PARKING_ID) {
						index = parking_data.Get_Index (access_ptr->To_ID ());
						id = park_location.Record (index);
						if (id != NULL) *id = access_ptr->From_ID ();
					} else if (transit_flag && access_ptr->To_Type () == STOP_ID) {
						index = stop_data.Get_Index (access_ptr->To_ID ());
						id = stop_location.Record (index);
						if (id != NULL) *id = access_ptr->From_ID ();
					}
				} else if (access_ptr->To_Type () == LOCATION_ID) {
					if (access_ptr->From_Type () == PARKING_ID) {
						index = parking_data.Get_Index (access_ptr->From_ID ());
						id = park_location.Record (index);
						if (id != NULL) *id = access_ptr->To_ID ();
					} else if (transit_flag && access_ptr->From_Type () == STOP_ID) {
						index = stop_data.Get_Index (access_ptr->From_ID ());
						id = stop_location.Record (index);
						if (id != NULL) *id = access_ptr->To_ID ();
					}
				}
			}
		}
	}
}
