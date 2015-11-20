//*********************************************************
//	Traveler_Emissions.cpp - summarize emissions for travelers
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Traveler_Emissions
//---------------------------------------------------------

void Emissions::Traveler_Emissions (void)
{
	int j, count, unit, link, period, road, vehicle, veh_base, bin_id, ems_id, year_id;
	int bin, bin1, bin2, facility, areatype, year, month, base_id, member, id, type, vehtype;
	double length, speed, spd1, spd2, factor;
	double vol1, vol2, vmt, vht, units, emissions;
	bool v_flag [200], at_flag [200], ft_flag [200], rt_flag [200], p_flag [200], sb_flag [200], u_flag [200];
	bool keep, base_flag;

	Category_Data *year_ptr, *mon_ptr, *per_ptr, *reg_ptr, *at_ptr, *fac_ptr, *rt_ptr, *veh_ptr, *pol_ptr, *bin_ptr, *hpms_ptr;
	Summary_Data *sum_ptr, sum_rec;
	Emission_Rate *rate_ptr;
	Traveler_Data *data_ptr;
	Link_Data *link_ptr;

	//---- initialize the data summary codes ----

	Show_Message ("Calculating Traveler Emissions -- Record");
	Set_Progress (10000);

	summary_data.Pollutants (num_pollutants);

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

	//---- process each traveler records ----

	for (data_ptr = traveler_data.First_Key (); data_ptr; data_ptr = traveler_data.Next_Key ()) {
		Show_Progress ();

		link = data_ptr->Link ();
		vehtype = data_ptr->Vehicle ();
		
		link_ptr = link_data.Get (link);

		if (link_ptr == NULL || link_ptr->Use () == 0) continue;

		facility = link_ptr->Type ();
		areatype = link_ptr->Aoffset ();

		period = time_periods.Period (data_ptr->Time ());		//---- time period ----	

		speed = UnRound (data_ptr->Speed ());
		if (speed == 0.0) speed = 0.001;
		speed = speed * MPSTOMPH;		//---- mph ----

		length = UnRound (data_ptr->Distance ());
		if (length == 0.0) length = 0.001;
		length = length / MILETOMETER;							//---- miles ----

		if (period == 0) continue;
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

		//---- process each vehicle volume ----

		memset (vmt_bin, '\0', sizeof (vmt_bin));
		memset (vht_bin, '\0', sizeof (vht_bin));

		vol1 = 1;
		vol2 = vol1 * factor;
		vol1 -= vol2;

		vmt_bin [bin1] = vol1 * length * MILETOMETER;
		vht_bin [bin1] = vol1;

		vmt_bin [bin2] += vol2 * length * MILETOMETER;
		vht_bin [bin2] += vol2;

		vehicle = vehtype;

		//---- calculate emissions ----

		id = vehtype;
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

		//---- speed bin loop ----

		for (bin=bin1; bin <= bin2; bin++) {

			vht = vht_bin [bin];
			vmt = vmt_bin [bin];

			if (vht == 0.0) continue;
			if (vmt <= 0.0) vmt = 1.0;

			//---- process the emissions rates ----

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
	}
	End_Progress ();
	return;

mem_error:
	Error ("Insufficient Memory for Emissions Data");
}
