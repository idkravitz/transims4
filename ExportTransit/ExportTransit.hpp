//*********************************************************
//	ExportTransit.hpp - Conversion Transit Network to Nodes
//*********************************************************

#ifndef EXPORTTRANSIT_HPP
#define EXPORTTRANSIT_HPP

#include "Network_Service.hpp"
#include "Route_Header_File.hpp"
#include "Route_Nodes_File.hpp"
#include "Time_Range.hpp"
#include "Data_Range.hpp"

#define MAX_PERIODS		24

//---------------------------------------------------------
//	ExportTransit - execution class definition
//---------------------------------------------------------

class ExportTransit : public Network_Service
{
public:
	ExportTransit (void);
	virtual ~ExportTransit (void);

	virtual void Execute (void);

protected:
	
	static char * WRITE_DWELL_FIELD;
	static char * WRITE_TIME_FIELD;
	static char * WRITE_SPEED_FIELD;
	static char * MINIMUM_DWELL_TIME;
	static char * MAXIMUM_DWELL_TIME;
	static char * TRANSIT_TIME_PERIODS;
	static char * PERIOD_TRAVEL_TIMES;
	static char * SELECT_TRANSIT_MODES;
	static char * SELECT_TRANSIT_ROUTES;

	virtual void Program_Control (void);

private:

	int nroute, nnodes, num_periods, min_dwell, max_dwell;
	bool time_flag, dwell_flag, ttime_flag, speed_flag;

	Time_Range time_period;

	Route_Header_File *header_file;
	Route_Nodes_File *nodes_file;

	bool select_modes [REGIONRAIL+1];
	Data_Range select_routes;

	//---- methods ----

	void Write_Route (void);
};
#endif
