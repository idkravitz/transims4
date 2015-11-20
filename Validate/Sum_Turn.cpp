//*********************************************************
//	Sum_Turn.cpp - summarize the turn time periods
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Sum_Turn
//---------------------------------------------------------

void Validate::Sum_Turn (void)
{
	int i, num, nmap, time, increment;
	int *volume_map, *volume;

	Connect_Time *connect_ptr;
	Range_Data *range_ptr;
    
	//---- read the volume file ----

	Show_Message ("Summarize the Turn Data -- Record");
	Set_Progress (10000);

	//---- check the data fields ----

	if (connect_time.Periods () < time_periods.Num_Ranges ()) goto field_error;

	for (range_ptr = time_periods.First (); range_ptr; range_ptr = time_periods.Next ()) {
		if (connect_time.Period (range_ptr->Low ()) == 0 ||
			connect_time.Period ((range_ptr->Low () + range_ptr->High () + 1) / 2) == 0) goto field_error;
	}

	//---- build the volume map ----

	nmap = connect_time.Periods () + 1;

	volume_map = new int [nmap];

	num = time_periods.Num_Ranges () + 1;

	volume = new int [num];

	if (volume_map == NULL || volume == NULL) {
		Error ("Insufficient Memory for the Volume Map");
	}
	increment = connect_time.Period_Size ();

	time = increment / 2;

	for (i=1; i < nmap; i++, time += increment) {
		volume_map [i] = time_periods.In_Index (time);
	}

	//---- process each turn movement ----

	for (connect_ptr = connect_time.First (); connect_ptr; connect_ptr = connect_time.Next ()) {
		Show_Progress ();

		memset (volume, '\0', num * sizeof (int));

		for (i=1; i < nmap; i++) {
			if (volume_map [i] > 0) {
				volume [volume_map [i]] += connect_ptr->Volume (i);
			}
		}
		if (!connect_ptr->Periods (num-1)) goto mem_error;

		for (i=1; i < num; i++) {
			connect_ptr->Volume (i, volume [i]);
			connect_ptr->Count (i, 0);
		}
	}
	End_Progress ();

	connect_time.Period_Size (1);
	connect_time.Periods (num - 1);

	delete [] volume_map;
	delete [] volume;
	return;

field_error:
	Error ("The Link Delay File has insufficient data to populate the Time Increments");

mem_error:
	Error ("Insufficient Memory for Time Period Volumes");
}
