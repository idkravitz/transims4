//*********************************************************
//	ArcTraveler.hpp - ArcView Activity/Trip/Traveler Files
//*********************************************************

#ifndef ARCTRAVELER_HPP
#define ARCTRAVELER_HPP

#include "Demand_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Utility.hpp"
#include "Problem_Service.hpp"
#include "Integer_Array.hpp"
#include "Traveler_File.hpp"
#include "Traveler_Data.hpp"

//---------------------------------------------------------
//	ArcTraveler - execution class definition
//---------------------------------------------------------

class ArcTraveler : public Demand_Service
{
public:
	ArcTraveler (void);
	virtual ~ArcTraveler (void);

	virtual void Execute (void);

protected:
	static char * TRAVELER_FILE;
	static char * TRAVELER_FORMAT;
	static char * ARCVIEW_TRAVELER_FILE;
	static char * LANE_WIDTH;
	static char * CENTER_ONEWAY_LINKS;
	static char * DRAW_NETWORK_LANES;
	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_TRAVELERS;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_SUBAREA_POLYGON;

	virtual void Program_Control (void);

private:

	Traveler_File traveler_file;
	Traveler_Array traveler_data;

	Arcview_File arcview_traveler;
	Arcview_File select_subarea;

	Point_Data points;

	double lane_width;

	bool traveler_flag, trip_flag, activity_flag;
	bool lanes_flag, center_flag, time_flag, shape_flag, subarea_flag;

	Time_Range times;
	Data_Range travelers;

	Projection_Service projection;	

	char shapename [FILE_NAME_SIZE];

	//---- methods ----

	void Read_Activity (void);
	void Read_Trip (void);
	void Read_Traveler (void);
};
#endif


