//*********************************************************
//	Write_Link_Sum.cpp - Write the MOVES Link Summary file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_Link_Sum
//---------------------------------------------------------

void Emissions::Write_Link_Sum (void)
{
	int i, j, lnk_field, cty_field, reg_field, rt_field, len_field, vol_field, spd_field, des_field;
	int link_dir, last_dir, road, num_rec, region, reg_id, num, n0, num_out, volume;
	double vmt, vht, vol, speed, length;
	char buffer [FIELD_BUFFER];
	
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Speed_Bin_Data *bin_ptr, *box_ptr, box_rec;
	Category_Data *reg_ptr;
	Value_Map *reg_map;
	Traveler_Data *data_ptr;
	Vol_Spd_Data *vsp_ptr;

	reg_ptr = category_data.Get (REGION);

	//---- create the data fields ----

	link_sum_file.Add_Field ("linkID", Db_Code::INTEGER, 10);
	lnk_field = 1;
	
	link_sum_file.Add_Field ("countyID", Db_Code::INTEGER, 10);
	cty_field = 2;
	
	link_sum_file.Add_Field ("zoneID", Db_Code::INTEGER, 10);
	reg_field = 3;

	link_sum_file.Add_Field ("roadTypeID", Db_Code::INTEGER, 3);
	rt_field = 4;
	
	link_sum_file.Add_Field ("linkLength", Db_Code::DOUBLE, 9, 2);
	len_field = 5;
	
	link_sum_file.Add_Field ("linkVolume", Db_Code::INTEGER, 10);
	vol_field = 6;

	link_sum_file.Add_Field ("linkAvgSpeed", Db_Code::DOUBLE, 9, 2);
	spd_field = 7;

	link_sum_file.Add_Field ("linkDescription", Db_Code::STRING, 16);
	des_field = 8;

	link_sum_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", link_sum_file.File_Type ());
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

			vmt = vht = 0;
			num = last_dir = 0;

			for (data_ptr = traveler_data.First_Key (); data_ptr; data_ptr = traveler_data.Next_Key ()) {
				if (data_ptr->Link_Dir () != dir_ptr->Link_Dir ()) continue;

				if (data_ptr->Traveler () != last_dir) {
					num++;
					last_dir = data_ptr->Traveler ();
				}
				vht += 1.0;
				vmt += UnRound (data_ptr->Distance ());
			}
			if (num == 0) continue;

			link_sum_file.Put_Field (lnk_field, (dir_ptr->Link () * 10 + dir_ptr->Dir ()));

			region = 1;

			if (reg_ptr != NULL) {
				reg_map = reg_ptr->First_Member (region, 1);

				if (reg_map == NULL) {
					reg_id = region;
				} else {
					reg_id = reg_map->Value ();
				}
				link_sum_file.Put_Field (reg_field, reg_id);
				link_sum_file.Put_Field (cty_field, reg_id);
			}

			road = link_ptr->Type ();

			if (road_map_flag) {
				road = road_type_map.Map (road, 1);
			}
			link_sum_file.Put_Field (rt_field, road);
		
			link_sum_file.Put_Field (len_field, UnRound (link_ptr->Length ()) / MILETOMETER);

			volume = (int) (vmt / UnRound (link_ptr->Length ()) + 0.5);

			link_sum_file.Put_Field (vol_field, volume);

			if (vht == 0.0) vht = 1.0;
			vmt = MPSTOMPH * vmt / vht;

			link_sum_file.Put_Field (spd_field, vmt);

			str_fmt (buffer, sizeof (buffer), "Travelers=%d", num);
			link_sum_file.Put_Field (des_field, buffer);

			if (!link_sum_file.Write ()) {
				Error ("Writing %s", link_sum_file.File_Type ());
			}
			num_rec++;
		}

	} else if (vol_spd_flag) {

		//---- volume speed data method ----

		last_dir = num = road = 0;

		for (vsp_ptr = vol_spd_data.First_Key (); ; vsp_ptr = vol_spd_data.Next_Key ()) {
			if (vsp_ptr != NULL) {
				Show_Progress ();

				link_dir = vsp_ptr->Link ();
			}
			if (link_dir != last_dir || vsp_ptr == NULL) {
				if (last_dir > 0 && num > 0) {
					link_sum_file.Put_Field (lnk_field, last_dir);

					if (vht > 0) {
						speed = vmt / vht;
					} else {
						speed = 0.0;
					}
					if (volume > 0) {
						length = MAX (vmt / volume, length);
					}
					link_sum_file.Put_Field (spd_field, speed);
					link_sum_file.Put_Field (len_field, length);
					link_sum_file.Put_Field (vol_field, volume);

					region = 1;

					if (reg_ptr != NULL) {
						reg_map = reg_ptr->First_Member (region, 1);

						if (reg_map == NULL) {
							reg_id = region;
						} else {
							reg_id = reg_map->Value ();
						}
						link_sum_file.Put_Field (reg_field, reg_id);
						link_sum_file.Put_Field (cty_field, reg_id);
					}
					link_sum_file.Put_Field (rt_field, road);

					link_sum_file.Put_Field (des_field, "VolSpd");

					if (!link_sum_file.Write ()) {
						Error ("Writing %s", link_sum_file.File_Type ());
					}
					num_rec++;
				}
				if (vsp_ptr == NULL) break;
				last_dir = link_dir;
				vmt = vht = 0;
				volume = num = 0;
				road = vsp_ptr->Road_Type ();
			}
			vol = 0;
			for (i=1; i <= vsp_ptr->Num_Volume (); i++) {
				vol += vsp_ptr->Volume (i);
			}
			volume += (int) (vol + 0.5);

			speed = vsp_ptr->Speed ();
			length = vsp_ptr->Length ();

			vmt += length * vol;
			vht += vol * length / speed;
			num++;
		}

	} else {

		last_dir = 0;
		num = data_group.Num_Records ();

		memset (spd_bin, '\0', sizeof (spd_bin));

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

					box_rec.link_dir = last_dir;
					box_rec.offset = 0;

					box_ptr = box_location.Get_GE (&box_rec);

					if (dir_ptr != NULL && box_ptr != NULL) {
						link_ptr = link_data.Get (dir_ptr->Link ());

						if (link_ptr != NULL) {
							link_sum_file.Put_Field (lnk_field, (dir_ptr->Link () * 10 + dir_ptr->Dir ()));

							region = box_ptr->bin [0] >> 8;

							if (reg_ptr != NULL) {
								reg_map = reg_ptr->First_Member (region, 1);

								if (reg_map == NULL) {
									reg_id = region;
								} else {
									reg_id = reg_map->Value ();
								}
								link_sum_file.Put_Field (reg_field, reg_id);
								link_sum_file.Put_Field (cty_field, reg_id);
							}

							road = link_ptr->Type ();

							if (road_map_flag) {
								road = road_type_map.Map (road, (box_ptr->bin [0] & 0xFF));
							}
							link_sum_file.Put_Field (rt_field, road);
						
							link_sum_file.Put_Field (len_field, UnRound (link_ptr->Length ()) / MILETOMETER);

							num_out = Speed_Bin_Distribution ();

							vmt = vht = 0.0;

							for (i=1; i <= num_out; i++) {
								vht += vht_bin [i];
								vmt += vmt_bin [i];
							}
							volume = (int) (vmt / UnRound (link_ptr->Length ()) + 0.5);

							link_sum_file.Put_Field (vol_field, volume);

							if (vht == 0.0) vht = 1.0;
							vmt = MPSTOMPH * vmt / vht;

							link_sum_file.Put_Field (spd_field, vmt);

							str_fmt (buffer, sizeof (buffer), "Link=%d", link_ptr->Link ());
							link_sum_file.Put_Field (des_field, buffer);

							if (!link_sum_file.Write ()) {
								Error ("Writing %s", link_sum_file.File_Type ());
							}
							num_rec++;
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
					spd_bin [j] += bin_ptr->bin [j + n0] * volume_factor;
				}
			}
		}
	}
	End_Progress ();

	link_sum_file.Close ();

	Print (2, "Number of %s Records = %d", link_sum_file.File_Type (), num_rec);
}
