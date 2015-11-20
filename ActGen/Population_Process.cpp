//*********************************************************
//	Population_Process.cpp - Read the Population File
//*********************************************************

#include "ActGen.hpp"

#include "Population_File.hpp"

//---------------------------------------------------------
//	Population_Processing
//---------------------------------------------------------

bool ActGen::Population_Processing (Db_File *fh)
{
	Population_File *file;

	file = (Population_File *) fh;

	if (First_Record ()) {
		int max_rec;

		if (hhlist_flag) {
			max_rec = nhh_list * 10;
		} else {
			max_rec = file->Estimate_Records ();
		}
		if (!population_data.Max_Records (max_rec)) {
			Error ("Insufficient Memory for Population Data");
		}
	}
	if (hhlist_flag) {
		if (hhold_list.Get_Index (file->Household ()) == 0) return (false);
	}
	return (Demand_Service::Population_Processing (fh));
}
