//*********************************************************
//	Read_Veh_Map.cpp - read the vehicle type map file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Read_Vehicle_Map
//---------------------------------------------------------

void Emissions::Read_Vehicle_Map (void)
{
	int count, type, subtype;
	char *record, *str_ptr;

	Type_Map map_rec, *map_ptr;
	Label_Type *type_ptr, type_rec;
	Data_Group *data_ptr;
	Speed_Bin_File *file;

	//---- read the vehicle type map file ----

	count = 0;
	record = veh_map_file.Record ();

	while (veh_map_file.Read ()) {

		str_ptr = Get_Integer (record, &type);
		if (type == 0 || str_ptr == NULL) continue;

		map_rec.Type (type);

		str_ptr = Get_Integer (str_ptr, &type);
		if (str_ptr == NULL) continue;

		map_rec.SubType (type);

		Get_Integer (str_ptr, &type);
		if (type == 0) continue;

		map_rec.Map (type);

		if (!vehicle_map.Add (&map_rec)) {
			Error ("Adding Vehicle Type");
		}
		count++;
	}
	veh_map_file.Close ();

	//---- report the map data ----

	Print (2, "Number of Vehicle Type Maps = %d", count);
	Print (2, "Type SubType  Map  Label");

	for (map_ptr = vehicle_map.First (); map_ptr; map_ptr = vehicle_map.Next ()) {
		Print (1, " %2d    %2d     %2d", map_ptr->Type (), map_ptr->SubType (), map_ptr->Map ());

		type_rec.type = map_ptr->Map ();

		type_ptr = (Label_Type *) vehicle_type.Get (&type_rec);
		if (type_ptr != NULL) {
			Print (0, "   %s", type_ptr->label);
		}
	}

	//---- check vehicle types in speed bin files ----

	for (data_ptr = data_group.First (); data_ptr; data_ptr = data_group.Next ()) {

		file = data_ptr->Speed_File ();

		//---- check the vehicle type ----

		type = file->Vehicle_Type ();
		subtype = file->Veh_SubType ();

		if (vehicle_map.Map (type, subtype) == 0) {
			Warning ("Group %d Vehicle Type %d-%d is Not in the Type Map", data_ptr->Group (), type, subtype);
		}
	}
	if (Warnings () > 0) Show_Message (1);
}
