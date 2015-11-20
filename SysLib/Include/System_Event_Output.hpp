//*********************************************************
//	System_Event_Output.hpp - System Event Output Interface
//*********************************************************

#ifndef SYSTEM_EVENT_OUTPUT_HPP
#define SYSTEM_EVENT_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Data_Array.hpp"
#include "System_Event_File.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	System_Event_Output Class definition
//---------------------------------------------------------

class System_Event_Output : public Static_Service
{
public:
	System_Event_Output (void);
	virtual ~System_Event_Output (void);

	enum Event_Type {PHASE_CHANGE, TIMING_CHANGE, MAX_SYSTEM};

	void Add_Keys (void);

	void Read_Control (int steps_per_second);

	bool In_Range (int step, Event_Type type, int node);

	void Output (void);

	void Close (void);

	void Time (int value)				{ time = value; }
	void Event (Event_Type value)		{ type = value; }
	void Node (int value)				{ node = value; }
	void Plan (int value)				{ plan = value; }
	void Phase (int value)				{ phase = value; }
	void Ring (int value)				{ ring = value; }
	void Group (int value)				{ group = value; }
	void Status (char * value)			{ str_cpy (status, sizeof (status), value); }

private:
	static char * OUTPUT_SYSTEM_EVENT_TYPE_x;
	static char * OUTPUT_SYSTEM_EVENT_FILE_x;
	static char * OUTPUT_SYSTEM_EVENT_FORMAT_x;
	static char * OUTPUT_SYSTEM_EVENT_TIME_FORMAT_x;
	static char * OUTPUT_SYSTEM_EVENT_TIME_RANGE_x;
	static char * OUTPUT_SYSTEM_EVENT_NODE_RANGE_x;

	typedef struct {
		bool type [MAX_SYSTEM+1];
		System_Event_File *file;
		Time_Range *time_range;
		Data_Range *node_range;
	} Output_Data;

	Event_Type Event_Code (char *text);
	char * Event_Code (Event_Type code);

	Data_Array output_data;

	bool output_flag [MAX_SYSTEM+1];

	Event_Type type;
	int time, node, plan, phase, ring, group;
	char status [24];

	static char *event_text [];
	static Event_Type event_code [];
};

#endif
