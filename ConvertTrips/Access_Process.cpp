//*********************************************************
//	Access.cpp - additional access link processing
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool ConvertTrips::Access_Processing (Db_File *file)
{
	if (Network_Service::Access_Processing (file)) {
		Convert_Location *loc_ptr;

		Access_Data *acc_ptr = (Access_Data *) access_data.New_Record ();

		if (acc_ptr->From_Type () == LOCATION_ID && acc_ptr->To_Type () == PARKING_ID) {

			loc_ptr = (Convert_Location *) location_data.Get (acc_ptr->From_ID ());

			if (loc_ptr != NULL) {
				loc_ptr->Org_Parking (acc_ptr->To_ID ());
			}
		} else if (acc_ptr->From_Type () == PARKING_ID && acc_ptr->To_Type () == LOCATION_ID) {
		
			loc_ptr = (Convert_Location *) location_data.Get (acc_ptr->To_ID ());

			if (loc_ptr != NULL) {
				loc_ptr->Des_Parking (acc_ptr->From_ID ());
			}
		}
	}

	//---- don't save the record ----

	return (false);
}
