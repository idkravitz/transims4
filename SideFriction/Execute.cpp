//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SideFriction.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SideFriction::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	//---- add lane use records ----

	Stop_Delay ();

	Print (2, "Number of New Lane Use Records = %d", new_use);
	Print (1, "Number of Total Lane Use Records = %d", num_use + new_use);

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case DELAY_SUM:			//---- Delay Summary Report ----
				//Delay_Sum_Report ();
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

void SideFriction::Page_Header (void)
{
	switch (Header_Number ()) {
		case DELAY_SUM:				//---- Delay Summary Report ----
			//Delay_Sum_Header ();
			break;
		default:
			break;
	}
}
