//*********************************************************
//	Output_Emissions.cpp - Write the Emission File
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Output_Emissions
//---------------------------------------------------------

void Emissions::Output_Emissions (void)
{
	int i, at_field, ft_field, rt_field, vt_field, vmt_field, vht_field, spd_field, *pollutant;
	char *str_ptr, buffer [FIELD_BUFFER];
	double vmt, vht, spd;
	
	Summary_Data *sum_ptr;
	Category_Data *cat_ptr;

	pollutant = new int [num_pollutants];
	if (pollutant == NULL) return;
	
	cat_ptr = category_data.Get (POLLUTANT);

	//---- create the data fields ----

	output_file.Add_Field ("AREA_TYPE", Db_Code::INTEGER, 5);
	at_field = 1;

	output_file.Add_Field ("FACILITY", Db_Code::INTEGER, 3);
	ft_field = 2;

	output_file.Add_Field ("ROAD_TYPE", Db_Code::INTEGER, 3);
	rt_field = 3;

	output_file.Add_Field ("VEHICLE", Db_Code::INTEGER, 3);
	vt_field = 4;

	output_file.Add_Field ("VMT", Db_Code::DOUBLE, 14, 2);
	vmt_field = 5;

	output_file.Add_Field ("VHT", Db_Code::DOUBLE, 14, 2);
	vht_field = 6;

	output_file.Add_Field ("SPEED", Db_Code::DOUBLE, 6, 1);
	spd_field = 7;

	i = 0;

	for (str_ptr = cat_ptr->First_Label (); str_ptr; str_ptr = cat_ptr->Next_Label ()) {
		str_fmt (buffer, sizeof (buffer), "%.10s", str_ptr);

		output_file.Add_Field (buffer, Db_Code::DOUBLE, 14, 2);
		pollutant [i] = 8 + i;
		i++;
	}

	output_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", output_file.File_Type ());
	Set_Progress ();

	for (sum_ptr = summary_data.First_Key (); sum_ptr; sum_ptr = summary_data.Next_Key ()) {
		Show_Progress ();

		vmt = sum_ptr->Meters ();
		vht = sum_ptr->Seconds ();

		if (vht > 0.0) {
			spd = vmt / vht;
		} else {
			spd = 0.0;
		}
		vmt /= 1000.0;
		vht /= 3600.0;

		output_file.Put_Field (at_field, sum_ptr->Area_Type ());
		output_file.Put_Field (ft_field, sum_ptr->Facility_Type ());
		output_file.Put_Field (rt_field, sum_ptr->Road_Type ());
		output_file.Put_Field (vt_field, sum_ptr->Vehicle_Type ());
		output_file.Put_Field (vmt_field, vmt);
		output_file.Put_Field (vht_field, vht);
		output_file.Put_Field (spd_field, spd);
		
		for (i=1; i <= num_pollutants; i++) {
			output_file.Put_Field (pollutant [i-1], sum_ptr->Emissions (i) / 1000.0);
		}
		if (!output_file.Write ()) {
			Error ("Writing %s", output_file.File_Type ());
		}
	}
	delete [] pollutant;

	End_Progress ();

	output_file.Close ();

	Print (2, "Number of %s Records = %d", output_file.File_Type (), Progress_Count ());
}
