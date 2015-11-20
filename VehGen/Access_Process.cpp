//*********************************************************
//	Access.cpp - additional access link processing
//*********************************************************

#include "VehGen.hpp"

#include "Access_File.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool VehGen::Access_Processing (Db_File *fh)
{
	Parking_Lot parking_rec;

	Access_File *file = (Access_File *) fh;

	parking_rec.location = End_Code (file->From_Type ());

	if (parking_rec.location == LOCATION_ID) {

		parking_rec.parking = End_Code (file->To_Type ());

		if (parking_rec.parking == PARKING_ID) {

			parking_rec.location = file->From_ID ();
			parking_rec.parking = file->To_ID ();

			if (parking_lot.Get (&parking_rec) == NULL) {

				if (!parking_lot.Add (&parking_rec)) {
					Error ("Adding Parking Location");
				}
			}
		}
	}

	//---- don't save the record ----

	return (false);
}
