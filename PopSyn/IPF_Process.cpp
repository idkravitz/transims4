//*********************************************************
//	IPF_Process.cpp - apply the IPF process
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	IPF_Process
//---------------------------------------------------------

void PopSyn::IPF_Process (Household_Model *model_ptr)
{
	int i, type, types, cell, cells, offset;

	PUMA_Data *data_ptr;
	PUMA_Zone *zone_ptr;
	PUMA_Index *index_ptr;
	Attribute_Data *at_ptr;

	//---- check the data record ----

	cells = model_ptr->Num_Cells ();
	if (cells == 0) return;

	data_ptr = model_ptr->Get_PUMA_Data ();

	//---- copy the control totals ----

	for (cell=1; cell <= cells; cell++) {
		ipf_data.Share (cell, model_ptr->PUMA_Share (cell));
	}

	//---- copy the targets for each attribute ----

	offset = cells;

	for (at_ptr = ipf_data.Attribute (i=1); at_ptr; at_ptr = ipf_data.Attribute (++i)) {
		types = at_ptr->Num_Types ();

		for (type=1; type <= types; type++) {
			at_ptr->Target (type, model_ptr->PUMA_Target (++offset));
		}
	}

	//---- execute the ipf ----

	if (!ipf_data.IPF ()) {
		index_ptr = puma_index [data_ptr->PUMA ()];

		Write (1);
		Warning ("PUMA %s did not Converge: Iterations=%d, Difference=%.9lf", 
			index_ptr->text, ipf_data.Num_Iterations (), ipf_data.Difference ());
	}

	//---- save the results ----

	if (puma_report_flag) {
		Header_Number (CROSS_CLASS);

		if (!Break_Check (cells + 6)) {
			Print (1);
			Cross_Class_Header ();
		}		
	}

	for (cell=1; cell <= cells; cell++) {
		model_ptr->PUMA_Share (cell, ipf_data.Share (cell));

		if (puma_report_flag) {
			Print (1, "");

			for (at_ptr = ipf_data.Attribute (i=1); at_ptr; at_ptr = ipf_data.Attribute (++i)) {
				Print (0, " %10d", at_ptr->Index (cell));
			}
			Print (0, "   %12.9lf", ipf_data.Share (cell));
		}
	}
	Header_Number (0);

	offset = cells;

	for (at_ptr = ipf_data.Attribute (i=1); at_ptr; at_ptr = ipf_data.Attribute (++i)) {
		types = at_ptr->Num_Types ();

		for (type=1; type <= types; type++) {
			model_ptr->PUMA_Target (++offset, at_ptr->Target (type));
		}
	}
	//---- distribute to each zone ----

	for (zone_ptr = data_ptr->First (); zone_ptr; zone_ptr = data_ptr->Next ()) {
		model_ptr->Set_PUMA_Zone (zone_ptr);

		if (!stage2_data.Copy_Stage_One (model_ptr)) break;

		if (!stage2_data.IPF ()) {
			Write (1);
			Warning ("Zone %d did not Converge: Iterations=%d, Difference=%.9lf", 
				zone_ptr->Zone (), stage2_data.Num_Iterations (), stage2_data.Difference ());
		}
		if (!stage2_data.Save_Shares (model_ptr)) {
			Error ("Saving IPF Data");
		}
	}

	//---- distribute households by zone and cell ---

	model_ptr->Distribute_Households ();

	//---- write households for each zone ----

	Show_Message ("Writing Households for Zone");
	Set_Progress (1);

	for (zone_ptr = data_ptr->First (); zone_ptr; zone_ptr = data_ptr->Next ()) {
		Show_Progress (zone_ptr->Zone ());

		model_ptr->Set_PUMA_Zone (zone_ptr);

		if (model_ptr->Zone_Total () > 0) {
			Write_Households (model_ptr);
		}
	}
	End_Progress ();
}

//---------------------------------------------------------
//	Cross_Class_Header
//---------------------------------------------------------

void PopSyn::Cross_Class_Header (void)
{
	int i;

	Household_Model *model_ptr;
	PUMA_Data *data_ptr;
	PUMA_Index *index_ptr;
	Attribute_Type *at_ptr;

	model_ptr = household_model.Record ();
	data_ptr = model_ptr->Get_PUMA_Data ();
	index_ptr = puma_index [data_ptr->PUMA ()];

	Print (1, "PUMA %s Cross Classification", index_ptr->text);
	Print (2, "");

	for (at_ptr = model_ptr->Attribute (i=1); at_ptr; at_ptr = model_ptr->Attribute (++i)) {
		Print (0, " %10.10s", at_ptr->Field_Name ());
	}
	Print (0, "        Share");
}
