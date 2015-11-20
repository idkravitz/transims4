//*********************************************************
//	Network_Code.cpp - network codes
//*********************************************************

#include "Network_Code.hpp"

#include "Utility.hpp"

//---- facility type codes ----

char * Network_Code::facility_text [] = {
	"FREEWAY", "EXPRESSWAY", "PRINCIPAL", "MAJOR", "MINOR", "COLLECTOR", "LOCAL", 
	"FRONTAGE", "RAMP", "BRIDGE", "EXTERNAL",
	"XPRESSWAY", "PRIARTER", "SECARTER", "ZONECONN", "OTHER", 
	"WALKWAY", "BIKEWAY", "BUSWAY", "LIGHTRAIL", "HEAVYRAIL", "FERRY", NULL
};

Network_Code::Facility_Type  Network_Code::facility_code [] = {
	FREEWAY, EXPRESSWAY, PRINCIPAL, MAJOR, MINOR, COLLECTOR, LOCAL, 
	FRONTAGE, RAMP, BRIDGE, EXTERNAL, 
	EXPRESSWAY, MAJOR, MINOR, EXTERNAL, BRIDGE, 
	WALKWAY, BIKEWAY, BUSWAY, LIGHTRAIL, HEAVYRAIL, FERRY
};

//---- use restriction codes ----

int Network_Code::use_mask [Network_Code::RESTRICTED+1] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

char * Network_Code::use_text [] = {
	"ANY", "WALK", "BICYCLE", "AUTO", "TRUCK", "BUS", "RAIL",
	"SOV", "HOV2", "HOV3", "HOV4", "LIGHTTRUCK", "HEAVYTRUCK", "RESTRICTED", 
	"CAR", "BIKE", "TAXI", 
	"TROLLEY", "STREETCAR", "LIGHTRAIL", "RAPIDRAIL", "REGIONRAIL", NULL
};

Network_Code::Use_Type  Network_Code::use_code [] = {
	ANY, WALK, BIKE, CAR, TRUCK, BUS, RAIL,
	SOV, HOV2, HOV3, HOV4, LIGHTTRUCK, HEAVYTRUCK, RESTRICTED,
	CAR, BIKE, TAXI, 
	RAIL, RAIL, RAIL, RAIL, RAIL
};

//---- location end type codes ----

char * Network_Code::end_text [] = {
	"NODE", "ACTIVITY", "PARKING", "TRANSIT", NULL
};

Network_Code::End_Type  Network_Code::end_code [] = {
	NODE_ID, LOCATION_ID, PARKING_ID, STOP_ID
};

//---- transit mode codes ----

char * Network_Code::transit_text [] = {
	"NONE", "NO_MODE", "N/A", "", "BUS", "LOCAL_BUS", "EXPRESS", "EXPRESS_BUS", 
	"TROLLEY", "STREETCAR", "LIGHTRAIL", "RAPIDRAIL", "REGIONRAIL", "ANY_MODE", "ANY", NULL
};

Network_Code::Transit_Type  Network_Code::transit_code [] = {
	NO_TRANSIT, NO_TRANSIT, NO_TRANSIT, NO_TRANSIT, LOCAL_BUS, LOCAL_BUS, EXPRESS_BUS, EXPRESS_BUS,
	TROLLEY, STREETCAR, LRT, RAPIDRAIL, REGIONRAIL, ANY_TRANSIT, ANY_TRANSIT
};

//---- transit class codes ----

char * Network_Code::class_text [] = {
	"CASH", "CARD", "SPECIAL", NULL
};

Network_Code::Class_Type  Network_Code::class_code [] = {
	CASH, CARD, SPECIAL
};

//---- transit stop codes ----

char * Network_Code::stop_text [] = {
	"STOP", "STATION", "EXTERNAL", "NEARSIDE", "FARSIDE", "MIDBLOCK", NULL
};

Network_Code::Stop_Type  Network_Code::stop_code [] = {
	STOP, STATION, EXTLOAD, NEARSIDE, FARSIDE, MIDBLOCK
};

//---- vehicle type map ----

Network_Code::Use_Type Network_Code::veh_type_map [] = {
	ANY, CAR, TRUCK, CAR, BUS, BUS, RAIL, RAIL, RAIL, RAIL, RAIL,
	HOV2, HOV3, HOV4, LIGHTTRUCK, HEAVYTRUCK, RESTRICTED
}; 

//---- transit use map ----

Network_Code::Use_Type Network_Code::transit_use_map [] = {
	ANY, BUS, BUS, RAIL, RAIL, RAIL, RAIL, RAIL, ANY
}; 

//---- lane use restriction codes ----

char * Network_Code::restrict_text [] = {
	"O", "R", "N", "ONLY", "REQUIRE", "PROHIBIT", "", NULL
};

Network_Code::Restrict_Type  Network_Code::restrict_code [] = {
	ONLY, REQUIRE, PROHIBIT, ONLY, REQUIRE, PROHIBIT, ONLY, 
};

//---- parking type codes ----

char * Network_Code::parking_text [] = {
	"STREET", "LOT", "BOUNDARY", "PARKRIDE", 
	"PRSTR", "HISTR", "DRVWY", "BNDRY", "PARKR", NULL
};

Network_Code::Parking_Type  Network_Code::parking_code [] = {
	STREET, LOT, BOUNDARY, PARKRIDE, 
	STREET, STREET, LOT, BOUNDARY, PARKRIDE
};

//---- pocket lane codes ----

char * Network_Code::pocket_text [] = {
	"T", "P", "M", "TURN", "POCKET", "MERGE", "", NULL
};

Network_Code::Pocket_Type  Network_Code::pocket_code [] = {
	TURN_LANE, POCKET, MERGE_LANE, TURN_LANE, POCKET, MERGE_LANE, TURN_LANE
};

//---- lane connection codes ----

char * Network_Code::connect_text [] = {
	"", "THRU", "LEFT", "RIGHT", "UTURN", "R_MERGE", "R_SPLIT", "L_MERGE", "L_SPLIT", "MERGE", "DIVERGE", 
	"0", "T", "L", "R", "U", "M", "D", "RM", "RS", "LM", "LS", NULL
};

Network_Code::Connect_Type  Network_Code::connect_code [] = {
	NO_TYPE, THRU, LEFT, RIGHT, UTURN, R_MERGE, R_SPLIT, L_MERGE, L_SPLIT, R_MERGE, R_SPLIT, 
	NO_TYPE, THRU, LEFT, RIGHT, UTURN, R_MERGE, R_SPLIT, R_MERGE, R_SPLIT, L_MERGE, L_SPLIT
};

//---- signal codes ----

char * Network_Code::signal_text [] = {
	"T", "A", "TIMED", "ACTUATED", "FIXED", "", NULL
};

Network_Code::Signal_Type  Network_Code::signal_code [] = {
	TIMED, ACTUATED, TIMED, ACTUATED, TIMED, ACTUATED, 
};

//---- ring codes ----

char * Network_Code::ring_text [] = {
	"S", "D", "T", "SINGLE", "DUAL", "TRIPLE", "SINGLE_RING", "DUAL_RING", "TRIPLE_RING", "", NULL
};

Network_Code::Ring_Type  Network_Code::ring_code [] = {
	SINGLE_RING, DUAL_RING, MAX_RING, SINGLE_RING, DUAL_RING, MAX_RING, 
	SINGLE_RING, DUAL_RING, MAX_RING, SINGLE_RING
};

//---- protection codes ----

char * Network_Code::protection_text [] = {
	"P", "U", "S", "PROTECTED", "PERMITTED", "STOP_PERMIT", "", NULL
};

Network_Code::Protection_Type  Network_Code::protection_code [] = {
	PROTECTED, PERMITTED, STOP_PERMIT, PROTECTED, PERMITTED, STOP_PERMIT, PERMITTED, 
};

//---- detector codes ----

char * Network_Code::detector_text [] = {
	"PRESENCE", "PASSAGE", NULL
};

Network_Code::Detector_Type  Network_Code::detector_code [] = {
	PRESENCE, PASSAGE
};

//---- sign codes ----

char * Network_Code::sign_text [] = {
	"S", "Y", "N", "STOP", "YIELD", "NONE", "", NULL
};

Network_Code::Sign_Type  Network_Code::sign_code [] = {
	STOP_SIGN, YIELD_SIGN, NO_SIGN, STOP_SIGN, YIELD_SIGN, NO_SIGN, NO_SIGN
};

//---- string buffer ----

char Network_Code::buffer [] = " ";

//---------------------------------------------------------
//	Network_Code constructor
//---------------------------------------------------------

Network_Code::Network_Code (void) : Db_Code ()
{
	if (use_mask [0] == -1) {
		Initialize_Use ();
	}
}

//---------------------------------------------------------
//	Initialize_Use
//---------------------------------------------------------

void Network_Code::Initialize_Use (void)
{
	static char *use_map [] = {
	//---WB1234LHBRXX---
		"111111111110", 
		"100000000000", 
		"010000000000", 
		"001111000000", 
		"000000110000", 
		"000000001000", 
		"000000000100",
		"001000000000",
		"000100000000",
		"000010000000",
		"000001000000",
		"000000100000",
		"000000010000",
		"000000000010",
		"000000000001"
	};

	//---- calculate the access mask values ----

	for (int i=0; i <= RESTRICTED; i++) {
		char *str_ptr = use_map [i];
		if (str_ptr == NULL) {
			exe->Error ("Use Map is Undefined for Type %d", i);
		}
		use_mask [i] = 0;

		for (int j=0; *str_ptr != '\0'; j++, str_ptr++) {
			if (*str_ptr == '1') {
				use_mask [i] |= (1 << j);
			}
		}
	}
}

//---------------------------------------------------------
//	Facility_Code
//---------------------------------------------------------

Network_Code::Facility_Type  Network_Code::Facility_Code (char *text)
{
	for (int i=0; facility_text [i] != NULL; i++) {
		if (str_cmp (text, facility_text [i]) == 0) {
			return (facility_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Facility Type \"%s\"", text);
	}
	return (EXTERNAL);
}

char * Network_Code::Facility_Code (Network_Code::Facility_Type code)
{
	for (int i=0; facility_text [i] != NULL; i++) {
		if (code == facility_code [i]) {
			return (facility_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Facility Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Use_Code
//---------------------------------------------------------

int Network_Code::Use_Code (char *text)
{
	int hov2, hov3, hov4;
	str_cpy (buffer, sizeof (buffer), text);
	text = buffer;

	if (*text == '\0') {
		if (Send_Messages ()) {
			exe->Error ("Use Permissions were Not Found");
		}
		return (0);
	}
	int i, use;
	char *ptr = text;

	use = 0;
	hov2 = hov3 = hov4 = 0;

	while (ptr != NULL) {
		text = ptr;
		ptr = strchr (text, '/');

		if (ptr != NULL) {
			*ptr++ = '\0';
		}
		for (i=0; use_text [i] != NULL; i++) {
			if (str_cmp (text, use_text [i]) == 0) {

				//---- check HOV codes ----

				if (use_code [i] == HOV2) hov2 = i;
				if (use_code [i] == HOV3) hov3 = i;
				if (use_code [i] == HOV4) hov4 = i;

				use |= use_mask [use_code [i]];
				break;
			}
		}
		if (use_text [i] == NULL) {
			if (Send_Messages ()) {
				exe->Error ("Converting Use Permission \"%s\"", text);
			}
			return (0);
		}
	}
	//---- fix LANL HOV codes ----

	if (hov2 || hov3 || hov4) {
		if ((use & use_mask [CAR]) == use_mask [CAR]) {
			use -= use_mask [CAR];

			if (hov2) use |= use_mask [use_code [hov2]];
			if (hov3) use |= use_mask [use_code [hov3]];
			if (hov4) use |= use_mask [use_code [hov4]];
		}
	}
	return (use);
}

//---------------------------------------------------------
//	Use_Code
//---------------------------------------------------------

char * Network_Code::Use_Code (int code)
{
	if (code == ANY) {
		str_cpy (buffer, sizeof (buffer), "ANY");
	} else {
		bool first = true;

		buffer [0] = '\0';

		for (int i=0; i <= RESTRICTED; i++) {
			if ((code & use_mask [i]) == use_mask [i]) {
				code -= use_mask [i];

				if (!first) {
					str_cat (buffer, sizeof (buffer), "/");
				} else {
					first = false;
				}
				str_cat (buffer, sizeof (buffer), use_text [i]);
			}
		}
		if (buffer [0] == '\0' || code != 0) {
			if (Send_Messages ()) {
				exe->Error ("Converting Use Code %d", code);
			}
		}
	}
	return (buffer);
}

//---------------------------------------------------------
//	End_Code
//---------------------------------------------------------

Network_Code::End_Type Network_Code::End_Code (char *text)
{
	for (int i=0; end_text [i] != NULL; i++) {
		if (str_cmp (text, end_text [i]) == 0) {
			return (end_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Access End Type \"%s\"", text);
	}
	return (NODE_ID);
}

char * Network_Code::End_Code (End_Type code)
{
	for (int i=0; end_text [i] != NULL; i++) {
		if (code == end_code [i]) {
			return (end_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Access End Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Transit_Code
//---------------------------------------------------------

Network_Code::Transit_Type Network_Code::Transit_Code (char *text)
{
	for (int i=0; transit_text [i] != NULL; i++) {
		if (str_cmp (text, transit_text [i]) == 0) {
			return (transit_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Transit Mode \"%s\"", text);
	}
	return (NO_TRANSIT);
}

char * Network_Code::Transit_Code (Transit_Type code)
{
	for (int i=0; transit_text [i] != NULL; i++) {
		if (code == transit_code [i]) {
			return (transit_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Transit Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Class_Code
//---------------------------------------------------------

Network_Code::Class_Type Network_Code::Class_Code (char *text)
{
	for (int i=0; class_text [i] != NULL; i++) {
		if (str_cmp (text, class_text [i]) == 0) {
			return (class_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Transit Class \"%s\"", text);
	}
	return (CASH);
}

char * Network_Code::Class_Code (Class_Type code)
{
	for (int i=0; class_text [i] != NULL; i++) {
		if (code == class_code [i]) {
			return (class_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Transit Class Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Stop_Code
//---------------------------------------------------------

Network_Code::Stop_Type Network_Code::Stop_Code (char *text)
{
	for (int i=0; stop_text [i] != NULL; i++) {
		if (str_cmp (text, stop_text [i]) == 0) {
			return (stop_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Transit Stop Type \"%s\"", text);
	}
	return (STOP);
}

char * Network_Code::Stop_Code (Stop_Type code)
{
	for (int i=0; stop_text [i] != NULL; i++) {
		if (code == stop_code [i]) {
			return (stop_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Transit Stop Type %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Veh_Use_Code
//---------------------------------------------------------

Network_Code::Use_Type Network_Code::Veh_Use_Code (char *text)
{
	for (int i=0; use_text [i] != NULL; i++) {
		if (str_cmp (text, use_text [i]) == 0) {
			return (use_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Vehicle Use Type \"%s\"", text);
	}
	return (ANY);
}

char * Network_Code::Veh_Use_Code (Use_Type code)
{
	for (int i=0; use_text [i] != NULL; i++) {
		if (code == use_code [i]) {
			return (use_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Vehicle Use Type %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Veh_Type_Map
//---------------------------------------------------------

Network_Code::Use_Type Network_Code::Veh_Type_Map (int code)
{
	for (int i=0; veh_type_map [i] != RESTRICTED; i++) {
		if (i == code) {
			return (veh_type_map [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Vehicle Type %d", code);
	}
	return (ANY);
}

//---------------------------------------------------------
//	Map_Veh_Type
//---------------------------------------------------------

int Network_Code::Map_Veh_Type (Network_Code::Use_Type code)
{
	for (int i=0; veh_type_map [i] != RESTRICTED; i++) {
		if (veh_type_map [i] == code) {
			return (i);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Vehicle Type %d", code);
	}
	return (0);
}

//---------------------------------------------------------
//	Transit_Use_Map
//---------------------------------------------------------

Network_Code::Use_Type Network_Code::Transit_Use_Map (Network_Code::Transit_Type code)
{
	return (transit_use_map [code]);
}

//---------------------------------------------------------
//	Restrict_Code
//---------------------------------------------------------

Network_Code::Restrict_Type Network_Code::Restrict_Code (char *text)
{
	for (int i=0; restrict_text [i] != NULL; i++) {
		if (str_cmp (text, restrict_text [i]) == 0) {
			return (restrict_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Restriction Type \"%s\"", text);
	}
	return (ONLY);
}

char * Network_Code::Restrict_Code (Restrict_Type code)
{
	for (int i=0; restrict_text [i] != NULL; i++) {
		if (code == restrict_code [i]) {
			return (restrict_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Restriction Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Parking_Code
//---------------------------------------------------------

Network_Code::Parking_Type Network_Code::Parking_Code (char *text)
{
	for (int i=0; parking_text [i] != NULL; i++) {
		if (str_cmp (text, parking_text [i]) == 0) {
			return (parking_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Parking Type \"%s\"", text);
	}
	return (STREET);
}

char * Network_Code::Parking_Code (Parking_Type code)
{
	for (int i=0; parking_text [i] != NULL; i++) {
		if (code == parking_code [i]) {
			return (parking_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Parking Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Pocket_Code
//---------------------------------------------------------

Network_Code::Pocket_Type Network_Code::Pocket_Code (char *text)
{
	for (int i=0; pocket_text [i] != NULL; i++) {
		if (str_cmp (text, pocket_text [i]) == 0) {
			return (pocket_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Pocket Lane Type \"%s\"", text);
	}
	return (TURN_LANE);
}

char * Network_Code::Pocket_Code (Pocket_Type code)
{
	for (int i=0; pocket_text [i] != NULL; i++) {
		if (code == pocket_code [i]) {
			return (pocket_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Pocket Lane Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Connect_Code
//---------------------------------------------------------

Network_Code::Connect_Type Network_Code::Connect_Code (char *text)
{
	for (int i=0; connect_text [i] != NULL; i++) {
		if (str_cmp (text, connect_text [i]) == 0) {
			return (connect_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Connection Type \"%s\"", text);
	}
	return (NO_TYPE);
}

char * Network_Code::Connect_Code (Connect_Type code)
{
	for (int i=0; connect_text [i] != NULL; i++) {
		if (code == connect_code [i]) {
			return (connect_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Connection Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Signal_Code
//---------------------------------------------------------

Network_Code::Signal_Type Network_Code::Signal_Code (char *text)
{
	for (int i=0; signal_text [i] != NULL; i++) {
		if (str_cmp (text, signal_text [i]) == 0) {
			return (signal_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Signal Type \"%s\"", text);
	}
	return (ACTUATED);
}

char * Network_Code::Signal_Code (Signal_Type code)
{
	for (int i=0; signal_text [i] != NULL; i++) {
		if (code == signal_code [i]) {
			return (signal_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Signal Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Ring_Code
//---------------------------------------------------------

Network_Code::Ring_Type Network_Code::Ring_Code (char *text)
{
	for (int i=0; ring_text [i] != NULL; i++) {
		if (str_cmp (text, ring_text [i]) == 0) {
			return (ring_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Ring Type \"%s\"", text);
	}
	return (SINGLE_RING);
}

char * Network_Code::Ring_Code (Ring_Type code)
{
	for (int i=0; ring_text [i] != NULL; i++) {
		if (code == ring_code [i]) {
			return (ring_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Ring Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Protection_Code
//---------------------------------------------------------

Network_Code::Protection_Type Network_Code::Protection_Code (char *text)
{
	for (int i=0; protection_text [i] != NULL; i++) {
		if (str_cmp (text, protection_text [i]) == 0) {
			return (protection_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Protection Type \"%s\"", text);
	}
	return (PERMITTED);
}

char * Network_Code::Protection_Code (Protection_Type code)
{
	for (int i=0; protection_text [i] != NULL; i++) {
		if (code == protection_code [i]) {
			return (protection_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Protection Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Detector_Code
//---------------------------------------------------------

Network_Code::Detector_Type Network_Code::Detector_Code (char *text)
{
	for (int i=0; detector_text [i] != NULL; i++) {
		if (str_cmp (text, detector_text [i]) == 0) {
			return (detector_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Detector Type \"%s\"", text);
	}
	return (PRESENCE);
}

char * Network_Code::Detector_Code (Detector_Type code)
{
	for (int i=0; detector_text [i] != NULL; i++) {
		if (code == detector_code [i]) {
			return (detector_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Detector Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Sign_Code
//---------------------------------------------------------

Network_Code::Sign_Type Network_Code::Sign_Code (char *text)
{
	for (int i=0; sign_text [i] != NULL; i++) {
		if (str_cmp (text, sign_text [i]) == 0) {
			return (sign_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Sign Type \"%s\"", text);
	}
	return (NO_SIGN);
}

char * Network_Code::Sign_Code (Sign_Type code)
{
	for (int i=0; sign_text [i] != NULL; i++) {
		if (code == sign_code [i]) {
			return (sign_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Sign Code %d", code);
	}
	return (NULL);
}
