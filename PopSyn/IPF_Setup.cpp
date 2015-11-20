//*********************************************************
//	IPF_Setup.cpp - initialize the IPF classes
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	IPF_Setup
//---------------------------------------------------------

void PopSyn::IPF_Setup (Household_Model *model_ptr)
{
	int i, attributes;

	Attribute_Type *at_ptr;

	//---- clear memory ----

	ipf_data.Clear ();

	//---- copy the attribute types ----

	attributes = model_ptr->Num_Attributes ();

	for (i=1; i <= attributes; i++) {
		at_ptr = model_ptr->Attribute (i);

		if (!ipf_data.Add_Attribute (at_ptr->Num_Types ())) {
			Error ("Adding IPF Attribute");
		}
	}

	//---- set data cells ----

	if (!ipf_data.Set_Cells ()) {
		Error ("Creating IPF Cells");
	}

	//---- initialize the stage2 process ----

	if (!stage2_data.Set_Stage_Two (&ipf_data)) {
		Error ("Creating Stage2 Data");
	}

	//---- sum the zone targets and normalize the weights ----

	model_ptr->Sum_Targets ();
	
}
