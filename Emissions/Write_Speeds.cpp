//*********************************************************
//	Write_Speeds.cpp - Write the Speed Summary File
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_Speeds
//---------------------------------------------------------

void Emissions::Write_Speeds (void)
{
	int vt_field, rt_field, tim_field, spd_field, vmt_field, vht_field, fac_field;
	int vt, rt, tim, bin, last_vt, last_rt, last_tim, num_rec, max_bin;
	double factor, total;
	
	Bin_Sum_Data *sum_ptr;

	//---- create the data fields ----

	bin_sum_file.Add_Field ("VEHICLE", Db_Code::INTEGER, 3);
	vt_field = 1;

	bin_sum_file.Add_Field ("ROAD_TYPE", Db_Code::INTEGER, 3);
	rt_field = 2;

	bin_sum_file.Add_Field ("TIME_BIN", Db_Code::INTEGER, 5);
	tim_field = 3;

	bin_sum_file.Add_Field ("SPEED_BIN", Db_Code::INTEGER, 5);
	spd_field = 4;

	bin_sum_file.Add_Field ("VMT", Db_Code::DOUBLE, 14, 2);
	vmt_field = 5;

	bin_sum_file.Add_Field ("VHT", Db_Code::DOUBLE, 14, 2);
	vht_field = 6;

	bin_sum_file.Add_Field ("FRACTION", Db_Code::DOUBLE, 9, 6);
	fac_field = 7;

	bin_sum_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", bin_sum_file.File_Type ());
	Set_Progress ();

	factor = 1.0;
	total = 0.0;
	last_vt = last_rt = last_tim = max_bin = num_rec = 0;

	for (sum_ptr = bin_sum_data.First_Key (); ; sum_ptr = bin_sum_data.Next_Key ()) {
		if (sum_ptr != NULL) {
			Show_Progress ();

			vt = sum_ptr->Vehicle_Type ();
			rt = sum_ptr->Road_Type ();
			tim = sum_ptr->Time_Bin ();
		}
		if (vt != last_vt || rt != last_rt || tim != last_tim || sum_ptr == NULL) {
			if (last_vt > 0) {
				bin_sum_file.Put_Field (vt_field, last_vt);
				bin_sum_file.Put_Field (rt_field, last_rt);
				bin_sum_file.Put_Field (tim_field, last_tim);

				for (bin=1; bin <= max_bin; bin++) {
					if (total > 0.0) {
						factor = vht_bin [bin] / total;
					} else {
						factor = 0.0;
					}
					bin_sum_file.Put_Field (spd_field, bin);
					bin_sum_file.Put_Field (vmt_field, vmt_bin [bin] / MILETOMETER);
					bin_sum_file.Put_Field (vht_field, vht_bin [bin] / 3600.0);
					bin_sum_file.Put_Field (fac_field, factor);

					if (!bin_sum_file.Write ()) {
						Error ("Writing %s", bin_sum_file.File_Type ());
					}
					num_rec++;
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

		vmt_bin [bin] = sum_ptr->Meters ();
		vht_bin [bin] = sum_ptr->Seconds ();
		total += sum_ptr->Seconds ();
	}
	End_Progress ();

	bin_sum_file.Close ();

	Print (2, "Number of %s Records = %d", bin_sum_file.File_Type (), num_rec);
}
