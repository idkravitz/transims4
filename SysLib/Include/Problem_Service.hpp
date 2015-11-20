//********************************************************* 
//	Problem_Service.hpp - Summarize Problem Messages
//*********************************************************

#ifndef PROBLEM_SERVICE_HPP
#define PROBLEM_SERVICE_HPP

#include "Static_Service.hpp"

//---------------------------------------------------------
//	Problem_Service Class definition
//---------------------------------------------------------

class  Problem_Service : public Static_Service
{
public:
	enum Problem_Type {TOTAL_PROBLEM, PATH_PROBLEM, TIME_PROBLEM, NODE_PROBLEM, 
		TYPE_PROBLEM, DIST_PROBLEM, MODE_PROBLEM, ACCESS_PROBLEM, WALK_PROBLEM, 
		WAIT_PROBLEM, LINK_PROBLEM, SIZE_PROBLEM, PARK_PROBLEM, BIKE_PROBLEM, 
		DEPARTURE_PROBLEM, ARRIVAL_PROBLEM, LINK_ACCESS_PROBLEM, CONNECT_PROBLEM, 
		PARKING_PROBLEM, MERGE_PROBLEM, LANE_PROBLEM, TURN_PROBLEM, POCKET_PROBLEM, 
		SPACING_PROBLEM, CONTROL_PROBLEM, USE_PROBLEM, STOP_PROBLEM, 
		LOCATION_PROBLEM, PASSENGER_PROBLEM, DURATION_PROBLEM, KISS_PROBLEM, 
		VEHICLE_PROBLEM, SORT_PROBLEM, WALK_LOC_PROBLEM, BIKE_LOC_PROBLEM,
		TRANSIT_LOC_PROBLEM, MATCH_PROBLEM, BOARDING_PROBLEM, DWELL_PROBLEM,
		MAX_PROBLEM};

	Problem_Service (int max_problems = 0);
	~Problem_Service (void);

	void Max_Problems (int _max_problems)	{ max_problems = _max_problems; };
	int  Max_Problems (void)				{ return (max_problems); };

	int  Num_Problems (void)				{ return (num_problem [TOTAL_PROBLEM]); };

	int  Set_Problem (Problem_Type problem);

	void Report_Problems (void);

	Problem_Type Problem_Code (char *text);
	char * Problem_Code (Problem_Type code);

private:
	int	  max_problems;
	int   num_problem [MAX_PROBLEM];

	static char *problem_text [];
	static Problem_Type problem_code [];
};

#endif
