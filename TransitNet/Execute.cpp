//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TransitNet::Execute (void)
{
	int old_location, old_access, old_parking, old_stop, old_route, old_schedule, old_driver;

	//---- read the network ----

	Network_Service::Execute ();

	old_location = nlocation;
	old_access = naccess;
	old_parking = nparking;
	old_stop = nstop;
	old_route = line_data.Route_Records ();
	old_schedule = line_data.Schedule_Records ();
	old_driver = driver_data.Driver_Records ();

	new_stop = max_stop;

	//---- prepare the network data ----

	Data_Setup ();

	Link_AreaType ();

	//---- read the park and ride file ----

	if (parkride_flag) {
		Read_ParkRide ();
	}

	//---- create the route data ----

	Read_Route ();

	//---- write the transit stops ----

	Write_Stop ();

	//---- write the transit routes ----

	Write_Route ();

	//---- write the transit driver file ----

	Write_Driver ();

	//---- write summary statistics ----

	Break_Check (10);
	Write (1);
	if (parkride_flag) {
		Write (1, "Number of Parking Records = %d (%d new)",
				nparking, (nparking - old_parking));
	}
	Write (1, "Number of Activity Location Records = %d (%d new)", 
			nlocation, (nlocation - old_location));
	Write (1, "Number of Process Link Records = %d (%d new)", 
			naccess, (naccess - old_access));
	Write (1, "Number of Transit Stop Records = %d", nstop);
	if (old_stop > 0) Write (0, " (%d new)", (nstop - old_stop));
	Write (1, "Number of Transit Route Records = %d", nroute);
	if (old_route > 0) {
		nroute -= old_route;
		if (route_edit > nroute) {
			Write (0, " (%d new %d changed)", nroute, (route_edit - nroute));
		} else {
			Write (0, " (%d new)", nroute);
		}
	}
	Write (1, "Number of Transit Schedule Records = %d", nschedule);
	if (old_schedule > 0) {
		nschedule -= old_schedule;
		if (schedule_edit > nschedule) {
			Write (0, " (%d new %d changed)", nschedule, (schedule_edit - nschedule));
		} else {
			Write (0, " (%d new)", nschedule);
		}
	}
	Write (1, "Number of Transit Driver Records = %d", ndriver);
	if (old_driver > 0) {
		ndriver -= old_driver;
		if (driver_edit > ndriver) {
			Write (0, " (%d new %d changed)", ndriver, (driver_edit - ndriver));
		} else {
			Write (0, " (%d new)", ndriver);
		}
	}

	if (end_warnings) {
		Write (2, "Number of Route End Node Warnings = %d", end_warnings);
	}
	if (parking_warnings) {
		Write (2, "Number of Parking Lot Warning Messages = %d", parking_warnings);
	}
	if (connect_warnings) {
		Error ("Number of Lane Connectivity Warning Messages = %d", connect_warnings);
	}
	Exit_Stat (DONE);
}


