//*********************************************************
//	Write_Link_Ops.cpp - Write the MOVES Link Operations file
//*********************************************************

#include "Emissions.hpp"

#include <math.h>

#define NUM_OP_MODES	25

//---------------------------------------------------------
//	Write_Link_Ops
//---------------------------------------------------------

void Emissions::Write_Link_Ops (void)
{
	int i, j, k, lnk_field, veh_field, tim_field, pol_field, op_field, fac_field;
	int link_dir, last_dir, num_rec, time, type;
	int vehicle, num, n0, num_out, veh_id, num_veh, op_mode;
	double vmt, vht, veh_vmt [2000], veh_wt, vsp, speed, length, last_spd, mph, ttime;

	Category_Data *pol_ptr, *veh_ptr;
	Value_Map *pol_map, *veh_map;
	Traveler_Data *data_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Speed_Bin_Data *bin_ptr;
	Range_Data *range_ptr;
	Vol_Spd_Data *vsp_ptr;

	//---- MOVES operating mode codes by VSP bin and speed ----

	typedef struct {
		int op_mode;
		int vsp_low;
		int vsp_high;
		int mph_low;
		int mph_high;
	} Op_Codes;

	Op_Codes op_codes [] = {
		{ 1, 0, 1, 0, 1 },
		{ 11, -1000, 0, 1, 25 },
		{ 12, 0, 3, 1, 25 },
		{ 13, 3, 6, 1, 25 },
		{ 14, 6, 9, 1, 25 },
		{ 15, 9, 12, 1, 25 },
		{ 16, 12, 1000, 1, 25 },
		{ 21, -1000, 0, 25, 50 },
		{ 22, 0, 3, 25, 50 },
		{ 23, 3, 6, 25, 50 },
		{ 24, 6, 9, 25, 50 },
		{ 25, 9, 12, 25, 50 },
		{ 27, 12, 18, 25, 50 },
		{ 28, 18, 24, 25, 50 },
		{ 29, 24, 30, 25, 50 },
		{ 30, 30, 1000, 25, 50 },
		{ 33, -1000, 6, 50, 100 },
		{ 35, 6, 12, 50, 100 },
		{ 37, 12, 18, 50, 100 },
		{ 38, 18, 24, 50, 100 },
		{ 39, 24, 30, 50, 100 },
		{ 40, 30, 100, 50, 100 },
		{ -1, 0, 0, 0, 0 }
	};
	veh_ptr = category_data.Get (VEHTYPE);
	pol_ptr = category_data.Get (POLLUTANT, true);

	range_ptr = time_periods.First ();
	time = 10 * ((range_ptr->Low () / 3600) + 1) + 5;

	//---- create the data fields ----

	link_ops_file.Add_Field ("sourceTypeID", Db_Code::INTEGER, 4);
	veh_field = 1;

	link_ops_file.Add_Field ("hourDayID", Db_Code::INTEGER, 4);
	tim_field = 2;

	link_ops_file.Add_Field ("linkID", Db_Code::INTEGER, 10);
	lnk_field = 3;

	link_ops_file.Add_Field ("polProcessID", Db_Code::INTEGER, 8);
	pol_field = 4;

	link_ops_file.Add_Field ("opModeID", Db_Code::INTEGER, 8);
	op_field = 5;
	
	link_ops_file.Add_Field ("opModeFraction", Db_Code::DOUBLE, 9, 6);
	fac_field = 6;

	link_ops_file.Write_Header ();

	link_ops_file.Put_Field (tim_field, time);

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", link_ops_file.File_Type ());
	Set_Progress ();

	num_rec = 0;

	//---- check the data source ----

	if (traveler_flag) {
		
		//---- traveler data method ----

		for (j=0; ; j++) {
			if (link_eq_flag) {
				if (j == 0) {
					link_dir = link_list.First_Key (); 
				} else {
					link_dir = link_list.Next_Key ();
				}
				if (link_dir == 0) break;

				dir_ptr = dir_data [link_dir];
				if (dir_ptr == NULL) continue;
			} else {
				if (j == 0) {
					dir_ptr = dir_data.First ();
				} else {
					dir_ptr = dir_data.Next ();
				}
				if (dir_ptr == NULL) break;
			}
			Show_Progress ();

			link_ptr = link_data.Get (dir_ptr->Link ());
			if (link_ptr == NULL) continue;

			num = last_dir = num_veh = num_out = 0;
			memset (veh_vmt, '\0', sizeof (veh_vmt));

			for (data_ptr = traveler_data.First_Key (); data_ptr; data_ptr = traveler_data.Next_Key ()) {
				if (data_ptr->Link_Dir () != dir_ptr->Link_Dir ()) continue;

				speed = UnRound (data_ptr->Speed ());

				if (data_ptr->Traveler () != last_dir) {
					num++;
					last_dir = data_ptr->Traveler ();
					last_spd = speed;
				}
				type = data_ptr->Vehicle ();
				if (type > num_out) num_out = type;

				vsp = speed * (1.1 * (speed - last_spd) + 0.132) + 0.000302 * pow (speed, 3);
				last_spd = speed;

				speed *= MPSTOMPH;
				op_mode = 0;

				for (k=0; op_codes [k].op_mode >= 0; k++) {
					if (op_codes [k].mph_low <= speed && speed < op_codes [k].mph_high) {
						if (op_codes [k].vsp_low <= vsp && vsp < op_codes [k].vsp_high) {
							op_mode = k;
							break;
						}
					}
				}
				veh_vmt [op_mode + (type - 1) * NUM_OP_MODES] += 1.0;
			}
			if (num == 0) continue;

			link_ops_file.Put_Field (lnk_field, (dir_ptr->Link () * 10 + dir_ptr->Dir ()));

			for (i=1; i <= num_out; i++) {

				//----- calculate the operating mode fractions for each vehicle type ----

				n0 = (i - 1) * NUM_OP_MODES;
				vmt = 0.0;

				for (k=0; op_codes [k].op_mode >= 0; k++) {
					vmt += vmt_bin [k] = veh_vmt [k + n0];
				}
				if (vmt > 1.0) {
					for (k=0; op_codes [k].op_mode >= 0; k++) {
						vmt_bin [k] /= vmt;
					}
				}

				//---- write the distribution for each source type and pollutant ----

				vehicle = i;
				for (veh_id = 0; ; veh_id++) {
					if (veh_id == 0) {
						veh_map = veh_ptr->First_Member (vehicle, 1);
					} else {
						veh_map = veh_ptr->Next_Member (vehicle, 1);
						if (veh_map == NULL) break;
					}
					if (veh_map == NULL) {
						veh_id = vehicle;
					} else {
						veh_id = veh_map->Value ();
					}
					link_ops_file.Put_Field (veh_field, veh_id);

					//---- pollutant ----

					for (pol_map = pol_ptr->First_Value (1); pol_map; pol_map = pol_ptr->Next_Value ()) {
						link_ops_file.Put_Field (pol_field, pol_map->Member () * 100 + 1);

						//---- output the fraction for each vehicle type ----

						for (k=0; op_codes [k].op_mode >= 0; k++) {

							link_ops_file.Put_Field (op_field, op_codes [k].op_mode);

							link_ops_file.Put_Field (fac_field, vmt_bin [k]);

							if (!link_ops_file.Write ()) {
								Error ("Writing %s", link_ops_file.File_Type ());
							}
							num_rec++;
						}
					}
				}
			}
		}

	} else if (vol_spd_flag) {

		//---- volume speed data method ----

		num = last_dir = num_veh = num_out = 0;
		memset (veh_vmt, '\0', sizeof (veh_vmt));

		for (vsp_ptr = vol_spd_data.First_Key (); ; vsp_ptr = vol_spd_data.Next_Key ()) {
			if (vsp_ptr != NULL) {
				Show_Progress ();

				link_dir = vsp_ptr->Link ();
			}
			if (link_dir != last_dir || vsp_ptr == NULL) {
				if (last_dir > 0 && num > 0) {

					link_ops_file.Put_Field (lnk_field, last_dir);

					//---- vehicle types ----

					for (i=0; i < vol_spd_data.Num_Volume (); i++) {

						//----- calculate the operating mode fractions for each vehicle type ----

						n0 = i * NUM_OP_MODES;
						vmt = 0.0;

						for (k=0; op_codes [k].op_mode >= 0; k++) {
							vmt += vmt_bin [k] = veh_vmt [k + n0];
						}
						if (vmt > 1.0) {
							for (k=0; op_codes [k].op_mode >= 0; k++) {
								vmt_bin [k] /= vmt;
							}
						}
						vehicle = i + 1;

						for (veh_id = 0; ; veh_id++) {
							if (veh_id == 0) {
								veh_map = veh_ptr->First_Member (vehicle, 1);
							} else {
								veh_map = veh_ptr->Next_Member (vehicle, 1);
								if (veh_map == NULL) break;
							}
							if (veh_map == NULL) {
								veh_id = vehicle;
							} else {
								veh_id = veh_map->Value ();
							}
							link_ops_file.Put_Field (veh_field, veh_id);

							//---- pollutant ----

							for (pol_map = pol_ptr->First_Value (1); pol_map; pol_map = pol_ptr->Next_Value ()) {
								link_ops_file.Put_Field (pol_field, pol_map->Member () * 100 + 1);

								//---- output the fraction for each vehicle type ----

								for (k=0; op_codes [k].op_mode >= 0; k++) {
									link_ops_file.Put_Field (op_field, op_codes [k].op_mode);

									link_ops_file.Put_Field (fac_field, vmt_bin [k]);

									if (!link_ops_file.Write ()) {
										Error ("Writing %s", link_ops_file.File_Type ());
									}
									num_rec++;
								}
							}
						}
					}
				}
				if (vsp_ptr == NULL) break;
				num = 0;
				last_dir = link_dir;
				memset (veh_vmt, '\0', sizeof (veh_vmt));
			}

			//---- identify the operating mode ----

			speed = vsp_ptr->Speed () / MPSTOMPH;

			vsp = speed * 0.132 + 0.000302 * pow (speed, 3);

			speed = vsp_ptr->Speed ();
			op_mode = 0;

			for (k=0; op_codes [k].op_mode >= 0; k++) {
				if (op_codes [k].mph_low <= speed && speed < op_codes [k].mph_high) {
					if (op_codes [k].vsp_low <= vsp && vsp < op_codes [k].vsp_high) {
						op_mode = k;
						break;
					}
				}
			}
			if (speed > 0) {
				veh_wt = vsp_ptr->Length () * 3600.0 / speed;
			} else {
				veh_wt = 1.0;
			}
			for (i=0; i < vsp_ptr->Num_Volume (); i++) {
				veh_vmt [op_mode + i * NUM_OP_MODES] += vsp_ptr->Volume (i+1) * veh_wt;
			}
			num++;
		}

	} else {

		last_dir = 0;
		last_spd = 0.0;

		num = data_group.Num_Records ();
		memset (veh_vmt, '\0', sizeof (veh_vmt));

		for (bin_ptr = speed_bin_data.First_Key (); ; bin_ptr = speed_bin_data.Next_Key ()) {
			Show_Progress ();

			if (bin_ptr != NULL) {
				link_dir = bin_ptr->link_dir;
			} else {
				link_dir = -1;
			}
			if (link_dir != last_dir) {
				if (last_dir > 0) {
					dir_ptr = dir_data [last_dir];

					if (dir_ptr != NULL) {
						link_ptr = link_data.Get (dir_ptr->Link ());

						if (link_ptr != NULL) {
							link_ops_file.Put_Field (lnk_field, (dir_ptr->Link () * 10 + dir_ptr->Dir ()));

							for (i=0; i < num; i++) {
								n0 = i * NUM_OP_MODES;
								vmt = 0.0;

								for (k=0; op_codes [k].op_mode >= 0; k++) {
									vmt += vmt_bin [k] = veh_vmt [k + n0];
								}
								if (vmt > 1.0) {
									for (k=0; op_codes [k].op_mode >= 0; k++) {
										vmt_bin [k] /= vmt;
									}
								}
								vehicle = i + 1;

								for (veh_id = 0; ; veh_id++) {
									if (veh_id == 0) {
										veh_map = veh_ptr->First_Member (vehicle, 1);
									} else {
										veh_map = veh_ptr->Next_Member (vehicle, 1);
										if (veh_map == NULL) break;
									}
									if (veh_map == NULL) {
										veh_id = vehicle;
									} else {
										veh_id = veh_map->Value ();
									}
									link_ops_file.Put_Field (veh_field, veh_id);

									//---- pollutant ----

									for (pol_map = pol_ptr->First_Value (1); pol_map; pol_map = pol_ptr->Next_Value ()) {
										link_ops_file.Put_Field (pol_field, pol_map->Member () * 100 + 1);

										//---- output the fraction for each vehicle type ----

										for (k=0; op_codes [k].op_mode >= 0; k++) {
											link_ops_file.Put_Field (op_field, op_codes [k].op_mode);

											link_ops_file.Put_Field (fac_field, vmt_bin [k]);

											if (!link_ops_file.Write ()) {
												Error ("Writing %s", link_ops_file.File_Type ());
											}
											num_rec++;
										}
									}
								}
							}
						}
					}
				}
				last_dir = link_dir;
				last_spd = 0.0;

				memset (veh_vmt, '\0', sizeof (veh_vmt));
			}
			if (bin_ptr == NULL) break;

			//---- sum the speed bin distribution ----

			length = UnRound (bin_ptr->offset);

			for (i=0; i < num; i++) {
				n0 = i * num_bins;

				for (j=0; j < num_bins; j++) {
					spd_bin [j] = bin_ptr->bin [j + n0] * volume_factor;
				}
				num_out = Speed_Bin_Distribution ();

				//---- calculate the average speed and travel time ----

				vmt = vht = 0.0;

				for (j=1; j <= num_out; j++) {
					vht += vht_bin [j];
					vmt += vmt_bin [j];
				}
				if (vht <= 0.0 || vmt <= 0.0 || length <= 0.0) continue;

				speed = vmt / vht;
				ttime = length / speed;
				veh_wt = vht / ttime;
				mph = speed * MPSTOMPH;

				//---- average speed method ----

				if (speed_flag) {
						
					//---- identify the operating mode ----

					if (last_spd > 0.0) {
						vsp = speed * (1.1 * (speed - last_spd) + 0.132) + 0.000302 * pow (speed, 3);
						op_mode = 0;

						for (k=0; op_codes [k].op_mode >= 0; k++) {
							if (op_codes [k].mph_low <= mph && mph < op_codes [k].mph_high) {
								if (op_codes [k].vsp_low <= vsp && vsp < op_codes [k].vsp_high) {
									op_mode = k;
									break;
								}
							}
						}
						veh_vmt [op_mode + i * NUM_OP_MODES] += 1.0 * veh_wt;
						ttime -= 1.0;
					}
					if (ttime > 0) {

						//---- identify the operating mode ----

						vsp = speed * 0.132 + 0.000302 * pow (speed, 3);
						op_mode = 0;

						for (k=0; op_codes [k].op_mode >= 0; k++) {
							if (op_codes [k].mph_low <= speed && speed < op_codes [k].mph_high) {
								if (op_codes [k].vsp_low <= vsp && vsp < op_codes [k].vsp_high) {
									op_mode = k;
									break;
								}
							}
						}
						veh_vmt [op_mode + i * NUM_OP_MODES] += ttime * veh_wt;
					}
					last_spd = speed;

				} else {

					//---- distribute the speed and travel time ----

					vht = 0.5 * ttime / vht;

					for (j=1; j <= num_out; j++) {
						if (vht_bin [j] > 0.0) {
							vmt_bin [j] = vmt_bin [j] / vht_bin [j];
							vht_bin [j] = vht_bin [j] * vht;
						}
					}

					//---- distribute time to increasing speed bins ----

					for (j=1; j <= num_out; j++) {
						if (vht_bin [j] <= 0.0) continue;

						speed = vmt_bin [j];
						ttime = vht_bin [j];
						mph = speed * MPSTOMPH;

						if (last_spd > 0.0) {
							vsp = speed * (1.1 * (speed - last_spd) + 0.132) + 0.000302 * pow (speed, 3);
							op_mode = 0;

							for (k=0; op_codes [k].op_mode >= 0; k++) {
								if (op_codes [k].mph_low <= mph && mph < op_codes [k].mph_high) {
									if (op_codes [k].vsp_low <= vsp && vsp < op_codes [k].vsp_high) {
										op_mode = k;
										break;
									}
								}
							}
							veh_vmt [op_mode + i * NUM_OP_MODES] += veh_wt;
							ttime -= 1.0;
						}
						if (ttime > 0) {

							//---- identify the operating mode ----

							vsp = speed * 0.132 + 0.000302 * pow (speed, 3);
							op_mode = 0;

							for (k=0; op_codes [k].op_mode >= 0; k++) {
								if (op_codes [k].mph_low <= speed && speed < op_codes [k].mph_high) {
									if (op_codes [k].vsp_low <= vsp && vsp < op_codes [k].vsp_high) {
										op_mode = k;
										break;
									}
								}
							}
							veh_vmt [op_mode + i * NUM_OP_MODES] += ttime * veh_wt;
						}
						last_spd = speed;
					}

					//---- distribute time to decreasing speed bins ----

					for (j=num_out; j > 0; j--) {
						if (vht_bin [j] <= 0.0) continue;

						speed = vmt_bin [j];
						ttime = vht_bin [j];
						mph = speed * MPSTOMPH;

						if (last_spd > 0.0) {
							vsp = speed * (1.1 * (speed - last_spd) + 0.132) + 0.000302 * pow (speed, 3);
							op_mode = 0;

							for (k=0; op_codes [k].op_mode >= 0; k++) {
								if (op_codes [k].mph_low <= mph && mph < op_codes [k].mph_high) {
									if (op_codes [k].vsp_low <= vsp && vsp < op_codes [k].vsp_high) {
										op_mode = k;
										break;
									}
								}
							}
							veh_vmt [op_mode + i * NUM_OP_MODES] += veh_wt;
							ttime -= 1.0;
						}
						if (ttime > 0) {

							//---- identify the operating mode ----

							vsp = speed * 0.132 + 0.000302 * pow (speed, 3);
							op_mode = 0;

							for (k=0; op_codes [k].op_mode >= 0; k++) {
								if (op_codes [k].mph_low <= speed && speed < op_codes [k].mph_high) {
									if (op_codes [k].vsp_low <= vsp && vsp < op_codes [k].vsp_high) {
										op_mode = k;
										break;
									}
								}
							}
							veh_vmt [op_mode + i * NUM_OP_MODES] += ttime * veh_wt;
						}
						last_spd = speed;
					}
				}
			}
		}
	}
	End_Progress ();

	link_ops_file.Close ();

	Print (2, "Number of %s Records = %d", link_ops_file.File_Type (), num_rec);
}
