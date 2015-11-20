//*********************************************************
//	Access_Process.cpp - additional access processing
//*********************************************************

#include "ArcNet.hpp"

#include "Access_File.hpp"

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool ArcNet::Access_Processing (Db_File *fh)
{
	int id, type;

	XYZ_Point point;
	Offset_Data *offset_ptr;

	Access_File *file = (Access_File *) fh;

	if (First_Record ()) {
		arcview_access.Num_Points (2);

		if (location_offset.Num_Records () == 0 || parking_offset.Num_Records () == 0) {
			Error ("Activity Location and Parking data are required for Process Link processing");
		}
	}
	id = file->ID ();
	if (id == 0) return (false);
	
	type = End_Code (file->From_Type ());
	id = file->From_ID ();

	if (type == LOCATION_ID) {
		offset_ptr = location_offset.Get (id);
	} else if (type == PARKING_ID) {
		offset_ptr = parking_offset.Get (id);
		if (select_time && offset_ptr == NULL) return (false);
	} else if (type == STOP_ID) {
		if (!Network_File_Flag (TRANSIT_STOP)) return (false);
		offset_ptr = stop_offset.Get (id);
	} else {
		return (false);
	}
	if (offset_ptr == NULL) {
		Warning ("Process Link from %s %d could not be located", End_Code ((End_Type) type), id);
		point.x = point.y = point.z = 0.0;
	} else {
		point.x = offset_ptr->X ();
		point.y = offset_ptr->Y ();
		point.z = offset_ptr->Z ();
	}
	arcview_access.points.Set (1, &point);

	type = End_Code (file->To_Type ());
	id = file->To_ID ();

	if (type == LOCATION_ID) {
		offset_ptr = location_offset.Get (id);
	} else if (type == PARKING_ID) {
		offset_ptr = parking_offset.Get (id);
		if (select_time && offset_ptr == NULL) return (false);
	} else if (type == STOP_ID) {
		if (!Network_File_Flag (TRANSIT_STOP)) return (false);
		offset_ptr = stop_offset.Get (id);
	} else {
		return (false);
	}
	if (offset_ptr == NULL) {
		Warning ("Process Link to %s %d could not be located", End_Code ((End_Type) type), id);
		point.x = point.y = point.z = 0.0;
	} else {
		point.x = offset_ptr->X ();
		point.y = offset_ptr->Y ();
		point.z = offset_ptr->Z ();
	}
	arcview_access.points.Set (2, &point);

	arcview_access.Copy_Fields (file);

	if (!arcview_access.Write_Record ()) {
		Error ("Writing ArcView Process Link File");
	}
	return (false);
}
