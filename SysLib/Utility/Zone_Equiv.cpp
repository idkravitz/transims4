//*********************************************************
//	Zone_Equiv.cpp - Process Zone Equivalence Files
//*********************************************************

#include "Zone_Equiv.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Zone_Equiv constructor
//---------------------------------------------------------

Zone_Equiv::Zone_Equiv (void) : Index_Array (sizeof (Group_Data), 10), Db_File ()
{
	num_zone = 0;
	num_warning = 0;
	num_missing = 0;

	zone_group.Data_Size (sizeof (Zone_Map));

	File_Type ("Zone Equivalence File");
}

//---------------------------------------------------------
//	Zone_Equiv destructor
//---------------------------------------------------------

Zone_Equiv::~Zone_Equiv (void)
{
	Group_Data *group;

	for (group = (Group_Data *) First (); group != NULL; group = (Group_Data *) Next ()) {
		if (group->label != NULL) {
			delete group->label;
		}
	}
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Zone_Equiv::Read (bool report_flag)
{
	int num, len, count;
	double low, high;
	char *ptr, buffer [32];
	bool first;

	Group_Data *group;
	Zone_Map *zone;

	Send_Messages (report_flag);
	report_flag = Send_Messages ();

	//---- read the zone equiv ----

	if (report_flag) {
		exe->New_Page ();
		exe->Print (1, "Zone Equivalence");
		exe->Print (1);
	}
	first = false;
	count = 0;

	while (Db_File::Read ()) {

		ptr = Clean_Record ();

		if (*ptr == '\0') continue;

		ptr = Get_Integer (ptr, &num);

		if (num == 0) continue;

		//---- check for a new group ----

		group = (Group_Data *) Get (&num);

		if (group == NULL) {
			group = (Group_Data *) New_Record (true);
			group->group = num;
			Add ();
			first = true;
		} else {
			first = false;
		}

		//---- check for a label record ----

		ptr = Get_Integer (ptr, &num);

		if (num == 0) {
			while (*ptr == ' ' || *ptr == '\t') ptr++;

			len = (int) (strlen (ptr)) + 1;

			if (group->label != NULL) {
				delete [] group->label;
			}
			group->label = new char [len];
			
			if (group->label != NULL) {
				str_cpy (group->label, len, ptr);
			}
		}

		//---- create a default label ----

		if (group->label == NULL) {
			str_fmt (buffer, sizeof (buffer), "Zone Group %d", group->group);

			len = (int) (strlen (buffer)) + 1;
			
			group->label = new char [len];
			
			if (group->label != NULL) {
				str_cpy (group->label, len, buffer);
			}
		}

		if (report_flag && first) {
			exe->Print (1, "[%-25.25s] %3d = ", group->label, group->group);
			count = 0;
		}
		if (num == 0) continue;

		//---- process the equiv ranges ----

		while (ptr != NULL) {
			ptr = Get_Range (ptr, &low, &high);

			if (low == 0.0 && high == 0.0) continue;

			if (low > high) {
				if (Send_Messages ()) {
					exe->Write (1, "Zone Range %.0lf..%.0lf is Illogical", low, high); 
				}
				return (false);
			}
			if (report_flag) {
				if (count != 0) {
					if (count == 5) {
						exe->Print (1, "%34c", BLANK);
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
				zone = (Zone_Map *) zone_group.Get (&num);

				if (zone != NULL) {
					if (Send_Messages ()) {
						exe->Warning ("Zone %d was Reassigned from Zone Group %d to %d", num, zone->group, group->group);
					}
					num_warning++;
				} else {
					zone = (Zone_Map *) zone_group.New_Record ();

					zone->group = group->group;
					zone->zone = num;

					zone_group.Add ();
				}
			}
		}
	}

	//---- scan for missing zones ----

	zone = (Zone_Map *) zone_group.Last_Key ();

	if (zone != NULL) {
		num_zone = zone->zone;
	}

	num_missing = 0;

	for (num=1; num <= num_zone; num++) {
		if (zone_group.Get (&num) == NULL) num_missing++;
	}
	return (true);	
}

//---------------------------------------------------------
//	Group_Label
//---------------------------------------------------------

char * Zone_Equiv::Group_Label (int group)
{
	Group_Data *ptr = (Group_Data *) Get (&group);

	if (ptr != NULL) {
		return (ptr->label);
	} else {
		return (NULL);
	}
}
//---------------------------------------------------------
//	Zone_Group
//---------------------------------------------------------

int Zone_Equiv::Zone_Group (int zone)
{
	Zone_Map *ptr = (Zone_Map *) zone_group.Get (&zone);

	if (ptr != NULL) {
		return (ptr->group);
	} else {
		return (0);
	}
}
