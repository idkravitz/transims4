//*********************************************************
//	Read_Subzone.cpp - Read the Subzone Data File
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Subzone
//---------------------------------------------------------

void LocationData::Read_Subzone (int num)
{
	int id, field, count;

	Subzone_Data data;
	Subzone *sub_ptr = subzone [num];
	Subzone_Map map;

	if (sub_ptr == NULL) return;

	if (sub_ptr->location_field == 0) return;

	if (!sub_ptr->data.Max_Records (sub_ptr->file.Num_Records ())) {
		Error ("Insufficient Memory for Subzone Data");
	}

	//---- read the subzone file ----

	Show_Message ("Reading %s -- Record", sub_ptr->file.File_Type ());
	Set_Progress (1000);

	count = 0;

	while (sub_ptr->file.Read ()) {
		Show_Progress ();

		id = sub_ptr->file.ID ();
		if (id == 0) continue;

		sub_ptr->file.Get_Field (sub_ptr->data_field, &field);
		if (field == 0 && sub_ptr->max_distance > 0) continue;

		//---- save the subzone data ----

		data.ID (id);
		if (subzone_map_flag) {
			map.zone = sub_ptr->file.Zone ();
			map.subzone = id;
			map.factor = 1.0;

			if (subzone_map.Get (&map) == NULL) {
				subzone_map.Add (&map);
			}
			data.Zone (0);
		} else {
			data.Zone (sub_ptr->file.Zone ());
		}
		data.X (Round (sub_ptr->file.X ()));
		data.Y (Round (sub_ptr->file.Y ()));
		data.Data (field);

		if (!sub_ptr->data.Add (&data)) {
			Error ("Adding Subzone Data");
		}
		count++;
	}
	End_Progress ();

	Print (2, "Number of Subzone #%d Data Records = %d", num+1, count);

	sub_ptr->file.Close ();
	return;
}
