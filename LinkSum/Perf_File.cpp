//*********************************************************
//	Perf_File.cpp - network performance data file
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
//	Perf_Detail_File
//---------------------------------------------------------

void LinkSum::Perf_Detail_File (void)
{
	int i, j, k, num, volume, lanes, max_lanes, tod, tod_list, in_link_dir;
	int text_field, value_field;
	double base_time, loaded_time, length, len, time_ratio, value, percent;
	char buffer [FIELD_BUFFER];
	bool connect_flag;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;
	Connect_Time *connect_ptr;

	Show_Message ("Writing Performance Data File -- Record");
	Set_Progress (10000);

	text_field = detail_file.Field_Number ("MEASURE");
	value_field = detail_file.Field_Number ("VALUE");

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

	//---- write the data ----

	for (i=0; i <= num_inc; i++) {
		len = sum_bin [i] [LINKS];
		if (len == 0.0) continue;

		detail_file.Put_Field (text_field, buffer);

		if (i == num_inc) {
			str_fmt (buffer, sizeof (buffer), "Time Period Total");
			len *= num_inc;
		} else {
			str_fmt (buffer, sizeof (buffer), "Time Period %12.12s", time_periods.Range_Format (i+1));

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
		detail_file.Put_Field (text_field, buffer);
		detail_file.Put_Field (value_field, "0.0");
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Number of Links");
		detail_file.Put_Field (value_field, sum_bin [i] [LINKS]);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Number of Roadway Miles");
		detail_file.Put_Field (value_field, sum_bin [i] [LENGTH] / MILETOMETER);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Number of Lane Miles");
		detail_file.Put_Field (value_field, sum_bin [i] [LANES] / MILETOMETER);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Vehicle Miles of Travel");
		detail_file.Put_Field (value_field, sum_bin [i] [VMT] / MILETOMETER);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Vehicle Hours of Travel");
		detail_file.Put_Field (value_field, sum_bin [i] [VHT] / 3600.0);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Vehicle Hours of Delay");
		detail_file.Put_Field (value_field, sum_bin [i] [VHD] / 3600.0);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Average Queued Vehicles");
		detail_file.Put_Field (value_field, sum_bin [i] [QUEUE]);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Maximum Queued Vehicles");
		detail_file.Put_Field (value_field, sum_bin [i] [MAX_QUEUE]);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Number of Cycle Failures");
		detail_file.Put_Field (value_field, sum_bin [i] [FAILURE]);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Number of Turning Movements");
		detail_file.Put_Field (value_field, sum_bin [i] [TURNS]);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Average Link Time Ratio");
		detail_file.Put_Field (value_field, sum_bin [i] [TIME_RATIO] / len);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Average Link Density");
		detail_file.Put_Field (value_field, sum_bin [i] [DENSITY] / len);
		detail_file.Write ();

		detail_file.Put_Field (text_field, "Average Link Max Density");
		detail_file.Put_Field (value_field, sum_bin [i] [MAX_DEN] / len);
		detail_file.Write ();

		length = sum_bin [i] [VMT] / MILETOMETER;
		loaded_time = sum_bin [i] [VHT] / 3600.0;
		if (loaded_time == 0.0) {
			loaded_time = length;
		} else {
			loaded_time = length / loaded_time;
		}
		detail_file.Put_Field (text_field, "Average Miles Per Hour");
		detail_file.Put_Field (value_field, loaded_time);
		detail_file.Write ();

		if (performance_data.Ratio_Flag ()) {
			value = sum_bin [i] [VMT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_VMT] / value;

			detail_file.Put_Field (text_field, "Percent VMT Congested");
			detail_file.Put_Field (value_field, percent);
			detail_file.Write ();

			value = sum_bin [i] [VHT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_VHT] / value;

			detail_file.Put_Field (text_field, "Percent VHT Congested");
			detail_file.Put_Field (value_field, percent);
			detail_file.Write ();

			value = sum_bin [i] [COUNT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_TIME] / value;

			detail_file.Put_Field (text_field, "Percent Time Congested");
			detail_file.Put_Field (value_field, percent);
			detail_file.Write ();
		}
	}
}
