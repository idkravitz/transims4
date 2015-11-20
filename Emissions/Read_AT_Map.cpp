//*********************************************************
//	Read_AT_Map.cpp - read the area type map file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Read_Area_Type_Map
//---------------------------------------------------------

void Emissions::Read_Area_Type_Map (void)
{
	int count, type, code;
	char *record, *str_ptr, buffer [FIELD_BUFFER];

	Label_Type *type_ptr, type_rec;
	Type_Map *map_ptr, map_rec;

	//---- read the area type map file ----

	count = 0;
	record = at_map_file.Record ();

	while (at_map_file.Read ()) {
		str_ptr = Get_Integer (record, &type);
		if (type == 0 || str_ptr == NULL) continue;

		if (type < 0) {
			Error ("Area Type %s is Out-of-Range", buffer);
		}
		str_ptr = Get_Integer (str_ptr, &code);
		if (code == 0) continue;

		map_rec.Type (type);
		map_rec.Map (code);

		if (!area_type_map.Add (&map_rec)) {
			Error ("Adding Area Type Type");
		}
		count++;
	}
	at_map_file.Close ();

	//---- report the map data ----

	Print (2, "Number of Area Type Maps = %d", count);
	Print (2, "Type  Map  Label");

	for (map_ptr = area_type_map.First (); map_ptr; map_ptr = area_type_map.Next ()) {
		Print (1, " %2d    %2d", map_ptr->Type (), map_ptr->Map ());

		type_rec.type = map_ptr->Map ();

		type_ptr = (Label_Type *) area_type.Get (&type_rec);
		if (type_ptr != NULL) {
			Print (0, "   %s", type_ptr->label);
		}
	}
}
