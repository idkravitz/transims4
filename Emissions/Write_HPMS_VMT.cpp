//*********************************************************
//	Write_HPMS_VMT.cpp - Write the MOVES HPMS VMT file
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_HPMS_VMT
//---------------------------------------------------------

void Emissions::Write_HPMS_VMT (void)
{
	int vt, vt_field, yr_field, fac_field, vmt_field, off_field, max_vt, num_vt, year;

	//---- create the data fields ----

	hpms_file.Add_Field ("HPMSVtypeID", Db_Code::INTEGER, 3);
	vt_field = 1;

	hpms_file.Add_Field ("yearID", Db_Code::INTEGER, 5);
	yr_field = 2;
	
	hpms_file.Add_Field ("VMTGrowthFactor", Db_Code::DOUBLE, 9, 6);
	fac_field = 3;
	
	hpms_file.Add_Field ("HPMSBaseYearVMT", Db_Code::DOUBLE, 20, 2);
	vmt_field = 4;
	
	hpms_file.Add_Field ("baseYearOffNetVMT", Db_Code::DOUBLE, 20, 2);
	off_field = 5;

	hpms_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", hpms_file.File_Type ());
	Set_Progress ();

	year = years.First ();
	if (year == 0) year = 2007;

	max_vt = category_data.Get (HPMS)->Max_Value ();
	num_vt = 0;

	for (vt=1; vt <= max_vt; vt++) {
		Show_Progress ();

		if (hpms_vmt [vt] == 0.0) continue;

		hpms_file.Put_Field (vt_field, vt);
		hpms_file.Put_Field (yr_field, year);
		hpms_file.Put_Field (fac_field, 0);
		hpms_file.Put_Field (vmt_field, hpms_vmt [vt] / MILETOMETER);
		hpms_file.Put_Field (off_field, 0);

		if (!hpms_file.Write ()) {
			Error ("Writing %s", hpms_file.File_Type ());
		}
		num_vt++;
	}
	End_Progress ();

	hpms_file.Close ();

	Print (2, "Number of %s Records = %d", hpms_file.File_Type (), num_vt);
}
