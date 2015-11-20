//*********************************************************
//	Perf_Detail.cpp - network performance details
//*********************************************************

#include "LinkSum.hpp"

#include "Utility.hpp"

#define LINKS		0
#define LENGTH		1
#define LANES		2
#define VMT			3
#define VHT			4
#define VHD			5
#define TIME_RATIO	6
#define DENSITY		7
#define MAX_DEN		8
#define QUEUE		9
#define MAX_QUEUE	10
#define FAILURE		11
#define TURNS		12
#define CONG_VMT	13
#define CONG_VHT	14
#define CONG_TIME	15
#define COUNT		16
#define PREV		14

//---------------------------------------------------------
//	Perf_Detail_Report
//---------------------------------------------------------

void LinkSum::Perf_Detail_Report (void)
{
	int i, j, k, num, volume, lanes, max_lanes, tod, tod_list, in_link_dir;
	double base_time, loaded_time, length, len, base, diff, time_ratio, value, percent;
	bool connect_flag;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;
	Connect_Time *connect_ptr;

	Show_Message ("Creating the Network Performance Details Report -- Record");
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
		len = 0.0;

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

			if (len == 0.0) {
				len = length;
			} else {
				len = 0.0;
			}
				
			tod_list = perf_ptr->TOD_List ();

			for (j=0; j < num; j++) {
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
				loaded_time = perf_ptr->TTime (j+1);
				volume = perf_ptr->Volume (j+1);
				time_ratio = perf_ptr->Time_Ratio (j+1);

				sum_bin [j] [LINKS] += 1;
				sum_bin [j] [LENGTH] += len;
				sum_bin [j] [LANES] += lanes * length;
				sum_bin [j] [VMT] += volume * length;
				sum_bin [j] [VHT] += volume * loaded_time;
				sum_bin [j] [VHD] += volume * (loaded_time - base_time);
				sum_bin [j] [TIME_RATIO] += time_ratio;
				sum_bin [j] [DENSITY] += perf_ptr->Density (j+1);
				sum_bin [j] [MAX_DEN] += perf_ptr->Max_Density (j+1);
				sum_bin [j] [QUEUE] += perf_ptr->Queue (j+1);
				sum_bin [j] [MAX_QUEUE] += perf_ptr->Max_Queue (j+1);
				sum_bin [j] [FAILURE] += perf_ptr->Failure (j+1);

				if (performance_data.Ratio_Flag ()) {
					sum_bin [j] [CONG_VMT] += perf_ptr->Ratio_VMT (j+1);
					sum_bin [j] [CONG_VHT] += perf_ptr->Ratio_VHT (j+1);
					sum_bin [j] [CONG_TIME] += perf_ptr->Ratio_Count (j+1);
					sum_bin [j] [COUNT] += perf_ptr->Count (j+1);
				}

				if (previous_flag) {
					loaded_time = perf_ptr->TTime2 (j+1);
					volume = perf_ptr->Volume2 (j+1);

					sum_bin [j] [VMT+PREV] += volume * length;
					sum_bin [j] [VHT+PREV] += volume * loaded_time;
					sum_bin [j] [VHD+PREV] += volume * (loaded_time - base_time);
					sum_bin [j] [TIME_RATIO+PREV] += perf_ptr->Time_Ratio2 (j+1);
					sum_bin [j] [DENSITY+PREV] += perf_ptr->Density2 (j+1);
					sum_bin [j] [MAX_DEN+PREV] += perf_ptr->Max_Density2 (j+1);
					sum_bin [j] [QUEUE+PREV] += perf_ptr->Queue2 (j+1);
					sum_bin [j] [MAX_QUEUE+PREV] += perf_ptr->Max_Queue2 (j+1);
					sum_bin [j] [FAILURE+PREV] += perf_ptr->Failure2 (j+1);

					if (performance_data.Ratio_Flag ()) {
						sum_bin [j] [CONG_VMT+PREV] += perf_ptr->Ratio_VMT2 (j+1);
						sum_bin [j] [CONG_VHT+PREV] += perf_ptr->Ratio_VHT2 (j+1);
						sum_bin [j] [CONG_TIME+PREV] += perf_ptr->Ratio_Count2 (j+1);
						sum_bin [j] [COUNT+PREV] += perf_ptr->Count2 (j+1);
					}
				}
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

						if (previous_flag) {
							sum_bin [j] [TURNS+PREV] += connect_ptr->Volume2 (j+1);
						}
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (PERF_DETAIL);

	if (!Break_Check (num_inc * 16)) {
		Print (1);
		Perf_Detail_Header ();
	}

	for (i=0; i <= num_inc; i++) {
		len = sum_bin [i] [LINKS];
		if (len == 0.0) continue;

		if (i) {
			if (!Break_Check ((performance_data.Ratio_Flag () ? 19 : 16))) {
				Print (1);
			}
		}
		Print (1, "Time Period%22c", BLANK);
		if (i == num_inc) {
			Print (0, "       Total");
			len *= num_inc;
		} else {
			Print (0, "%12.12s", time_periods.Range_Format (i+1));

			sum_bin [num_inc] [LINKS] = MAX (sum_bin [i] [LINKS], sum_bin [num_inc] [LINKS]);
			sum_bin [num_inc] [LENGTH] = MAX (sum_bin [i] [LENGTH], sum_bin [num_inc] [LENGTH]);
			sum_bin [num_inc] [LANES] = MAX (sum_bin [i] [LANES], sum_bin [num_inc] [LANES]);

			for (k = VMT; k <= COUNT; k++) {
				sum_bin [num_inc] [k] += sum_bin [i] [k];

				if (previous_flag) {
					sum_bin [num_inc] [k+PREV] += sum_bin [i] [k+PREV];
				}
			}
		}
		Print (1, "Number of Links                 %13.2lf", sum_bin [i] [LINKS]);
		Print (1, "Number of Roadway Miles         %13.2lf", sum_bin [i] [LENGTH] / MILETOMETER);
		Print (1, "Number of Lane Miles            %13.2lf", sum_bin [i] [LANES] / MILETOMETER);
		Print (1, "Vehicle Miles of Travel         %13.2lf", sum_bin [i] [VMT] / MILETOMETER);
		if (previous_flag) {
			base = sum_bin [i] [VMT+PREV] / MILETOMETER;
			diff = sum_bin [i] [VMT] / MILETOMETER - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		Print (1, "Vehicle Hours of Travel         %13.2lf", sum_bin [i] [VHT] / 3600.0);
		if (previous_flag) {
			base = sum_bin [i] [VHT+PREV] / 3600.0;
			diff = sum_bin [i] [VHT] / 3600.0 - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}	
		Print (1, "Vehicle Hours of Delay          %13.2lf", sum_bin [i] [VHD] / 3600.0);
		if (previous_flag) {
			base = sum_bin [i] [VHD+PREV] / 3600.0;
			diff = sum_bin [i] [VHD] / 3600.0 - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		Print (1, "Average Queued Vehicles         %13.2lf", sum_bin [i] [QUEUE]);
		if (previous_flag) {
			base = sum_bin [i] [QUEUE+PREV];
			diff = sum_bin [i] [QUEUE] - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		Print (1, "Maximum Queued Vehicles         %13.2lf", sum_bin [i] [MAX_QUEUE]);
		if (previous_flag) {
			base = sum_bin [i] [MAX_QUEUE+PREV];
			diff = sum_bin [i] [MAX_QUEUE] - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		Print (1, "Number of Cycle Failures        %13.2lf", sum_bin [i] [FAILURE]);
		if (previous_flag) {
			base = sum_bin [i] [FAILURE+PREV];
			diff = sum_bin [i] [FAILURE] - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		Print (1, "Number of Turning Movements     %13.2lf", sum_bin [i] [TURNS]);
		if (previous_flag) {
			base = sum_bin [i] [TURNS+PREV];
			diff = sum_bin [i] [TURNS] - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		Print (1, "Average Link Time Ratio         %13.2lf", sum_bin [i] [TIME_RATIO] / len);
		if (previous_flag) {
			base = sum_bin [i] [TIME_RATIO+PREV] / len;
			diff = sum_bin [i] [TIME_RATIO] / len - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		Print (1, "Average Link Density            %13.2lf", sum_bin [i] [DENSITY] / len);
		if (previous_flag) {
			base = sum_bin [i] [DENSITY+PREV] / len;
			diff = sum_bin [i] [DENSITY] / len - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		Print (1, "Average Link Max Density        %13.2lf", sum_bin [i] [MAX_DEN] / len);
		if (previous_flag) {
			base = sum_bin [i] [MAX_DEN+PREV] / len;
			diff = sum_bin [i] [MAX_DEN] / len - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}
		length = sum_bin [i] [VMT] / MILETOMETER;
		loaded_time = sum_bin [i] [VHT] / 3600.0;
		if (loaded_time == 0.0) {
			loaded_time = length;
		} else {
			loaded_time = length / loaded_time;
		}
		Print (1, "Average Miles Per Hour          %13.2lf", loaded_time);
		if (previous_flag) {
			length = sum_bin [i] [VMT+PREV] / MILETOMETER;
			base = sum_bin [i] [VHT+PREV] / 3600.0;
			if (base == 0.0) {
				base = length;
			} else {
				base = length / base;
			}
			diff = loaded_time - base;

			Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
		}

		if (performance_data.Ratio_Flag ()) {
			value = sum_bin [i] [VMT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_VMT] / value;

			Print (1, "Percent VMT Congested           %13.2lf", percent);
			if (previous_flag) {
				value = sum_bin [i] [VMT+PREV];
				if (value == 0.0) value = 1.0;
				base = 100.0 * sum_bin [i] [CONG_VMT+PREV] / value;
				diff = percent - base;

				Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
			}
			value = sum_bin [i] [VHT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_VHT] / value;

			Print (1, "Percent VHT Congested           %13.2lf", percent);
			if (previous_flag) {
				value = sum_bin [i] [VHT+PREV];
				if (value == 0.0) value = 1.0;
				base = 100.0 * sum_bin [i] [CONG_VHT+PREV] / value;
				diff = percent - base;

				Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
			}
			value = sum_bin [i] [COUNT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_TIME] / value;

			Print (1, "Percent Time Congested          %13.2lf", percent);
			if (previous_flag) {
				value = sum_bin [i] [COUNT+PREV];
				if (value == 0.0) value = 1.0;
				base = 100.0 * sum_bin [i] [CONG_TIME+PREV] / value;
				diff = percent - base;

				Print (0, " %13.2lf %13.2lf  (%.2lf%%)", base, diff, ((base > 0.0) ? (100.0 * diff / base) : 0.0));
			}
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Perf_Detail_Header
//---------------------------------------------------------

void LinkSum::Perf_Detail_Header (void)
{
	Print (1, "Network Performance Details");
	Print (2, "%36cCurrent", BLANK);
	if (previous_flag) {
		Print (0, "      Previous      Difference  Percent");
	}
	Print (1);
}

/*********************************************|***********************************************

	Network Performance Details
	
	                                    Current      Previous      Difference  Percent

	Time Period                      xx:xx..xx:xx
	Number of Links                  fffffffff.ff
	Number of Roadway Miles          fffffffff.ff
	Number of Lane Miles             fffffffff.ff
	Vehicle Miles of Travel          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Vehicle Hours of Travel          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Vehicle Hours of Delay           fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Queued Vehicles          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Maximum Queued Vehicles          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Number of Cycle Failures         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Number of Turning Movements      fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Time Ratio          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Density             fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Max Density         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Miles Per Hour           fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of VMT Congested         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of VHT Congested         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of Time Congested        fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)

**********************************************|***********************************************/ 
