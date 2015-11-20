//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ActBalance.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ActBalance::Execute (void)
{
	int i;
	Factor_Group *group;
	
	//---- create the purpose map ----

	for (i=1; i <= max_purpose; i++) {
		for (group = (Factor_Group *) factor_group.First (); group; group = (Factor_Group *) factor_group.Next ()) {
			if (group->purpose->In_Range (i)) {
				if (purpose_map [i] != 0) {
					Error ("Purpose %d was assigned to Multiple Groups", i);
				}
				purpose_map [i] = factor_group.Record_Index ();
			}
		}
	}

	Demand_Service::Execute ();

	//---- read the balancing target file ----

	Read_Targets ();

	//---- process each file ----

	for (i=0; ; i++) {
		if (!Read_Activity (i)) break;
	}
	if (i == 0) {
		Error ("No Activity Files were Found");
	}
	if (i > 1) {
		Print (2, "Total Number of Activity Records Read = %d", total_act);
		Print (1, "Total Number of Activity Records Used = %d", total_used);
		if (total_act > 0) {
			Print (0, " (%.1lf%%)", total_used * 100.0 / total_act);
		}
	}

	//---- write the balancing factors ----

	Write_Factors ();

	//---- print reports ----

	for (i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case ZONE_SUM:
				Zone_Report ();
				break;
			case DISTRICT_SUM:
				District_Report ();
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ActBalance::Page_Header (void)
{
	switch (Header_Number ()) {
		case ZONE_SUM:	
			Zone_Header ();
			break;
		case DISTRICT_SUM:
			District_Header ();
			break;
		default:
			break;
	}
}
