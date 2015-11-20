//*********************************************************
//	Compare_Plans.cpp - Compare Plan Files
//*********************************************************

#include "PlanCompare.hpp"

#define MAX_LEGS	12

//---------------------------------------------------------
//	Compare_Plans
//---------------------------------------------------------

void PlanCompare::Compare_Plans (void)
{
	int i, j, num1, num2, *path1, *path2, *p2, j1, percent, type;
	int diff, ndiff, tod, period, in_ttime, compare_ttime, base, tim_diff, cst_diff;
	int mode_in [5], mode_compare [5], mode;

	int next_in, next_trip_in;
	int next_compare, next_trip_compare;

	int last_in, last_trip_in, last_start_in, last_end_in, last_hhold, last_person;
	int last_start_id_in, last_end_id_in, last_start_id_compare, last_end_id_compare;
	int last_start_type_in, last_end_type_in, last_start_type_compare, last_end_type_compare;
	int last_compare, last_trip_compare, last_start_compare, last_end_compare;
	int last_cost_in, last_cost_compare;
	int legs_in, board_in [MAX_LEGS], alight_in [MAX_LEGS], route_in [MAX_LEGS];
	int legs_compare, board_compare [MAX_LEGS], alight_compare [MAX_LEGS], route_compare [MAX_LEGS];

	bool read_compare, read_in, save_in, hhold_flag, found, drive_in, drive_compare;
	bool match_flag, time_select, transit_in, transit_compare, access_flag, trip_data_flag;
	double share, share2, factor;
	FILE *out_file, *bad_file;

	Travel_Diff diff_rec;
	Select_Trips sel_rec;
	Trip_Time *time_ptr;
	Location_Diff loc_rec, *loc_ptr;
	Location_Data *in_loc_ptr, *compare_loc_ptr;
	Trip_Data *in_trip_ptr, *compare_trip_ptr;
	Period_Diff *period_ptr;

	if (input_plans.Extend ()) {
		Show_Message ("Comparing Plan Files %s -- Plan", input_plans.Extension ());
	} else {
		Show_Message ("Comparing Plan Files -- Plan");
	}
	Set_Progress (10000);

	mode = 0;
	read_compare = read_in = save_in = false;
	hhold_flag = true;
	drive_in = drive_compare = match_flag = access_flag = false;

	time_select = (select_time.Num_Ranges () > 0);

	trip_data_flag = (input_trip_data.Num_Records () > 0 && compare_trip_data.Num_Records () > 0);

	if (match_out_flag) {
		out_file = match_file.File ();
	}
	if (incomplete_flag) {
		bad_file = incomplete_file.File ();
	}

	//---- read the first record in the input file ----

	last_in = last_trip_in = last_hhold = legs_in = 0;
	tim_diff = 0;

	if (input_plans.Read ()) {
		Show_Progress ();
		next_in = input_plans.Traveler ();
		next_trip_in = input_plans.Trip ();
	} else {
		next_in = MAX_INTEGER;
	}

	//---- read the first record in the compare file ----

	last_compare = last_trip_compare = legs_compare = 0;

	if (compare_plans.Read ()) {
		next_compare = compare_plans.Traveler ();
		next_trip_compare = compare_plans.Trip ();
	} else {
		next_compare = MAX_INTEGER;
	}

	//---- interleaf plan records based on traveler ID ----

	for (;;) {

		//---- check the processing conditions ----

		if (last_in < last_compare || (last_in == last_compare && last_trip_in <= last_trip_compare)) {
			if (last_in > 0 && last_in < MAX_INTEGER) {
				if (hhlist_flag) {
					hhold_flag = (hhold_list.Get (&last_hhold) != NULL);
				}
				if (hhold_flag && traveler_flag) {
					hhold_flag = travelers.In_Range (last_in);
				}
				if (hhold_flag) {
					share = share2 = 1.0;
					diff = ndiff = tim_diff = 0;

					match_flag = (last_in == last_compare && last_trip_in == last_trip_compare);

					//---- check for complete trips ----

					if (match_flag && trip_data_flag) {
						match_flag = false;

						in_trip_ptr = input_trip_data.Get (last_in, last_trip_in);
						diff = type = mode = 0;

						if (in_trip_ptr != NULL) {
							diff = in_trip_ptr->End_Time () - in_trip_ptr->Start_Time ();
							mode = in_trip_ptr->Mode ();

							access_flag = (drive_access_flag && (mode == 5 || mode == 6 || mode == 13 || mode == 14));

							if (last_start_id_in == in_trip_ptr->Origin () && 
								last_end_id_in == in_trip_ptr->Destination ()) {

								compare_trip_ptr = compare_trip_data.Get (last_compare, last_trip_compare);

								if (compare_trip_ptr != NULL) {
									if (last_start_id_compare == compare_trip_ptr->Origin () && 
										last_end_id_compare == compare_trip_ptr->Destination ()) {

										if (compare_trip_ptr->Mode () == mode) {
											ndiff = compare_trip_ptr->End_Time () - compare_trip_ptr->Start_Time ();

											if (ndiff > 0) diff = abs (diff - ndiff) * 100 / ndiff;

											if (diff < 5) {
												diff = abs (in_trip_ptr->Start_Time () - compare_trip_ptr->Start_Time ());

												if (diff < 900) {
													if (Network_Data_Flag (ACTIVITY_LOCATION) && compare_loc_flag) {
														in_loc_ptr = location_data.Get (in_trip_ptr->Origin ());
														compare_loc_ptr = compare_loc_data.Get (compare_trip_ptr->Origin ());

														if (in_loc_ptr != NULL && compare_loc_ptr != NULL) {
															diff = abs (in_loc_ptr->X () - compare_loc_ptr->X ());

															if (diff < Round (100)) {
																diff = abs (in_loc_ptr->Y () - compare_loc_ptr->Y ());

																if (diff < Round (100)) {
																	in_loc_ptr = location_data.Get (in_trip_ptr->Destination ());
																	compare_loc_ptr = compare_loc_data.Get (compare_trip_ptr->Destination ());

																	if (in_loc_ptr != NULL && compare_loc_ptr != NULL) {
																		diff = abs (in_loc_ptr->X () - compare_loc_ptr->X ());

																		if (diff < Round (100)) {
																			diff = abs (in_loc_ptr->Y () - compare_loc_ptr->Y ());

																			if (diff < Round (100)) {
																				match_flag = true;
																			} else {
																				type = 12;
																			}
																		} else {
																			type = 11;
																		}
																	} else {
																		type = 10;
																	}
																} else {
																	type = 9;
																}
															} else {
																type = 8;
															}
														} else {
															type = 7;
														}
													} else {
														match_flag = true;
													}
												} else {
													type = 6;
												}
											} else {
												type = 5;
											}
										} else {
											type = 4;
										}
									} else {
										type = 3;
									}
								} else {
									type = 2;
								}
							} else {
								type = 1;
							}
						}
						if (match_flag && access_flag) {
							tim_diff = in_ttime - compare_ttime;
							cst_diff = last_cost_in - last_cost_compare;

							if (cost_flag) {
								mode_in [0] = last_cost_in;
								mode_compare [0] = last_cost_compare;
							} else {
								mode_in [0] = in_ttime;
								mode_compare [0] = compare_ttime;
							}
							if (cst_diff > 0) {
								match_flag = (tim_diff < 300 && 
									mode_in [1] < 1.5 * mode_compare [1] &&
									mode_in [2] < 1.5 * mode_compare [2] &&
									mode_in [3] < 1.5 * mode_compare [3] &&
									mode_in [4] < 1.5 * mode_compare [4]);
							} else {
								match_flag = (tim_diff > -600); 
							}
							type = 13;
						}
						if (!match_flag) {
							if (incomplete_flag) {
								if (type == 1) {
									compare_trip_ptr = compare_trip_data.Get (last_compare, last_trip_compare);

									if (compare_trip_ptr != NULL) {
										diff = compare_trip_ptr->End_Time () - compare_trip_ptr->Start_Time ();
										mode = compare_trip_ptr->Mode ();

										if (last_start_id_compare == compare_trip_ptr->Origin () && 
											last_end_id_compare == compare_trip_ptr->Destination ()) {
											type = 14;
										} else {
											type = 15;
										}
									}
								}
	   							compare_ttime = last_end_compare - last_start_compare;
								in_ttime = last_end_in - last_start_in;
								tim_diff = in_ttime - compare_ttime;
								cst_diff = last_cost_in - last_cost_compare;

								fprintf (bad_file, "\n%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
									last_in, last_trip_in, mode, diff, type, in_ttime, last_cost_in, 
									compare_ttime, last_cost_compare, tim_diff, cst_diff);

								//if (type == 1) {
								//	fprintf (bad_file, "\ttrip=%d %d vs. %d %d",
								//		in_trip_ptr->Origin (), in_trip_ptr->Destination (), last_start_id_in, last_end_id_in);
								//}
								//if (type == 3) {
								//	fprintf (bad_file, "\ttrip=%d %d vs %d %d",
								//		compare_trip_ptr->Origin (), compare_trip_ptr->Destination (), last_start_id_compare, last_end_id_compare);
								//}
								//if (type == 4) {
								//	fprintf (bad_file, "\tmode=%d vs %d", mode, compare_trip_ptr->Mode ());
								//}
								//if (type == 13) {
								//	fprintf (bad_file, "\tmode diff = %d %d %d %d vs. %d %d %d %d", 
								//		mode_in [1], mode_in [2], mode_in [3], mode_in [4], 
								//		mode_compare [1], mode_compare [2], mode_compare [3], mode_compare [4]);
								//}
							}
							num_bad++;
						}
					}
					if (match_flag) {
						num_matched++;
						save_in = false;

						//---- drive only comparisons ----

						if (drive_flag) {
							match_flag = (drive_in && drive_compare);

							if (match_flag) {
								last_start_id_in = input_temp.Start_ID ();
								last_start_type_in = input_temp.Start_Type ();
								last_end_id_in = input_temp.End_ID ();
								last_end_type_in = input_temp.End_Type ();
								last_start_id_compare = compare_temp.Start_ID ();
								last_start_type_compare = compare_temp.Start_Type ();
								last_end_id_compare = compare_temp.End_ID ();
								last_end_type_compare = compare_temp.End_Type ();

								match_flag = ((last_start_id_in == last_start_id_compare &&
									last_end_id_in == last_end_id_compare) || ignore_flag);

								if (match_flag) {
									num_drive++;

									last_start_in = input_temp.Time ();
									last_end_in = input_temp.Stop_Time ();
									last_cost_in = input_temp.GCF ();

									last_start_compare = compare_temp.Time ();
									last_end_compare = compare_temp.Stop_Time ();
									last_cost_compare = compare_temp.GCF ();
								}
							}

						} else {

							//---- match the origin and destination ----

							if (!trip_data_flag) {
								match_flag = (last_start_id_in == last_start_id_compare && 
									last_end_id_in == last_end_id_compare);
							}
							if (match_flag) {
								num_complete++;
							} else if (drive_in && drive_compare) {

								//---- match drive legs ----

								last_start_id_in = input_temp.Start_ID ();
								last_start_type_in = input_temp.Start_Type ();
								last_end_id_in = input_temp.End_ID ();
								last_end_type_in = input_temp.End_Type ();
								last_start_id_compare = compare_temp.Start_ID ();
								last_start_type_compare = compare_temp.Start_Type ();
								last_end_id_compare = compare_temp.End_ID ();
								last_end_type_compare = compare_temp.End_Type ();

								match_flag = ((last_start_id_in == last_start_id_compare &&
									last_end_id_in == last_end_id_compare) || ignore_flag);

								if (match_flag) {
									num_drive++;

									last_start_in = input_temp.Time ();
									last_end_in = input_temp.Stop_Time ();
									last_cost_in = input_temp.GCF ();

									last_start_compare = compare_temp.Time ();
									last_end_compare = compare_temp.Stop_Time ();
									last_cost_compare = compare_temp.GCF ();
								}
							} else if (!drive_in && !drive_compare && ignore_flag) {
								match_flag = true;
							}
						}

						//---- check for time constraints ----

						if (match_flag && time_select) {
							if (!select_time.Span_Range (last_start_compare, last_end_compare)) {
								match_flag = false;
							}
						}

						//---- check the low-high range ----

						if (match_flag) {
							if (cost_flag) {
								diff = last_cost_in - last_cost_compare;
							} else {
								compare_ttime = last_end_compare - last_start_compare;
								in_ttime = last_end_in - last_start_in;

								diff = in_ttime - compare_ttime;
							}
							match_flag = (low_diff <= diff && diff <= high_diff);
						}

						//---- process a traveler and trip match ----

						if (match_flag) {
							num_compared++;

							//---- time difference ----

							if (period_point == 0) {
								tod = (last_start_compare + last_end_compare) >> 1;
							} else if (period_point == 1) {
								tod = last_start_compare;
							} else {
								tod = last_end_compare;
							}
							compare_ttime = last_end_compare - last_start_compare;
							if (compare_ttime < 0) {
								Warning ("Compare Traveler %d Trip %d Travel Time is %d", last_compare, last_trip_compare, compare_ttime);
								compare_ttime = 0;
							}
							in_ttime = last_end_in - last_start_in;
							if (in_ttime < 0) {
								Warning ("Input Traveler %d Trip %d Travel Time is %d", last_in, last_trip_in, in_ttime);
								in_ttime = 0;
							}
							tim_diff = in_ttime - compare_ttime;
							cst_diff = last_cost_in - last_cost_compare;

							if (cost_flag) {
								if (mode_flag) {
									mode_in [0] = last_cost_in;
									mode_compare [0] = last_cost_compare;
								}
								base = last_cost_compare;
								diff = abs (cst_diff);
							} else {
								if (mode_flag) {
									mode_in [0] = in_ttime;
									mode_compare [0] = compare_ttime;
								}
								base = compare_ttime;
								diff = abs (tim_diff);
							}
							if (base > 0) {
								share = (double) diff / (double) base;
							} else {
								share = (double) diff / 1;
							}

							//---- add to the difference distribution ----

							period = time_periods.In_Index (tod);

							if (period > 0) {
								time_diff.Add_Trip (tod, in_ttime, compare_ttime);
								cost_diff.Add_Trip (tod, last_cost_in, last_cost_compare);

								if (mode_file_flag) {
									mode_diff.Add_Trip_Mode (5, mode_in, mode_compare);
								}
								if (mode_data_flag) {
									mode_data.Add_Trip_Mode (5, mode_in, mode_compare);
								}

								//---- difference file ----

								if (diff_file_flag) {
									if (cost_flag) {
										fprintf (diff_file.File (), "%d\t%d\t%d\t%d\t%d\t%d\n",
											last_hhold, last_person, last_trip_in, last_cost_in, 
											last_cost_compare, cst_diff);
									} else {
										fprintf (diff_file.File (), "%d\t%d\t%d\t%d\t%d\t%d\n",
											last_hhold, last_person, last_trip_in, in_ttime, 
											compare_ttime, tim_diff);
									}
								}
								if (start_flag) {
									if (loc_flag) {
										loc_rec.location = last_start_id_compare;
										loc_rec.type = last_start_type_compare;
									} else {
										loc_rec.location = last_start_id_in;
										loc_rec.type = last_start_type_in;
									}
									loc_rec.count = 1;
									if (cost_flag) {
										loc_rec.input = last_cost_in;
										loc_rec.compare = last_cost_compare;
										loc_rec.diff = cst_diff;
									} else {
										loc_rec.input = in_ttime;
										loc_rec.compare = compare_ttime;
										loc_rec.diff = tim_diff;
									}

									loc_ptr = (Location_Diff *) start_diff.Get (&loc_rec);

									if (loc_ptr == NULL) {
										if (!start_diff.Add (&loc_rec)) goto mem_error;
									} else {
										loc_ptr->count++;
										loc_ptr->input += loc_rec.input;
										loc_ptr->compare += loc_rec.compare;
										loc_ptr->diff += loc_rec.diff;
									}
								}
								if (end_flag) {
									if (loc_flag) {
										loc_rec.location = last_end_id_compare;
										loc_rec.type = last_end_type_compare;
									} else {
										loc_rec.location = last_end_id_in;
										loc_rec.type = last_end_type_in;
									}
									loc_rec.count = 1;
									if (cost_flag) {
										loc_rec.input = last_cost_in;
										loc_rec.compare = last_cost_compare;
										loc_rec.diff = cst_diff;
									} else {
										loc_rec.input = in_ttime;
										loc_rec.compare = compare_ttime;
										loc_rec.diff = tim_diff;
									}

									loc_ptr = (Location_Diff *) end_diff.Get (&loc_rec);

									if (loc_ptr == NULL) {
										if (!end_diff.Add (&loc_rec)) goto mem_error;
									} else {
										loc_ptr->count++;
										loc_ptr->input += loc_rec.input;
										loc_ptr->compare += loc_rec.compare;
										loc_ptr->diff += loc_rec.diff;
									}
								}
								if (gap_data_flag) {
									period_ptr = (Period_Diff *) period_diff.Record (period);
									period_ptr->count++;

									if (cost_flag) {
										period_ptr->input += last_cost_in;
										period_ptr->compare += last_cost_compare;
										period_ptr->diff += cst_diff;
									} else {
										period_ptr->input += in_ttime;
										period_ptr->compare += compare_ttime;
										period_ptr->diff += tim_diff;
									}
								}
							}

							//---- save the percentile distribution ----

							if (compare_flag) {
								if (tod >= MIDNIGHT) tod = tod % MIDNIGHT;

								tod = tod * TOTAL_TIME_PERIOD / MIDNIGHT;

								if (tod < 0) tod = 0;
								if (tod >= TOTAL_TIME_PERIOD) tod = TOTAL_TIME_PERIOD - 1;

								if (compare_ttime > 0) {
									factor = (double) abs (tim_diff) / (double) compare_ttime;
								} else {
									factor = (double) abs (tim_diff) / 1;
								}
								j = (int) (factor * NUM_CHANGE_BINS + 0.5);
								if (j >= NUM_CHANGE_BINS) j = NUM_CHANGE_BINS - 1;

								time_changes [tod] [j]++;
								time_changes [TOTAL_TIME_PERIOD] [j]++;

								percent = (int) (factor * 100.0 + 0.5);

								if (percent >= dump_time) {
									Print (2, "Travel Time Dump: Traveler=%d, Start Time=%d, Compare=%d, Input=%d, Change=%.1lf%%", 
										last_in, last_start_in, compare_ttime, in_ttime, factor * 100.0);
								}
								if (cost_data_flag) {
									if (last_cost_compare > 0) {
										factor = (double) abs (cst_diff) / (double) last_cost_compare;
									} else {
										factor = (double) abs (cst_diff) / 1;
									}
									j = (int) (factor * NUM_CHANGE_BINS + 0.5);
									if (j >= NUM_CHANGE_BINS) j = NUM_CHANGE_BINS - 1;

									cost_changes [tod] [j]++;
									cost_changes [TOTAL_TIME_PERIOD] [j]++;

									percent = (int) (factor * 100.0 + 0.5);

									if (percent >= dump_cost) {
										Print (2, "Generalized Cost Dump: Traveler=%d, Start Time=%d, Compare=%d, Input=%d, Change=%.1lf%%", 
											last_in, last_start_in, last_cost_compare, last_cost_in, factor * 100.0);
									}
								}
							}

							//---- trip time data ----

							if (trip_flag) {
								time_ptr = trip_time + (last_start_in % MIDNIGHT) / 900;
								time_ptr->input_start++;

								time_ptr = trip_time + (last_end_in % MIDNIGHT) / 900;
								time_ptr->input_end++;

								time_ptr = trip_time + (last_start_compare % MIDNIGHT) / 900;
								time_ptr->compare_start++;
								time_ptr->start_diff += (last_start_in - last_start_compare);

								time_ptr = trip_time + (last_end_compare % MIDNIGHT) / 900;
								time_ptr->compare_end++;
								time_ptr->end_diff += (last_end_in - last_end_compare);
							}

							//---- set the selection flag ----

							if (time_diff_flag && diff >= min_time) {
								if (diff < max_time) {
									if (share >= percent_diff) {
										if (gap_flag) {
											save_in = (random.Probability () <= share);
											share = ((save_in) ? 1.0 : 0.0);
										} else {
											save_in = true;
										}
									}
								} else {
									save_in = true;
									share = 1.0;
								}
							}

							//---- path difference ----

							if (((drive_in && drive_compare) || (transit_in && transit_compare)) && 
								(((save_in || !time_diff_flag) && path_diff_flag) || (compare_flag && path_flag))) {

								//---- compare drive paths ----

								if (drive_compare && drive_in) {
									path1 = compare_temp.Path (&num1);
									path2 = input_temp.Path (&num2);

									if (num2 > num1) {
										ndiff = num2 - num1;
									} else {
										ndiff = 0;
									}
									for (i=j1=1; i <= num1; i++, path1++) {
										p2 = path2;
										found = false;

										for (j=j1; j <= num2; j++, p2++) {
											if (*path1 == *p2) {
												path2 += j - j1 + 1;
												j1 = j + 1;
												found = true;
												break;
											}
										}
										if (!found) ndiff++;
									}
									if (num1 > 0) {
										share2 = (double) ndiff / (double) num1;
										if (share2 > 1.0) share2 = 1.0;
									} else {
										share2 = 1.0;
									}
								} else {

									//---- compare transit paths ----

									if (legs_in > legs_compare) {
										ndiff = legs_in - legs_compare;
									} else {
										ndiff = 0;
									}
									for (i=j1=0; i < legs_compare; i++) {
										found = false;

										for (j=j1; j < legs_in; j++) {
											if (route_compare [i] == route_in [j] &&
												board_compare [i] == board_in [j] && 
												alight_compare [i] == alight_in [j]) {
												j1 = j + 1;
												found = true;
												break;
											}
										}
										if (!found) ndiff++;
									}
									if (legs_compare > 0) {
										share2 = (double) ndiff / (double) legs_compare;
										if (share2 > 1.0) share2 = 1.0;
									} else {
										share2 = 1.0;
									}
								}

								//---- save the difference distribution ----

								if (compare_flag && path_flag) {
									j = (int) (share2 * NUM_CHANGE_BINS + 0.5);
									if (j >= NUM_CHANGE_BINS) j = NUM_CHANGE_BINS - 1;

									path_changes [tod] [j]++;
									path_changes [TOTAL_TIME_PERIOD] [j]++;

									percent = (int) (share2 * 100.0 + 0.5);

									if (percent >= dump_path) {
										Print (2, "Path Dump: Traveler=%d, Start Time=%d, Change=%.1lf%%", 
											last_in, last_start_in, share2 * 100.0);

										if (drive_in && drive_compare) {
											Print (1, "  Compare: ");
											path1 = compare_temp.Path (&num1);

											for (i=0; i < num1; i++, path1++) {
												if (i && !(i % 10)) Print (1, "%11c", BLANK);
												Print (0, "%d ", *path1);
											}
											Print (1, "  Input  : ");
											path2 = input_temp.Path (&num2);

											for (i=0; i < num2; i++, path2++) {
												if (i && !(i % 10)) Print (1, "%11c", BLANK);
												Print (0, "%d ", *path2);
											}
										} else {
											Print (1, "  Compare: ");

											for (i=0; i < legs_compare; i++) {
												if (i && !(i % 3)) Print (1, "%11c", BLANK);
												Print (0, "from %d to %d on %d  ", board_compare [i], alight_compare [i], route_compare [i]);
											}

											Print (1, "  Input  : ");
											for (i=0; i < legs_in; i++) {
												if (i && !(i % 3)) Print (1, "%11c", BLANK);
												Print (0, "from %d to %d on %d  ", board_in [i], alight_in [i], route_in [i]);
											}
										}
									}
								}

								//---- check the selection criteria ----

								if (!time_diff_flag) {
									if (ndiff >= min_path) {
										if (ndiff < max_path) {
											if (share2 >= path_diff) save_in = true;
										} else {
											save_in = true;
											share2 = 1.0;
										}
									}
								} else if (save_in && path_diff_flag) {
									if (ndiff < min_path) {
										save_in = false;
									} else if (ndiff < max_path) {
										if (share2 < path_diff) save_in = false;
									} else {
										share2 = 1.0;
									}
								}
							}
							if (!save_in) {
								num_equal++;
							} else {
								abs_diff += diff;
								tot_base += base;
								sig_diff += diff - min_time;
							}
							sig_tot += base;
						}
						if (match_out_flag) {
							in_ttime = last_end_in - last_start_in;
							compare_ttime = last_end_compare - last_start_compare;

							fprintf (out_file, "\n%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
								last_in, last_trip_in, 
								last_start_in, last_start_id_in, last_start_type_in, 
								last_end_in, last_end_id_in, last_end_type_in, 
								in_ttime, last_cost_in);
							fprintf (out_file, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",  
								last_start_compare, last_start_id_compare, last_start_type_compare, 
								last_end_compare, last_end_id_compare, last_end_type_compare, 
								compare_ttime, last_cost_compare, 
								(compare_ttime - in_ttime), (last_cost_compare - last_cost_in));
						}
					} else {
						save_in = true;

						if (match_out_flag) {
							in_ttime = last_end_in - last_start_in;

							fprintf (out_file, "\n%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
								last_in, last_trip_in, 
								last_start_in, last_start_id_in, last_start_type_in, 
								last_end_in, last_end_id_in, last_end_type_in, 
								in_ttime, last_cost_in);
						}
					}

					//---- write the input plan in the output list ----

					if (save_in) {
						sel_rec.traveler = last_in;
						sel_rec.trip = last_trip_in;

						if (!select_trips.Add (&sel_rec)) {
							Error ("Adding Trip to the Selection List");
						}
						if (diff_flag || gap_flag) {
							if (path_diff_flag) share += share2;

							diff_rec.percent = (int) (share * 10000.0 + 0.5);
							diff_rec.traveler = last_in;
							diff_rec.trip = last_trip_in;
							diff_rec.time_diff = tim_diff;

							if (!travel_diff.Add (&diff_rec)) {
								Error ("Adding Traveler to the Travel Difference List");
							}
						}
						nsel++;
					}
				}
			}
			if (next_in == MAX_INTEGER && next_compare == MAX_INTEGER) break;

			if (next_in == MAX_INTEGER) {
				last_in = MAX_INTEGER;
				last_trip_in = 0;
			}
			if (next_compare == MAX_INTEGER) {
				last_compare = MAX_INTEGER;
				last_trip_compare = 0;
			}
		}

		if (last_in == 0 && last_compare == 0) {
			read_in = true;
			read_compare = true;
		} else if (next_compare < next_in) {
			read_in = false;
			read_compare = true;
			compare_only++;

			if (match_out_flag) {
				compare_ttime = last_end_compare - last_start_compare;

				fprintf (out_file, "\n%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
					last_compare, last_trip_compare, 0, 0, 0, 0, 0, 0, 0, 0);
				fprintf (out_file, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",  
					last_start_compare, last_start_id_compare, last_start_type_compare, 
					last_end_compare, last_end_id_compare, last_end_type_compare, 
					compare_ttime, last_cost_compare);
			}
		} else if (next_compare > next_in) {
			read_in = true;
			read_compare = false;
			in_only++;
		} else if (next_trip_compare < next_trip_in) {
			read_in = false;
			read_compare = true;
			compare_only++;
			if (match_out_flag) {
				compare_ttime = last_end_compare - last_start_compare;

				fprintf (out_file, "\n%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
					last_compare, last_trip_compare, 0, 0, 0, 0, 0, 0, 0, 0);
				fprintf (out_file, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",  
					last_start_compare, last_start_id_compare, last_start_type_compare, 
					last_end_compare, last_end_id_compare, last_end_type_compare, 
					compare_ttime, last_cost_compare);
			}
		} else if (next_trip_compare > next_trip_in) {
			read_in = true;
			read_compare = false;
			in_only++;
		} else {
			read_in = true;
			read_compare = true;
		}
		Show_Progress ();

		//---- get the next input plan ----

		if (read_in) {
			if (mode_flag) memset (mode_in, '\0', sizeof (mode_in));

			last_hhold = input_plans.Household ();
			last_person = input_plans.Person ();
			last_in = input_plans.Traveler ();
			last_trip_in = input_plans.Trip ();
			last_start_in = input_plans.Time ();
			last_start_id_in = input_plans.Start_ID ();
			last_start_type_in = input_plans.Start_Type ();
			drive_in = transit_in = false;
			legs_in = last_cost_in = 0;

			for (; next_in == last_in && next_trip_in == last_trip_in; ) {
				last_end_in = input_plans.Stop_Time ();
				last_end_id_in = input_plans.End_ID ();
				last_end_type_in = input_plans.End_Type ();
				last_cost_in += input_plans.GCF ();

				if (mode_flag) {
					mode_in [4] += input_plans.Cost ();
					in_ttime = input_plans.Stop_Time () - input_plans.Time ();
				}
				if (input_plans.Start_Type () == Network_Code::PARKING_ID && 
					input_plans.End_Type () == Network_Code::PARKING_ID) {
					input_temp.Plan (input_plans.Plan ());
					drive_in = true;
					if (mode_flag) {
						mode_in [2] += in_ttime;
					}
				} else if (input_plans.Start_Type () == Network_Code::STOP_ID && 
					input_plans.End_Type () == Network_Code::STOP_ID) {
					transit_in = true;
					if (legs_in < MAX_LEGS) {
						board_in [legs_in] = input_plans.Start_ID ();
						alight_in [legs_in] = input_plans.End_ID ();
						route_in [legs_in] = input_plans.Route ();
						legs_in++;
					}
					if (mode_flag) {
						mode_in [3] += in_ttime;
					}
				} else if (mode_flag) {
					mode_in [1] += in_ttime;
				}
				if (input_plans.Read ()) {
					next_in = input_plans.Traveler ();
					next_trip_in = input_plans.Trip ();
				} else {
					next_in = MAX_INTEGER;
					next_trip_in = 0;
				}
			}
			if (next_in < last_in) {
				Error ("The Input Plan File is Not Sorted by Traveler (%d < %d)", next_in, last_in);
			}
		}

		//---- get the next compare plan ----

		if (read_compare) {
			if (mode_flag) memset (mode_compare, '\0', sizeof (mode_compare));

			last_compare = compare_plans.Traveler ();
			last_trip_compare = compare_plans.Trip ();
			last_start_compare = compare_plans.Time ();
			last_start_id_compare = compare_plans.Start_ID ();
			last_start_type_compare = compare_plans.Start_Type ();
			drive_compare = transit_compare = false;
			legs_compare = last_cost_compare = 0;

			for (; next_compare == last_compare && next_trip_compare == last_trip_compare; ) {
				last_end_compare = compare_plans.Stop_Time ();
				last_end_id_compare = compare_plans.End_ID ();
				last_end_type_compare = compare_plans.End_Type ();
				last_cost_compare += compare_plans.GCF ();

				if (mode_flag) {
					mode_compare [4] += compare_plans.Cost ();
					compare_ttime = compare_plans.Stop_Time () - compare_plans.Time ();
				}
				if (compare_plans.Start_Type () == Network_Code::PARKING_ID && 
					compare_plans.End_Type () == Network_Code::PARKING_ID) {
					compare_temp.Plan (compare_plans.Plan ());
					drive_compare = true;
					if (mode_flag) {
						mode_compare [2] += compare_ttime;
					}
				} else if (compare_plans.Start_Type () == Network_Code::STOP_ID && 
					compare_plans.End_Type () == Network_Code::STOP_ID) {
					transit_compare = true;
					if (legs_compare < MAX_LEGS) {
						board_compare [legs_compare] = compare_plans.Start_ID ();
						alight_compare [legs_compare] = compare_plans.End_ID ();
						route_compare [legs_compare] = compare_plans.Route ();
						legs_compare++;
					}
					if (mode_flag) {
						mode_compare [3] += compare_ttime;
					}
				} else if (mode_flag) {
					mode_compare [1] += compare_ttime;
				}
				if (compare_plans.Read ()) {
					next_compare = compare_plans.Traveler ();
					next_trip_compare = compare_plans.Trip ();
				} else {
					next_compare = MAX_INTEGER;
					next_trip_compare = 0;
				}
			}
			if (next_compare < last_compare) {
				Error ("The Compare Plan File is Not Sorted by Traveler (%d < %d)", next_compare, last_compare);
			}
		}
	}
	End_Progress ();

	input_plans.Close ();
	compare_plans.Close ();

	return;

mem_error:
	Error ("Insufficient Memory for Location Data");
}
