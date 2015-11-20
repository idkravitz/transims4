//*********************************************************
//	Write_Speed_Hour.cpp - Write the MOVES Speed Hour File
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_Speed_Hour
//---------------------------------------------------------

void Emissions::Write_Speed_Hour (void)
{
	int i, vt_field, rt_field, tim_field, spd_field, fac_field;
	int vt, rt, tim, bin, last_vt, last_rt, last_tim, max_bin, num_rec;
	double factor, total;
	bool weekend_flag;
	
	Bin_Sum_Data *sum_ptr;

	//---- create the data fields ----

	spd_file.Add_Field ("sourceTypeID", Db_Code::INTEGER, 3);
	vt_field = 1;

	spd_file.Add_Field ("roadTypeID", Db_Code::INTEGER, 3);
	rt_field = 2;

	spd_file.Add_Field ("hourDayID", Db_Code::INTEGER, 5);
	tim_field = 3;

	spd_file.Add_Field ("avgSpeedBinID", Db_Code::INTEGER, 5);
	spd_field = 4;

	spd_file.Add_Field ("avgSpeedFraction", Db_Code::DOUBLE, 9, 6);
	fac_field = 5;

	spd_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", spd_file.File_Type ());
	Set_Progress ();

	factor = 1.0;
	total = 0.0;
	last_vt = last_rt = last_tim = max_bin = num_rec = 0;
	weekend_flag = (weekend_factor > 0.0 && weekend_factor < 1.0);

	for (sum_ptr = bin_sum_data.First_Key (); ; sum_ptr = bin_sum_data.Next_Key ()) {
		if (sum_ptr != NULL) {
			Show_Progress ();

			vt = sum_ptr->Vehicle_Type ();
			rt = sum_ptr->Road_Type ();
			tim = sum_ptr->Time_Bin ();
		}
		if (vt != last_vt || rt != last_rt || tim != last_tim || sum_ptr == NULL) {
			if (last_vt > 0) {
				spd_file.Put_Field (vt_field, last_vt);
				spd_file.Put_Field (rt_field, last_rt);

				if (weekend_flag) {
					vmt_bin [max_bin] += vmt_bin [max_bin+1];
				}
				for (i=2; i <= 5; i += 3) {
					if (i == 2 && weekend_factor == 0.0) continue;

					spd_file.Put_Field (tim_field, (last_tim * 10 + i));

					for (bin=1; bin <= max_bin; bin++) {
						if (total > 0.0) {
							if (i == 2 && weekend_flag) {
								factor = vmt_bin [bin] / total;
							} else {
								factor = vht_bin [bin] / total;
							}
						} else {
							factor = 0.0;
						}
						spd_file.Put_Field (spd_field, bin);
						spd_file.Put_Field (fac_field, factor);

						if (!spd_file.Write ()) {
							Error ("Writing %s", spd_file.File_Type ());
						}
						num_rec++;
					}
				}
			}
			if (sum_ptr == NULL) break;
			last_vt = vt;
			last_rt = rt;
			last_tim = tim;
			max_bin = 0;

			memset (vmt_bin, '\0', sizeof (vmt_bin));
			memset (vht_bin, '\0', sizeof (vht_bin));
			total = 0.0;
		}
		bin = sum_ptr->Speed_Bin ();
		if (bin > max_bin) max_bin = bin;

		vht_bin [bin] += sum_ptr->Seconds ();
		total += sum_ptr->Seconds ();

		if (weekend_flag) {
			vmt_bin [bin] += sum_ptr->Seconds () * weekend_factor;
			vmt_bin [bin+1] += sum_ptr->Seconds () * (1.0 - weekend_factor);
		}
	}
	End_Progress ();

	spd_file.Close ();

	Print (2, "Number of %s Records = %d", spd_file.File_Type (), num_rec);
}
