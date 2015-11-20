//*********************************************************
//	Write_Ramp_Frac.cpp - Write the MOVES Ramp Fraction file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_Ramp_Fraction
//---------------------------------------------------------

void Emissions::Write_Ramp_Fraction (void)
{
	int rt_field, des_field, ramp_field, rt, max_rt;
	double factor;
	char label [FIELD_BUFFER];
	
	Bin_Sum_Data *sum_ptr;
	Label_Type *type_ptr;

	//---- create the data fields ----

	ramp_file.Add_Field ("roadTypeID", Db_Code::INTEGER, 3);
	rt_field = 1;

	ramp_file.Add_Field ("roadDesc", Db_Code::STRING, 32);
	des_field = 2;

	ramp_file.Add_Field ("rampFraction", Db_Code::DOUBLE, 9, 6);
	ramp_field = 3;

	ramp_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", ramp_file.File_Type ());
	Set_Progress ();

	factor = 1.0;
	max_rt = 0;

	memset (vmt_bin, '\0', sizeof (vmt_bin));

	for (sum_ptr = bin_sum_data.First_Key (); sum_ptr; sum_ptr = bin_sum_data.Next_Key ()) {
		Show_Progress ();

		rt = sum_ptr->Road_Type ();
		if (rt > max_rt) max_rt = rt;

		vmt_bin [rt] += sum_ptr->Meters ();
	}

	for (rt=1; rt <= max_rt; rt++) {

		if (vmt_bin [rt] > 0.0) {
			factor = ramp_vmt [rt] / vmt_bin [rt];
		} else {
			factor = 0.0;
		}
		ramp_file.Put_Field (rt_field, rt);

		type_ptr = (Label_Type *) road_type.Get (&rt);

		if (type_ptr == NULL) {
			str_fmt (label, sizeof (label), "Road Type %d", rt);
		} else {
			str_cpy (label, sizeof (label), type_ptr->label);
		}
		ramp_file.Put_Field (des_field, label);
		ramp_file.Put_Field (ramp_field, factor);

		if (!ramp_file.Write ()) {
			Error ("Writing %s", ramp_file.File_Type ());
		}
	}
	End_Progress ();

	ramp_file.Close ();

	Print (2, "Number of %s Records = %d", ramp_file.File_Type (), max_rt);
}
