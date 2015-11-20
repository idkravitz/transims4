//*********************************************************
//	Read_Zones.cpp - read the PUMS households
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	Read_Zones
//---------------------------------------------------------

void PopSyn::Read_Zones (void)
{
	int i, zone, puma, type, types, offset;
	double total, target, sum;
	char state [16];

	Household_Model *model_ptr;
	PUMA_Data *data_ptr;
	PUMA_Zone *zone_ptr;
	PUMA_Index *index_ptr;
	Attribute_Type *at_ptr;

	//---- read the zone file ----

	Show_Message ("Reading %s -- Record", zone_file.File_Type ());
	Set_Progress (50000);
	
	while (zone_file.Read ()) {
		Show_Progress ();

		//---- get the zone id ----

		zone_file.Get_Field (zone_id_field, &zone);
		if (zone <= 0) continue;

		//---- get the puma ID and weight ----

		zone_file.Get_Field (zone_state_field, state);
		zone_file.Get_Field (zone_puma_field, &puma);

		index_ptr = puma_index.PUMA_Key (state, puma);
		if (index_ptr == NULL) continue;

		puma = puma_index.Get_Index (index_ptr);
		if (puma == 0) continue;

		//---- find the model puma ----

		for (model_ptr = household_model.First (); model_ptr; model_ptr = household_model.Next ()) {
			for (data_ptr = model_ptr->First (); data_ptr; data_ptr = model_ptr->Next ()) {
				if (data_ptr->PUMA () == puma) break;
			}
			if (data_ptr == NULL) continue;
 
			model_ptr->Set_PUMA_Data (data_ptr);

			zone_ptr = data_ptr->New_Zone (zone);
			if (zone_ptr == NULL) {
				Error ("Adding Zone Data Record");
			}
			model_ptr->Set_PUMA_Zone (zone_ptr);

			//---- get the attribute targets ----
			
			total = 0.0;

			for (at_ptr = model_ptr->Attribute (i=1); at_ptr; at_ptr = model_ptr->Attribute (++i)) {
				types = at_ptr->Num_Types ();
				offset = at_ptr->Offset ();
				sum = 0.0;

				for (type=1; type <= types; type++, offset++) {

					zone_file.Get_Field (at_ptr->Zone_Field (type), &target);

					model_ptr->Zone_Target (offset, target);
					sum += target;
				}
				total += sum;

				//---- normalize the distribution ----

				if (sum > 0.0 && sum != 1.0) {
					offset = at_ptr->Offset ();

					for (type=1; type <= types; type++, offset++) {
						model_ptr->Zone_Target (offset, model_ptr->Zone_Target (offset) / sum);
					}
				}
			}

			//---- get the total field ----

			if (model_ptr->Total_Field () > 0) {
				zone_file.Get_Field (model_ptr->Total_Field (), &total);
			} else {
				total /= model_ptr->Num_Attributes ();
			}
			model_ptr->Zone_Total (total);		
		}
	}
	End_Progress ();
	zone_file.Close ();

	Print (2, "Number of %s Records = %d", zone_file.File_Type (), Progress_Count ());
	Print (1);
	return;
}
