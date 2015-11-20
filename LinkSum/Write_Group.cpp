//*********************************************************
//	Write_Group.cpp - Write the Zone Group Travel File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Group
//---------------------------------------------------------

void LinkSum::Write_Group (void)
{
	int i, j, nrec, zone, group, index;
	char *str_ptr, buffer [FIELD_BUFFER];
	double *vmt, *vht;

	Show_Message ("Writing %s -- Record", group_file.File_Type ());
	Set_Progress ();

	vmt = new double [num_inc];
	vht = new double [num_inc];

	//---- set the data fields and file header ----

	group_file.Add_Field ("GROUP", INTEGER, 10);

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

		group_file.Add_Field (buffer, DOUBLE, 16, 2);

		buffer [0] = 'H';

		group_file.Add_Field (buffer, DOUBLE, 16, 2);
	}
	group_file.Write_Header ();

	//---- process each group ----

	nrec = 0;

	for (group=1; group <= zone_equiv.Num_Groups (); group++) {

		memset (vmt, '\0', num_inc * sizeof (double));
		memset (vht, '\0', num_inc * sizeof (double));

		//---- find the zones in the group ----

		for (zone = zone_list.First_Key (); zone > 0; zone = zone_list.Next_Key ()) {
			if (group != zone_equiv.Zone_Group (zone)) continue;

			index = zone_list.Record_Index ();

			for (i=0, j=2; i < num_inc; i++) {
				vmt [i] += zone_vmt [i] [index];
				vht [i] += zone_vht [i] [index];
			}
		}

		//---- save the group data ----

		group_file.Put_Field (1, group);

		for (i=0, j=2; i < num_inc; i++) {
			group_file.Put_Field (j++, vmt [i] / MILETOMETER);
			group_file.Put_Field (j++, vht [i] / 3600.0);
		}
		if (!group_file.Write ()) {
			Error ("Writing %s", group_file.File_Type ());
		}
		nrec++;
	}
	End_Progress ();

	Print (2, "Number of Group Travel Records = %d", nrec);

	delete [] vmt;
	delete [] vht;
}
