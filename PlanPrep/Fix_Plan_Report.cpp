//*********************************************************
//	Fix_Plan_Report.cpp - Create Fix Plan Report
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Fix_Plan_Report
//---------------------------------------------------------

void PlanPrep::Fix_Plan_Report (void)
{
	int i, lines;
	bool first;

	Fix_List *fix_ptr;
	
	Show_Message ("Fix Plan List Report");

	//---- print the report ----

	Header_Number (FIX_PLAN);
	New_Page ();
	first = true;

	for (fix_ptr = (Fix_List *) fix_list.First (); fix_ptr; fix_ptr = (Fix_List *) fix_list.Next ()) {

		lines = (fix_ptr->num_old + 7) / 8 + (fix_ptr->num_new + 7) / 8 + 4;

		if (!first && !Break_Check (lines)) {
			Print (1);
		}
		first = false;

		Print (1, "Fix List Number = %d, Percent Selected = %.1lf", fix_ptr->number, fix_ptr->share * 100);
		Print (1, "Number of Plans Found = %d, Number of Plans Modified = %d", fix_ptr->num_in, fix_ptr->num_out);
		Print (1, "Number of Old Plan Legs = %d", fix_ptr->num_old);

		for (i=0; i < fix_ptr->num_old; i++) {
			if (!(i % 8)) {
				Print (1, "%d", fix_ptr->old_list [i]);
			} else {
				Print (0, ", %d", fix_ptr->old_list [i]);
			}
		}
		Print (1, "Number of New Plan Legs = %d", fix_ptr->num_new);

		for (i=0; i < fix_ptr->num_new; i++) {
			if (!(i % 8)) {
				Print (1, "%d", fix_ptr->new_list [i]);
			} else {
				Print (0, ", %d", fix_ptr->new_list [i]);
			}
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Fix_Plan_Header
//---------------------------------------------------------

void PlanPrep::Fix_Plan_Header (void)
{
	Print (1, "Fix Plan List Report");
	Print (1);
}

/*********************************************|***********************************************

	Fix Plan List Report

	Fix List Number = %d, Percent Selected = %.1lf
	Number of Plans Found = %d, Number of Plans Modified = %d
	Number of Old Plan Legs = %d
	dddd, dddd, dddd
	dddd, dddd, dddd
	Number of New Plan Legs = %d
	dddd, dddd, dddd
	dddd, dddd, dddd

**********************************************|***********************************************/ 
