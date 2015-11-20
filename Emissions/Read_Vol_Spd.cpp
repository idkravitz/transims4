//*********************************************************
//	Read_Vol_Spd.cpp - read the volume speed file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Read_Volume_Speed
//---------------------------------------------------------

void Emissions::Read_Volume_Speed (void)
{
	int i, j, count, unit, link, period, road, vehicle, per_id, road_id, veh_id, veh_base, bin_id, ems_id, year_id;
	int bin, bin1, bin2, facility, areatype, hpms, year, month, base_id, member, id, type, subtype;
	double weight, per_wt, road_wt, veh_wt, length, speed, volume, spd1, spd2, factor;
	double vol1, vol2, time, vmt, vht, units, emissions;
	bool v_flag [200], at_flag [200], ft_flag [200], rt_flag [200], p_flag [200], sb_flag [200], u_flag [200];
	bool ramp_flag, keep, base_flag;

	Category_Data *year_ptr, *mon_ptr, *per_ptr, *reg_ptr, *at_ptr, *fac_ptr, *rt_ptr, *veh_ptr, *pol_ptr, *bin_ptr, *hpms_ptr;
	Value_Map *per_map, *road_map, *veh_map;
	Bin_Sum_Data *bsum_ptr, bsum_rec;
	Summary_Data *sum_ptr, sum_rec;
	Vol_Spd_Data *vsp_ptr;
	Emission_Rate *rate_ptr;

	//---- initialize the data summary codes ----

	Show_Message ("Reading %s -- Record", vol_spd_file.File_Type ());
	Set_Progress (10000);

	if (input_flag) bin_sum_data.Max_Records (vol_spd_file.Num_Records ());
	if (rate_flag) summary_data.Pollutants (num_pollutants);

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

	memset (v_flag, '\0', sizeof (v_flag));
	memset (at_flag, '\0', sizeof (at_flag));
	memset (ft_flag, '\0', sizeof (ft_flag));
	memset (rt_flag, '\0', sizeof (rt_flag));
	memset (p_flag, '\0', sizeof (p_flag));
	memset (sb_flag, '\0', sizeof (sb_flag));
	memset (u_flag, '\0', sizeof (u_flag));
	memset (ramp_vmt, '\0', sizeof (ramp_vmt));
	memset (hpms_vmt, '\0', sizeof (hpms_vmt));

	year = month = 1;

	if (bin_ptr != NULL) {
		max_bin = bin_ptr->Max_Member ();
	} else {
		max_bin = 16;
	}

	//---- process each link record ----

	while (vol_spd_file.Read ()) {
		Show_Progress ();

		vol_spd_file.Get_Field ("LINK", &link);

		if (link_eq_flag && link_list.Get_Index (link) == 0) continue;

		vol_spd_file.Get_Field (length_field, &length);
		vol_spd_file.Get_Field (facility_field, &facility);
		vol_spd_file.Get_Field (areatype_field, &areatype);
		vol_spd_file.Get_Field (period_field, &time);
		vol_spd_file.Get_Field (speed_field, &speed);

		if (facility == 0) continue;

		if (length_units > 0) {	
			if (length_units == 1) {			//---- kilometers ----
				length *= 1000.0 / MILETOMETER;
			} else if (length_units == 2) {		//---- meters ----
				length /= MILETOMETER;
			} else {							//---- feet ----
				length /= MILETOFEET;
			}
		}										//---- miles ----

		if (speed_units > 0) {
			if (speed_units == 1) {				//---- KPH ----
				speed *= 1000.0 / MILETOMETER;
			} else if (speed_units == 2) {		//---- MPS ----
				speed *= MPSTOMPH;
			} else {							//---- FPS ----
				speed *= 3600.0 / MILETOFEET;
			}
		}										//---- MPH ----

		if (period_units > 0) {
			if (period_units == 1) {			//---- day ----
				time *= 24.0;
			} else if (period_units = 2) {		//---- minutes ----
				time /= 60.0;
			} else {							//---- seconds ----
				time /= 3600.0;
			}
		}										//---- hours ----
		period = MAX ((int) (time * 3600.0 - 1.0), 0);
		period = time_periods.Period (period);

		if (period == 0) continue;

		if (speed <= 1.0) speed = 1.0;
		if (areatype == 0) areatype = 1;

		//---- get the road type ----

		road = facility;

		if (road_map_flag) {
			road = road_type_map.Map (facility, areatype);

			if (road == 0) {
				road = facility * 10 + areatype;
				if (!rt_flag [road]) {
					Warning ("Road Type %d-%d is Not in the Type Map", facility, areatype);
					rt_flag [road] = true;
				}
				continue;
			}
		}		
		ramp_flag = (facility == ramp_facility);

		if (link_flag) {
			vsp_ptr = vol_spd_data.New_Record (true);
			if (vsp_ptr == NULL) goto mem_error;

			if (!vsp_ptr->Num_Volume (vol_spd_data.Num_Volume ())) goto mem_error;

			vsp_ptr->Link (link);
			vsp_ptr->Period (period);
			vsp_ptr->Fac_Type (facility);
			vsp_ptr->Area_Type (areatype);
			vsp_ptr->Road_Type (road);
			vsp_ptr->Length (length);
			vsp_ptr->Speed (speed);
		}

		if (speed < 2.5) {
			bin1 = bin2 = 1;
			spd1 = 1.0;
			spd2 = 2.5;
		} else {
			bin1 = (int) (speed / 5.0) + 1;
			bin2 = bin1 + 1;

			if (bin1 == 1) {
				spd1 = 2.5;
			} else {
				spd1 = (bin1 - 1) * 5.0;
			}
			spd2 = (bin2 - 1) * 5.0;
		}
		if (bin1 > max_bin) bin1 = max_bin;
		if (bin2 > max_bin) bin2 = max_bin;

		if (interpolate_flag) {
			factor = (speed - spd1) / (spd2 - spd1);

			spd1 = 1.0 / spd1;
			spd2 = 1.0 / spd2;
			speed = 1.0 / speed;
		} else {
			spd1 = 1.0 / spd1;
			spd2 = 1.0 / spd2;
			speed = 1.0 / speed;

			factor = (speed - spd1) / (spd2 - spd1);
		}

		//---- calculate rate index ----

		if (rate_flag) {
			base_id = (reg_ptr == NULL) ? 0 : reg_ptr->Base_Index ();

			sum_rec.Road_Type (road);

			if (rt_ptr != NULL && rt_ptr->Key_Flag ()) {
				member = rt_ptr->Member (road);

				if (member == 0) {
					road = facility * 10 + areatype;
					if (!rt_flag [road]) {
						Warning ("Road Type %d-%d does not have Emission Rates", facility, areatype);
						rt_flag [road] = true;
					}
					continue;
				}
				base_id += member * rt_ptr->Base_Index ();
			}

			//---- get the area type ----

			id = type = areatype;

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

			id = type = facility;

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
		}

		//---- process each vehicle volume ----

		for (i=1; i <= num_vol_fld; i++) {
			if (volume_field [i] == 0) continue;

			vol_spd_file.Get_Field (volume_field [i], &volume);

			if (link_flag) {
				vsp_ptr->Volume (i, volume * volume_factor);
			}
			if (volume == 0.0) continue;

			memset (vmt_bin, '\0', sizeof (vmt_bin));
			memset (vht_bin, '\0', sizeof (vht_bin));

			vol1 = volume * volume_factor;
			vol2 = vol1 * factor;
			vol1 -= vol2;

			vmt_bin [bin1] = vol1 * length * MILETOMETER;
			vht_bin [bin1] = vol1 * 3600.0 * length * spd1;

			vmt_bin [bin2] += vol2 * length * MILETOMETER;
			vht_bin [bin2] += vol2 * 3600.0 * length * spd2;

			vehicle = i;

			//---- calculate emissions ----

			if (rate_flag) {

				//---- check the vehicle type ----

				id = type = vehicle;
				subtype = 0;

				if (veh_map_flag) {
					id = vehicle_map.Map (type, subtype);

					if (id == 0) {
						if (!v_flag [type]) {
							Warning ("Vehicle Type %d is Not in the Type Map", type);
							v_flag [type] = true;
						}
						continue;
					}
				}
				sum_rec.Vehicle_Type (id);

				veh_base = base_id;

				if (veh_ptr != NULL && veh_ptr->Key_Flag ()) {
					member = veh_ptr->Member (id);

					if (member == 0) {
						if (!v_flag [type]) {
							Warning ("Vehicle Type %d does not have Emission Rates", type);
							v_flag [type] = true;
						}
						continue;
					}
					veh_base += member * veh_ptr->Base_Index ();
				}

				//---- get the summary record ----

				sum_ptr = summary_data.Get (&sum_rec);

				if (sum_ptr == NULL) {
					sum_ptr = new Summary_Data (num_pollutants);
					sum_ptr->Zero (&sum_rec);

					if (!summary_data.Add (sum_ptr)) goto mem_error;

					sum_ptr = summary_data.Record ();
				}
			}

			//---- speed bin loop ----

			for (bin=bin1; bin <= bin2; bin++) {

				vht = vht_bin [bin];
				vmt = vmt_bin [bin];

				if (vht == 0.0) continue;

				//---- process the emissions rates ----

				if (rate_flag) {
					bin_id = veh_base;

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
								member = year_ptr->Member (type);
								if (member == 0) continue;

								year_id += member * year_ptr->Base_Index ();
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
									member = mon_ptr->Member (type);
									if (member == 0) continue;

									id += member * mon_ptr->Base_Index ();
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

					//---- road type loop ----

					for (road_id = 0; ; road_id++) {
						if (road_id == 0) {
							road_map = rt_ptr->First_Member (road, 1);
						} else {
							road_map = rt_ptr->Next_Member (road, 1);
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
								per_map = per_ptr->First_Member (period, 1);
							} else {
								per_map = per_ptr->Next_Member (period, 1);
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

							//---- vehicle loop ----

							for (veh_id = 0; ; veh_id++) {
								if (veh_id == 0) {
									veh_map = veh_ptr->First_Member (vehicle, 1);
								} else {
									veh_map = veh_ptr->Next_Member (vehicle, 1);
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

								weight = per_wt * road_wt * veh_wt;

								//---- get the bin record ----

								bsum_ptr = bin_sum_data.Get (&bsum_rec);

								if (bsum_ptr == NULL) {
									bsum_ptr = bin_sum_data.New_Record (true);

									bsum_ptr->Zero (&bsum_rec);

									if (!bin_sum_data.Add ()) goto mem_error;
								}
								bsum_ptr->Add_Meters (vmt * weight);
								bsum_ptr->Add_Seconds (vht * weight);

								if (ramp_flag) {
									ramp_vmt [road_id] += vmt * weight;
								}
							}
						}
					}

					//---- summary the hpms vmt ----

					if (hpms_flag) {
						hpms = hpms_type_map.Map (vehicle, 0);

						if (hpms == 0) {
							Warning ("HPMS Type %d is Not in the Type Map", vehicle);
						}
						for (veh_id = 0; ; veh_id++) {
							if (veh_id == 0) {
								veh_map = hpms_ptr->First_Member (hpms, 1);
							} else {
								veh_map = hpms_ptr->Next_Member (hpms, 1);
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

		//---- save the vol_spd_data ----

		if (link_flag) {
			if (!vol_spd_data.Add ()) goto mem_error;
		}
	}
	End_Progress ();
	return;

mem_error:
	Error ("Insufficient Memory for Volume-Speed Data");
}
