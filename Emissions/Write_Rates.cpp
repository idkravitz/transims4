//*********************************************************
//	Write_Rates.cpp - write emission rate files
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Write_Rates
//---------------------------------------------------------

void Emissions::Write_Rates (void)
{
	int i, id, member, num_cat;

	Emission_Rate *rate_ptr;
	Category_Data *cat_ptr;	

	//---- create the data fields ----

	num_cat = 0;

	for (cat_ptr = category_data.First (); cat_ptr; cat_ptr = category_data.Next ()) {
		if (cat_ptr->Key_Flag ()) {
			new_rate_file.Add_Field (cat_ptr->Name (), Db_Code::INTEGER, 4);
			num_cat++;
		}
	}
	new_rate_file.Add_Field ("Rate", Db_Code::DOUBLE, 14, 9);

	new_rate_file.Write_Header ();

	//---- save the data to the file ----

	Show_Message ("Writing %s -- Record", new_rate_file.File_Type ());
	Set_Progress ();

	//---- write the emission rate file ----

	for (rate_ptr = emission_rate.First_Key (); rate_ptr; rate_ptr = emission_rate.Next_Key ()) {
		Show_Progress ();

		id = rate_ptr->ID ();
		i = num_cat;

		for (cat_ptr = category_data.Last (); cat_ptr; cat_ptr = category_data.Previous ()) {
			if (cat_ptr->Key_Flag ()) {
				member = (id % (cat_ptr->Num_Members () + 1));
				id = id / (cat_ptr->Num_Members () + 1);

				new_rate_file.Put_Field (i, member);
				i--;
			}
		}
		new_rate_file.Put_Field (num_cat+1, rate_ptr->Rate ());

		if (!new_rate_file.Write ()) {
			Error ("Writing %s", new_rate_file.File_Type ());
		}
	}
	End_Progress ();

	new_rate_file.Close ();

	Print (2, "Number of %s Records = %d", new_rate_file.File_Type (), Progress_Count ());
}
