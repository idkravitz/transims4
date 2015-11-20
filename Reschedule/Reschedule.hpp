//*********************************************************
//	Reschedule.hpp - reschedule transit routes
//*********************************************************

#ifndef RESCHEDULE_HPP
#define RESCHEDULE_HPP

#include "Demand_Service.hpp"
#include "Rider_Data.hpp"
#include "Time_Range.hpp"
#include "Time_Diff_Output.hpp"
#include "Data_Range.hpp"
#include "Random.hpp"
#include "Schedule_File.hpp"
#include "Link_Equiv.hpp"

//---------------------------------------------------------
//	Reschedule - execution class definition
//---------------------------------------------------------

class Reschedule : public Demand_Service
{
public:
	Reschedule (void);
	virtual ~Reschedule (void);

	virtual void Execute (void);

protected:

	static char * SELECT_TRANSIT_MODES;
	static char * SELECT_TRANSIT_ROUTES;
	static char * SELECT_TIME_PERIODS;
	static char * TIME_PERIOD_METHOD;
	static char * TIME_OF_DAY_FORMAT;
	static char * TRANSIT_TIME_PERIODS;
	static char * TRANSIT_TRAVEL_TIME_FACTOR;
	static char * MINIMUM_DWELL_TIME;
	static char * MAXIMUM_DWELL_TIME;
	static char * NEW_DISTRIBUTION_FILE;
	static char * RANDOM_SCHEDULE_SHIFT;
	static char * RANDOM_NUMBER_SEED;
	static char * RUN_START_TIME_FILE;
	static char * TRANSIT_SERVICE_FACTOR;
	static char * SEGMENT_SCHEDULE_FACTOR;
	static char * LINK_EQUIVALENCE_FILE;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum Reschedule_Reports { TOTAL_CHANGE = 1, PERIOD_CHANGE, PERIOD_SUM, LINK_EQUIV};
	
	enum Time_Period_Method { RUN_START = 1, RUN_MID, RUN_END };

	Time_Range time_periods, select_periods;
	Time_Diff_Output time_diff;

	bool delay_flag, turn_delay, diff_flag, rider_flag, veh_type_flag, period_flag, header_flag;
	bool service_flag, equiv_flag;
	int *min_dwell, *max_dwell, max_shift, method;
	double *time_factor, service_factor, link_factor;

	bool shift_flag, route_flag, select_modes [REGIONRAIL+1], run_flag;

	Schedule_File run_file;

	Data_Range select_routes;
	Link_Equiv link_equiv;

	Random random;

	//---- methods ----

	void Read_Run_Start (void);
	void Factor_Service (void);
	void Segment_Schedule (void);
	void Write_Schedule (void);
};

#endif
