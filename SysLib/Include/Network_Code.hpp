//*********************************************************
//	Network_Code.hpp - network codes
//*********************************************************

#ifndef NETWORK_CODE_HPP
#define NETWORK_CODE_HPP

#include "Db_Code.hpp"

#include "System_Defines.hpp"

//---------------------------------------------------------
//	Network_Code Class definition
//---------------------------------------------------------

class Network_Code : public Db_Code
{
public:
	Network_Code (void);

	enum Facility_Type {FREEWAY = 1, EXPRESSWAY, PRINCIPAL, MAJOR, MINOR, COLLECTOR, LOCAL,
		FRONTAGE, RAMP, BRIDGE, WALKWAY, BIKEWAY, BUSWAY, LIGHTRAIL, HEAVYRAIL, FERRY, EXTERNAL};

	enum Mode_Type {NO_MODE, ALL_WALK, DRIVE_ALONE, TRANSIT, RAIL_TRANSIT, PNR_OUT, PNR_IN, 
		BICYCLE, MAGIC_MOVE, SCHOOL_BUS, CARPOOL2, CARPOOL3, CARPOOL4, KNR_OUT, KNR_IN, MAX_MODE};

	enum Use_Type {ANY, WALK, BIKE, CAR, TRUCK, BUS, RAIL,
		SOV, HOV2, HOV3, HOV4, LIGHTTRUCK, HEAVYTRUCK, TAXI, RESTRICTED};
	
	enum Transit_Type {NO_TRANSIT, LOCAL_BUS, EXPRESS_BUS, TROLLEY, STREETCAR, LRT, 
		RAPIDRAIL, REGIONRAIL, ANY_TRANSIT};

	enum Class_Type {CASH, CARD, SPECIAL};

	enum Stop_Type {STOP, STATION, EXTLOAD, NEARSIDE, FARSIDE, MIDBLOCK};

	enum Parking_Type {STREET, LOT, BOUNDARY, PARKRIDE};

	enum Restrict_Type {ONLY, REQUIRE, PROHIBIT};

	enum Pocket_Type {TURN_LANE, POCKET, MERGE_LANE};

	enum Connect_Type {NO_TYPE, THRU, R_SPLIT, L_SPLIT, R_MERGE, L_MERGE, RIGHT, LEFT, UTURN};

	enum End_Type {NODE_ID, LOCATION_ID, PARKING_ID, STOP_ID};

	enum Signal_Type {TIMED, ACTUATED};

	enum Ring_Type {NO_RING, SINGLE_RING, DUAL_RING, MAX_RING};

	enum Protection_Type {PROTECTED, PERMITTED, STOP_PERMIT};

	enum Detector_Type {PRESENCE, PASSAGE};

	enum Sign_Type {NO_SIGN, STOP_SIGN, YIELD_SIGN};

//protected:

	Facility_Type Facility_Code (char *text);
	char * Facility_Code (Facility_Type code);

	Parking_Type Parking_Code (char *text);
	char * Parking_Code (Parking_Type code);

	Restrict_Type Restrict_Code (char *text);
	char * Restrict_Code (Restrict_Type code);
	
	Pocket_Type Pocket_Code (char *text);
	char * Pocket_Code (Pocket_Type code);
	
	Connect_Type Connect_Code (char *text);
	char * Connect_Code (Connect_Type code);

	Transit_Type Transit_Code (char *text);
	char * Transit_Code (Transit_Type code);
	
	Class_Type Class_Code (char *text);
	char * Class_Code (Class_Type code);

	Stop_Type Stop_Code (char *text);
	char * Stop_Code (Stop_Type code);

	Use_Type Veh_Use_Code (char *text);
	char * Veh_Use_Code (Use_Type code);

	Use_Type Veh_Type_Map (int code);
	int Map_Veh_Type (Use_Type code);

	Use_Type Transit_Use_Map (Transit_Type code);

	End_Type End_Code (char *text);
	char * End_Code (End_Type code);

	Signal_Type Signal_Code (char *text);
	char * Signal_Code (Signal_Type code);
	
	Ring_Type Ring_Code (char *text);
	char * Ring_Code (Ring_Type code);

	Protection_Type Protection_Code (char *text);
	char * Protection_Code (Protection_Type code);

	Detector_Type Detector_Code (char *text);
	char * Detector_Code (Detector_Type code);

	Sign_Type Sign_Code (char *text);
	char * Sign_Code (Sign_Type code);

	int Use_Code (char *text);
	char * Use_Code (int code);

	bool Use_Permission (int use_code, Use_Type type)	{ return ((use_code & use_mask [type]) != 0); }

private:
	void Initialize_Use (void);

	static int use_mask [RESTRICTED+1];

	static char *facility_text [];
	static Facility_Type facility_code [];

	static char *parking_text [];
	static Parking_Type parking_code [];

	static char *restrict_text [];
	static Restrict_Type restrict_code [];

	static char *pocket_text [];
	static Pocket_Type pocket_code [];
	
	static char *connect_text [];
	static Connect_Type connect_code [];

	static char *transit_text [];
	static Transit_Type transit_code [];
	
	static char *class_text [];
	static Class_Type class_code [];

	static char *stop_text [];
	static Stop_Type stop_code [];

	static Use_Type veh_type_map [];
	static Use_Type transit_use_map [];

	static char *end_text [];
	static End_Type end_code [];

	static char *signal_text [];
	static Signal_Type signal_code [];

	static char *ring_text [];
	static Ring_Type ring_code [];

	static char *protection_text [];
	static Protection_Type protection_code [];

	static char *detector_text [];
	static Detector_Type detector_code [];

	static char *sign_text [];
	static Sign_Type sign_code [];

	static char *use_text [];
	static Use_Type use_code [];

	static char buffer [FIELD_BUFFER];
};
#endif
