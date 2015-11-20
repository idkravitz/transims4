//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "RiderPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void RiderPrep::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	//---- merge the subarea ridership data ----

	Merge_Riders ();

	//---- output the new ridership file ----

	Write_Riders ();

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case LINE_RIDER:		//---- Line Ridership Report ----
				//Line_Rider_Report ((int) Report_Data ());
				//break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void RiderPrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case LINE_RIDER:			//---- Line Ridership Report ----
		//	Line_Rider_Header ();
		//	break;
		default:
			break;
	}
}
