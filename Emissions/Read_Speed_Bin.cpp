//*********************************************************
//	Read_Speed_Bin.cpp - read the speed bin files
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Read_Speed_Bin
//---------------------------------------------------------

void Emissions::Read_Speed_Bin (void)
{
	int i, j, num, link, dir, link_dir, sample_time, time, type, subtype, size, group, bin;
	int period, road, vehicle, per_id, road_id, veh_id, bin_id, num_out, hpms;
	int id, member, veh_base, base_id, ems_id, year_id, year, month, count, unit;
	double weight, per_wt, road_wt, veh_wt, emissions, vmt, vht, units;
	bool ramp_flag, base_flag, keep;
	bool rg_flag [200], at_flag [200], ft_flag [200], rt_flag [200], p_flag [200], sb_flag [200], u_flag [200];

	Category_Data *per_ptr, *rt_ptr, *veh_ptr, *bin_ptr, *hpms_ptr;
	Category_Data *year_ptr, *mon_ptr, *reg_ptr, *at_ptr, *fac_ptr, *pol_ptr;
	Value_Map *per_map, *road_map, *veh_map;

	Speed_Bin_File *file;
	Speed_Bin_Data *box_ptr, box_rec, *spd_ptr;
	Data_Group *data_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Bin_Sum_Data *bsum_ptr, bsum_rec;
	Summary_Data *sum_ptr, sum_rec;
	Emission_Rate *rate_ptr;
	
	//---- initialize the data summary codes ----

	if (input_flag) {
		size = data_group.Num_Records () * EXTERNAL * time_periods.Num_Ranges () * num_bins;
		bin_sum_data.Max_Records (size);
	}
	if (rate_flag) {
		summary_data.Pollutants (num_pollutants);
	}

	year_ptr = category_data.Get (YEAR, true);
	mon_ptr = category_data.Get (MONTH, true);
	per_ptr = category_data.Get (PERIOD);
	reg_ptr = category_data.Get (REGION, true);
	at_ptr = category_data.Get (AREATYPE, true);
	fac_ptr = category_data.Get (FACILITY, true);
	rt_ptr = category_data.Get (ROADTYPE);
	veh_ptr = category_data.Get (VEHTYPE);
	pol_ptr = category_data.Get (POLLUTANT, true);
	bin_ptr = category_data.Get (SPEED_BIN, true);
	hpms_ptr = category_data.Get (HPMS);

	memset (rg_flag, '\0', sizeof (rg_flag));
	memset (at_flag, '\0', sizeof (at_flag));
	memset (ft_flag, '\0', sizeof (ft_flag));
	memset (rt_flag, '\0', sizeof (rt_flag));
	memset (p_flag, '\0', sizeof (p_flag));
	memset (sb_flag, '\0', sizeof (sb_flag));
	memset (u_flag, '\0', sizeof (u_flag));
	memset (rt_flag, '\0', sizeof (rt_flag));
	memset (ramp_vmt, '\0', sizeof (ramp_vmt));
	memset (hpms_vmt, '\0', sizeof (hpms_vmt));

	year = month = 1;
	spd_ptr = NULL;
	num = 0;

	//---- process each data group ----

	for (data_ptr = data_group.First (); data_ptr; data_ptr = data_group.Next ()) {

		file = data_ptr->Speed_File ();

		file->Rewind ();

		group = data_ptr->Group ();
		num++;

		//---- check the vehicle type ----

		vehicle = type = file->Vehicle_Type ();
		subtype = file->Veh_SubType ();

		if (veh_map_flag) {
			vehicle = vehicle_map.Map (type, subtype);

			if (vehicle == 0) {
				Warning ("Vehicle Type %d-%d is Not in the Type Map", type, subtype);
				continue;
			}
		}
		if (rate_flag) {
			sum_rec.Vehicle_Type (vehicle);

			if (veh_ptr != NULL) {
				member = veh_ptr->Member (vehicle);

				if (member == 0) {
					Warning ("Vehicle Type %d-%d does not have Emission Rates", type, subtype);
					continue;
				}
				veh_base = member * veh_ptr->Base_Index ();
			} else {
				veh_base = 1;
			}
		}
		if (hpms_flag) {
			hpms = hpms_type_map.Map (type, subtype);

			if (hpms == 0) {
				Warning ("HPMS Type %d-%d is Not in the Type Map", type, subtype);
			}
		}

		//---- set the calculation values ----

		sample_time = file->Sample_Time ();
		if (sample_time < 1) sample_time = 1;

		//---- process each record ----

		Show_Message ("Reading %s -- Record", file->File_Type ());
		Set_Progress (10000);

		while (file->Read ()) {
			Show_Progress ();

			//---- identify the time period ----

			time = time_periods.Step (file->End_Time ()) - 1;
			period = time_periods.Period (time);

			if (period == 0) continue;

			//---- identify the link_dir index ----

			link = file->Link ();
			dir = file->Dir ();

			if (file->LinkDir_Type () == LINK_SIGN) {
				if (link < 0) {
					link = -link;
					dir = 1;
				}
			}
			link_ptr = link_data.Get (link);

			if (link_ptr == NULL) {
				Warning ("Link %d was Not Found in the Link File", link);
				continue;
			}
			if (file->LinkDir_Type () == LINK_NODE) {
				if (dir == link_ptr->Anode ()) {
					dir = 1;
				} else if (dir == link_ptr->Bnode ()) {
					dir = 0;
				} else {
					Error ("Node %d Link %d is Incompatible", dir, link);
				}
			}

			//---- get the direction index ----

			if (dir == 0) {
				link_dir = link_ptr->AB_Dir ();
			} else {
				link_dir = link_ptr->BA_Dir ();
			}
			if (link_eq_flag && link_list.Get_Index (link_dir) == 0) continue;

			dir_ptr = dir_data [link_dir];

			if (dir_ptr == NULL || dir_ptr->Thru () == 0) {
				Warning ("Link %d in the %s Direction has No Lanes", link, ((dir == 0) ? "AB" : "BA"));
				continue;
			}

			//---- retrieve the segment records ----

			box_rec.link_dir = link_dir;
			box_rec.offset = Round (file->Offset ());

			box_ptr = box_location.Get (&box_rec);

			if (box_ptr == NULL) {
				Warning ("Link %d Offset %.1lf was Not Found", link, UnRound (box_rec.offset));
				continue;
			}
			if (link_flag) {
				spd_ptr = speed_bin_data.Get (&box_rec);

				if (spd_ptr != NULL) {
					j = (num - 1) * num_bins;

					for (i=0; i < num_bins; i++) {
						spd_ptr->bin [i+j] += file->Bin (i) * sample_time;
					}
				}
			}

			//---- get the road type ----

			subtype = box_ptr->bin [0] & 0xFF;
			if (subtype == 0) continue;

			road = type = link_ptr->Type ();
			if (type == 0) continue;

			ramp_flag = (type == RAMP);

			if (road_map_flag) {
				road = road_type_map.Map (type, subtype);

				if (road == 0) {
					road = type * 10 + subtype;
					if (!rt_flag [road]) {
						Warning ("Road Type %d-%d is Not in the Type Map", type, subtype);
						rt_flag [road] = true;
					}
					continue;
				}
			}

			//---- calculate the rate index ----

			if (rate_flag) {
				base_id = veh_base;

				sum_rec.Road_Type (road);

				if (rt_ptr != NULL && rt_ptr->Key_Flag ()) {
					member = rt_ptr->Member (road);

					if (member == 0) {
						road = type * 10 + subtype;
						if (!rt_flag [road]) {
							Warning ("Road Type %d-%d does not have Emission Rates", type, subtype);
							rt_flag [road] = true;
						}
						continue;
					}
					base_id += member * rt_ptr->Base_Index ();
				}

				//---- get the region ----

				type = box_ptr->bin [0] >> 8;

				if (reg_ptr != NULL) {
					member = reg_ptr->Member (type);

					if (member == 0) {
						if (!rg_flag [type]) {
							Warning ("Region %d does not have Emission Rates", type);
							rg_flag [type] = true;
						}
						continue;
					}
					base_id += member * reg_ptr->Base_Index ();
				}

				//---- get the area type ----

				id = type = box_ptr->bin [0] & 0xFF;

				if (at_map_flag) {
					id = area_type_map.Map (type);

					if (id == 0) {
						if (!at_flag [type]) {
							Warning ("Area Type %d is Not in the Type Map", type);
							at_flag [type] = true;
						}
						continue;
					}
				}
				sum_rec.Area_Type (id);

				if (at_ptr != NULL) {
					member = at_ptr->Member (id);

					if (member == 0) {
						if (!at_flag [type]) {
							Warning ("Area Type %d does not have Emission Rates", type);
							at_flag [type] = true;
						}
						continue;
					}
					base_id += member * at_ptr->Base_Index ();
				}

				//---- identify the facility type ----

				id = type = link_ptr->Type ();

				if (fac_map_flag) {
					id = facility_map [type];

					if (id == 0) {
						if (!ft_flag [type]) {
							Warning ("Facility Type %d is Not in the Type Map", type);
							ft_flag [type] = true;
						}
						continue;
					}
				}
				sum_rec.Facility_Type (id);

				if (fac_ptr != NULL) {
					member = fac_ptr->Member (id);

					if (member == 0) {
						if (!ft_flag [type]) {
							Warning ("Facility Type %d does not have Emission Rates", type);
							ft_flag [type] = true;
						}
						continue;
					}
					base_id += member * fac_ptr->Base_Index ();
				}

				//---- identify the time period ----

				if (per_ptr != NULL && per_ptr->Key_Flag ()) {
					member = per_ptr->Member (period);

					if (member == 0) {
						if (!p_flag [period]) {
							Warning ("Period %d does not have Emission Rates", period);
							p_flag [period] = true;
						}
						continue;
					}
					base_id += member * per_ptr->Base_Index ();
				}

				//---- get the summary record ----

				sum_ptr = summary_data.Get (&sum_rec);

				if (sum_ptr == NULL) {
					sum_ptr = new Summary_Data (num_pollutants);
					sum_ptr->Zero (&sum_rec);

					if (!summary_data.Add (sum_ptr)) {
						Error ("Adding Summary Record");
					}
					sum_ptr = summary_data.Record ();
				}
			}

			//---- distribute the speed data ----

			for (j=0; j <= num_bins; j++) {
				spd_bin [j] = file->Bin (j) * sample_time * data_ptr->Factor ();
			}
			num_out = Speed_Bin_Distribution (group);

			//---- speed bin loop ----

			for (bin=1; bin <= num_out; bin++) {
				vht = vht_bin [bin];
				vmt = vmt_bin [bin];

				if (vht == 0.0) continue;

				//---- process the emissions rates ----

				if (rate_flag) {
					bin_id = base_id;

					if (bin_ptr != NULL) {
						member = bin_ptr->Member (bin);

						if (member == 0) {
							if (!sb_flag [bin]) {
								Warning ("Speed Bin %d does not have Emission Rates", bin);
								sb_flag [bin] = true;
							}
							continue;
						}
						bin_id += member * bin_ptr->Base_Index ();
					}

					//---- save each pollutant ----

					keep = false;
					count = 0;

					for (j=1; j <= num_pollutants; j++) {

						//---- rate units ----

						unit = pollutant_units [j];
						if (unit == NO_UNITS) {
							if (!u_flag [j]) {
								Warning ("Pollutant %d does not have Emission Rate Units", j);
								u_flag [j] = true;
							}
							continue;
						}

						if (unit == GRAMS_PER_HOUR || unit == POUNDS_PER_HOUR ||
							unit == JOULES_PER_HOUR || unit == BTUS_PER_HOUR) {

							base_flag = false;
							units = 1.0 / 3600.0;
						} else if (unit == GRAMS_PER_MILE || unit == POUNDS_PER_MILE ||
							unit == JOULES_PER_MILE || unit == BTUS_PER_MILE) {

							base_flag = true;
							units = 1.0 / MILETOMETER;
						} else {
							base_flag = true;
							units = 1.0 / 1000.0;	//---- meters to kilometers
						}

						//---- calculate the rate index ----
		
						ems_id = bin_id + j * pol_ptr->Base_Index ();
						emissions = 0.0;

						//---- sum the years ----

						for (year = 1; year; ) {
							year_id = ems_id;

							if (year_flag) {
								if (year == 1) {
									year = years.First ();
								}
								type = year;
								year = years.Next ();
							} else {
								type = year;
								year = 0;
							}
							if (year_ptr != NULL) {
								type = year_ptr->Member (type);
								if (type == 0) continue;

								year_id += type * year_ptr->Base_Index ();
							}

							//---- sum the months ----

							for (month = 1; month; ) {
								id = year_id;

								if (month_flag) {
									if (month == 1) {
										month = months.First ();
									}
									type = month;
									month = months.Next ();
								} else {
									type = month;
									month = 0;
								}
								if (mon_ptr != NULL) {
									type = mon_ptr->Member (type);
									if (type == 0) continue;

									id += type * mon_ptr->Base_Index ();
								}

								//---- retrieve the emission rate ----

								rate_ptr = emission_rate.Get (id);
								if (rate_ptr == NULL) continue;

								if (base_flag) {
									emissions += rate_ptr->Rate () * vmt * units;
								} else {
									emissions += rate_ptr->Rate () * vht * units;
								}
								count++;
							}
						}
						sum_ptr->Add_Emissions (j, emissions);
						keep = true;
					}
					if (keep) {
						sum_ptr->Add_Meters (vmt * count / num_pollutants);
						sum_ptr->Add_Seconds (vht * count / num_pollutants);
					}
				}

				//---- create MOVES input data ----

				if (input_flag) {
					bsum_rec.Speed_Bin (bin);

					//---- vehicle loop ----

					for (veh_id = 0; ; veh_id++) {
						if (veh_id == 0) {
							veh_map = veh_ptr->First_Member (vehicle, group);
						} else {
							veh_map = veh_ptr->Next_Member (vehicle, group);
							if (veh_map == NULL) break;
						}
						if (veh_map == NULL) {
							veh_id = vehicle;
							veh_wt = 1.0;
						} else {
							veh_id = veh_map->Value ();
							veh_wt = veh_map->Weight ();
						}
						bsum_rec.Vehicle_Type (veh_id);

						//---- road type loop ----

						for (road_id = 0; ; road_id++) {
							if (road_id == 0) {
								road_map = rt_ptr->First_Member (road, group);
							} else {
								road_map = rt_ptr->Next_Member (road, group);
								if (road_map == NULL) break;
							}
							if (road_map == NULL) {
								road_id = road;
								road_wt = 1.0;
							} else {
								road_id = road_map->Value ();
								road_wt = road_map->Weight ();
							}
							bsum_rec.Road_Type (road_id);

							//---- time period loop ----

							for (per_id = 0; ; per_id++) {
								if (per_id == 0) {
									per_map = per_ptr->First_Member (period, group);
								} else {
									per_map = per_ptr->Next_Member (period, group);
									if (per_map == NULL) break;
								}
								if (per_map == NULL) {
									per_id = period;
									per_wt = 1.0;
								} else {
									per_id = per_map->Value ();
									per_wt = per_map->Weight ();
								}
								bsum_rec.Time_Bin (per_id);

								weight = per_wt * road_wt * veh_wt;

								//---- get the bin record ----

								bsum_ptr = bin_sum_data.Get (&bsum_rec);

								if (bsum_ptr == NULL) {
									bsum_ptr = bin_sum_data.New_Record (true);

									bsum_ptr->Zero (&bsum_rec);

									if (!bin_sum_data.Add ()) {
										Error ("Adding Speed Summary Record");
									}
								}
								bsum_ptr->Add_Meters (vmt * weight);
								bsum_ptr->Add_Seconds (vht * weight);

								if (ramp_flag) {
									ramp_vmt [road_id] += vmt * weight;
								}
							}
						}
					}

					//---- apply hpms distribution factors ----

					if (hpms_flag) {
						for (veh_id = 0; ; veh_id++) {
							if (veh_id == 0) {
								veh_map = hpms_ptr->First_Member (hpms, group);
							} else {
								veh_map = hpms_ptr->Next_Member (hpms, group);
								if (veh_map == NULL) break;
							}
							if (veh_map == NULL) {
								veh_id = hpms;
								veh_wt = 1.0;
							} else {
								veh_id = veh_map->Value ();
								veh_wt = veh_map->Weight ();
							}
							hpms_vmt [veh_id] += vmt * veh_wt;
						}
					}
				}
			}
		}
		End_Progress ();
	}

	//---- close the smooth sample file ----

	if (sample_flag) {
		sample_flag = false;
		sample_file.Close ();
		Print (2, "Number of Smooth Sample File Records = %d", num_sample);
	}
}
