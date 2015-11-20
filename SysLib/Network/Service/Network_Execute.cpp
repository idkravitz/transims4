//*********************************************************
//	Network_Execute.cpp - main execution procedure
//*********************************************************

#include "Network_Service.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Network_Service::Execute (void)
{
	File_Data *file, *new_file;

	if (!control_flag) {
		Error ("Network_Service::Program_Control has Not Be Called");
	}

	//---- node ----

	file = network_file + NODE;
	new_file = network_file + NEW_NODE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (node_array == NULL) {
			node_array = new Node_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Node ();

	//---- zone ----

	file = network_file + ZONE;
	new_file = network_file + NEW_ZONE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (zone_array == NULL) {
			zone_array = new Zone_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Zone ();

	//---- shape ----

	file = network_file + SHAPE;
	new_file = network_file + NEW_SHAPE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (shape_array == NULL) {
			shape_array = new Shape_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Shape ();

	//---- link ----

	file = network_file + LINK;
	new_file = network_file + NEW_LINK;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (link_array == NULL) {
			link_array = new Link_Array ();
		}
		if (dir_array == NULL) {
			dir_array = new Dir_Array ();
		}
		if (AB_Key_Flag ()) {
			ab_key_array = new AB_Key ();
		}
	}
	if (file->flag && file->access == READ) Read_Link ();

	//---- pocket ----

	file = network_file + POCKET_LANE;
	new_file = network_file + NEW_POCKET_LANE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (pocket_array == NULL) {
			pocket_array = new Pocket_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Pocket ();

	//---- lane use ----

	file = network_file + LANE_USE;
	new_file = network_file + NEW_LANE_USE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (lane_use_array == NULL) {
			lane_use_array = new Lane_Use_Array ();
		}
		if (link_use_array == NULL) {
			link_use_array = new Link_Use_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Lane_Use ();

	//---- toll ----

	file = network_file + TOLL;
	new_file = network_file + NEW_TOLL;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (toll_array == NULL) {
			toll_array = new Toll_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Toll ();

	//---- connectivity ----

	file = network_file + LANE_CONNECTIVITY;
	new_file = network_file + NEW_LANE_CONNECTIVITY;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (connect_array == NULL) {
			connect_array = new Connect_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Connect ();

	//---- turn ----

	file = network_file + TURN_PROHIBITION;
	new_file = network_file + NEW_TURN_PROHIBITION;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (turn_array == NULL) {
			turn_array = new Turn_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Turn ();

	//---- parking ----

	file = network_file + PARKING;
	new_file = network_file + NEW_PARKING;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (parking_array == NULL) {
			parking_array = new Parking_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Parking ();

	//---- location ----

	file = network_file + ACTIVITY_LOCATION;
	new_file = network_file + NEW_ACTIVITY_LOCATION;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (location_array == NULL) {
			location_array = new Location_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Location ();

	//---- stop ----

	file = network_file + TRANSIT_STOP;
	new_file = network_file + NEW_TRANSIT_STOP;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (stop_array == NULL) {
			stop_array = new Stop_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Stop ();

	//---- access ----

	file = network_file + PROCESS_LINK;
	new_file = network_file + NEW_PROCESS_LINK;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (access_array == NULL) {
			access_array = new Access_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Access ();

	//---- sign ----

	file = network_file + UNSIGNALIZED_NODE;
	new_file = network_file + NEW_UNSIGNALIZED_NODE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (sign_array == NULL) {
			sign_array = new Sign_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Sign ();

	//---- timing ----

	file = network_file + TIMING_PLAN;
	new_file = network_file + NEW_TIMING_PLAN;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (timing_array == NULL) {
			timing_array = new Timing_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Timing ();

	//---- coordinator ----

	file = network_file + SIGNAL_COORDINATOR;
	new_file = network_file + NEW_SIGNAL_COORDINATOR;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (coordinator_array == NULL) {
			coordinator_array = new Coordinator_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Coordinator ();

	//---- signal ----

	file = network_file + SIGNALIZED_NODE;
	new_file = network_file + NEW_SIGNALIZED_NODE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (signal_array == NULL) {
			signal_array = new Signal_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Signal ();

	//---- detector ----

	file = network_file + DETECTOR;
	new_file = network_file + NEW_DETECTOR;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (detector_array == NULL) {
			detector_array = new Detector_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Detector ();

	//---- phasing ----

	file = network_file + PHASING_PLAN;
	new_file = network_file + NEW_PHASING_PLAN;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (phasing_array == NULL) {
			phasing_array = new Phasing_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Phasing ();

	//---- fare ----

	file = network_file + TRANSIT_FARE;
	new_file = network_file + NEW_TRANSIT_FARE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (fare_array == NULL) {
			fare_array = new Fare_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Fare ();

	//---- line ----

	file = network_file + TRANSIT_ROUTE;
	new_file = network_file + NEW_TRANSIT_ROUTE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (line_array == NULL) {
			line_array = new Line_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Line ();

	//---- schedule ----

	file = network_file + TRANSIT_SCHEDULE;
	new_file = network_file + NEW_TRANSIT_SCHEDULE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (line_array == NULL) {
			Error ("Transit Route Data are required to Process Transit Schedules");
		}
	}
	if (file->flag && file->access == READ) Read_Schedule ();

	//---- schedule ----

	file = network_file + TRANSIT_DRIVER;
	new_file = network_file + NEW_TRANSIT_DRIVER;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (driver_array == NULL) {
			driver_array = new Driver_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Driver ();

	//---- route header ----

	file = network_file + ROUTE_HEADER;
	new_file = network_file + NEW_ROUTE_HEADER;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (route_array == NULL) {
			route_array = new Route_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Route_Header ();

	//---- route nodes ----

	file = network_file + ROUTE_NODES;
	new_file = network_file + NEW_ROUTE_NODES;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (route_array == NULL) {
			route_array = new Route_Array ();
		}
	}
	if (file->flag && file->access == READ) Read_Route_Nodes ();
}
