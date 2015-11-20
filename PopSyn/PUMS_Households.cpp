//*********************************************************
//	PUMS_Households.cpp - read the PUMS households
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	PUMS_Households
//---------------------------------------------------------

void PopSyn::PUMS_Households (void)
{
	int i, hhold, puma, type, index, db_hhold_field, db_person_field;
	double weight;
	char state [16];
	
	Household_Data *household_ptr;
	Household_Model *model_ptr;
	PUMA_Data *data_ptr;
	PUMA_Index *index_ptr;
	Attribute_Type *at_ptr;

	db_hhold_field = household_db.Field_Number ("HHOLD");
	db_person_field = household_db.Field_Number ("PERSONS");

	if (!household_db.Max_Records (pums_hh_file.Num_Records ())) goto mem_error;
	if (!household_data.Max_Records (pums_hh_file.Num_Records ())) goto mem_error;

	//---- read the PUMS household file ----

	Show_Message ("Reading %s -- Record", pums_hh_file.File_Type ());
	Set_Progress (50000);
	
	while (pums_hh_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		pums_hh_file.Get_Field (hh_hhold_field, &hhold);
		if (hhold <= 0) continue;

		//---- get the puma ID and weight ----

		pums_hh_file.Get_Field (hh_state_field, state);
		pums_hh_file.Get_Field (hh_puma_field, &puma);

		index_ptr = puma_index.PUMA_Key (state, puma);
		if (index_ptr == NULL) continue;

		puma = puma_index.Get_Index (index_ptr);
		if (puma == 0) continue;

		//---- save the household database ----

		household_db.Copy_Fields (&pums_hh_file);

		household_db.Put_Field (db_hhold_field, hhold);

		if (!household_db.Add_Record ()) {
			Error ("Writing %s Database", household_db.File_Type ());
		}
		index = household_db.Record_Index ();

		//---- save the household data ----

		household_ptr = household_data.New_Record (true);
		if (household_ptr == NULL) goto mem_error;

		household_ptr->Household (hhold);

		if (!household_data.Add ()) goto mem_error;

		//---- get the weight field ----

		if (weight_field > 0) {
			pums_hh_file.Get_Field (weight_field, &weight);
		} else {
			weight = 1.0;
		}

		//---- find the model puma ----

		for (model_ptr = household_model.First (); model_ptr; model_ptr = household_model.Next ()) {
			for (data_ptr = model_ptr->First (); data_ptr; data_ptr = model_ptr->Next ()) {
				if (data_ptr->PUMA () == puma) break;
			}
			if (data_ptr == NULL) continue;

			model_ptr->Set_PUMA_Data (data_ptr);

			for (at_ptr = model_ptr->Attribute (i=1); at_ptr; at_ptr = model_ptr->Attribute (++i)) {
				pums_hh_file.Get_Field (at_ptr->PUMS_Field (), &type);

				at_ptr->Type (at_ptr->PUMS_Type (type));
			}
			model_ptr->Add_Share (index, weight);
		}
	}
	End_Progress ();
	pums_hh_file.Close ();

	Print (2, "Number of %s Records = %d", pums_hh_file.File_Type (), Progress_Count ());
	Print (1, "Number of %s Records = %d", household_db.File_Type (), household_db.Num_Records ());

	household_data.Optimize ();
	return;

mem_error:
	Error ("Insufficient Memory for Household Database");
}
