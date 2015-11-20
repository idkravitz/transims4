//*********************************************************
//	RiderPrep.hpp - Prepare Transit Ridership Data
//*********************************************************

#ifndef RIDERPREP_HPP
#define RIDERPREPM_HPP

#include "Demand_Service.hpp"
#include "Ridership_File.hpp"
#include "Rider_Data.hpp"

//---------------------------------------------------------
//	RiderPrep - execution class definition
//---------------------------------------------------------

class RiderPrep : public Demand_Service
{
public:
	RiderPrep (void);
	virtual ~RiderPrep (void);

	virtual void Execute (void);

protected:

	static char * SUBAREA_RIDERSHIP_FILE;
	static char * SUBAREA_RIDERSHIP_FORMAT;
	static char * OUTPUT_ALL_TRANSIT_STOPS;
	static char * TIME_OF_DAY_FORMAT;

	virtual void Program_Control (void);

	virtual void Page_Header (void);

private:
	enum RiderPrep_Reports { LINE_RIDER = 1 };

	bool stops_flag;

	Ridership_File subarea_file;
	Time_Step trip_time;

	//---- methods ----

	void Merge_Riders (void);
	void Write_Riders (void);

	//void Line_Rider_Report (int number);
	//void Line_Rider_Header (void);

};
#endif
