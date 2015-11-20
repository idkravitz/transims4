//*********************************************************
//	Access_Process.cpp - read process links
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool PlanTrips::Access_Processing (Db_File *fh)
{
	int id;

	Access_Data *access_ptr;
	Location_Access location_rec, *location_ptr;

	if (Network_Service::Access_Processing (fh)) {

		access_ptr = access_data.New_Record ();

		//---- parking access ----

		if (access_ptr->From_Type () == PARKING_ID) {
			if (access_ptr->To_Type () == LOCATION_ID) {
				id = access_ptr->From_ID ();

				location_ptr = (Location_Access *) parking_access.Get (&id);

				if (location_ptr == NULL) {
					location_rec.id = id;
					location_rec.to_loc = access_ptr->To_ID ();
					location_rec.to_time = Resolve (access_ptr->Time ());
					location_rec.from_loc = 0;

					if (!parking_access.Add (&location_rec)) goto mem_error;
				} else {
					if (location_ptr->to_loc == 0) {
						location_ptr->to_loc = access_ptr->To_ID ();
						location_ptr->to_time = Resolve (access_ptr->Time ());
					}
				}
			}
		}
		if (access_ptr->To_Type () == PARKING_ID) {
			if (access_ptr->From_Type () == LOCATION_ID) {
				id = access_ptr->To_ID ();

				location_ptr = (Location_Access *) parking_access.Get (&id);

				if (location_ptr == NULL) {
					location_rec.id = id;
					location_rec.from_loc = access_ptr->From_ID ();
					location_rec.from_time = Resolve (access_ptr->Time ());
					location_rec.to_loc = 0;

					if (!parking_access.Add (&location_rec)) goto mem_error;
				} else {
					if (location_ptr->from_loc == 0) {
						location_ptr->from_loc = access_ptr->From_ID ();
						location_ptr->from_time = Resolve (access_ptr->Time ());
					}
				}
			}
		}

		//---- transit stop access ----

		if (access_ptr->From_Type () == STOP_ID) {
			if (access_ptr->To_Type () == LOCATION_ID) {
				id = access_ptr->From_ID ();

				location_ptr = (Location_Access *) stop_access.Get (&id);

				if (location_ptr == NULL) {
					location_rec.id = id;
					location_rec.to_loc = access_ptr->To_ID ();
					location_rec.to_time = Resolve (access_ptr->Time ());
					location_rec.from_loc = 0;

					if (!stop_access.Add (&location_rec)) goto mem_error;
				} else {
					if (location_ptr->to_loc == 0) {
						location_ptr->to_loc = access_ptr->To_ID ();
						location_ptr->to_time = Resolve (access_ptr->Time ());
					}
				}
			}
		}
		if (access_ptr->To_Type () == STOP_ID) {
			if (access_ptr->From_Type () == LOCATION_ID) {
				id = access_ptr->To_ID ();

				location_ptr = (Location_Access *) stop_access.Get (&id);

				if (location_ptr == NULL) {
					location_rec.id = id;
					location_rec.from_loc = access_ptr->From_ID ();
					location_rec.from_time = Resolve (access_ptr->Time ());
					location_rec.to_loc = 0;

					if (!stop_access.Add (&location_rec)) goto mem_error;
				} else {
					if (location_ptr->from_loc == 0) {
						location_ptr->from_loc = access_ptr->From_ID ();
						location_ptr->from_time = Resolve (access_ptr->Time ());
					}
				}
			}
		}
	}
	return (false);

mem_error:
	Error ("Adding Location Access Record");
	return (false);
}
