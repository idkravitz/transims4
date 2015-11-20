//*********************************************************
//	Write_Count.cpp - write the household person count file
//*********************************************************

#include "ActivityPattern.hpp"

//---------------------------------------------------------
//	Write_Count
//---------------------------------------------------------

void ActivityPattern::Write_Count (void)
{
	char buffer [STRING_BUFFER];

	Survey_Person *person_ptr;

	Show_Message ("Writing %s -- Record", count_file.File_Type ());
	Set_Progress (10000);

	for (person_ptr = survey_pop_data.First_Key (); person_ptr; person_ptr = survey_pop_data.Next_Key ()) {
		Show_Progress ();

		str_fmt (buffer, sizeof (buffer), "%d\t%d\t%d", 
			person_ptr->Household (), person_ptr->Person (), person_ptr->Count ());

		if (!count_file.Write (buffer)) {
			Error ("Writing %s", count_file.File_Type ());
		}
	}
	End_Progress ();
	count_file.Close ();

	Print (2, "Number of %s Records = %d", count_file.File_Type (), Progress_Count ());
}
