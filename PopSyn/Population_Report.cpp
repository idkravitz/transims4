//*********************************************************
//	Population_Report.cpp - synthetic population report
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	Population_Report
//---------------------------------------------------------

void PopSyn::Population_Report (int num_pop)
{
	int field;
	double total, average, minimum, maximum;
	
	Db_Field *fld;	

	if (num_pop == 0) return;

	//---- print the report header ----

	Header_Number (SYN_POPULATION);

	if (!Break_Check (pop_fields + 6)) {
		Print (1);
		Population_Header ();
	}

	//----- summarize each field ----

	for (field=1; field <= pop_fields; field++) {

		fld = new_pop_file->Field (field);
		if (fld == NULL) continue;

		if (fld->Type () == Db_Code::STRING) continue;

		total = pop_total [field];
		minimum = pop_minimum [field];
		maximum = pop_maximum [field];
		
		//---- print the results ----

		average = total;

		if (num_pop > 0) {
			average /= num_pop;
		}
		Print (1, "%-20.20s %14.2lf %13.2lf %13.2lf %13.2lf", fld->Name (), total, average, minimum, maximum);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Population_Header
//---------------------------------------------------------

void PopSyn::Population_Header (void)
{
	Print (1, "Synthetic Population Summary");
	Print (2, "Attribute Name               Total       Average       Minimum       Maximum");
	Print (1);
}

/*********************************************|***********************************************

	Synthetic Population Summary

	Attribute Name                Total       Average       Minimum       Maximum

	ssssssssssssssssssss fffffffffff.ff ffffffffff.ff ffffffffff.ff ffffffffff.ff

**********************************************|***********************************************/ 
