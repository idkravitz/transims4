//*********************************************************
//	Fare_Report.cpp - Report Transit Fares
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Fare_Report
//---------------------------------------------------------

void Router::Fare_Report (void)
{
	Fare_Data *fare_ptr;

	Show_Message ("Transit Fare Report -- Record");
	Set_Progress (100);

	Header_Number (FARE_DATA);

	if (!Break_Check (fare_data.Num_Records () + 6)) {
		Print (1);
		Fare_Header ();
	}

	//---- process each fare record ----

	for (fare_ptr = fare_data.First_Key (); fare_ptr; fare_ptr = fare_data.Next_Key ()) {
		Show_Progress ();

		Print (1, "%5d %5d %12s %12s %18s %10s %6d", 
			fare_ptr->from_zone, fare_ptr->to_zone, 
			Transit_Code ((Transit_Type) fare_ptr->from_mode),
			Transit_Code ((Transit_Type) fare_ptr->to_mode),
			fare_data.Period_Format (fare_ptr->period),
			Class_Code ((Class_Type) fare_ptr->type),
			fare_ptr->fare);
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Fare_Header
//---------------------------------------------------------

void Router::Fare_Header (void)
{
	Print (1, "Transit Fare Data Report");
	Print (2, " From   To          From          To                           Fare   Fare");
	Print (1, " Zone  Zone         Mode         Mode        Time Period      Class  Cents");
	Print (1);
}

/*********************************************|***********************************************

	Transit Fare Data Report

	 From   To          From          To                           Fare   Fare                       
	 Zone  Zone         Mode         Mode        Time Period      Class  Cents

	ddddd ddddd ssssssssssss ssssssssssss ssssssssssssssssss ssssssssss  ddddd

**********************************************|***********************************************/ 
