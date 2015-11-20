//*********************************************************
//	VC_Report.cpp - Create Volume Capacity Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Volume_Capacity_Report
//---------------------------------------------------------

void LinkSum::Volume_Capacity_Report (void)
{
	int i, j, k, k1, num, vc, volume, percent, max_lanes, lanes, tod, tod_list;
	int percentile [NUM_PERCENTILES];
	double capacity, cap, total, sum, lane_miles, len;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;

	Show_Message ("Creating the V/C Ratio Distribution -- Record");
	Set_Progress (10000);

	//---- clear the summary bins -----

	for (i=0; i <= num_inc; i++) {
		memset (sum_bin [i], '\0', NUM_SUM_BINS * sizeof (double));
	}

	//---- process each link ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		if (select_flag && link_ptr->Use () == 0) continue;

		len = UnRound (link_ptr->Length ());

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

			capacity = perf_ptr->Capacity ();

			if (capacity < 20) capacity = 1000.0;
			cap = capacity *= cap_factor;

			tod_list = perf_ptr->TOD_List ();

			lanes = perf_ptr->Thru ();
			if (lanes < 1) lanes = 1;
			max_lanes = lanes;

			//---- process each time period ----

			for (j=0; j < num; j++) {
				volume = perf_ptr->Volume (j+1);

				if (volume < min_volume) continue;

				if (tod_list > 0) {

					//---- get the time period ----

					range_ptr = time_periods [j+1];

					tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);
					cap = capacity;
					lanes = max_lanes;

					for (k = tod_list; k; k = use_ptr->TOD_List ()) {
						use_ptr = link_use_data [k]; 

						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							lanes = use_ptr->Lanes ();
							cap = capacity * lanes / max_lanes;
							break;
						}
					}
				}

				//---- save the vc ratio ----

				vc = (int) (volume * RESOLUTION / cap + 0.5);

				if (vc < 0 || vc >= NUM_SUM_BINS) vc = NUM_SUM_BINS - 1;

				lane_miles = lanes * len;

				sum_bin [j] [vc] += lane_miles;
				sum_bin [num] [vc] += lane_miles;
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (VC_RATIOS);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Volume_Capacity_Header ();
	}

	for (i=0; i <= num_inc; i++) {

		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			Print (1, "%-12.12s ", time_periods.Range_Format (i+1));
		}

		//---- get the total observations ----

		total = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			total += sum_bin [i] [j];
		}
		if (total == 0.0) continue;

		//---- identify the percentile values ----

        memset (percentile, '\0', sizeof (percentile));

		k1 = 0;
		sum = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			sum += sum_bin [i] [j];

			percent = (int) (sum * 100 / total + 0.5);

			for (k=k1; k < NUM_PERCENTILES; k++) {
				if (percent_break [k] <= percent) {
					percentile [k] = j;
					k1 = k + 1;
				} else {
					break;
				}
			}
		}

		//---- print the percentiles ----

		for (k=0; k < NUM_PERCENTILES; k++) {
			Print (0, " %6.2lf", (double) percentile [k] / RESOLUTION);
		}
		Print (0, "  %7.0lf", total / 1000.0);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Volume_Capacity_Header
//---------------------------------------------------------

void LinkSum::Volume_Capacity_Header (void)
{
	int i;

	Print (1, "Volume Capacity Ratio Distribution");
	Print (2, "%31cVolume / Capacity Ratio", BLANK);
	Print (1, "%15c-------------Percentile Distribution by Time Period----------     Lane", BLANK);
	Print (1, "Time Period   ");

	for (i=0; i < NUM_PERCENTILES; i++) {
		Print (0, "  %3d%% ", percent_break [i]);
	}
	Print (0, "     KM");
	Print (1);
}

/*********************************************|***********************************************

	Volume Capacity Ratio Distribution

                                   Volume / Capacity Ratio
	               -------------Percentile Distribution by Time Period----------    Lane
 	Time Period     50%    60%    65%    70%    75%    80%    85%    95%    99%      KM
	
	xx:xx..xx:xx   dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  ddddddd

	Total       
**********************************************|***********************************************/ 
