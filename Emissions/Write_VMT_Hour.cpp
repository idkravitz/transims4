//*********************************************************
//	Write_VMT_Hour.cpp - Write the MOVES VMT Hour file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_VMT_Hour
//---------------------------------------------------------

void Emissions::Write_VMT_Hour (void)
{
	int i, vt_field, rt_field, tim_field, day_field, vmt_field, num_rec;
	int vt, rt, tim, last_vt, last_rt, max_tim;
	double factor, total;
	
	Bin_Sum_Data *sum_ptr;

	//---- create the data fields ----

	vmt_file.Add_Field ("sourceTypeID", Db_Code::INTEGER, 3);
	vt_field = 1;

	vmt_file.Add_Field ("roadTypeID", Db_Code::INTEGER, 3);
	rt_field = 2;

	vmt_file.Add_Field ("dayID", Db_Code::INTEGER, 5);
	day_field = 3;

	vmt_file.Add_Field ("hourID", Db_Code::INTEGER, 5);
	tim_field = 4;

	vmt_file.Add_Field ("hourVMTFraction", Db_Code::DOUBLE, 9, 6);
	vmt_field = 5;


	vmt_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", vmt_file.File_Type ());
	Set_Progress ();

	factor = 1.0;
	total = 0.0;
	last_vt = last_rt = max_tim = num_rec = 0;

	for (sum_ptr = bin_sum_data.First_Key (); sum_ptr; sum_ptr = bin_sum_data.Next_Key ()) {
		Show_Progress ();

		vt = sum_ptr->Vehicle_Type ();
		rt = sum_ptr->Road_Type ();

		if (vt != last_vt || rt != last_rt) {
			if (last_vt > 0) {
				vmt_file.Put_Field (vt_field, last_vt);
				vmt_file.Put_Field (rt_field, last_rt);

				for (i=2; i <= 5; i += 3) {
					vmt_file.Put_Field (day_field, i);

					for (tim=1; tim <= max_tim; tim++) {
						if (total > 0.0) {
							factor = vmt_bin [tim] / total;
						} else {
							factor = 0.0;
						}
						vmt_file.Put_Field (tim_field, tim);
						vmt_file.Put_Field (vmt_field, factor);

						if (!vmt_file.Write ()) {
							Error ("Writing %s", vmt_file.File_Type ());
						}
						num_rec++;
					}
				}
			}
			last_vt = vt;
			last_rt = rt;
			max_tim = 0;

			memset (vmt_bin, '\0', sizeof (vmt_bin));
			total = 0.0;
		}
		tim = sum_ptr->Time_Bin ();
		if (tim > max_tim) max_tim = tim;

		vmt_bin [tim] += sum_ptr->Meters ();
		total += sum_ptr->Meters ();
	}
	if (last_vt > 0) {
		vmt_file.Put_Field (vt_field, last_vt);
		vmt_file.Put_Field (rt_field, last_rt);
		
		for (i=2; i <= 5; i += 3) {
			vmt_file.Put_Field (day_field, i);

			for (tim=1; tim <= max_tim; tim++) {
				if (total > 0.0) {
					factor = vmt_bin [tim] / total;
				} else {
					factor = 0.0;
				}
				vmt_file.Put_Field (tim_field, tim);
				vmt_file.Put_Field (vmt_field, factor);

				if (!vmt_file.Write ()) {
					Error ("Writing %s", vmt_file.File_Type ());
				}
				num_rec++;
			}
		}
	}
	End_Progress ();

	vmt_file.Close ();

	Print (2, "Number of %s Records = %d", vmt_file.File_Type (), num_rec);
}
