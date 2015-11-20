//*********************************************************
//	Time_Equiv.cpp - Time Period Equivalence File
//*********************************************************

#include "Time_Equiv.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Time_Equiv constructor
//---------------------------------------------------------

Time_Equiv::Time_Equiv (void) : Data_Pointer_Array (10), Db_File ()
{
	File_Type ("Time Period Equivalence File");
}

//---------------------------------------------------------
//	Delete_Pointers
//---------------------------------------------------------

void Time_Equiv::Delete_Pointers (void)
{
	Time_Range *period;

	for (period = (Time_Range *) First (); period; period = (Time_Range *) Next ()) {
		delete period;
	}
	Data_Array::Reset ();
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Time_Equiv::Read (bool report_flag)
{
	char *str_ptr;

	Time_Range *period;

	Send_Messages (report_flag);
	report_flag = Send_Messages ();

	//---- read the time equiv ----

	if (report_flag) {
		exe->New_Page ();
		exe->Print (1, "Time Period Equivalence");
		exe->Print (1);
	}

	while (Db_File::Read ()) {

		str_ptr = Clean_Record ();
		if (*str_ptr == '\0') continue;

		period = new Time_Range ();
		if (period == NULL) return (false);

		if (!period->Add_Ranges (str_ptr)) return (false);

		if (!Add (period)) return (false);

		if (report_flag) {
			exe->Print (1, "Period %d = %s", Num_Periods (), str_ptr);
		}
	}
	Close ();

	return (true);
}

//---------------------------------------------------------
//	Period
//---------------------------------------------------------

int Time_Equiv::Period (int time)
{
	Time_Range *period;

	for (period = (Time_Range *) First (); period; period = (Time_Range *) Next ()) {
		if (period->In_Range (time)) {
			return (Record_Index ());
		}
	}
	return (0);
}
