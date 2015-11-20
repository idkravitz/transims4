//*********************************************************
//	Write_Link_Veh.cpp - Write the MOVES Link Vehicle file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_Link_Veh
//---------------------------------------------------------

void Emissions::Write_Link_Veh (void)
{
	int i, j, k, lnk_field, veh_field, fac_field, link_dir, last_dir, num_rec;
	int vehicle, num, n0, num_out, veh_num [100], veh_id, num_veh;
	double vmt, veh_vmt [100], veh_wt;
	
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Speed_Bin_Data *bin_ptr;
	Category_Data *veh_ptr;
	Value_Map *veh_map;
	Traveler_Data *data_ptr;
	Vol_Spd_Data *vsp_ptr;

	veh_ptr = category_data.Get (VEHTYPE);

	//---- create the data fields ----

	link_veh_file.Add_Field ("linkID", Db_Code::INTEGER, 10);
	lnk_field = 1;
	
	link_veh_file.Add_Field ("sourceTypeID", Db_Code::INTEGER, 4);
	veh_field = 2;

	link_veh_file.Add_Field ("sourceTypeHourFraction", Db_Code::DOUBLE, 9, 2);
	fac_field = 3;

	link_veh_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", link_veh_file.File_Type ());
	Set_Progress ();

	num_rec = 0;
	memset (vmt_bin, '\0', sizeof (vmt_bin));

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
			memset (vmt_bin, '\0', sizeof (vmt_bin));

			for (data_ptr = traveler_data.First_Key (); data_ptr; data_ptr = traveler_data.Next_Key ()) {
				if (data_ptr->Link_Dir () != dir_ptr->Link_Dir ()) continue;

				if (data_ptr->Traveler () != last_dir) {
					num++;
					last_dir = data_ptr->Traveler ();
				}
				vmt_bin [data_ptr->Vehicle ()] += UnRound (data_ptr->Distance ());
				if (data_ptr->Vehicle () > num_out) num_out = data_ptr->Vehicle ();
			}
			if (num == 0) continue;

			link_veh_file.Put_Field (lnk_field, (dir_ptr->Link () * 10 + dir_ptr->Dir ()));

			for (i=1; i <= num_out; i++) {
				vehicle = i;
				vmt = vmt_bin [i];

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
					
					//---- scan existing vehicle IDs ----

					for (k=0; k < num_veh; k++) {
						if (veh_id == veh_num [k]) {
							veh_vmt [k] += vmt * veh_wt;
							break;
						}
					}
					if (k == num_veh) {
						veh_num [k] = veh_id;
						veh_vmt [k] = vmt * veh_wt;
						num_veh++;
					}
				}
			}

			//---- calculate the total link vmt ----

			vmt = 0.0;

			for (i=0; i < num_veh; i++) {
				vmt += veh_vmt [i];
			}
			if (vmt > 0.0) {

				//---- output the fraction for each vehicle type ----

				for (i=0; i < num_veh; i++) {
					link_veh_file.Put_Field (veh_field, veh_num [i]);	

					link_veh_file.Put_Field (fac_field, veh_vmt [i] / vmt);

					if (!link_veh_file.Write ()) {
						Error ("Writing %s", link_veh_file.File_Type ());
					}
					num_rec++;
				}
			}
		}

	} else if (vol_spd_flag) {

		//---- volume speed data method ----

		last_dir = num = 0;

		for (vsp_ptr = vol_spd_data.First_Key (); ; vsp_ptr = vol_spd_data.Next_Key ()) {
			if (vsp_ptr != NULL) {
				Show_Progress ();

				link_dir = vsp_ptr->Link ();
			}
			if (link_dir != last_dir || vsp_ptr == NULL) {
				if (last_dir > 0 && num > 0) {

					link_veh_file.Put_Field (lnk_field, last_dir);

					num_veh = 0;

					for (i=1; i <= vol_spd_data.Num_Volume (); i++) {
						vehicle = i;
						vmt = vmt_bin [i];

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
							
							//---- scan existing vehicle IDs ----

							for (k=0; k < num_veh; k++) {
								if (veh_id == veh_num [k]) {
									veh_vmt [k] += vmt * veh_wt;
									break;
								}
							}
							if (k == num_veh) {
								veh_num [k] = veh_id;
								veh_vmt [k] = vmt * veh_wt;
								num_veh++;
							}
						}
					}

					//---- calculate the total link vmt ----

					vmt = 0.0;

					for (i=0; i < num_veh; i++) {
						vmt += veh_vmt [i];
					}
					if (vmt > 0.0) {

						//---- output the fraction for each vehicle type ----

						for (i=0; i < num_veh; i++) {
							link_veh_file.Put_Field (veh_field, veh_num [i]);	

							link_veh_file.Put_Field (fac_field, veh_vmt [i] / vmt);

							if (!link_veh_file.Write ()) {
								Error ("Writing %s", link_veh_file.File_Type ());
							}
							num_rec++;
						}
					}
				}
				if (vsp_ptr == NULL) break;
				last_dir = link_dir;
				num = 0;

				memset (vmt_bin, '\0', sizeof (vmt_bin));
			}
			for (i=1; i <= vsp_ptr->Num_Volume (); i++) {
				vmt_bin [i] += vsp_ptr->Volume (i) * vsp_ptr->Length ();
			}
			num++;
		}

	} else {

		last_dir = 0;
		num = data_group.Num_Records ();

		for (bin_ptr = speed_bin_data.First_Key (); ; bin_ptr = speed_bin_data.Next_Key ()) {
			if (bin_ptr != NULL) {
				Show_Progress ();
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
							link_veh_file.Put_Field (lnk_field, (dir_ptr->Link () * 10 + dir_ptr->Dir ()));

							num_veh = 0;

							for (i=0; i < num; i++) {
								n0 = i * num_bins;

								if (i > 0) {
									for (j=0; j < num_bins; j++) {
										spd_bin [j] = spd_bin [j + n0];
									}
								}
								num_out = Speed_Bin_Distribution ();

								vmt = 0.0;

								for (j=1; j <= num_out; j++) {
									vmt += vmt_bin [i];
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
										veh_wt = 1.0;
									} else {
										veh_id = veh_map->Value ();
										veh_wt = veh_map->Weight ();
									}
									
									//---- scan existing vehicle IDs ----

									for (k=0; k < num_veh; k++) {
										if (veh_id == veh_num [k]) {
											veh_vmt [k] += vmt * veh_wt;
											break;
										}
									}
									if (k == num_veh) {
										veh_num [k] = veh_id;
										veh_vmt [k] = vmt * veh_wt;
										num_veh++;
									}
								}
							}

							//---- calculate the total link vmt ----

							vmt = 0.0;

							for (i=0; i < num_veh; i++) {
								vmt += veh_vmt [i];
							}
							if (vmt > 0.0) {

								//---- output the fraction for each vehicle type ----

								for (i=0; i < num_veh; i++) {
									link_veh_file.Put_Field (veh_field, veh_num [i]);	

									link_veh_file.Put_Field (fac_field, veh_vmt [i] / vmt);

									if (!link_veh_file.Write ()) {
										Error ("Writing %s", link_veh_file.File_Type ());
									}
									num_rec++;
								}
							}
						}
					}
				}
				last_dir = link_dir;

				memset (spd_bin, '\0', sizeof (spd_bin));
			}
			if (bin_ptr == NULL) break;

			//---- sum the speed bin distribution ----

			for (i=0; i < num; i++) {
				n0 = i * num_bins;

				for (j=0; j <= num_bins; j++) {
					spd_bin [j + n0] += bin_ptr->bin [j + n0] * volume_factor;
				}
			}
		}
	}
	End_Progress ();

	link_veh_file.Close ();

	Print (2, "Number of %s Records = %d", link_veh_file.File_Type (), num_rec);
}
