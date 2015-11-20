//*********************************************************
//	Link_Equiv.cpp - Process Link Equivalence Files
//*********************************************************

#include "Link_Equiv.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Link_Equiv constructor
//---------------------------------------------------------

Link_Equiv::Link_Equiv (void) : Index_Array (sizeof (Group_Data), 10), Db_File ()
{
	File_Type ("Link Equivalence File");
	max_group = 0;
}

//---------------------------------------------------------
//	Link_Equiv destructor
//---------------------------------------------------------

Link_Equiv::~Link_Equiv (void)
{
	Group_Data *group;

	for (group = (Group_Data *) First (); group != NULL; group = (Group_Data *) Next ()) {
		if (group->label != NULL) {
			delete group->label;
		}
		if (group->list != NULL) {
			delete group->list;
		}
	}
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Link_Equiv::Read (bool report_flag)
{
	int num, len, count;
	double low, high;
	char *ptr;
	bool first, error;

	Group_Data *group;
	char buffer [40];

	Send_Messages (report_flag);
	report_flag = Send_Messages ();

	//---- read the link equiv ----

	if (report_flag) {
		exe->New_Page ();
		exe->Print (1, "Link Equivalence");
		exe->Print (1);
	}
	first = error = false;
	max_group = count = 0;

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
			group->group = num;
			Add ();
			first = true;
			if (num > max_group) max_group = num;
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
		} else if (num < 0) {
			error = true;
			continue;
		}

		//---- create a default label ----

		if (group->label == NULL) {
			str_fmt (buffer, sizeof (buffer), "Link Group %d", group->group);

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
					exe->Write (1, "Link Range %.0lf..%.0lf is Illogical", low, high); 
				}
				return (false);
			}
			if (report_flag) {
				if (count != 0) {
					if (count == 8) {
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
				if (!group->list->Add (&num)) {
					if (Send_Messages ()) {
						exe->Write (2, "Insufficient Memory for Link Group Data");
					}
					return (false);
				}
			}
		}
	}
	return (!error);
}

//---------------------------------------------------------
//	Group_Label
//---------------------------------------------------------

char * Link_Equiv::Group_Label (int group)
{
	Group_Data *ptr = (Group_Data *) Get (&group);

	if (ptr != NULL) {
		return (ptr->label);
	} else {
		return (NULL);
	}
}

//---------------------------------------------------------
//	Group_Label
//---------------------------------------------------------

Integer_List * Link_Equiv::Group_List (int group)
{
	Group_Data *ptr = (Group_Data *) Get (&group);

	if (ptr != NULL) {
		return (ptr->list);
	} else {
		return (NULL);
	}
}

//---------------------------------------------------------
//	First_Group
//---------------------------------------------------------

int Link_Equiv::First_Group (void)
{
	Group_Data *ptr = (Group_Data *) First ();

	if (ptr == NULL) {
		return (0);
	} else {
		return (ptr->group);
	}
}

//---------------------------------------------------------
//	Next_Group
//---------------------------------------------------------

int Link_Equiv::Next_Group (void)
{
	Group_Data *ptr = (Group_Data *) Next ();

	if (ptr == NULL) {
		return (0);
	} else {
		return (ptr->group);
	}
}
