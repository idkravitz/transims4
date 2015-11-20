//*********************************************************
//	TransimsNet.cpp - Network Conversion Utility
//*********************************************************

#include "TransimsNet.hpp"

char * TransimsNet::KEEP_NODE_LIST					= "KEEP_NODE_LIST";
char * TransimsNet::LINK_NODE_EQUIVALENCE			= "LINK_NODE_EQUIVALENCE";
char * TransimsNet::POCKET_LENGTHS_FOR_FACILITY_x	= "POCKET_LENGTHS_FOR_FACILITY_*";
char * TransimsNet::POCKET_TO_FACILITY_BY_AREA_TYPE	= "POCKET_TO_FACILITY_BY_AREA_TYPE";
char * TransimsNet::SIGNAL_WARRANT_FOR_AREA_TYPE_x	= "SIGNAL_WARRANT_FOR_AREA_TYPE_*";
char * TransimsNet::STOP_WARRANT_FOR_AREA_TYPE_x	= "STOP_WARRANT_FOR_AREA_TYPE_*";
char * TransimsNet::ACTIVITY_LOCATION_SIDE_OFFSET	= "ACTIVITY_LOCATION_SIDE_OFFSET";
char * TransimsNet::FACILITY_TYPE_ACCESS_FLAGS		= "FACILITY_TYPE_ACCESS_FLAGS";
char * TransimsNet::MAXIMUM_ACCESS_POINTS			= "MAXIMUM_ACCESS_POINTS";
char * TransimsNet::MINIMUM_SPLIT_LENGTHS			= "MINIMUM_SPLIT_LENGTHS";
char * TransimsNet::CELL_SIZE						= "CELL_SIZE";
char * TransimsNet::MINIMUM_LINK_LENGTH				= "MINIMUM_LINK_LENGTH";
char * TransimsNet::MAXIMUM_LENGTH_TO_XY_RATIO		= "MAXIMUM_LENGTH_TO_XY_RATIO";
char * TransimsNet::MAXIMUM_CONNECTION_ANGLE		= "MAXIMUM_CONNECTION_ANGLE";
char * TransimsNet::ADD_UTURN_TO_DEADEND_LINKS		= "ADD_UTURN_TO_DEADEND_LINKS";
char * TransimsNet::INTERSECTION_SETBACK_DISTANCE	= "INTERSECTION_SETBACK_DISTANCE";
char * TransimsNet::FIRST_EXTERNAL_ZONE_NUMBER		= "FIRST_EXTERNAL_ZONE_NUMBER";
char * TransimsNet::KEEP_ZONE_CONNECTORS_AS_LOCALS	= "KEEP_ZONE_CONNECTORS_AS_LOCALS";
char * TransimsNet::REPLICATE_MPO_NETWORK			= "REPLICATE_MPO_NETWORK";
char * TransimsNet::COLLAPSE_NODES_FLAG				= "COLLAPSE_NODES_FLAG";
char * TransimsNet::UPDATE_NODE_RANGE				= "UPDATE_NODE_RANGE";
char * TransimsNet::UPDATE_LINK_RANGE				= "UPDATE_LINK_RANGE";
char * TransimsNet::UPDATE_NODE_FILE				= "UPDATE_NODE_FILE";
char * TransimsNet::UPDATE_LINK_FILE				= "UPDATE_LINK_FILE";
char * TransimsNet::DELETE_NODE_RANGE				= "DELETE_NODE_RANGE";
char * TransimsNet::DELETE_LINK_RANGE				= "DELETE_LINK_RANGE";
char * TransimsNet::DELETE_NODE_FILE				= "DELETE_NODE_FILE";
char * TransimsNet::DELETE_LINK_FILE				= "DELETE_LINK_FILE";
char * TransimsNet::LINK_DETAIL_FILE				= "LINK_DETAIL_FILE";

//---------------------------------------------------------
//	TransimsNet constructor
//---------------------------------------------------------

TransimsNet::TransimsNet (void) : Network_Service ()
{
	Program ("TransimsNet");
	Version ("4.0.33");
	Title ("Network Conversion Utility");

	Network_File required_network [] = {
		NODE, LINK, NEW_NODE, NEW_LINK, NEW_ACTIVITY_LOCATION, NEW_PARKING, NEW_PROCESS_LINK,
		NEW_POCKET_LANE, NEW_LANE_CONNECTIVITY, NEW_UNSIGNALIZED_NODE, NEW_SIGNALIZED_NODE, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, ZONE, SHAPE, TURN_PROHIBITION, 
		ACTIVITY_LOCATION, PARKING, PROCESS_LINK, POCKET_LANE, LANE_CONNECTIVITY, LANE_USE,
		UNSIGNALIZED_NODE, SIGNALIZED_NODE,
		NEW_DIRECTORY, NEW_ZONE, NEW_SHAPE, NEW_LANE_USE,
		NEW_TURN_PROHIBITION, END_NETWORK
	};

	char *keys [] = {
		KEEP_NODE_LIST,
		LINK_NODE_EQUIVALENCE,
		POCKET_LENGTHS_FOR_FACILITY_x,
		POCKET_TO_FACILITY_BY_AREA_TYPE,
		SIGNAL_WARRANT_FOR_AREA_TYPE_x,
		STOP_WARRANT_FOR_AREA_TYPE_x,
		ACTIVITY_LOCATION_SIDE_OFFSET,
		FACILITY_TYPE_ACCESS_FLAGS,
		MAXIMUM_ACCESS_POINTS,
		MINIMUM_SPLIT_LENGTHS,
		CELL_SIZE,
		MINIMUM_LINK_LENGTH,
		MAXIMUM_LENGTH_TO_XY_RATIO,
		MAXIMUM_CONNECTION_ANGLE,
		ADD_UTURN_TO_DEADEND_LINKS,
		INTERSECTION_SETBACK_DISTANCE,
		FIRST_EXTERNAL_ZONE_NUMBER,
		KEEP_ZONE_CONNECTORS_AS_LOCALS,
		REPLICATE_MPO_NETWORK,
		COLLAPSE_NODES_FLAG,
		UPDATE_NODE_RANGE,
		UPDATE_LINK_RANGE,
		UPDATE_NODE_FILE,
		UPDATE_LINK_FILE,
		DELETE_NODE_RANGE,
		DELETE_LINK_RANGE,
		DELETE_NODE_FILE,
		DELETE_LINK_FILE,
		LINK_DETAIL_FILE,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);
	projection.Add_Keys ();

	zone_flag = zout_flag, turn_flag = shape_flag = name_flag = uturn_flag = false;
	update_flag = delete_flag = connect_flag = replicate_flag = pocket_to_flag = false;
	update_link_flag = update_node_flag = delete_link_flag = delete_node_flag = false;
	detail_flag = lane_use_flag = false;
	collapse_flag = true;
	max_length_factor = 1.2;
	location_side = 15.0;
	cell_size = 7.5;

	nnode = nlink = nparking = nactivity = nprocess = npocket = nconnect = nsign = nsignal = nlane_use = 0;
	mparking = mactivity = mprocess = mpocket;
	xlink = xnode = xparking = xactivity = xprocess = xpocket = xconnect = xsign = xsignal = 0;
	nshort = nlength = nexternal = nzone = nzout = ext_zone = high_zone = max_splits = 0;
	nfixed1 = nfixed2 = nfixed3 = nactuated1 = nactuated2 = nactuated3 = nstop = nyield = 0;
	min_length = setback = 0;
	nshape = nshapes = nturn = 0;

	straight_diff = compass.Num_Points () * 9 / 360;		//---- +/- 9 degrees ----
	thru_diff = compass.Num_Points () * 50 / 360;			//---- +/- 50 degrees ----
	max_angle = compass.Num_Points () * 120 / 360;			//---- +/- 120 degrees ----
	short_length = Round (200.0);							//---- 200 meters ----
	external_offset = Round (30.0);							//---- 30 meters ----

	access_flag [FREEWAY] = false;
	access_flag [EXPRESSWAY] = false;
	access_flag [PRINCIPAL] = false;
	access_flag [MAJOR] = true;
	access_flag [MINOR] = true;
	access_flag [COLLECTOR] = true;
	access_flag [LOCAL] = true;
	access_flag [FRONTAGE] = true;
	access_flag [RAMP] = false;
	access_flag [BRIDGE] = false;
	access_flag [WALKWAY] = false;
	access_flag [BIKEWAY] = false;
	access_flag [BUSWAY] = false;
	access_flag [LIGHTRAIL] = false;
	access_flag [HEAVYRAIL] = false;
	access_flag [FERRY] = false;
	access_flag [EXTERNAL] = false;
	
	node_array = (Node_Array *) new Node_List_Array ();
	link_array = (Link_Array *) new Link_Nodes_Array ();
	dir_array = (Dir_Array *) new Dir_Connect_Array ();
}

//---------------------------------------------------------
//	TransimsNet destructor
//---------------------------------------------------------

TransimsNet::~TransimsNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TransimsNet *exe = new TransimsNet ();

	return (exe->Start_Execution (commands, control));
}
