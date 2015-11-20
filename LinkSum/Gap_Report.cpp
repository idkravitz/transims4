//*********************************************************
//	Relative_Gap_Report.cpp - relative gap by time of day
//*********************************************************

#include "LinkSum.hpp"

#include "Utility.hpp"
#include <math.h>

#define VHT			0
#define VHD			1

//---------------------------------------------------------
//	Relative_Gap_Report
//---------------------------------------------------------

void LinkSum::Relative_Gap_Report (void)
{
	int i, j, num;
	double volume, volume2, loaded_time;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;

	Show_Message ("Creating the Relative Gap Report -- Record");
	Set_Progress (10000);

	//---- clear the summary bins -----

	for (i=0; i <= num_inc; i++) {
		memset (sum_bin [i], '\0', NUM_SUM_BINS * sizeof (double));
	}

	//---- process each link ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		if (select_flag && link_ptr->Use () == 0) continue;

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_ptr->Use () == -1) continue;
				perf_ptr = performance_data [link_ptr->BA_Dir ()];
			} else {
				if (link_ptr->Use () == -2) continue;
				perf_ptr = performance_data [link_ptr->AB_Dir ()];
			}
			if (perf_ptr == NULL) continue;

			num = perf_ptr->Periods ();

			for (j=0; j < num; j++) {
				volume = perf_ptr->Volume (j+1);
				volume2 = perf_ptr->Volume2 (j+1);
				loaded_time = perf_ptr->TTime2 (j+1);

				sum_bin [j] [VHT] += volume * loaded_time;
				sum_bin [j] [VHD] += volume2 * loaded_time;
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (RELATIVE_GAP);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Relative_Gap_Header ();
	}

	for (i=0; i <= num_inc; i++) {

		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			Print (1, "%-12.12s ", time_periods.Range_Format (i+1));

			sum_bin [num_inc] [VHT] += sum_bin [i] [VHT];
			sum_bin [num_inc] [VHD] += sum_bin [i] [VHD];
		}
		volume = sum_bin [i] [VHT];
		volume2 = sum_bin [i] [VHD];

		if (volume > 0) {
			loaded_time = (volume2 - volume) / volume;
		} else {
			loaded_time = 0;
		}
		Print (0, "  %14.1lf   %14.1lf  %13.6lf", (volume / 3600.0), (volume2 / 3600.0), loaded_time);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Relative_Gap_Header
//---------------------------------------------------------

void LinkSum::Relative_Gap_Header (void)
{
	Print (1, "Relative Gap Report");
	Print (2, "Time Period      Current VHT     Previous VHT    Relative Gap");
	Print (1);
}

/*********************************************|***********************************************

	Relative Gap Report

	Time Period      Current VHT     Previous VHT    Relative Gap         
	
	xx:xx..xx:xx   ffffffffffff.f   ffffffffffff.f  ffffff.ffffff  

	Total          ffffffffffff.f   ffffffffffff.f  ffffff.ffffff  

**********************************************|***********************************************/ 
