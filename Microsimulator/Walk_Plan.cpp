//*********************************************************
//	Walk_Plan.cpp - Process a Plan Record
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Walk_Plan
//---------------------------------------------------------

void Microsimulator::Walk_Plan (Traveler_Data *traveler_ptr, Travel_Plan_Data *plan_ptr)
{
	int start, end, mode;
	start = traveler_ptr->End_Time ();

	if (plan_ptr->Leg () != 1 || start > plan_ptr->Start_Time ()) {
		end = plan_ptr->End_Time () - plan_ptr->Start_Time () + start;
	} else {
		start = plan_ptr->Start_Time ();
		end = plan_ptr->End_Time ();
	}
	traveler_ptr->End_Time (end);
	mode = plan_ptr->Mode ();

	//---- save the events ----

	if (event_output.In_Range (start, START_EVENT, mode)) {
		int offset, link, dir;

		switch (plan_ptr->Start_Type ()) {
			case PARKING_ID:
				{
					Parking_Data *ptr = parking_data [plan_ptr->Start_Index ()];
					link = ptr->Link ();
					dir = ptr->Dir ();
					offset = ptr->Offset ();
				}
				break;
			case STOP_ID:
				{
					Stop_Data *ptr = stop_data [plan_ptr->Start_Index ()];
					link = ptr->Link ();
					dir = ptr->Dir ();
					offset = ptr->Offset ();
				}
				break;
			case LOCATION_ID:
			default:
				{
					Location_Data *ptr = location_data [plan_ptr->Start_Index ()];
					link = ptr->Link ();
					dir = ptr->Dir ();
					offset = ptr->Offset ();
				}
				break;
		}
		Link_Data *link_ptr = link_data [link];

		if (offset > link_ptr->Length ()) {
			offset = link_ptr->Length ();
		} else if (offset < 0) {
			offset = 0;
		}
		event_output.Household (traveler_ptr->Traveler () / Traveler_Scale ());
		event_output.Person (traveler_ptr->Traveler () % Traveler_Scale ());
		event_output.Trip (plan_ptr->Trip ());
		event_output.Leg (plan_ptr->Leg ());
		event_output.Mode (mode);
		event_output.Event (START_EVENT);
		event_output.Schedule (plan_ptr->Start_Time ());
		event_output.Actual (start);
		if (dir > 0) {
			event_output.Link (-link_ptr->Link ());
		} else {
			event_output.Link (link_ptr->Link ());
		}
		event_output.Offset (Resolve (offset));

		event_output.Output ();
	}

	if (event_output.In_Range (end, END_EVENT, mode)) {
		int offset, link, dir;

		switch (plan_ptr->End_Type ()) {
			case PARKING_ID:
				{
					Parking_Data *ptr = parking_data [plan_ptr->End_Index ()];
					link = ptr->Link ();
					dir = ptr->Dir ();
					offset = ptr->Offset ();
				}
				break;
			case STOP_ID:
				{
					Stop_Data *ptr = stop_data [plan_ptr->End_Index ()];
					link = ptr->Link ();
					dir = ptr->Dir ();
					offset = ptr->Offset ();
				}
				break;
			case LOCATION_ID:
			default:
				{
					Location_Data *ptr = location_data [plan_ptr->End_Index ()];
					link = ptr->Link ();
					dir = ptr->Dir ();
					offset = ptr->Offset ();
				}
				break;
		}
		Link_Data *link_ptr = link_data [link];

		if (offset > link_ptr->Length ()) {
			offset = link_ptr->Length ();
		} else if (offset < 0) {
			offset = 0;
		}
		event_output.Household (traveler_ptr->Traveler () / Traveler_Scale ());
		event_output.Person (traveler_ptr->Traveler () % Traveler_Scale ());
		event_output.Trip (plan_ptr->Trip ());
		event_output.Leg (plan_ptr->Leg ());
		event_output.Mode (mode);
		event_output.Event (END_EVENT);
		event_output.Schedule (plan_ptr->End_Time ());
		event_output.Actual (end);
		if (dir > 0) {
			event_output.Link (-link_ptr->Link ());
		} else {
			event_output.Link (link_ptr->Link ());
		}
		event_output.Offset (Resolve (offset));

		event_output.Output ();
	}		
	delete plan_ptr;
}
