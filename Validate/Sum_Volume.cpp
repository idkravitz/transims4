//*********************************************************
//	Sum_Volume.cpp - summarize the volume time periods
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Sum_Volume
//---------------------------------------------------------

void Validate::Sum_Volume (void)
{
	int i, num, nmap, time, increment;
	int *volume_map, *volume;

	TTime_Data *ttime_ptr;
	Range_Data *range_ptr;
    
	//---- read the volume file ----

	Show_Message ("Summarize the Volume Data -- Record");
	Set_Progress (10000);

	//---- check the data fields ----

	if (ttime_data.Periods () < time_periods.Num_Ranges ()) goto field_error;

	for (range_ptr = time_periods.First (); range_ptr; range_ptr = time_periods.Next ()) {
		if (ttime_data.Period (range_ptr->Low ()) == 0 ||
			ttime_data.Period ((range_ptr->Low () + range_ptr->High () + 1) / 2) == 0) goto field_error;
	}

	//---- build the volume map ----

	nmap = ttime_data.Periods () + 1;

	volume_map = new int [nmap];

	num = time_periods.Num_Ranges () + 1;

	volume = new int [num];

	if (volume_map == NULL || volume == NULL) {
		Error ("Insufficient Memory for the Volume Map");
	}
	increment = ttime_data.Period_Size ();

	time = increment / 2;
	time = increment - 1;

	for (i=1; i < nmap; i++, time += increment) {
		volume_map [i] = time_periods.In_Index (time);
	}

	//---- process each directional link ----

	for (ttime_ptr = ttime_data.First (); ttime_ptr; ttime_ptr = ttime_data.Next ()) {
		Show_Progress ();

		memset (volume, '\0', num * sizeof (int));

		for (i=1; i < nmap; i++) {
			if (volume_map [i] > 0) {
				volume [volume_map [i]] += ttime_ptr->Volume (i);
			}
		}
		if (!ttime_ptr->Periods (num-1)) goto mem_error;

		for (i=1; i < num; i++) {
			ttime_ptr->Volume (i, volume [i]);
			ttime_ptr->Count (0);
		}
	}
	End_Progress ();

	ttime_data.Period_Size (1);
	ttime_data.Periods (num - 1);

	delete [] volume_map;
	delete [] volume;
	return;

field_error:
	Error ("The Link Delay File has insufficient data to populate the Time Increments");

mem_error:
	Error ("Insufficient Memory for Time Period Volumes");
}
