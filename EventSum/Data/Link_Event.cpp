//*********************************************************
//	Link_Event.cpp - link event data
//*********************************************************

#include "Link_Event.hpp"

//---------------------------------------------------------
//	Link_Event constructor
//---------------------------------------------------------

Link_Event::Link_Event (int link) : Class2_Index ()
{
	Link (link);
	Period (0);
	Plan_Start (0);
	Started (0);
	Start_Diff ();
	Plan_End (0);
	Ended (0);
	End_Diff (0);
}

//---------------------------------------------------------
//	Link_Event_Array constructor
//---------------------------------------------------------

Link_Event_Array::Link_Event_Array (int max_records) : 
	Class2_Array (sizeof (Link_Event), max_records)
{
}

