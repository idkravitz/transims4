//*********************************************************
//	Read_Equiv.cpp - Read the Link Node Equivalence File
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Read_Equiv
//---------------------------------------------------------

void LinkData::Read_Equiv (void)
{
	int link, anode, bnode;
	char *str_ptr, *ptr;

	Link_Equiv equiv;
	Link_Data data_rec;

	memset (&data_rec, '\0', sizeof (data_rec));

	Show_Message ("Reading %s -- Record", link_node.File_Type ());
	Set_Progress ();

	while (link_node.Read ()) {
		Show_Progress ();

		//---- read the field ----

		str_ptr = link_node.Record ();

		str_ptr = Get_Integer (str_ptr, &link);
		if (link == 0) continue;

		equiv.link = link;

		ptr = strchr (str_ptr, '=');
		if (ptr != NULL) {
			ptr++;
			while (*ptr == ' ') {
				ptr++;
			}
			str_ptr = ptr;
		}
		str_ptr = Get_Integer (str_ptr, &anode);
		if (anode == 0) continue;

		data_rec.link = link;
		data_rec.anode = anode;

		while (str_ptr != NULL) {
			str_ptr = Get_Integer (str_ptr, &bnode);
			if (bnode == 0) break;

			equiv.anode = anode;
			equiv.bnode = bnode;

			if (!link_equiv.Add (&equiv)) {
				Error ("Adding a Link to the Link Equiv List");
			}
			anode = bnode;
			nab++;
		}
		data_rec.bnode = anode;

		if (!link_data.Add (&data_rec)) {
			Error ("Adding a Link to the Link Data");
		}
	}
	End_Progress ();
	nequiv = Progress_Count ();

	link_node.Close ();
}
