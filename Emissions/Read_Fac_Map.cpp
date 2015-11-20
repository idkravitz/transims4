//*********************************************************
//	Read_Fac_Map.cpp - read the facility type map file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Read_Facility_Map
//---------------------------------------------------------

void Emissions::Read_Facility_Map (void)
{
	int count, type, code;
	char *record, *str_ptr, buffer [FIELD_BUFFER];

	Label_Type *type_ptr, type_rec;

	//---- read the facility type map file ----

	count = 0;
	record = fac_map_file.Record ();

	while (fac_map_file.Read ()) {
		str_ptr = Get_Token (record, buffer, sizeof (buffer));
		if (buffer [0] == '\0') continue;

		Get_Integer (str_ptr, &code);
		if (code == 0) continue;

		if (buffer [0] >= '0' && buffer [0] <= '9') {
			type = atoi (buffer);
		} else {
			type = Facility_Code (buffer);
		}
		if (type == 0 || type > EXTERNAL) {
			Error ("Facility Type %s is Out-of-Range", buffer);
		}
		facility_map [type] = code;
		count++;
	}
	fac_map_file.Close ();

	//---- report the map data ----

	Print (2, "Number of Facility Type Maps = %d", count);
	Print (2, "Type Text             Map  Label");

	for (type = 1; type <= EXTERNAL; type++) {
		Print (1, " %2d  %-15.15s  %2d", type, Facility_Code ((Facility_Type) type), facility_map [type]);

		type_rec.type = facility_map [type];

		type_ptr = (Label_Type *) facility_type.Get (&type_rec);
		if (type_ptr != NULL) {
			Print (0, "   %s", type_ptr->label);
		}
	}
}
