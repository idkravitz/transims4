//*********************************************************
//	Share_Details.cpp - process a trip record
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Share_Details_Report
//---------------------------------------------------------

void ModeChoice::Share_Details_Report (Travel_Leg &leg)
{
	char start [FIELD_BUFFER], arrive [FIELD_BUFFER];

	time_step.Format_Step (leg.start, start);
	time_step.Format_Step (leg.end, arrive);

	Print (1, "%9d %5d %5d %3d  %10.10s %9d %5d %10.10s %9d %5d", 
		leg.hhold, leg.person, leg.trip, leg.purpose, start, leg.origin, leg.org_zone,
		arrive, leg.destination, leg.des_zone);

	if (mode_flag) {
		Mode_Group *mode_ptr;

		for (mode_ptr = mode_group.First (); mode_ptr; mode_ptr = mode_group.Next ()) {
			if (mode_ptr->Mode () == leg.mode) {
				Print (0, " %9.9s", mode_ptr->Label ());
				break;
			}
		}
		if (mode_ptr == NULL) {
			str_fmt (arrive, sizeof (arrive), "Mode #%d", leg.mode);

			Print (0, " %9.9s", arrive);
		}
		for (mode_ptr = mode_group.First (); mode_ptr; mode_ptr = mode_group.Next ()) {
			Print (0, " %9.6lf", mode_ptr->Share ());
		}
	} else {
		str_fmt (arrive, sizeof (arrive), "Mode #%d", leg.mode);

		Print (0, " %9.9s", arrive);
	}
}

//---------------------------------------------------------
//	Share_Details_Header
//---------------------------------------------------------

void ModeChoice::Share_Details_Header (void)
{
	int space1, space2;
	
	char *bar = "----------------------------------------------------------------";

	Print (1, "Mode Share Details");
	Print (1, "%22cPur- -----------Origin--------- --------Destination-------    Mode ", BLANK);

	if (mode_flag) {
		space1 = 10 * mode_group.Num_Records () - 12;
		space2 = space1 / 2;
		space1 -= space2;
		if (space1 < 1) space1 = 1;
		if (space2 < 1) space2 = 1;

		Print (0, "  %*.*sMode Shares%*.*s", space1, space1, bar, space2, space2, bar);
	}

	Print (1, "Household Person Trip pose      Start  Location  Zone     Arrive  Location  Zone   Choice ");

	if (mode_flag) {
		Mode_Group *mode_ptr;

		for (mode_ptr = mode_group.First (); mode_ptr; mode_ptr = mode_group.Next ()) {
			Print (0, " %9.9s", mode_ptr->Label ());
		}
	}
	Print (1);
}

/*********************************************|***********************************************

	Mode Share Details
	                      Pur- -----------Origin--------- --------Destination-------    Mode   ---------------------------Mode Shares-------------------------------
	Household Person Trip pose      Start  Location  Zone     Arrive  Location  Zone   Choice  sssssssss sssssssss sssssssss sssssssss sssssssss sssssssss sssssssss
	
	ddddddddd ddddd ddddd  dd  d@dd:dd:dd ddddddddd ddddd d@dd:dd:dd ddddddddd ddddd sssssssss  f.ffffff  f.ffffff  f.ffffff  f.ffffff  f.ffffff  f.ffffff  f.ffffff

**********************************************|***********************************************/ 
 
