//*********************************************************
//	Write_Link_Dir.cpp - Write the Link Direction File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Link_Dir
//---------------------------------------------------------

void LinkSum::Write_Link_Dir (Dir_Group *group)
{
	int i, j, k, num, nrec, tod_list, tod, lanes;
	bool save;	
	double data, cap, capacity;

	Performance_Data *perf_ptr;
	Link_Data *link_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;

	nrec = 0;

	Show_Message ("Writing %s -- Record", group->file->File_Type ());
	Set_Progress ();

	//---- process each link in sorted order ----

    for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {
		Show_Progress ();

		if (select_flag && link_ptr->Use () == 0) continue;

		group->file->Zero_Fields ();

		//---- process each direction ----

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_ptr->Use () == -1) continue;
				perf_ptr = performance_data [link_ptr->BA_Dir ()];
			} else {
				if (link_ptr->Use () == -2) continue;
				perf_ptr = performance_data [link_ptr->AB_Dir ()];
			}
			if (perf_ptr == NULL) continue;

			save = false;
			num = perf_ptr->Periods ();

			if (num == 0) continue;

			//---- get the capacity data ----

			if (group->field == VC_DATA) {
				capacity = perf_ptr->Capacity ();

				if (capacity < 20) capacity = 1000.0;
				cap = capacity *= cap_factor;

				tod_list = perf_ptr->TOD_List ();

				lanes = perf_ptr->Thru ();
				if (lanes < 1) lanes = 1;
			}

			//---- process each time period ----

			for (j=1; j <= num; j++) {
				switch (group->field) {
					case VOLUME_DATA:
						data = perf_ptr->Volume (j);
						break;
					case TTIME_DATA:
						data = perf_ptr->TTime (j);
						break;
					case VC_DATA:
						data = perf_ptr->Volume (j);

						if (data > 0.0) {
							if (tod_list > 0) {
								range_ptr = time_periods [j];

								tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);
								cap = capacity;

								for (k = tod_list; k; k = use_ptr->TOD_List ()) {
									use_ptr = link_use_data [k]; 

									if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
										cap = capacity * use_ptr->Lanes () / lanes;
										break;
									}
								}
							}
							if (cap > 0.0) {
								data = data / cap;
							} else {
								data = 0.0;
							}
						}
						break;
					case RATIO_DATA:
						data = perf_ptr->Time_Ratio (j);
						break;
					case SPEED_DATA:
						data = perf_ptr->Avg_Speed (j);
						break;
					case ENTER_DATA:
						data = perf_ptr->Enter (j);
						break;
					case EXIT_DATA:
						data = perf_ptr->Exit (j);
						break;
					case DELAY_DATA:
						data = perf_ptr->Delay (j);
						break;
					case DENSITY_DATA:
						data = perf_ptr->Density (j);
						break;
					case MAX_DENSITY_DATA:
						data = perf_ptr->Max_Density (j);
						break;
					case QUEUE_DATA:
						data = perf_ptr->Queue (j);
						break;
					case MAX_QUEUE_DATA:
						data = perf_ptr->Max_Queue (j);
						break;
					case FAILURE_DATA:
						data = perf_ptr->Failure (j);
						break;
					case VMT_DATA:
						data = perf_ptr->Distance (j);
						break;
					case VHT_DATA:
						data = perf_ptr->Time (j);
						break;
					case CONG_VMT_DATA:
						data = perf_ptr->Ratio_VMT (j);
						break;
					case CONG_VHT_DATA:
						data = perf_ptr->Ratio_VHT (j);
						break;
					case CONG_TIME_DATA:
						data = perf_ptr->Count (j);
						if (data <= 0.0) data = 1.0;
						data = 100.0 * perf_ptr->Ratio_Count (j) / data;
						break;
				}
				if (data != 0.0) {
					group->file->Data (j, data);
					save = true;
				}

				//---- add the previous value ----

				if (previous_flag) {
					switch (group->field) {
						case VOLUME_DATA:
							data = perf_ptr->Volume2 (j);
							break;
						case TTIME_DATA:
							data = perf_ptr->TTime2 (j);
							break;
						case VC_DATA:
							data = perf_ptr->Volume2 (j);

							if (data > 0.0) {
								if (tod_list > 0) {
									range_ptr = time_periods [j];

									tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);
									cap = capacity;

									for (k = tod_list; k; k = use_ptr->TOD_List ()) {
										use_ptr = link_use_data [k]; 

										if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
											cap = capacity * use_ptr->Lanes () / lanes;
											break;
										}
									}
								}
								if (cap > 0.0) {
									data = data / cap;
								} else {
									data = 0.0;
								}
							}
							break;
						case RATIO_DATA:
							data = perf_ptr->Time_Ratio2 (j);
							break;
						case SPEED_DATA:
							data = perf_ptr->Avg_Speed2 (j);
							break;
						case ENTER_DATA:
							data = perf_ptr->Enter2 (j);
							break;
						case EXIT_DATA:
							data = perf_ptr->Exit2 (j);
							break;
						case DELAY_DATA:
							data = perf_ptr->Delay2 (j);
							break;
						case DENSITY_DATA:
							data = perf_ptr->Density2 (j);
							break;
						case MAX_DENSITY_DATA:
							data = perf_ptr->Max_Density2 (j);
							break;
						case QUEUE_DATA:
							data = perf_ptr->Queue2 (j);
							break;
						case MAX_QUEUE_DATA:
							data = perf_ptr->Max_Queue2 (j);
							break;
						case FAILURE_DATA:
							data = perf_ptr->Failure2 (j);
							break;
						case VMT_DATA:
							data = perf_ptr->Distance2 (j);
							break;
						case VHT_DATA:
							data = perf_ptr->Time2 (j);
							break;
						case CONG_VMT_DATA:
							data = perf_ptr->Ratio_VMT2 (j);
							break;
						case CONG_VHT_DATA:
							data = perf_ptr->Ratio_VHT2 (j);
							break;
						case CONG_TIME_DATA:
							data = perf_ptr->Count2 (j);
							if (data <= 0.0) data = 1.0;
							data = 100.0 * perf_ptr->Ratio_Count2 (j) / data;
							break;
					}
					if (data != 0.0) {
						group->file->Data2 (j, data);
						save = true;
					}
				}
			}

			//---- save non-zero records ----

			if (save) {
				nrec++;

				group->file->Link_Dir (link_ptr->Link () * 2 + i);
				group->file->Link (link_ptr->Link ());
				group->file->Dir (i);
				if (previous_flag) {
					group->file->Difference ();
				}
				if (!group->file->Write ()) {
					Error ("Writing %s", group->file->File_Type ());
				}
			}
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", group->file->File_Type (), nrec);

	group->file->Close ();
}
