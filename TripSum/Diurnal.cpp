//*********************************************************
//	Diurnal.cpp - Write the Trip Time File
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Diurnal_Distribution
//---------------------------------------------------------

void TripSum::Diurnal_Distribution (void)
{
	int i;
	char *ptr1, *ptr2;
	
	FILE *file = time_file.File ();

	if (file == NULL) return;

	Show_Message ("Writing %s", time_file.File_Type ());

	fprintf (file, "Time1\tTime2\tStart\tMid-Trip\tEnd\n");

	for (i=1; i <= time_periods.Num_Ranges (); i++) {
		
		ptr1 = time_periods.Range_Format (i);
		ptr2 = strstr (ptr1, "..");
		*ptr2 = '\0';
		ptr2 += 2;

		if (fprintf (file, "%s\t%s\t%d\t%d\t%d\n",
			ptr1, ptr2, start_time [i], mid_time [i], end_time [i]) <= 0) {

			Error ("Writing %s", time_file.File_Type ());
		}
	}
	Show_Message (1);
}
