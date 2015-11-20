//*********************************************************
//	Demand_Code.hpp - demand codes
//*********************************************************

#ifndef DEMAND_CODE_HPP
#define DEMAND_CODE_HPP

#include "Static_Service.hpp"

//---------------------------------------------------------
//	Demand_Code Class definition
//---------------------------------------------------------

class Demand_Code : public Static_Service
{
public:
	Demand_Code (void);
	
	enum Event_Type {START_EVENT, END_EVENT, DURATION_EVENT, MAX_EVENT};

	enum Constraint_Type { NO_CONSTRAINT, FIXED_TIME, START_TIME, END_TIME, DURATION, PASSENGER };
	
	enum Priority_Type { NON_ANCHOR, ANCHOR, PRIMARY_ANCHOR };

	enum Distance_Type { STRAIGHT, RIGHT_ANGLE, SIMPLE_AVERAGE, WEIGHTED_AVERAGE };
	
	enum Performance_Type { VOLUME_DATA, TTIME_DATA, VC_DATA, RATIO_DATA, SPEED_DATA,  
		ENTER_DATA, EXIT_DATA, DELAY_DATA, DENSITY_DATA, MAX_DENSITY_DATA,
		QUEUE_DATA, MAX_QUEUE_DATA, FAILURE_DATA, VMT_DATA, VHT_DATA,
		CONG_VMT_DATA, CONG_VHT_DATA, CONG_TIME_DATA };

	enum Units_Type { NO_UNITS, 
		GRAMS_PER_KILOMETER, GRAMS_PER_MILE, GRAMS_PER_HOUR, 
		POUNDS_PER_KILOMETER, POUNDS_PER_MILE, POUNDS_PER_HOUR,
		JOULES_PER_KILOMETER, JOULES_PER_MILE, JOULES_PER_HOUR,
		BTUS_PER_KILOMETER, BTUS_PER_MILE, BTUS_PER_HOUR };
	
protected:
	
	Event_Type Event_Code (char *text);
	char * Event_Code (Event_Type code);

	Constraint_Type Constraint_Code (char *text);
	char * Constraint_Code (Constraint_Type code);

	Priority_Type Priority_Code (char *text);
	char * Priority_Code (Priority_Type code);

	Distance_Type Distance_Code (char *text);
	char * Distance_Code (Distance_Type code);

	Performance_Type Performance_Code (char *field);
	char * Performance_Code (Performance_Type code);
	
	Units_Type Units_Code (char *text);
	char * Units_Code (Units_Type code);

private:

	static char *event_text [];
	static Event_Type event_code [];

	static char *constraint_text [];
	static Constraint_Type constraint_code [];

	static char *priority_text [];
	static Priority_Type priority_code [];

	static char *distance_text [];
	static Distance_Type distance_code [];

	static char *performance_text [];
	static Performance_Type performance_code [];

	static char *units_text [];
	static Units_Type units_code [];
};
#endif
