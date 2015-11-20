//*********************************************************
//	Write_Share.cpp - process the share file
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Write_Share_File
//---------------------------------------------------------

void ModeChoice::Write_Share_File (Travel_Leg &leg)
{
	char buffer [FIELD_BUFFER];

	share_file.Put_Field (1, leg.hhold);
	share_file.Put_Field (2, leg.person);
	share_file.Put_Field (3, leg.trip);
	share_file.Put_Field (4, leg.purpose);
	share_file.Put_Field (5, time_step.Format_Step (leg.start));
	share_file.Put_Field (6, leg.origin);
	share_file.Put_Field (7, leg.org_zone);
	share_file.Put_Field (8, time_step.Format_Step (leg.end));
	share_file.Put_Field (9, leg.destination);
	share_file.Put_Field (10, leg.des_zone);

	if (mode_flag) {
		Mode_Group *mode_ptr;

		for (mode_ptr = mode_group.First (); mode_ptr; mode_ptr = mode_group.Next ()) {
			if (mode_ptr->Mode () == leg.mode) {
				share_file.Put_Field (11, mode_ptr->Label ());
				break;
			}
		}
		if (mode_ptr == NULL) {
			str_fmt (buffer, sizeof (buffer), "Mode #%d", leg.mode);

			share_file.Put_Field (11, buffer);
		}
		for (mode_ptr = mode_group.First (); mode_ptr; mode_ptr = mode_group.Next ()) {
			share_file.Put_Field (11 + mode_group.Record_Index (), mode_ptr->Share ());
		}
	} else {
		str_fmt (buffer, sizeof (buffer), "Mode #%d", leg.mode);

		share_file.Put_Field (11, buffer);
	}
	if (!share_file.Write ()) {
		Error ("Writing %s", share_file.File_Type ());
	}
}
