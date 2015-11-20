//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "DynusTNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void DynusTNet::Program_Control (void)
{
	int i, lvalue;
	char *str_ptr, buffer [STRING_BUFFER], project [STRING_BUFFER];

	//---- create the network files ----

	Demand_Service::Program_Control ();

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_range.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);
	
	//---- get the select time period ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIOD);

	if (str_ptr != NULL) {
		Print (2, "Select Time Period = %s", str_ptr);

		if (!time_range.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	} else {
		time_range.Add_Ranges ("0:00..24:00");
	}

	//---- saturation flow rates ----

	str_ptr = Get_Control_String (SATURATION_FLOW_RATES);
	i = 0;

	while (str_ptr != NULL && i < EXTERNAL) {
		str_ptr = Get_Integer (str_ptr, &lvalue);
		if (lvalue == 0) break;

		if (lvalue < 1000 || lvalue > 3000) {
			Error ("Saturation Flow Rate %d is Out of Range (1000..3000)", lvalue);
		}
		sat_flow [++i] = lvalue;
	}
	if (i) {
		for (; i <= EXTERNAL; i++) {
			sat_flow [i] = lvalue;
		}
	}
	Print (2, "Saturation Flow Rates = ");

	for (i=1; i <= EXTERNAL; i++) {
		Print (0, "%d ", sat_flow [i]);
	}

	//---- service flow rates ----

	str_ptr = Get_Control_String (SERVICE_FLOW_RATES);
	i = 0;

	while (str_ptr != NULL && i < EXTERNAL) {
		str_ptr = Get_Integer (str_ptr, &lvalue);
		if (lvalue == 0) break;

		if (lvalue < 1000 || lvalue > 3000) {
			Error ("Service Flow Rate %d is Out of Range (1000..3000)", lvalue);
		}
		ser_flow [++i] = lvalue;
	}
	if (i) {
		for (; i <= EXTERNAL; i++) {
			ser_flow [i] = lvalue;
		}
	}
	Print (2, "Service Flow Rates = ");

	for (i=1; i <= EXTERNAL; i++) {
		Print (0, "%d ", ser_flow [i]);
	}

	//---- open the DynusT project name  ----
	
	str_ptr = Get_Control_String (DYNUST_PROJECT_NAME);

	if (str_ptr == NULL) {
		str_ptr = "ProjectName";
	}
	Print (2, "DynusT Project Name = %s", str_ptr);

	str_cpy (project, sizeof (project), str_ptr);

	//---- open the DynusT network directory  ----
	
	str_ptr = Get_Control_String (DYNUST_NETWORK_DIRECTORY);

	if (str_ptr == NULL) goto control_error;
	str_cpy (net_dir, sizeof (net_dir), Project_Filename (str_ptr));

	Print (2, "DynusT Network Directory = %s", net_dir);

	//---- project dws ----

	dws_file.File_Type ("DynusT Project File");

	str_fmt (buffer, sizeof (buffer), "%s/%s.dws", net_dir, project);

	dws_file.Create (buffer);

	fprintf (dws_file.File (), "VERSION = \"%3.1lf\"\nORIGIN = \"BOTTOM_LEFT\"\n", 1.3);

	dws_file.Close ();

	//---- xy.dat ----

	xy_file.File_Type ("DynusT Node Coordinate File");

	str_fmt (buffer, sizeof (buffer), "%s/xy.dat", net_dir);

	xy_file.Create (buffer);

	//---- network.dat ----

	net_file.File_Type ("DynusT Network File");

	str_fmt (buffer, sizeof (buffer), "%s/network.dat", net_dir);

	net_file.Create (buffer);

	if (Network_File_Flag (SHAPE)) {

		//---- linkxy.dat ----

		shp_file.File_Type ("DynusT Link Coordinate File");

		str_fmt (buffer, sizeof (buffer), "%s/linkxy.dat", net_dir);

		shp_file.Create (buffer);
	}

	//---- linkname.dat ----

	name_file.File_Type ("DynusT Link Name File");

	str_fmt (buffer, sizeof (buffer), "%s/linkname.dat", net_dir);

	name_file.Create (buffer);

	//---- movement.dat ----

	move_file.File_Type ("DynusT Movement File");

	str_fmt (buffer, sizeof (buffer), "%s/movement.dat", net_dir);

	move_file.Create (buffer);

	//---- control.dat ----

	ctl_file.File_Type ("DynusT Control File");

	str_fmt (buffer, sizeof (buffer), "%s/control.dat", net_dir);

	ctl_file.Create (buffer);

	//---- origin.dat ----

	org_file.File_Type ("DynusT Generation Link File");

	str_fmt (buffer, sizeof (buffer), "%s/origin.dat", net_dir);

	org_file.Create (buffer);

	//---- destination.dat ----

	des_file.File_Type ("DynusT Destination File");

	str_fmt (buffer, sizeof (buffer), "%s/destination.dat", net_dir);

	des_file.Create (buffer);

	//---- bus.dat ----

	bus_file.File_Type ("DynusT Bus File");

	str_fmt (buffer, sizeof (buffer), "%s/bus.dat", net_dir);

	bus_file.Create (buffer);

	//---- GradeLengthPCE.dat ----

	pce_file.File_Type ("DynusT Grade Length PCE File");

	str_fmt (buffer, sizeof (buffer), "%s/GradeLengthPCE.dat", net_dir);

	pce_file.Create (buffer);

	//---- leftcap.dat ----

	left_file.File_Type ("DynusT Left Capacity File");

	str_fmt (buffer, sizeof (buffer), "%s/leftcap.dat", net_dir);

	left_file.Create (buffer);

	//---- output_option.dat ----

	out_file.File_Type ("DynusT Output Option File");

	str_fmt (buffer, sizeof (buffer), "%s/output_option.dat", net_dir);

	out_file.Create (buffer);

	//---- ramp.dat ----

	ramp_file.File_Type ("DynusT Ramp File");

	str_fmt (buffer, sizeof (buffer), "%s/ramp.dat", net_dir);

	ramp_file.Create (buffer);

	//---- StopCap2Way.dat ----

	cap2_file.File_Type ("DynusT Stop Capacity 2 Way File");

	str_fmt (buffer, sizeof (buffer), "%s/StopCap2Way.dat", net_dir);

	cap2_file.Create (buffer);

	//---- StopCap4Way.dat ----

	cap4_file.File_Type ("DynusT Stop Capacity 4 Way File");

	str_fmt (buffer, sizeof (buffer), "%s/StopCap4Way.dat", net_dir);

	cap4_file.Create (buffer);

	//---- superzone.dat ----

	super_file.File_Type ("DynusT SuperZone File");

	str_fmt (buffer, sizeof (buffer), "%s/superzone.dat", net_dir);

	super_file.Create (buffer);

	//---- TrafficFlowModel.dat ----

	model_file.File_Type ("DynusT Traffic Flow Model File");

	str_fmt (buffer, sizeof (buffer), "%s/TrafficFlowModel.dat", net_dir);

	model_file.Create (buffer);

	//---- vms.dat ----

	vms_file.File_Type ("DynusT VMS File");

	str_fmt (buffer, sizeof (buffer), "%s/vms.dat", net_dir);

	vms_file.Create (buffer);

	//---- yield.dat ----

	yield_file.File_Type ("DynusT Yield Capacity File");

	str_fmt (buffer, sizeof (buffer), "%s/YieldCap.dat", net_dir);

	yield_file.Create (buffer);

	//---- zone.dat ----

	zone_file.File_Type ("DynusT Zone File");

	str_fmt (buffer, sizeof (buffer), "%s/zone.dat", net_dir);

	zone_file.Create (buffer);

	//---- demand.dat ----

	dem_file.File_Type ("DynusT Demand File");

	str_fmt (buffer, sizeof (buffer), "%s/Demand.dat", net_dir);

	dem_file.Create (buffer);

	//---- demand_truck.dat ----

	trk_file.File_Type ("DynusT Demand Truck File");

	str_fmt (buffer, sizeof (buffer), "%s/demand_truck.dat", net_dir);

	trk_file.Create (buffer);

	//---- demand_hov.dat ----

	hov_file.File_Type ("DynusT Demand HOV File");

	str_fmt (buffer, sizeof (buffer), "%s/demand_HOV.dat", net_dir);

	hov_file.Create (buffer);

	//---- zone_map.dat ----

	zmap_file.File_Type ("Zone Number Equivalence File");

	str_fmt (buffer, sizeof (buffer), "%s/zone_map.dat", net_dir);

	zmap_file.Create (buffer);

	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
