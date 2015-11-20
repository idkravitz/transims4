//*********************************************************
//	Read_Population.cpp - Read the Population File
//*********************************************************

#include "Demand_Service.hpp"

#include "Population_File.hpp"

//---------------------------------------------------------
//	Read_Population
//---------------------------------------------------------

void Demand_Service::Read_Population (void)
{
	Db_File *file = Demand_Db_File (POPULATION);

	//---- store the population data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (50000);

	if (Check_Data () || Renumber ()) {
		if (!Demand_File_Flag (HOUSEHOLD) && Demand_Option_Flag (HOUSEHOLD)) {
			Error (file_error, "Household", file->File_ID ());
		}
	}
	//file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Population_Processing (file)) {
			if (!population_data.Add ()) {
				Error ("Adding Record to the Population Data Array");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = population_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Population Data Records = %d", num);
	}
	population_data.Optimize ();

	if (num > 0) Demand_Data_True (POPULATION);
}

//---------------------------------------------------------
//	Population_Processing
//---------------------------------------------------------

bool Demand_Service::Population_Processing (Db_File *fh)
{
	bool household_flag;
	int lvalue, index;

	Population_Data *population_ptr;

	Population_File *file = (Population_File *) fh;

	household_flag = Demand_Data_Flag (HOUSEHOLD);

	//---- reserve memory ----

	if (population_data.Max_Records () == 0) {
		if (!population_data.Max_Records (file->Estimate_Records ())) goto mem_error;
	}

	//---- check the population id ----

	lvalue = file->Household ();
	if (lvalue <= 0) return (false);

	population_ptr = population_data.New_Record (true);
	if (population_ptr == NULL) goto mem_error;

	if (household_flag) {
		index = household_data.Get_Index (lvalue);

		if (index == 0) {
			Error ("Population household %d was Not Found in the Household File", lvalue);
		}
		if (Renumber ()) {
			lvalue = index;
		}
	}
	population_ptr->Household (lvalue);

	population_ptr->Person (file->Person ());
	population_ptr->Relate (file->Relate ());
	population_ptr->Age (file->Age ());
	population_ptr->Gender (file->Gender ());
	population_ptr->Work (file->Work ());
	population_ptr->Drive (file->Drive ());

	return (true);

mem_error:
	Error ("Insufficient Memory for Population Data");
	return (false);
}
