//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "DynusTNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void DynusTNet::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	//---- add node links ----

	Data_Setup ();

	//---- write coordinate records ----

	Write_Coordinates ();

	//---- write network records ----

	Write_Network ();

	//---- write shape records ----

	if (Network_File_Flag (SHAPE)) {
		Write_Shapes ();
	}

	//---- write the link connections ----

	Write_Connections ();

	//---- write the traffic controls ----

	Write_Controls ();

	//---- write origin/destination zones ----

	Write_Zones ();

	//---- write the dummy files ----

	Write_Dummy ();

	//---- end the process ----

	Exit_Stat (DONE);
}
