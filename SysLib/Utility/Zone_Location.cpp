//*********************************************************
//	Zone_Location.cpp - Process Zone Location Map File
//*********************************************************

#include "Zone_Location.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Zone_Location_Map constructor
//---------------------------------------------------------

Zone_Location_Map::Zone_Location_Map (void) : Index_Array (sizeof (Group_Data), 10), Db_File ()
{
	File_Type ("Zone Location Map File");
	max_zone = 0;
}

//---------------------------------------------------------
//	Zone_Location_Map destructor
//---------------------------------------------------------

Zone_Location_Map::~Zone_Location_Map (void)
{
	Group_Data *group;

	for (group = (Group_Data *) First (); group != NULL; group = (Group_Data *) Next ()) {
		if (group->list != NULL) {
			delete group->list;
		}
	}
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Zone_Location_Map::Read (bool report_flag)
{
	int num, count;
	double low, high;
	char *ptr;
	bool first, error;

	Group_Data *group;

	Send_Messages (report_flag);
	report_flag = Send_Messages ();

	//---- read the link equiv ----

	if (report_flag) {
		exe->New_Page ();
		exe->Print (1, "Zone Location Map");
		exe->Print (1);
	}
	first = error = false;
	max_zone = count = 0;

	while (Db_File::Read ()) {

		ptr = Clean_Record ();

		if (*ptr == '\0') continue;

		ptr = Get_Integer (ptr, &num);

		if (num == 0) continue;
		if (num < 0) {
			error = true;
			continue;
		}

		//---- check for a new group ----

		group = (Group_Data *) Get (&num);

		if (group == NULL) {
			group = (Group_Data *) New_Record (true);
			group->list = new Integer_List (10);
			group->zone = num;
			Add ();
			first = true;
			if (num > max_zone) max_zone = num;
		} else {
			first = false;
		}

		if (report_flag && first) {
			exe->Print (1, "%5d = ", group->zone);
			count = 0;
		}
		if (num == 0) continue;

		//---- process the equiv ranges ----

		while (ptr != NULL) {
			ptr = Get_Range (ptr, &low, &high);

			if (low == 0.0 && high == 0.0) continue;

			if (low > high) {
				if (Send_Messages ()) {
					exe->Write (1, "Location Range %.0lf..%.0lf is Illogical", low, high); 
				}
				return (false);
			}
			if (report_flag) {
				if (count != 0) {
					if (count == 10) {
						exe->Print (1, "%8", BLANK);
						count = 0;
					} else {
						exe->Print (0, ", ");
					}
				}
				count++;

				if (low != high) {
					exe->Print (0, "%.0lf..%.0lf", low, high);
				} else {
					exe->Print (0, "%.0lf", low);
				}
			}
			for (num = (int) low; num <= high; num++) {
				if (!group->list->Add (&num)) {
					if (Send_Messages ()) {
						exe->Write (2, "Insufficient Memory for Zone Location Map");
					}
					return (false);
				}
			}
		}
	}
	return (!error);
}

//---------------------------------------------------------
//	Locations
//---------------------------------------------------------

Integer_List * Zone_Location_Map::Locations (int zone)
{
	Group_Data *ptr = (Group_Data *) Get (&zone);

	if (ptr != NULL) {
		return (ptr->list);
	} else {
		return (NULL);
	}
}

//---------------------------------------------------------
//	First_Zone
//---------------------------------------------------------

int Zone_Location_Map::First_Zone (void)
{
	Group_Data *ptr = (Group_Data *) First ();

	if (ptr == NULL) {
		return (0);
	} else {
		return (ptr->zone);
	}
}

//---------------------------------------------------------
//	Next_Zone
//---------------------------------------------------------

int Zone_Location_Map::Next_Zone (void)
{
	Group_Data *ptr = (Group_Data *) Next ();

	if (ptr == NULL) {
		return (0);
	} else {
		return (ptr->zone);
	}
}
