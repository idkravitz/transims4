//*********************************************************
//	Restrictions.cpp - time of day restrictions
//*********************************************************

#include "Network_Service.hpp"

#define MAX_LANE	32

#pragma pack (push, 4)

//---------------------------------------------------------
//	Time_of_Day_Restrictions
//---------------------------------------------------------

void Network_Service::Time_of_Day_Restrictions (void)
{
	typedef struct {
		int start_time;
		int end_time;
		int lane;
		int use;
		int type;
		int offset;
		int length;
	} Restriction;

	typedef struct {
		int start_time;
		int end_time;
	} Time_Period;

	bool range_flag;
	int link_dir, dir, low, high, lane, l1, l2, lane_use [MAX_LANE], use, lanes, hov;
	int use_code, sov_code, hov_code;
	
	Link_Data *link_ptr = NULL;
	Dir_Data *dir_ptr = NULL;
	Lane_Use_Data *use_ptr = NULL;

	Link_Use_Data use_rec;
	Restriction *res_ptr, res_rec;
	Time_Period *period_ptr, period_rec;

	Data_Array restriction (sizeof (Restriction), 10);
	Data_Array time_period (sizeof (Time_Period), 10);

	link_dir = dir = low = high = 0;

	use_code = Use_Code ("CAR/TRUCK");
	sov_code = Use_Code ("SOV");
	hov_code = Use_Code ("HOV2/HOV3/HOV4");

	for (use_ptr = lane_use_data.First_Key (); ; use_ptr = lane_use_data.Next_Key ()) {

		//---- check for a new link ----

		if (use_ptr == NULL || use_ptr->Link_Dir () != link_dir) {

			//---- process the existing link ----

			if (link_dir > 0) {

				for (period_ptr = (Time_Period *) time_period.First (); period_ptr != NULL;	
					period_ptr = (Time_Period *) time_period.Next ()) {

					use_rec.TOD_List (dir_ptr->TOD_List ());
					use_rec.Start (period_ptr->start_time);
					use_rec.End (period_ptr->end_time);
					use_rec.Offset (0);
					use_rec.Length (0);

					range_flag = false;

					for (lane=low; lane <= high; lane++) {
						lane_use [lane] = link_ptr->Use ();
					}
					for (res_ptr = (Restriction *) restriction.First (); res_ptr != NULL; 
						res_ptr = (Restriction *) restriction.Next ()) {

						if (period_ptr->start_time >= res_ptr->end_time || 
							period_ptr->end_time <= res_ptr->start_time) continue;

						if (res_ptr->length > 0 || res_ptr->offset > 0) {
							range_flag = true;
							continue;
						}
						if (res_ptr->lane > 0) {
							l1 = l2 = res_ptr->lane;
						} else {
							l1 = low;
							l2 = high;
						}
						if (res_ptr->type == ONLY) {
							for (lane = l1; lane <= l2; lane++) {
								lane_use [lane] = res_ptr->use;
							}
						} else if (res_ptr->type == REQUIRE) {
							for (lane = low; lane <= high; lane++) {
								if (lane < l1 || lane > l2) {
									lane_use [lane] &= ~res_ptr->use;
								}
							}
						} else {		//---- prohibit ----
							for (lane = l1; lane <= l2; lane++) {
								lane_use [lane] &= ~res_ptr->use;
							}
						}
					}
					
					//---- save the current data ----

					if (Lane_Use ()) {
						use = -1;

						for (lane=low; lane <= high; lane++) {
							if (use != lane_use [lane]) {
								if (use >= 0) {
									if (use != link_ptr->Use ()) {
										if (!link_use_data.Add (&use_rec)) goto mem_error;

										dir_ptr->TOD_List (link_use_data.Record_Index ());
									}
								}
								use = lane_use [lane];
								use_rec.Use (use);
								use_rec.Low_Lane (lane);
								use_rec.High_Lane (lane);
								use_rec.TOD_List (dir_ptr->TOD_List ());
								use_rec.Offset (0);
								use_rec.Length (0);
							} else {
								use_rec.High_Lane (lane);
							}
						}
						if (use != link_ptr->Use ()) {
							if (!link_use_data.Add (&use_rec)) goto mem_error;

							dir_ptr->TOD_List (link_use_data.Record_Index ());
						}

						//---- add offset-length restrictions ----

						if (range_flag) {
							use_rec.TOD_List (dir_ptr->TOD_List ());
							use_rec.Start (period_ptr->start_time);
							use_rec.End (period_ptr->end_time);

							for (lane=low; lane <= high; lane++) {
								lane_use [lane] = link_ptr->Use ();
							}
							for (res_ptr = (Restriction *) restriction.First (); res_ptr != NULL; 
								res_ptr = (Restriction *) restriction.Next ()) {

								if (period_ptr->start_time >= res_ptr->end_time || 
									period_ptr->end_time <= res_ptr->start_time) continue;

								if (res_ptr->length == 0 && res_ptr->offset == 0) continue;

								use_rec.Offset (res_ptr->offset);
								use_rec.Length (res_ptr->length);

								if (res_ptr->lane > 0) {
									l1 = l2 = res_ptr->lane;
								} else {
									l1 = low;
									l2 = high;
								}

								if (res_ptr->type == ONLY) {
									for (lane = l1; lane <= l2; lane++) {
										lane_use [lane] = res_ptr->use;
									}
								} else if (res_ptr->type == REQUIRE) {
									for (lane = low; lane <= high; lane++) {
										if (lane < l1 || lane > l2) {
											lane_use [lane] &= ~res_ptr->use;
										}
									}
								} else {		//---- prohibit ----
									for (lane = l1; lane <= l2; lane++) {
										lane_use [lane] &= ~res_ptr->use;
									}
								}
							
								//---- save the current data ----

								use = -1;

								for (lane=low; lane <= high; lane++) {
									if (use != lane_use [lane]) {
										if (use >= 0) {
											if (use != link_ptr->Use ()) {
												if (!link_use_data.Add (&use_rec)) goto mem_error;

												dir_ptr->TOD_List (link_use_data.Record_Index ());
											}
										}
										use = lane_use [lane];
										use_rec.Use (use);
										use_rec.Low_Lane (lane);
										use_rec.High_Lane (lane);
										use_rec.TOD_List (dir_ptr->TOD_List ());
									} else {
										use_rec.High_Lane (lane);
									}
								}
								if (use != link_ptr->Use ()) {
									if (!link_use_data.Add (&use_rec)) goto mem_error;

									dir_ptr->TOD_List (link_use_data.Record_Index ());
								}
								for (lane=low; lane <= high; lane++) {
									lane_use [lane] = link_ptr->Use ();
								}
							}
						}

					} else {
						use = lanes = hov = 0;

						for (lane=low; lane <= high; lane++) {
							use |= lane_use [lane];
							if ((lane_use [lane] & use_code) != 0) lanes++;
							if ((lane_use [lane] & hov_code) != 0 && 
								(lane_use [lane] & sov_code) == 0) hov++;
						}
						use_rec.Use (use);
						use_rec.Lanes (lanes);
						use_rec.HOV_Lanes (hov);

						if (!link_use_data.Add (&use_rec)) goto mem_error;

						dir_ptr->TOD_List (link_use_data.Record_Index ());
					}
				}
			}
			if (use_ptr == NULL) break;

			//---- initialize the new link ----

			link_dir = 0;
			if (Renumber ()) {
				link_ptr = link_data [use_ptr->Link ()];
			} else {
				link_ptr = link_data.Get (use_ptr->Link ());
			}
			if (link_ptr == NULL) {
				Warning ("Lane Use could not be applied on Link %d", link_ptr->Link ());
				continue;
			}
			if (use_ptr->Dir ()) {
				dir = link_ptr->BA_Dir ();
			} else {
				dir = link_ptr->AB_Dir ();
			}
			dir_ptr = dir_data [dir];

			if (dir_ptr == NULL) {
				Warning ("Lane Use could not be applied on Link %d in the %s Direction", 
					link_ptr->Link (), ((use_ptr->Dir ()) ? "BA" : "AB"));
				continue;
			}

			link_dir = use_ptr->Link_Dir ();

			if (Lane_Use ()) {
				low = 1;
				high = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right ();
			} else {
				low = dir_ptr->Left () + 1;
				high = low + dir_ptr->Thru () - 1;
			}
			if (high >= MAX_LANE) {
				Warning ("Too Many Lanes %d on Link %d for Lane Use Applications", 
					high, link_ptr->Link ());
				high = MAX_LANE-1;
			}
			restriction.Reset ();
			time_period.Reset ();
		}

		//---- check the lane range ----

		if (use_ptr->Lane () > 0) {
			l1 = use_ptr->Lane ();
			if (l1 < low || l1 > high) continue;
		}

		//---- save the current data ----

		res_rec.start_time = use_ptr->Start ();
		res_rec.end_time = use_ptr->End ();
		res_rec.lane = use_ptr->Lane ();
		res_rec.use = use_ptr->Use ();
		res_rec.type = use_ptr->Type ();
		res_rec.offset = use_ptr->Offset ();
		res_rec.length = use_ptr->Length ();

		restriction.Add (&res_rec);

		//---- check the time period ----

		for (period_ptr = (Time_Period *) time_period.First (); period_ptr != NULL; 
			period_ptr = (Time_Period *) time_period.Next ()) {

			if (period_ptr->start_time >= res_rec.end_time || 
				period_ptr->end_time <= res_rec.start_time) continue;

			if (period_ptr->start_time == res_rec.start_time) {
				if (period_ptr->end_time == res_rec.end_time) break;

				if (period_ptr->end_time < res_rec.end_time) {
					res_rec.start_time = period_ptr->end_time;
				} else {
					period_ptr->start_time = res_rec.end_time;
				}
			} else if (period_ptr->end_time == res_rec.end_time) {
				if (period_ptr->start_time < res_rec.start_time) {
					period_ptr->end_time = res_rec.start_time;
				} else {
					res_rec.end_time = period_ptr->start_time;
				}
			} else {
				int index = time_period.Record_Index ();
	
				if (period_ptr->start_time < res_rec.start_time) {
					period_rec.start_time = period_ptr->start_time;
					period_rec.end_time = res_rec.start_time;

					time_period.Add (&period_rec);
					time_period.Record_Index (index);

					if (period_ptr->end_time > res_rec.end_time) {
						period_ptr->start_time = res_rec.end_time;
					} else {
						period_ptr->start_time = res_rec.start_time;
						res_rec.start_time = period_ptr->end_time;
					}
				} else {
					period_rec.start_time = res_rec.start_time;
					period_rec.end_time = period_ptr->start_time;

					time_period.Add (&period_rec);
					time_period.Record_Index (index);

					if (period_ptr->end_time > res_rec.end_time) {
						res_rec.start_time = period_ptr->start_time;
						period_ptr->start_time = res_rec.end_time;
					} else {
						res_rec.start_time = period_ptr->end_time;
					}
				}
			}
		}
		if (period_ptr == NULL) {
			period_rec.start_time = res_rec.start_time;
			period_rec.end_time = res_rec.end_time;

			time_period.Add (&period_rec);
		}
	}
	Print (1, "Number of Link Use Time Restrictions = %d", link_use_data.Num_Records ());
	return;

mem_error:
	Error ("Adding Record to the Link Use Data List");
}
#pragma pack (pop)
