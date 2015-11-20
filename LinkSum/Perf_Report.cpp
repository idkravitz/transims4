//*********************************************************
//	Perf_Report.cpp - network performance report
//*********************************************************

#include "LinkSum.hpp"

#include "Utility.hpp"

#define LANE_MILES	0
#define VMT			1
#define VHT			2
#define VHD			3
#define TURNS		4

//---------------------------------------------------------
//	Performance_Report
//---------------------------------------------------------

void LinkSum::Performance_Report (void)
{
	int i, j, k, num, volume, lanes, max_lanes, tod, tod_list, in_link_dir;
	double base_time, loaded_time, length;
	bool connect_flag;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;
	Connect_Time *connect_ptr;

	Show_Message ("Creating the Network Performance Summary Report -- Record");
	Set_Progress (10000);

	//---- clear the summary bins -----

	for (i=0; i <= num_inc; i++) {
		memset (sum_bin [i], '\0', NUM_SUM_BINS * sizeof (double));
	}
	connect_flag = Network_Data_Flag (LANE_CONNECTIVITY);

	//---- process each link ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		if (select_flag && link_ptr->Use () == 0) continue;

		length = UnRound (link_ptr->Length ());

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_ptr->Use () == -1) continue;
				perf_ptr = performance_data [link_ptr->BA_Dir ()];
			} else {
				if (link_ptr->Use () == -2) continue;
				perf_ptr = performance_data [link_ptr->AB_Dir ()];
			}
			if (perf_ptr == NULL) continue;

			//**** temporary change to avoid negatives *****
			base_time = UnRound (perf_ptr->Speed ());

			if (base_time <= 0.0) {
				base_time = UnRound (perf_ptr->Time0 ());
			} else {
				base_time = length / base_time;
			}
			if (base_time <= 0.0) continue;	

			num = perf_ptr->Periods ();
			max_lanes = lanes = perf_ptr->Thru ();
				
			tod_list = perf_ptr->TOD_List ();

			for (j=0; j < num; j++) {
				loaded_time = perf_ptr->TTime (j+1);
				volume = perf_ptr->Volume (j+1);

				if (tod_list > 0) {

					//---- get the time period ----

					range_ptr = time_periods [j+1];

					tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);
					lanes = max_lanes;

					for (k = tod_list; k; k = use_ptr->TOD_List ()) {
						use_ptr = link_use_data [k]; 

						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							lanes = use_ptr->Lanes ();
							break;
						}
					}
				}
				sum_bin [j] [LANE_MILES] += lanes * length;
				sum_bin [j] [VMT] += volume * length;
				sum_bin [j] [VHT] += volume * loaded_time;
				sum_bin [j] [VHD] += volume * (loaded_time - base_time);
			}

			//---- get the turning movements ----

			if (connect_flag) {
				in_link_dir = perf_ptr->Link_Dir ();

				for (connect_ptr = connect_time.Get_GE (in_link_dir, 0); connect_ptr; 
					connect_ptr = connect_time.Next_Key ()) {

					if (connect_ptr->In_Link_Dir () != in_link_dir) break;
					if (connect_ptr->Type () != LEFT && connect_ptr->Type () != RIGHT &&
						connect_ptr->Type () != UTURN) continue;

					num = connect_ptr->Periods ();
					if (num == 0) continue;

					for (j=0; j < num; j++) {
						sum_bin [j] [TURNS] += connect_ptr->Volume (j+1);
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (PERF_REPORT);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Performance_Header ();
	}

	for (i=0; i <= num_inc; i++) {

		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			Print (1, "%-12.12s ", time_periods.Range_Format (i+1));

			sum_bin [num_inc] [LANE_MILES] = MAX (sum_bin [i] [LANE_MILES], sum_bin [num_inc] [LANE_MILES]);
			sum_bin [num_inc] [VMT] += sum_bin [i] [VMT];
			sum_bin [num_inc] [VHT] += sum_bin [i] [VHT];
			sum_bin [num_inc] [VHD] += sum_bin [i] [VHD];
			sum_bin [num_inc] [TURNS] += sum_bin [i] [TURNS];
		}
		length = (sum_bin [i] [VMT] / MILETOMETER);
		base_time = (sum_bin [i] [VHT] / 3600.0);
		if (base_time == 0.0) {
			loaded_time = length;
		} else {
			loaded_time = length / base_time;
		}
		Print (0, "%12.2lf  %13.2lf  %13.2lf %7.2lf %13.2lf  %10d", 
			(sum_bin [i] [LANE_MILES] / MILETOMETER),
			length, base_time, loaded_time, 
			(sum_bin [i] [VHD] / 3600.0),
			(int) (sum_bin [i] [TURNS]));

	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Performance_Header
//---------------------------------------------------------

void LinkSum::Performance_Header (void)
{
	Print (1, "Network Performance Summary");
	Print (2, "                   Lane         Vehicle        Vehicle    Miles     Hours of       Number");
	Print (1, "Time Period       Miles          Miles          Hours     /Hour       Delay       of Turns");
	Print (1);
}

/*********************************************|***********************************************

	Network Performance Summary

	                  Lane         Vehicle        Vehicle     Miles    Hours of       Number
	Time Period      Miles          Miles          Hours      /Hour      Delay       of Turns
	
	xx:xx..xx:xx fffffffff.ff  ffffffffff.ff  ffffffffff.ff ffff.ff ffffffffff.ff  dddddddddd

	Total        fffffffff.ff  ffffffffff.ff  ffffffffff.ff ffff.ff ffffffffff.ff  dddddddddd

**********************************************|***********************************************/ 
