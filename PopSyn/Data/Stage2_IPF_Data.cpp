//*********************************************************
//	Stage2_IPF_Data.cpp - Two Stage IPF data class
//*********************************************************

#include "Stage2_IPF_Data.hpp"

#include <string.h>

//---------------------------------------------------------
//	Stage2_IPF_Data constructor
//---------------------------------------------------------

Stage2_IPF_Data::Stage2_IPF_Data (void) : IPF_Data ()
{
}

//---- set stage two ----

bool Stage2_IPF_Data::Set_Stage_Two (IPF_Data *IPF_ptr) 
{
	int i, cell, cells, type, types;
	Attribute_Data *at_ptr, *new_ptr;

	//---- clear memory ----

	Clear ();

	//---- add the control totals ----

	cells = IPF_ptr->Num_Cells ();

	if (!Add_Attribute (cells)) return (false);

	//---- copy and expand each attribute ----

	for (at_ptr = IPF_ptr->Attribute (i=1); at_ptr; at_ptr = IPF_ptr->Attribute (++i)) {
		if (!Add_Attribute (2 * at_ptr->Num_Types ())) return (false);
	}

	//---- set the number of cells ----

	if (!Num_Cells (2 * cells)) return (false);

	//---- add indices and targets to the control totals ----

	new_ptr = Attribute (1);

	if (!new_ptr->Num_Cells (Num_Cells ())) return (false);

	for (cell=1; cell <= cells; cell++) {
		new_ptr->Index (cell, cell);
		new_ptr->Index (cell + cells, cell);
	}

	//---- calculate cell indices for attributes ----

	for (at_ptr = IPF_ptr->Attribute (i=1); at_ptr; at_ptr = IPF_ptr->Attribute (++i)) {
		new_ptr = Attribute (i+1);

		if (!new_ptr->Num_Cells (Num_Cells ())) return (false);

		types = at_ptr->Num_Types ();

		for (cell=1; cell <= cells; cell++) {
			type = at_ptr->Index (cell);

			new_ptr->Index (cell, type);
			new_ptr->Index (cell + cells, type + types);
		}
	}
	return (true);
}

//---- copy stage one ----

bool Stage2_IPF_Data::Copy_Stage_One (PUMA_Data_Array *puma_ptr) 
{
	int i, type, types, offset;
	double weight, target, share;

	Attribute_Type *at_ptr;
	Attribute_Data *new_ptr;

	//---- check the data record ----

	if (Num_Cells () == 0) return (false);

	//---- copy the control total attribute ----

	new_ptr = Attribute (1);
	types = new_ptr->Num_Types ();

	for (type=1; type <= types; type++) { 
		new_ptr->Target (type, puma_ptr->PUMA_Share (type));
	}
	weight = puma_ptr->PUMA_Total ();
	if (weight > 0.0) {
		weight = puma_ptr->Zone_Total () / weight;
	}

	//---- copy the targets for each attribute ----

	for (at_ptr = puma_ptr->Attribute (i=1); at_ptr; at_ptr = puma_ptr->Attribute (++i)) {
		new_ptr = Attribute (i+1);

		types = at_ptr->Num_Types ();
		offset = at_ptr->Offset ();

		for (type=1; type <= types; type++, offset++) {
			share = weight * puma_ptr->Zone_Target (offset);
			target = puma_ptr->PUMA_Target (offset) - share;

			new_ptr->Target (type, share);
			new_ptr->Target (type + types, target);
		}
	}
	Zero_Share ();
	return (true);
}

//---- save shares ----

bool Stage2_IPF_Data::Save_Shares (PUMA_Data_Array *puma_ptr) 
{
	int cell, cells;
	double total, share;
	bool flag;

	//---- check the data record ----

	if (Num_Cells () == 0) return (false);

	//---- calculate the scaling factor ----

	cells = puma_ptr->Num_Cells ();
	total = 0.0;

	for (cell=1; cell <= cells; cell++) {
		total += Share (cell);
	}

	//---- copy the factored share to the zone record ----

	flag = (total != 0.0);
	if (!flag) share = 0.0;

	for (cell=1; cell <= cells; cell++) {
		if (flag) {
			share = Share (cell) / total;
		}
		puma_ptr->Zone_Share (cell, share);
	}
	return (true);
}
