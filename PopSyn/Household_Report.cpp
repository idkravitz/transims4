//*********************************************************
//	Household_Report.cpp - synthetic household report
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	Household_Report
//---------------------------------------------------------

void PopSyn::Household_Report (int num_hh)
{
	int field;
	double total, average, minimum, maximum;
	
	Db_Field *fld;	

	if (num_hh == 0) return;

	//---- print the report header ----

	Header_Number (SYN_HOUSEHOLD);

	if (!Break_Check (hh_fields + 6)) {
		Print (1);
		Household_Header ();
	}

	//----- summarize each field ----

	for (field=1; field <= hh_fields; field++) {

		fld = new_hh_file->Field (field);
		if (fld == NULL) continue;

		if (fld->Type () == Db_Code::STRING) continue;

		total = hh_total [field];
		minimum = hh_minimum [field];
		maximum = hh_maximum [field];
		
		//---- print the results ----

		average = total;

		if (num_hh > 0) {
			average /= num_hh;
		}
		Print (1, "%-20.20s %14.2lf %13.2lf %13.2lf %13.2lf", fld->Name (), total, average, minimum, maximum);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Household_Header
//---------------------------------------------------------

void PopSyn::Household_Header (void)
{
	Print (1, "Synthetic Household Summary");
	Print (2, "Attribute Name               Total       Average       Minimum       Maximum");
	Print (1);
}

/*********************************************|***********************************************

	Synthetic Household Summary

	Attribute Name                Total       Average       Minimum       Maximum

	ssssssssssssssssssss fffffffffff.ff ffffffffff.ff ffffffffff.ff ffffffffff.ff

**********************************************|***********************************************/ 
