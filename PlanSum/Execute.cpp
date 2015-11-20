//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanSum::Execute (void)
{
	int nfile, i, j, num;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- prepare data ----

	Data_Preparation ();

	//---- read the household list ----

	if (hhlist_flag) {
		Read_HHList ();
	}

	//---- read the household person count ----

	if (count_flag) {
		Read_Count ();
	}

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

		if (update_flag) {
			if (!new_plan_file.Open (nfile)) {
				Error ("Opening %s", new_plan_file.File_Type ());
			}
		}

		//---- process the plan files ----

		Read_Plans ();
	}

	if (nfile == 0) {
		File_Error ("No Plan Files were Found", plan_file.Filename ());
	}

	//---- write summary statistics ----

	Break_Check (6);

	Write (2, "Number of Plan Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", plan_file.Num_Plans ());
	Write (1, "Number of Input Records = %d", plan_file.Num_Records ());
	Write (1, "Number of Input Travelers = %d", plan_file.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", plan_file.Num_Trips ());

	if (update_flag) {
		Break_Check (5);

		Write (2, "Number of Output Plans = %d", new_plan_file.Num_Plans ());
		Write (1, "Number of Output Records = %d", new_plan_file.Num_Records ());
		Write (1, "Number of Output Travelers = %d", new_plan_file.Num_Travelers ());
		Write (1, "Number of Output Trips = %d", new_plan_file.Num_Trips ());
	}
	Write (1);

	//---- write the volume file ----

	if (volume_flag) {
		Write_Volume ();
	}

	//---- write the trip time file ----

	if (time_flag) {
		Diurnal_Distribution ();
	}

	//---- write the link delay file ----

	if (delay_out) {
		Link_Delay ();
	}

	//---- transit ridership output ----

	if (transit_flag) {
		if (Demand_File_Flag (NEW_RIDERSHIP)) {
			Write_Riders ();
		}
	}

	//---- write the skim file ----

	if (skim_flag) {
		if (time_skim_flag) {
			if (length_flag) {
				Write_Time_Length ();
			} else {
				Write_Time ();
			}
		} else {
			Write_Skim ();
		}
	}

	//---- write the trip table ----

	if (trip_flag) {
		Write_Trips ();
	}

	//---- write the turn movements ----

	if (turn_flag) {
		Write_Turns ();
	}

	//---- print reports ----

	Show_Message ("Writing Performance Reports");	

	for (i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TOP_100:			//---- Top 100 V/C Ratio Report ----
				Top_100_Report ();
				break;
			case VC_RATIO:			//---- All V/C Ratios Greater Than x.xx ---
				VC_Ratio (Report_Data ());
				break;
			case LINK_GROUP:		//---- Link Group Report -----
				Link_Group (Report_Data ());
				break;
			case SUM_RIDERS:		//---- Transit Ridership Report ----
				Transit_Report ();
				break;
			case SUM_STOPS:			//---- Transit Stop Report ----
				Stop_Report ();
				break;
			case SUM_TRANSFERS:		//---- Transit Transfer Report ----
				Transfer_Report (0);
				break;
			case XFER_DETAILS:		//---- Transit Transfer Details ----
				num = time_periods.Num_Ranges () * 2 + 1;

				for (j=1; j < num; j++) {
					Transfer_Report (j);
				}
				break;
			case STOP_GROUP:		//---- Transit Stop Group Report ----
				Stop_Group_Report ();
				break;
			case SUM_PASSENGERS:	//---- Transit Passenger Summary ----
				Passenger_Report ();
				break;
			case RIDER_GROUP:		//---- Transit Link Group Summary ----
				Rider_Group ();
				break;
			case TRIP_TIME:			//---- Trip Time Report ----
				Trip_Time ();
				break;
			case SUM_TRAVEL:		//---- Travel Summary Report ----
				Travel_Report ();
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

void PlanSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOP_100:			//---- Top 100 V/C Ratio Report ----
			Top_100_Header ();
			break;
		case VC_RATIO:			//---- All V/C Ratios Greater Than x.xx ---
			VC_Ratio_Header ();
			break;
		case LINK_GROUP:		//---- Link Group Report -----
			Link_Group_Header ();
			break;
		case SUM_RIDERS:		//---- Transit Ridership Report ----
			Transit_Header ();
			break;
		case SUM_STOPS:			//---- Transit Stop Report ----
			Stop_Header ();
			break;
		case SUM_TRANSFERS:		//---- Transit Transfer Report ----
		case XFER_DETAILS:		//---- Transit Transfer Details ----
			Transfer_Header ();
			break;
		case STOP_GROUP:		//---- Transit Stop Group Report ----
			Stop_Group_Header ();
			break;
		case SUM_PASSENGERS:	//---- Transit Passenger Summary ----
			Passenger_Header ();
			break;
		case RIDER_GROUP:		//---- Transit Link Group Summary ----
			Rider_Header ();
			break;
		case TRIP_TIME:			//---- Trip Time Report ----
			Trip_Time_Header ();
			break;
		case SUM_TRAVEL:		//---- Travel Summary Report ----
			Travel_Header ();
			break;
		default:
			break;
	}
}
