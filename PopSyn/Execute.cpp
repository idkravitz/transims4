//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PopSyn::Execute (void)
{
	int hhold0, pop0, veh0, hhold1, pop1, veh1, hhold2, pop2, veh2;

	Household_Model *model_ptr;
	PUMA_Data *puma_ptr;
	PUMA_Index *index_ptr;

	//---- read the network and travel data ----

	Demand_Service::Execute ();

	//---- set the index numbers ----

	if (max_hh >= household) {
		household = (((max_hh + 1) / 1000) + 1) * 1000;
	}
	if (max_veh >= vehicle) {
		vehicle = (((max_veh + 1) / 1000) + 1) * 1000;
	}
	if (old_hh >= household) {
		household = old_hh + 1;
	}
	if (old_pop >= population) {
		population = old_pop + 1;
	}
	if (old_veh >= vehicle) {
		vehicle = old_veh + 1;
	}
	hhold0 = household;
	pop0 = population;
	veh0 = vehicle;

	//---- read zone location file ----

	if (zone_loc_flag) {
		Read_Zone_Locations ();
	}

	//---- vehicle type distribution ----

	if (veh_type_flag) {
		Vehicle_Types ();
	}

	//---- calculate the activity location weights ----

	Location_Weights ();

	//---- read the PUMS household file ----

	PUMS_Households ();

	//---- read the PUMS population file ----

	PUMS_Population ();

	//---- read the zone file ----

	Read_Zones ();

	//---- process each household model ----

	for (model_ptr = household_model.First (); model_ptr; model_ptr = household_model.Next ()) {

		Print (2, "Household Model #%d", model_ptr->Model ());
			
		hhold1 = household;
		pop1 = population;
		veh1 = vehicle;

		//---- initialize the IPF structures ----

		IPF_Setup (model_ptr);

		//---- process each puma in the household model ----

		for (puma_ptr = model_ptr->First (); puma_ptr; puma_ptr = model_ptr->Next ()) {

			index_ptr = puma_index [puma_ptr->PUMA ()];

			if (puma_ptr->Num_Zones () == 0) {
				Warning ("No Data for Model %d PUMA %s", model_ptr->Model (), index_ptr->text);
				Write (1);
				continue;
			}
			Show_Message ("Processing Model %d PUMA %s", model_ptr->Model (), index_ptr->text);

			model_ptr->Set_PUMA_Data (puma_ptr);

			hhold2 = household;
			pop2 = population;
			veh2 = vehicle;

			IPF_Process (model_ptr);

			Print (1, "\tPUMA %s Households = %d, Population = %d, Vehicles = %d", index_ptr->text, 
				(household - hhold2), (population - pop2), (vehicle - veh2));
		}
		Print (1, "Model #%d Total Households = %d, Population = %d, Vehicles = %d", model_ptr->Model (), 
			(household - hhold1), (population - pop1), (vehicle - veh1));
	}

	//---- close the output files ----

	new_hh_file->Close ();
	new_pop_file->Close ();
	new_veh_file->Close ();

	//---- print reports ----

	Show_Message ("Writing Reports");	

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case PUMS_HOUSEHOLD:		//---- PUMS Household Summary ----
				PUMS_HH_Summary ();
				break;
			case PUMS_POPULATION:		//---- PUMS Population Summary ---
				PUMS_Pop_Summary ();
				break;
			case SYN_HOUSEHOLD:			//---- Synthetic Household Summary ----
				Household_Report (household - hhold0);
				break;
			case SYN_POPULATION:		//----- Synthetic Population Summary ----
				Population_Report (population - pop0);
				break;
			default:
				break;
		}
	}

	//---- summarize the results ----

	Break_Check (4);
	Print (1);
	Write (1, "Number of New Household Records = %d", household - hhold0);
	Write (1, "Number of New Population Records = %d", population - pop0);
	Write (1, "Number of New Vehicle Records = %d", vehicle - veh0);

	if (old_hh > 0 || old_pop > 0 || old_veh > 0) {
		Break_Check (4);
		Print (1);
		if (old_hh > 0) {
			Write (1, "Number of Total Household Records = %d", household - hhold0 + old_hh);
		}
		if (old_pop > 0) {
			Write (1, "Number of Total Population Records = %d", population - pop0 + old_pop);
		}
		if (old_veh > 0) {
			Write (1, "Number of Total Vehicle Records = %d", vehicle - veh0 + old_veh);
		}
	}

	//---- print the problem summary report ----

	Report_Problems ();

	Exit_Stat (DONEX);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PopSyn::Page_Header (void)
{
	switch (Header_Number ()) {
		case CROSS_CLASS:			//---- PUMA Cross Classification ----
			Cross_Class_Header ();
			break;
		case PUMS_HOUSEHOLD:		//---- PUMS Household Summary ----
			PUMS_HH_Header ();
			break;
		case PUMS_POPULATION:		//---- PUMS Population Summary ----
			PUMS_Pop_Header ();
			break;
		case SYN_HOUSEHOLD:			//---- Synthetic Household Summary ----
			Household_Header ();
			break;
		case SYN_POPULATION:		//---- Synthetic Population Summary ----
			Population_Header ();
			break;
		default:
			break;
	}
}
