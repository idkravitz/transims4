//*********************************************************
//	Write_Link_Sps.cpp - Write the MOVES Link Speed file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_Link_Spd
//---------------------------------------------------------

void Emissions::Write_Link_Spd (void)
{
	int i, j, lnk_field, sec_field, spd_field, gd_field;
	int link_dir, last_dir, num_rec, num, n0, num_out, time, second;
	int traveler, last_traveler;
	double vmt, vht, ttime, speed, length, last_offset, bucket;
	
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Speed_Bin_Data *bin_ptr;
	Traveler_Data *data_ptr;
	Vol_Spd_Data *vsp_ptr;

	//---- create the data fields ----

	link_spd_file.Add_Field ("linkID", Db_Code::INTEGER, 10);
	lnk_field = 1;
	
	link_spd_file.Add_Field ("secondID", Db_Code::INTEGER, 10);
	sec_field = 2;
	
	link_spd_file.Add_Field ("speed", Db_Code::DOUBLE, 6, 1);
	spd_field = 3;

	link_spd_file.Add_Field ("grade", Db_Code::DOUBLE, 5, 1);
	gd_field = 4;

	link_spd_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", link_spd_file.File_Type ());
	Set_Progress ();

	num_rec = last_dir = 0;
	num = data_group.Num_Records ();

	link_spd_file.Put_Field (gd_field, 0.0);

	//---- check the data source ----

	if (traveler_flag) {

		//---- traveler data method ----

		last_traveler = last_dir = 0;

		for (data_ptr = traveler_data.First_Key (); data_ptr; data_ptr = traveler_data.Next_Key ()) {
			Show_Progress ();

			link_ptr = link_data.Get (data_ptr->Link ());
			if (link_ptr == NULL) continue;

			if (data_ptr->Dir () == 0) {
				link_dir = link_ptr->AB_Dir ();
			} else {
				link_dir = link_ptr->BA_Dir ();
			}
			traveler = data_ptr->Traveler ();
			time = data_ptr->Time ();

			if (traveler != last_traveler || link_dir != last_dir) {
				last_traveler = traveler;
				last_dir = link_dir;
				second = 0;

				link_spd_file.Put_Field (lnk_field, (data_ptr->Link () * 10 + data_ptr->Dir ()));
			}
			speed = UnRound (data_ptr->Speed ()) * MPSTOMPH;

			link_spd_file.Put_Field (spd_field, speed);
			link_spd_file.Put_Field (sec_field, ++second);

			if (!link_spd_file.Write ()) {
				Error ("Writing %s", link_spd_file.File_Type ());
			}
			num_rec++;
		}

	} else if (vol_spd_flag) {

		//---- volume speed data method ----

		last_dir = 0;

		for (vsp_ptr = vol_spd_data.First_Key (); ; vsp_ptr = vol_spd_data.Next_Key ()) {
			if (vsp_ptr != NULL) {
				Show_Progress ();

				link_dir = vsp_ptr->Link ();
			}
			if (link_dir != last_dir || vsp_ptr == NULL) {
				if (last_dir > 0 && num > 0) {
					link_spd_file.Put_Field (lnk_field, last_dir);

					speed /= num;
					length /= num;

					link_spd_file.Put_Field (spd_field, speed);

					second = (int) (length * 3600.0 / speed + 0.5);

					for (i=1; i <= second; i++) {
						link_spd_file.Put_Field (sec_field, i);

						if (!link_spd_file.Write ()) {
							Error ("Writing %s", link_spd_file.File_Type ());
						}
						num_rec++;
					}
				}
				if (vsp_ptr == NULL) break;
				last_dir = link_dir;
				num = 0;
				speed = length = 0;
			}
			speed += vsp_ptr->Speed ();
			length += vsp_ptr->Length ();
			num++;
		}

	} else {

		//---- speed bin data method ----

		for (bin_ptr = speed_bin_data.First_Key (); bin_ptr ; bin_ptr = speed_bin_data.Next_Key ()) {
			Show_Progress ();

			link_dir = bin_ptr->link_dir;
			length = UnRound (bin_ptr->offset);

			if (link_dir != last_dir) {
				last_dir = link_dir;
				last_offset = length;
				second = 0;
			} else {
				length -= last_offset;
			}

			dir_ptr = dir_data [link_dir];

			if (dir_ptr != NULL) {
				link_ptr = link_data.Get (dir_ptr->Link ());

				if (link_ptr != NULL) {
					link_spd_file.Put_Field (lnk_field, (dir_ptr->Link () * 10 + dir_ptr->Dir ()));

					//---- distribute the speeds ----

					memset (spd_bin, '\0', sizeof (spd_bin));

					for (i=0; i < num; i++) {
						n0 = i * num_bins;

						for (j=0; j <= num_bins; j++) {
							spd_bin [j] += bin_ptr->bin [j + n0] * volume_factor;
						}
					}
					num_out = Speed_Bin_Distribution ();

					//---- calculate the average speed and travel time ----

					vmt = vht = 0.0;

					for (i=1; i <= num_out; i++) {
						vht += vht_bin [i];
						vmt += vmt_bin [i];
					}
					if (vht <= 0.0 || vmt <= 0.0) continue;

					speed = vmt / vht;
					ttime = length / speed;

					if (ttime < 0.5) {
						last_offset -= length;
						continue;
					}

					//---- average speed method ----

					if (speed_flag) {
						time = (int) (ttime + 0.5);
							
						link_spd_file.Put_Field (spd_field, speed * MPSTOMPH);

						for (j=1; j <= time; j++) {
							link_spd_file.Put_Field (sec_field, ++second);

							if (!link_spd_file.Write ()) {
								Error ("Writing %s", link_spd_file.File_Type ());
							}
							num_rec++;
						}

					} else {

						//---- distribute the speed and travel time ----

						vht = 0.5 * ttime / vht;

						for (i=1; i <= num_out; i++) {
							if (vht_bin [i] > 0.0) {
								vmt_bin [i] = MPSTOMPH * vmt_bin [i] / vht_bin [i];
								vht_bin [i] = vht_bin [i] * vht;
							}
						}

						//---- distribute time to increasing speed bins ----

						bucket = 0.5;

						for (i=1; i <= num_out; i++) {
							if (vht_bin [i] <= 0.0) continue;

							vht = vht_bin [i] + bucket;
							time = (int) vht;
							bucket = vht - time;

							if (time == 0) continue;
							
							link_spd_file.Put_Field (spd_field, vmt_bin [i]);

							for (j=1; j <= time; j++) {
								link_spd_file.Put_Field (sec_field, ++second);

								if (!link_spd_file.Write ()) {
									Error ("Writing %s", link_spd_file.File_Type ());
								}
								num_rec++;
							}
						}

						//---- distribute time to decreasing speed bins ----

						for (i=num_out; i > 0; i--) {
							if (vht_bin [i] <= 0.0) continue;

							vht = vht_bin [i] + bucket;
							time = (int) vht;
							bucket = vht - time;

							if (time == 0) continue;
							
							link_spd_file.Put_Field (spd_field, vmt_bin [i]);

							for (j=1; j <= time; j++) {
								link_spd_file.Put_Field (sec_field, ++second);

								if (!link_spd_file.Write ()) {
									Error ("Writing %s", link_spd_file.File_Type ());
								}
								num_rec++;
							}
						}
					}
				}
			}
		}
	}
	End_Progress ();

	link_spd_file.Close ();

	Print (2, "Number of %s Records = %d", link_spd_file.File_Type (), num_rec);
}
