//*********************************************************
//	Event_Output.hpp - Output Interface Class
//*********************************************************

#ifndef EVENT_OUTPUT_HPP
#define EVENT_OUTPUT_HPP

#include "Demand_Code.hpp"
#include "Data_Array.hpp"
#include "Event_File.hpp"
#include "Time_Range.hpp"
#include "Data_Range.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Event_Output Class definition
//---------------------------------------------------------

class Event_Output : public Demand_Code
{
public:
	Event_Output (void);
	virtual ~Event_Output (void);

	void Add_Keys (void);

	void Read_Control (int steps_per_second);

	bool In_Range (int step, Event_Type type, int mode);

	void Output (void);

	void Close (void);

	void Household (int value)			{ hhold = value; }
	void Person (int value)				{ person = value; }
	void Trip (int value)				{ trip = value; }
	void Leg (int value)				{ leg = value; }
	void Mode (int value)				{ mode = value; }
	void Event (Event_Type value)		{ type = value; }
	void Schedule (int value)			{ schedule = value; }
	void Actual (int value)				{ actual = value; }
	void Link (int value)				{ link = value; }
	void Offset (int value)				{ offset = value; }

private:
	static char * OUTPUT_EVENT_TYPE_x;
	static char * OUTPUT_EVENT_FILE_x;
	static char * OUTPUT_EVENT_FORMAT_x;
	static char * OUTPUT_EVENT_FILTER_x;
	static char * OUTPUT_EVENT_TIME_FORMAT_x;
	static char * OUTPUT_EVENT_TIME_RANGE_x;
	static char * OUTPUT_EVENT_MODE_RANGE_x;

	typedef struct {
		bool type [MAX_EVENT+1];
		Event_File *file;
		int filter;
		Time_Range *time_range;
		Data_Range *mode_range;
	} Output_Data;

	Data_Array output_data;

	bool output_flag [MAX_EVENT+1];

	Event_Type type;
	int hhold, person, mode, trip, leg, schedule, actual, link, offset;
};

#endif
