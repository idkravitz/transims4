//*********************************************************
//	Read_Rates.cpp - read emission rate files
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Read_Rates
//---------------------------------------------------------

void Emissions::Read_Rates (void)
{
	int total_records, save_records, id, base, group, units, *unit_ptr;
	double rate, weight;

	Emission_Rate *rate_ptr, rate_rec;

	Category_Data *cat_ptr;	
	Category_Data *year_ptr, *mon_ptr, *per_ptr, *reg_ptr, *at_ptr, *fac_ptr, *rt_ptr, *veh_ptr, *pol_ptr, *bin_ptr;
	Value_Map *year_map, *mon_map, *per_map, *reg_map, *at_map, *fac_map, *rt_map, *veh_map, *pol_map, *bin_map;
	int year, month, period, region, areatype, facility, roadtype, vehicle, pollutant, speed_bin;
	int year_id, mon_id, per_id, reg_id, at_id, fac_id, rt_id, veh_id, pol_id, bin_id;
	double year_wt, mon_wt, per_wt, reg_wt, at_wt, fac_wt, rt_wt, veh_wt, pol_wt, bin_wt;

	Rate_Group *file_ptr;
	Db_Header *file;

	total_records = save_records = 0;

	//---- retrieve the category and data values ----

	year_ptr = category_data.Get (YEAR, true);
	mon_ptr = category_data.Get (MONTH, true);
	per_ptr = category_data.Get (PERIOD, true);
	reg_ptr = category_data.Get (REGION, true);
	at_ptr = category_data.Get (AREATYPE, true);
	fac_ptr = category_data.Get (FACILITY, true);
	rt_ptr = category_data.Get (ROADTYPE, true);
	veh_ptr = category_data.Get (VEHTYPE, true);
	pol_ptr = category_data.Get (POLLUTANT, true);
	bin_ptr = category_data.Get (SPEED_BIN, true);

	//---- set the base index for each category ----

	base = 1;

	for (cat_ptr = category_data.Last (); cat_ptr; cat_ptr = category_data.Previous ()) {
		if (cat_ptr->Key_Flag ()) {
			cat_ptr->Base_Index (base);
			base *= (cat_ptr->Num_Members () + 1);
		}
	}

	//---- initialize the polluntant units ----

	num_pollutants = pol_ptr->Num_Members ();

	if (num_pollutants > 0 && pollutant_units.Num_Records () < num_pollutants) {
		id = pollutant_units.Num_Records () + 1;
		pollutant_units.Num_Records (num_pollutants);

		units = 0;

		for (; id <= num_pollutants; id++) {
			pollutant_units.Set (id, units);
		}
	}

	//---- add the emission rates to the member categories ----

	for (file_ptr = rate_group.First (); file_ptr; file_ptr = rate_group.Next ()) {

		file = file_ptr->Rate_File ();

		units = file_ptr->Rate_Units ();

		Show_Message ("Reading %s -- Record", file->File_Type ());
		Set_Progress ();

		while (file->Read ()) {
			Show_Progress ();
			total_records++;

			//---- check the selection criteria ----

			year = file_ptr->Year ();
			month = file_ptr->Month ();

			if (year_flag && year > 0) {
				if (!years.Get_Index (year)) continue;
			}
			if (month_flag && month > 0) {
				if (!months.Get_Index (month)) continue;
			}

			//---- retrieve the data values ----

			period = file_ptr->Period ();
			region = file_ptr->Region ();
			areatype = file_ptr->Area_Type ();
			facility = file_ptr->Facility ();
			roadtype = file_ptr->Road_Type ();
			vehicle = file_ptr->Vehicle ();
			speed_bin = file_ptr->Speed_Bin ();
			pollutant = file_ptr->Pollutant ();

			if (pollutant == 0 && pol_ptr != NULL && pol_ptr->Key_Flag ()) {
				pollutant = 100 + file_ptr->Group ();
			}

			//---- pollutant ----

			pol_id = -1;
			pol_map = NULL;

			while (pol_id != 0) {
				if (pol_ptr != NULL) {
					if (pol_id == -1) {
						pol_map = pol_ptr->First_Value (pollutant);
					} else {
						pol_map = pol_ptr->Next_Value ();
					}
					if (pol_map == NULL || pol_map->Value () != pollutant) break;
					pol_id = pol_map->Member () * pol_ptr->Base_Index ();
					pol_wt = pol_map->Weight ();
					group = pol_map->Group ();
					if (group != 0 && group != file_ptr->Group ()) continue;

					unit_ptr = pollutant_units.Record (pol_map->Member ());

					if (unit_ptr != NULL && *unit_ptr == 0) {
						*unit_ptr = units;
					}
				} else {
					pol_id = 0;
					pol_wt = 1.0;
				}

				//---- year ----

				year_id = -1;
				year_map = NULL;

				while (year_id != 0) {
					if (year_ptr != NULL) {
						if (year_id == -1) {
							year_map = year_ptr->First_Value (year);
						} else {
							year_map = year_ptr->Next_Value ();
						}
						if (year_map == NULL || year_map->Value () != year) break;
						year_id = year_map->Member () * year_ptr->Base_Index ();
						year_wt = year_map->Weight ();
						group = year_map->Group ();
						if (group != 0 && group != file_ptr->Group ()) continue;
					} else {
						year_id = 0;
						year_wt = 1.0;
					}

					//---- month ----

					mon_id = -1;
					mon_map = NULL;

					while (mon_id != 0) {
						if (mon_ptr != NULL) {
							if (mon_id == -1) {
								mon_map = mon_ptr->First_Value (month);
							} else {
								mon_map = mon_ptr->Next_Value ();
							}
							if (mon_map == NULL || mon_map->Value () != month) break;
							mon_id = mon_map->Member () * mon_ptr->Base_Index ();
							mon_wt = mon_map->Weight ();
							group = mon_map->Group ();
							if (group != 0 && group != file_ptr->Group ()) continue;
						} else {
							mon_id = 0;
							mon_wt = 1.0;
						}

						//---- period ----

						per_id = -1;
						per_map = NULL;

						while (per_id != 0) {
							if (per_ptr != NULL) {
								if (per_id == -1) {
									per_map = per_ptr->First_Value (period);
								} else {
									per_map = per_ptr->Next_Value ();
								}
								if (per_map == NULL || per_map->Value () != period) break;
								per_id = per_map->Member () * per_ptr->Base_Index ();
								per_wt = per_map->Weight ();
								group = per_map->Group ();
								if (group != 0 && group != file_ptr->Group ()) continue;
							} else {
								per_id = 0;
								per_wt = 1.0;
							}

							//---- region ----

							reg_id = -1;
							reg_map = NULL;

							while (reg_id != 0) {
								if (reg_ptr != NULL) {
									if (reg_id == -1) {
										reg_map = reg_ptr->First_Value (region);
									} else {
										reg_map = reg_ptr->Next_Value ();
									}
									if (reg_map == NULL || reg_map->Value () != region) break;
									reg_id = reg_map->Member () * reg_ptr->Base_Index ();
									reg_wt = reg_map->Weight ();
									group = reg_map->Group ();
									if (group != 0 && group != file_ptr->Group ()) continue;
								} else {
									reg_id = 0;
									reg_wt = 1.0;
								}

								//---- area type ----

								at_id = -1;
								at_map = NULL;

								while (at_id != 0) {
									if (at_ptr != NULL) {
										if (at_id == -1) {
											at_map = at_ptr->First_Value (areatype);
										} else {
											at_map = reg_ptr->Next_Value ();
										}
										if (at_map == NULL || at_map->Value () != areatype) break;
										at_id = at_map->Member () * at_ptr->Base_Index ();
										at_wt = at_map->Weight ();
										group = at_map->Group ();
										if (group != 0 && group != file_ptr->Group ()) continue;
									} else {
										at_id = 0;
										at_wt = 1.0;
									}

									//---- facility ----

									fac_id = -1;
									fac_map = NULL;

									while (fac_id != 0) {
										if (fac_ptr != NULL) {
											if (fac_id == -1) {
												fac_map = fac_ptr->First_Value (facility);
											} else {
												fac_map = fac_ptr->Next_Value ();
											}
											if (fac_map == NULL || fac_map->Value () != facility) break;
											fac_id = fac_map->Member () * fac_ptr->Base_Index ();
											fac_wt = fac_map->Weight ();
											group = fac_map->Group ();
											if (group != 0 && group != file_ptr->Group ()) continue;
										} else {
											fac_id = 0;
											fac_wt = 1.0;
										}

										//---- road type ----

										rt_id = -1;
										rt_map = NULL;

										while (rt_id != 0) {
											if (rt_ptr != NULL) {
												if (rt_id == -1) {
													rt_map = rt_ptr->First_Value (roadtype);
												} else {
													rt_map = rt_ptr->Next_Value ();
												}
												if (rt_map == NULL || rt_map->Value () != roadtype) break;
												rt_id = rt_map->Member () * rt_ptr->Base_Index ();
												rt_wt = rt_map->Weight ();
												group = rt_map->Group ();
												if (group != 0 && group != file_ptr->Group ()) continue;
											} else {
												rt_id = 0;
												rt_wt = 1.0;
											}

											//---- vehicle ----

											veh_id = -1;
											veh_map = NULL;

											while (veh_id != 0) {
												if (veh_ptr != NULL) {
													if (veh_id == -1) {
														veh_map = veh_ptr->First_Value (vehicle);
													} else {
														veh_map = veh_ptr->Next_Value ();
													}
													if (veh_map == NULL || veh_map->Value () != vehicle) break;
													veh_id = veh_map->Member () * veh_ptr->Base_Index ();
													veh_wt = veh_map->Weight ();
													group = veh_map->Group ();
													if (group != 0 && group != file_ptr->Group ()) continue;
												} else {
													veh_id = 0;
													veh_wt = 1.0;
												}

												//---- speed bin ----

												bin_id = -1;
												bin_map = NULL;

												while (bin_id != 0) {
													if (bin_ptr != NULL) {
														if (bin_id == -1) {
															bin_map = bin_ptr->First_Value (speed_bin);
														} else {
															bin_map = bin_ptr->Next_Value ();
														}
														if (bin_map == NULL || bin_map->Value () != speed_bin) break;
														bin_id = bin_map->Member () * bin_ptr->Base_Index ();
														bin_wt = bin_map->Weight ();
														group = bin_map->Group ();
														if (group != 0 && group != file_ptr->Group ()) continue;
													} else {
														bin_id = 0;
														bin_wt = 1.0;
													}

													//---- calculate the emission index and composite weighting factor ----
													
													id = pol_id + year_id + mon_id + per_id + reg_id + at_id + fac_id + rt_id + veh_id + bin_id;
													weight = pol_wt * year_wt * mon_wt * per_wt * reg_wt * at_wt * fac_wt * rt_wt * veh_wt * bin_wt;

													if (weight == 0.0) continue;

													rate = file_ptr->Rate () * weight;

													//---- retrieve the emission rate ----

													rate_ptr = emission_rate.Get (id);
													save_records++;

													if (rate_ptr == NULL) {
														rate_rec.ID (id);
														rate_rec.Rate (rate);
														rate_rec.Weight (weight);
														if (!emission_rate.Add (&rate_rec)) {
															Error ("Adding Emission Rate");
														}
													} else {
														rate_ptr->Add_Rate (rate);
														rate_ptr->Add_Weight (weight);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		End_Progress ();
	}
	Print (2, "Number of Emission Rate Records Read = %d", total_records);
	Print (1, "Number of Emission Rate Records Used = %d", save_records);
	Print (1, "Number of Emission Rates Generated = %d", emission_rate.Num_Records ());

	//---- normalize the emission rates ----

	for (rate_ptr = emission_rate.First (); rate_ptr; rate_ptr = emission_rate.Next ()) {
		rate_ptr->Rate (rate_ptr->Rate () / rate_ptr->Weight ());
	}
}
