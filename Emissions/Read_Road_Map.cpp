//*********************************************************
//	Read_Road_Map.cpp - read the road type map file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Read_Road_Map
//---------------------------------------------------------

void Emissions::Read_Road_Map (void)
{
	int count, type, subtype, map;
	char *record, *str_ptr, buffer [FIELD_BUFFER];

	Type_Map map_rec, *map_ptr;
	Label_Type *type_ptr, type_rec;

	//---- read the road type map file ----

	count = 0;
	record = road_map_file.Record ();

	while (road_map_file.Read ()) {
		str_ptr = Get_Token (record, buffer, sizeof (buffer));
		if (buffer [0] == '\0') continue;

		str_ptr = Get_Integer (str_ptr, &subtype);
		if (str_ptr == NULL) continue;

		Get_Integer (str_ptr, &map);
		if (map == 0) continue;

		if (buffer [0] >= '0' && buffer [0] <= '9') {
			type = atoi (buffer);
		} else {
			type = Facility_Code (buffer);
		}
		if (type == 0 || type > EXTERNAL) {
			Error ("Facility Type %s is Out-of-Range", buffer);
		}
		map_rec.Type (type);
		map_rec.SubType (subtype);
		map_rec.Map (map);

		if (!road_type_map.Add (&map_rec)) {
			Error ("Adding Road Type");
		}
		count++;
	}
	road_map_file.Close ();

	//---- report the map data ----

	Print (2, "Number of Road Type Maps = %d", count);
	Print (2, "Facility_Type  Area_Type   Road_Type  Label");

	for (map_ptr = road_type_map.First (); map_ptr; map_ptr = road_type_map.Next ()) {
		type = map_ptr->Type ();

		Print (1, "%-15.15s    %2d          %2d", Facility_Code ((Facility_Type) type), map_ptr->SubType (), map_ptr->Map ());

		type_rec.type = map_ptr->Map ();

		type_ptr = (Label_Type *) road_type.Get (&type_rec);
		if (type_ptr != NULL) {
			Print (0, "     %s", type_ptr->label);
		}
	}
}
