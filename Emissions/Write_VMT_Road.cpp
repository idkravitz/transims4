//*********************************************************
//	Write_VMT_Road.cpp - Write the MOVES VMT Road file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_VMT_Road
//---------------------------------------------------------

void Emissions::Write_VMT_Road (void)
{
	int vt_field, rt_field, vmt_field, num_rec;
	int vt, rt, last_vt, max_rt;
	double factor, total;
	
	Bin_Sum_Data *sum_ptr;

	//---- create the data fields ----

	road_file.Add_Field ("sourceTypeID", Db_Code::INTEGER, 3);
	vt_field = 1;

	road_file.Add_Field ("roadTypeID", Db_Code::INTEGER, 3);
	rt_field = 2;

	road_file.Add_Field ("roadTypeVMTFraction", Db_Code::DOUBLE, 9, 6);
	vmt_field = 3;

	road_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", road_file.File_Type ());
	Set_Progress ();

	factor = 1.0;
	total = 0.0;
	last_vt = max_rt = num_rec = 0;

	memset (vmt_bin, '\0', sizeof (vmt_bin));

	for (sum_ptr = bin_sum_data.First_Key (); sum_ptr; sum_ptr = bin_sum_data.Next_Key ()) {
		Show_Progress ();

		vt = sum_ptr->Vehicle_Type ();

		if (vt != last_vt) {
			if (last_vt > 0) {
				road_file.Put_Field (vt_field, last_vt);

				for (rt=1; rt <= max_rt; rt++) {
					if (total > 0.0) {
						factor = vmt_bin [rt] / total;
					} else {
						factor = 0.0;
					}
					road_file.Put_Field (rt_field, rt);
					road_file.Put_Field (vmt_field, factor);

					if (!road_file.Write ()) {
						Error ("Writing %s", road_file.File_Type ());
					}
					num_rec++;
				}
			}
			last_vt = vt;
			max_rt = 0;

			memset (vmt_bin, '\0', sizeof (vmt_bin));
			total = 0.0;
		}
		rt = sum_ptr->Road_Type ();
		if (rt > max_rt) max_rt = rt;

		vmt_bin [rt] += sum_ptr->Meters ();
		total += sum_ptr->Meters ();
	}
	if (last_vt > 0) {
		road_file.Put_Field (vt_field, last_vt);

		for (rt=1; rt <= max_rt; rt++) {
			if (total > 0.0) {
				factor = vmt_bin [rt] / total;
			} else {
				factor = 0.0;
			}
			road_file.Put_Field (rt_field, rt);
			road_file.Put_Field (vmt_field, factor);

			if (!road_file.Write ()) {
				Error ("Writing %s", road_file.File_Type ());
			}
			num_rec++;
		}
	}
	End_Progress ();

	road_file.Close ();

	Print (2, "Number of %s Records = %d", road_file.File_Type (), num_rec);
}
