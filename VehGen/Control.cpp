//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "VehGen.hpp"

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void VehGen::Program_Control (void)
{
	Db_Header *file;

	Demand_Service::Program_Control ();

	file = (Db_Header *) Demand_Db_Base (HOUSEHOLD);

	vehicle_flag = (file->Optional_Field ("VEHICLES", "AUTOS") != 0);
} 
