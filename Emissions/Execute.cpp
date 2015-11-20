//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Emissions::Execute (void)
{
	//---- read the area type map ----

	if (at_map_flag) {
		Read_Area_Type_Map ();
	}

	//---- read the facility type map ----

	if (fac_map_flag) {
		Read_Facility_Map ();
	}

	//---- read the vehicle type map ----

	if (veh_map_flag) {
		Read_Vehicle_Map ();
	}

	//---- read the road type map ----

	if (road_map_flag) {
		Read_Road_Map ();
	}

	//---- read the hpms type map ----

	if (hpms_flag) {
		Read_HPMS_Map ();
	}

	//---- read the network and demand files ----

	Demand_Service::Execute ();

	//---- create the link list ----

	if (link_eq_flag) {
		Set_Link_List ();
	}

	//---- set the link box offsets ----

	if (!vol_spd_flag && !traveler_flag) {
		Set_Location ();
	} else if (traveler_flag) {
		Set_Link_Types ();
	}

	//---- calculate emissions ----

	if (rate_flag) {

		//---- identify category data ----

		if (scan_flag) {
			Scan_Rates ();
		}

		//---- read the emission rates ----

		Read_Rates ();

		//---- save the emission rates ----

		if (new_rate_flag) {
			Write_Rates ();
		}
	}

	//---- read data files ----

	if (vol_spd_flag) {
		Read_Volume_Speed ();
	} else if (traveler_flag) {
		Read_Traveler ();
	} else {
		Read_Speed_Bin ();
	}

	//---- generate MOVES input files ----

	if (bin_sum_flag) {
		Write_Speeds ();
	}
	if (hpms_flag) {
		Write_HPMS_VMT ();
	}
	if (ramp_flag) {
		Write_Ramp_Fraction ();
	}
	if (spd_flag) {
		Write_Speed_Hour ();
	}
	if (vmt_flag) {
		Write_VMT_Hour ();
	}
	if (road_flag) {
		Write_VMT_Road ();
	}
	if (input_flag) bin_sum_data.Clear ();

	//---- generate MOVES link files ----

	if (link_sum_flag) {
		Write_Link_Sum ();
	}
	if (link_veh_flag) {
		Write_Link_Veh ();
	}
	if (link_spd_flag) {
		Write_Link_Spd ();
	}
	if (link_ops_flag) {
		Write_Link_Ops ();
	}

	//---- save the emission summary ----

	if (output_flag) {
		Output_Emissions ();
	}

	Show_Message ("Writing Summary Reports");	

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case AT_REPORT:				//---- Emissions by Area Type ----
				Type_Reports (AT_REPORT, AREA_TYPE);
				break;
			case FT_REPORT:				//---- Emissions by Facility Type ----
				Type_Reports (FT_REPORT, FACILITY_TYPE);
				break;
			case VT_REPORT:				//---- Emissions by Vehicle Type ----
				Type_Reports (VT_REPORT, VEH_TYPE);
				break;
			case RT_REPORT:				//---- Emissions by Road Type ----
				Type_Reports (RT_REPORT, ROAD_TYPE);
				break;
			case AT_FT_REPORT:			//---- Emissions by Area and Facility ----
				Type_Reports (AT_FT_REPORT, AREA_TYPE, FACILITY_TYPE);
				break;
			case AT_VT_REPORT:			//---- Emissions by Area and Vehicle ----
				Type_Reports (AT_VT_REPORT, AREA_TYPE, VEH_TYPE);
				break;
			case FT_VT_REPORT:			//---- Emissions by Facility and Vehicle ----
				Type_Reports (FT_VT_REPORT, FACILITY_TYPE, VEH_TYPE);
				break;
			case RT_VT_REPORT:			//---- Emissions by Road and Vehicle ----
				Type_Reports (RT_VT_REPORT, ROAD_TYPE, VEH_TYPE);
				break;
			case SUMMARY_REPORT:		//---- Emissions Summary Report ----
				Summary_Report ();
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

void Emissions::Page_Header (void)
{
	switch (Header_Number ()) {
		case AT_REPORT:				//---- Emissions by Area Type ----
			Type_Headers ("Area Type");
			break;
		case FT_REPORT:				//---- Emissions by Facility Type ----
			Type_Headers ("Facility Type");
			break;
		case VT_REPORT:				//---- Emissions by Vehicle Type ----
			Type_Headers ("Vehicle Type");
			break;
		case RT_REPORT:				//---- Emissions by Road Type ----
			Type_Headers ("Road Type");
			break;
		case AT_FT_REPORT:			//---- Emissions by Area and Facility ----
			Type_Headers ("Area and Facility Type");
			break;
		case AT_VT_REPORT:			//---- Emissions by Area and Vehicle ----
			Type_Headers ("Area and Vehicle Type");
			break;
		case FT_VT_REPORT:			//---- Emissions by Facility and Vehicle ----
			Type_Headers ("Facility and Vehicle Type");
			break;
		case RT_VT_REPORT:			//---- Emissions by Road and Vehicle ----
			Type_Headers ("Road and Vehicle Type");
			break;
		case SUMMARY_REPORT:		//---- emissions summary report ----
			Summary_Header ();
			break;
		default:
			break;
	}
}
