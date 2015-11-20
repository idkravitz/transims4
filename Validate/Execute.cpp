//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Validate::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();
	
	//----- create the node to zone equivalence -----

	if (zone_flag) {
		if (!node_zone.Num_Records (node_data.Num_Records ())) {
			Error ("Insufficient Memory for Zone Data");
		}

		for (Node_Data *node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
			int *ptr = node_zone.Record (node_data.Record_Index ());

			if (ptr != NULL) {
				*ptr = Closest_Zone (node_ptr->X (), node_ptr->Y ());
			}
		}
	}

	//---- process link volume data ----

	if (link_flag) {
		if (delay_flag) {
			Sum_Volume ();
		} else {
			Read_Volume (&volume_file, false);
		}

		//---- read the count file ----

		Read_Volume (&count_file, true);
	}

	//---- process turning volume data ----

	if (turn_flag) {
		if (delay_flag) {
			Sum_Turn ();
		} else {
			Read_Turn (&turn_volume, false);
		}

		//---- read the count file ----

		Read_Turn (&turn_count, true);
	}

	//---- write the output volume file ----

	if (output_flag) {
		Write_Volume ();
	}

	//---- summarize the transit ridership ----

	if (line_flag || stop_flag) {
		rider_data.Summarize_Riders ();
	}

	//---- generate each report ----

	Show_Message ("Writing Performance Reports");	

	New_Page ();

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		group_data.Reset ();
		memset (&total, '\0', sizeof (total));

		switch (i) {
			case VOLUME_LEVEL:
                Volume_Stats ();
				break;
			case FACILITY_TYPE:
				Facility_Stats ();
				break;
			case AREA_TYPE:
				Area_Stats ();
				break;
			case ZONE_GROUP:
				Zone_Group_Stats ();
				break;
			case LINK_GROUP:
				Link_Group_Stats ();
				break;
			case GROUP_DETAILS:
				Link_Group_Details ();
				break;
			case TURN_MOVEMENT:
				Turn_Stats ();
				break;
			case TURN_LOS:
				Turn_LOS ();
				break;
			case LINE_GROUP:
				Line_Group_Stats ();
				break;
			case STOP_GROUP:
			case BOARD_GROUP:
			case ALIGHT_GROUP:
				Stop_Group_Stats (i);
				break;
			default:
				break;
		}
	}

	//---- process complete ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Validate::Page_Header (void)
{
	switch (Header_Number ()) {
		case VOLUME_LEVEL:
            Volume_Header ();
			break;
		case FACILITY_TYPE:
			Facility_Header ();
			break;
		case AREA_TYPE:
			Area_Header ();
			break;
		case ZONE_GROUP:
			Zone_Group_Header ();
			break;
		case LINK_GROUP:
			Link_Group_Header ();
			break;
		case GROUP_DETAILS:
			Group_Detail_Header ();
			break;
		case TURN_MOVEMENT:
			Turn_Header ();
			break;
		case TURN_LOS:
			Turn_LOS_Header ();
			break;
		case LINE_GROUP:
			Line_Group_Header ();
			break;
		case STOP_GROUP:
		case BOARD_GROUP:
		case ALIGHT_GROUP:
			Stop_Group_Header (Header_Number ());
			break;
		default:
			break;
	}
}
