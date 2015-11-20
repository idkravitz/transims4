//*********************************************************
//	Read_Subzone_Map.cpp - Read the Subzone Zone File
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Subzone_Map
//---------------------------------------------------------

void LocationData::Read_Subzone_Map (void)
{
	int i, id, zone, num_field, field;
	double factor;
	int id_field, zone_field [20], fac_field [20];
	char name1 [20], name2 [20], name3 [20], name4 [20];

	Subzone_Map map;

	//---- get the field names ----

	id_field = subzone_zone_file.Required_Field ("ID", "SUBZONE", "RECORD", "REC_ID", "SUB_ID");

	num_field = 0;

	for (i=1; i <= 20; i++) {
		str_fmt (name1, sizeof (name1), "ZONE_%d", i);
		str_fmt (name2, sizeof (name2), "TAZ_%d", i);
		str_fmt (name3, sizeof (name3), "ZONE%d", i);
		str_fmt (name4, sizeof (name4), "TAZ%d", i);

		field = subzone_zone_file.Optional_Field (name1, name2, name3, name4);

		if (field == 0) break;
		num_field = i;
		zone_field [i-1] = field;

		str_fmt (name1, sizeof (name1), "ZONE_%d_PER", i);
		str_fmt (name2, sizeof (name2), "TAZ_%d_PER", i);
		str_fmt (name3, sizeof (name3), "ZONE%d_PER", i);
		str_fmt (name4, sizeof (name4), "TAZ%d_PER", i);

		field = subzone_zone_file.Optional_Field (name1, name2, name3, name4);

		if (field == 0) {
			str_fmt (name1, sizeof (name1), "ZONE_%d_FAC", i);
			str_fmt (name2, sizeof (name2), "TAZ_%d_FAC", i);
			str_fmt (name3, sizeof (name3), "ZONE%d_FAC", i);
			str_fmt (name4, sizeof (name4), "TAZ%d_FAC", i);

			field = subzone_zone_file.Optional_Field (name1, name2, name3, name4);
		}
		if (field == 0) {
			Error ("A Subzone Factor Field Name for %d was Not Found", 
				subzone_zone_file.Field (zone_field [i-1])->Name ());
		}
		fac_field [i-1] = field;
	}
	if (num_field == 0) {
		Error ("No Subzone Zone Field Names were Found");
	}

	//---- read the subzone zone file ----

	Show_Message ("Reading %s -- Record", subzone_zone_file.File_Type ());
	Set_Progress (1000);

	while (subzone_zone_file.Read ()) {
		Show_Progress ();

		subzone_zone_file.Get_Field (id_field, &id);
		if (id == 0) continue;

		map.subzone = id;

		for (i=0; i < num_field; i++) {
			subzone_zone_file.Get_Field (zone_field [i], &zone);

			if (zone == 0) break;

			subzone_zone_file.Get_Field (fac_field [i], &factor);

			if (factor == 0.0) continue;
			if (factor > 1.0) factor /= 100.0;
			
			map.zone = zone;
			map.factor = factor;

			if (!subzone_map.Add (&map)) {
				Error ("Adding Subzone Map Data");
			}
		}
	}
	End_Progress ();

	Print (2, "Number of Subzone Zone Data Records = %d", Progress_Count ());

	subzone_zone_file.Close ();
}
