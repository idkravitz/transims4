//*********************************************************
//	Demand_Code.cpp - demand codes
//*********************************************************

#include "Demand_Code.hpp"

#include "Utility.hpp"

//---- event type codes ----

char * Demand_Code::event_text [] = {
	"START_TIME", "END_TIME", "RUN_TIME", NULL
};

Demand_Code::Event_Type  Demand_Code::event_code [] = {
	START_EVENT, END_EVENT, DURATION_EVENT
};

//---- constraint type codes ----

char * Demand_Code::constraint_text [] = {
	"NONE", "FIXED", "START", "END", "DURATION", "PASSENGER", 
	"NO_CONSTRAINT", "FIXED_TIME", "START_TIME", "END_TIME", NULL
};

Demand_Code::Constraint_Type  Demand_Code::constraint_code [] = {
	NO_CONSTRAINT, FIXED_TIME, START_TIME, END_TIME, DURATION, PASSENGER,
	NO_CONSTRAINT, FIXED_TIME, START_TIME, END_TIME
};

//---- priority type codes ----

char * Demand_Code::priority_text [] = {
	"NON_ANCHOR", "ANCHOR", "PRIMARY_ANCHOR", NULL
};

Demand_Code::Priority_Type  Demand_Code::priority_code [] = {
	NON_ANCHOR, ANCHOR, PRIMARY_ANCHOR 
};

//---- distance type codes ----

char * Demand_Code::distance_text [] = {
	"STRAIGHT_LINE", "RIGHT_ANGLE", "SIMPLE_AVERAGE", "WEIGHTED_AVERAGE", NULL
};

Demand_Code::Distance_Type  Demand_Code::distance_code [] = {
	STRAIGHT, RIGHT_ANGLE, SIMPLE_AVERAGE, WEIGHTED_AVERAGE 
};

//---- performance type codes ----

char * Demand_Code::performance_text [] = {
	"VOLUME", "TRAVEL_TIME", "VC_RATIO", "TIME_RATIO", "SPEED",  
	"VOLUME_IN", "VOLUME_OUT", "DELAY", "DENSITY", "MAX_DENSITY",
	"QUEUE", "MAX_QUEUE", "CYCLE_FAILURE", "VEH_METERS", "VEH_SECONDS",  
	"ENTER", "EXIT", "AVG_DENSITY", "AVERAGE_DENSITY", "MAXIMUM_DENSITY",
	"AVG_QUEUE", "AVERAGE_QUEUE", "MAXIMUM_QUEUE", "VEHICLE_METERS",
	"VEHICLE_SECONDS", "CONGESTED_VMT", "CONGESTED_VHT", "CONGESTED_TIME", NULL
};

Demand_Code::Performance_Type  Demand_Code::performance_code [] = {
	VOLUME_DATA, TTIME_DATA, VC_DATA, RATIO_DATA, SPEED_DATA,  
	ENTER_DATA, EXIT_DATA, DELAY_DATA, DENSITY_DATA, MAX_DENSITY_DATA,
	QUEUE_DATA, MAX_QUEUE_DATA, FAILURE_DATA, VMT_DATA, VHT_DATA,
	ENTER_DATA, EXIT_DATA, DENSITY_DATA, DENSITY_DATA, MAX_DENSITY_DATA,
	QUEUE_DATA, QUEUE_DATA, MAX_QUEUE_DATA, VMT_DATA, VHT_DATA,
	CONG_VMT_DATA, CONG_VHT_DATA, CONG_TIME_DATA
};

//---- units type codes ----

char * Demand_Code::units_text [] = {
	"GRAMS_PER_KILOMETER", "GRAMS_PER_MILE", "GRAMS_PER_HOUR", 
	"POUNDS_PER_KILOMETER", "POUNDS_PER_MILE", "POUNDS_PER_HOUR",
	"JOULES_PER_KILOMETER", "JOULES_PER_MILE", "JOULES_PER_HOUR",
	"BTUS_PER_KILOMETER", "BTUS_PER_MILE", "BTUS_PER_HOUR", NULL
};

Demand_Code::Units_Type  Demand_Code::units_code [] = {
		GRAMS_PER_KILOMETER, GRAMS_PER_MILE, GRAMS_PER_HOUR, 
		POUNDS_PER_KILOMETER, POUNDS_PER_MILE, POUNDS_PER_HOUR,
		JOULES_PER_KILOMETER, JOULES_PER_MILE, JOULES_PER_HOUR,
		BTUS_PER_KILOMETER, BTUS_PER_MILE, BTUS_PER_HOUR
};

//---------------------------------------------------------
//	Demand_Code constructor
//---------------------------------------------------------

Demand_Code::Demand_Code (void) : Static_Service ()
{
}

//---------------------------------------------------------
//	Event_Code
//---------------------------------------------------------

Demand_Code::Event_Type  Demand_Code::Event_Code (char *text)
{
	for (int i=0; event_text [i] != NULL; i++) {
		if (str_cmp (text, event_text [i]) == 0) {
			return (event_code [i]);
		}
	}
	exe->Error ("Converting Event Type %s", text);

	return (START_EVENT);
}

char * Demand_Code::Event_Code (Demand_Code::Event_Type code)
{
	for (int i=0; event_text [i] != NULL; i++) {
		if (code == event_code [i]) {
			return (event_text [i]);
		}
	}
	exe->Error ("Converting Event Code %d", code);

	return (NULL);
}

//---------------------------------------------------------
//	Constraint_Code
//---------------------------------------------------------

Demand_Code::Constraint_Type  Demand_Code::Constraint_Code (char *text)
{
	for (int i=0; constraint_text [i] != NULL; i++) {
		if (str_cmp (text, constraint_text [i]) == 0) {
			return (constraint_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Constraint Type \"%s\"", text);
	}
	return (NO_CONSTRAINT);
}

char * Demand_Code::Constraint_Code (Demand_Code::Constraint_Type code)
{
	for (int i=0; constraint_text [i] != NULL; i++) {
		if (code == constraint_code [i]) {
			return (constraint_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Constraint Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Priority_Code
//---------------------------------------------------------

Demand_Code::Priority_Type  Demand_Code::Priority_Code (char *text)
{
	for (int i=0; priority_text [i] != NULL; i++) {
		if (str_cmp (text, priority_text [i]) == 0) {
			return (priority_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Priority Type \"%s\"", text);
	}
	return (NON_ANCHOR);
}

char * Demand_Code::Priority_Code (Demand_Code::Priority_Type code)
{
	for (int i=0; priority_text [i] != NULL; i++) {
		if (code == priority_code [i]) {
			return (priority_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Priority Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Distance_Code
//---------------------------------------------------------

Demand_Code::Distance_Type  Demand_Code::Distance_Code (char *text)
{
	for (int i=0; distance_text [i] != NULL; i++) {
		if (str_cmp (text, distance_text [i]) == 0) {
			return (distance_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Distance Type \"%s\"", text);
	}
	return (STRAIGHT);
}

char * Demand_Code::Distance_Code (Demand_Code::Distance_Type code)
{
	for (int i=0; distance_text [i] != NULL; i++) {
		if (code == distance_code [i]) {
			return (distance_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Distance Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Performance_Code
//---------------------------------------------------------

Demand_Code::Performance_Type  Demand_Code::Performance_Code (char *text)
{
	for (int i=0; performance_text [i] != NULL; i++) {
		if (str_cmp (text, performance_text [i]) == 0) {
			return (performance_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Performance Type \"%s\"", text);
	}
	return (VOLUME_DATA);
}

char * Demand_Code::Performance_Code (Demand_Code::Performance_Type code)
{
	for (int i=0; performance_text [i] != NULL; i++) {
		if (code == performance_code [i]) {
			return (performance_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Performance Code %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Units_Code
//---------------------------------------------------------

Demand_Code::Units_Type Demand_Code::Units_Code (char *text)
{
	for (int i=0; units_text [i] != NULL; i++) {
		if (str_cmp (text, units_text [i]) == 0) {
			return (units_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Units Type \"%s\"", text);
	}
	return (NO_UNITS);
}

char * Demand_Code::Units_Code (Demand_Code::Units_Type code)
{
	for (int i=0; units_text [i] != NULL; i++) {
		if (code == units_code [i]) {
			return (units_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Units Code %d", code);
	}
	return (NULL);
}
