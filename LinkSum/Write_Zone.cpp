//*********************************************************
//	Write_Zone.cpp - Write the Zone Travel File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Zone
//---------------------------------------------------------

void LinkSum::Write_Zone (void)
{
	int i, j, nrec, zone, index;
	char *str_ptr, buffer [FIELD_BUFFER];

	Show_Message ("Writing %s -- Record", zone_file.File_Type ());
	Set_Progress ();

	//---- set the data fields and file header ----

	zone_file.Add_Field ("ZONE", INTEGER, 10);

	for (i=1; i <= num_inc; i++) {
		str_ptr = time_periods.Range_Format (i);

		buffer [0] = 'M';

		for (j=1; *str_ptr != '\0'; str_ptr++) {
			if (*str_ptr == ':') continue;
			if (*str_ptr == '.' && *(str_ptr+1) == '.') { 
				buffer [j++] = '_';
				str_ptr++;
			} else {
				buffer [j++] = *str_ptr;
			}
		}
		buffer [j] = '\0';

		zone_file.Add_Field (buffer, DOUBLE, 16, 2);

		buffer [0] = 'H';

		zone_file.Add_Field (buffer, DOUBLE, 16, 2);
	}
	zone_file.Write_Header ();

	//---- process each zone ----

	nrec = 0;

	for (zone = zone_list.First_Key (); zone > 0; zone = zone_list.Next_Key ()) {
		index = zone_list.Record_Index ();

		zone_file.Put_Field (1, zone);

		for (i=0, j=2; i < num_inc; i++) {
			zone_file.Put_Field (j++, zone_vmt [i] [index] / MILETOMETER);
			zone_file.Put_Field (j++, zone_vht [i] [index] / 3600.0);
		}
		if (!zone_file.Write ()) {
			Error ("Writing %s", zone_file.File_Type ());
		}
		nrec++;
	}
	End_Progress ();

	Print (2, "Number of Zone Travel Records = %d", nrec);
}
