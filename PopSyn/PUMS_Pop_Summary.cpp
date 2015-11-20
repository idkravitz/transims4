//*********************************************************
//	PUMS_Pop_Summary.cpp - summarize the PUMS population
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	PUMS_Pop_Summary
//---------------------------------------------------------

void PopSyn::PUMS_Pop_Summary (void)
{
	int num, field, count;
	int db_hhold_field, db_person_field;
	double total, average, minimum, maximum, dvalue;
	
	Db_Field *fld;	

	Show_Message ("Summarizing %s -- Record", population_db.File_Type ());

	//---- get field numbers ----

	db_hhold_field = population_db.Field_Number ("HHOLD");
	db_person_field = population_db.Field_Number ("PERSON");

	num = population_db.Num_Fields ();

	//---- print the report header ----

	Header_Number (PUMS_POPULATION);

	if (!Break_Check (num + 6)) {
		Print (1);
		PUMS_Pop_Header ();
	}

	//----- summarize each field ----

	for (field=1; field <= num; field++) {
		if (field == db_hhold_field || field == db_person_field) continue;

		fld = population_db.Field (field);
		if (fld == NULL) continue;

		if (fld->Type () == Db_Code::STRING) continue;

		population_db.Rewind ();
		Set_Progress (10000);

		count = 0;
		total = maximum = 0.0;
		minimum = (double) MAX_INTEGER;
		
		while (population_db.Read_Record ()) {
			Show_Progress ();

			population_db.Get_Field (field, &dvalue);

			count++;
			total += dvalue;
			if (dvalue < minimum) minimum = dvalue;
			if (dvalue > maximum) maximum = dvalue;
		}

		//---- print the results ----

		average = total;

		if (count > 0) {
			average /= count;
		} else {
			minimum = 0.0;
		}
		Print (1, "%-20.20s %14.2lf %13.2lf %13.2lf %13.2lf", fld->Name (), total, average, minimum, maximum);
	}
	Header_Number (0);
	End_Progress ();
}

//---------------------------------------------------------
//	PUMS_Pop_Header
//---------------------------------------------------------

void PopSyn::PUMS_Pop_Header (void)
{
	Print (1, "PUMS Population Summary");
	Print (2, "Attribute Name                Total       Average       Minimum       Maximum");
	Print (1);
}

/*********************************************|***********************************************

	PUMS Population Summary

	Attribute Name               Total       Average       Minimum       Maximum

	ssssssssssssssssssss ffffffffff.ff ffffffffff.ff ffffffffff.ff ffffffffff.ff

**********************************************|***********************************************/ 
