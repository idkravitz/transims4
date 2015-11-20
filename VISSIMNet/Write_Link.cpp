//*********************************************************
//	Write_Link.cpp - convert the link data
//*********************************************************

#include "VISSIMNet.hpp"

//---------------------------------------------------------
//	Write_Link
//---------------------------------------------------------

void VISSIMNet::Write_Link (void)
{
	int i;
	bool flip;
	XYZ *point;

	Node_List *node_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ptr;
	Shape_Data *shape_ptr;

	//---- write the facility types ----

	fprintf (file, "\n<LINKTYPES>");

	for (int type=1; type <= EXTERNAL; type++) {
		fprintf (file, "\n<LINKTYPE NO=\"%d\" NAME=\"%s\" DRIVINGBEHAVIOR=\"Urban\" />", 
			type, Facility_Code ((Facility_Type) type));
	}
	fprintf (file, "\n</LINKTYPES>");

	//---- write the link data ----

	fprintf (file, "\n<LINKS>");

	for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {
		flip = (link_ptr->Anode () > link_ptr->Bnode ());
		if (link_ptr->Shape () > 0) {
			shape_ptr = shape_data [link_ptr->Shape ()];

			if (shape_ptr == NULL) Write (1, "null shape for %d %d", link_ptr->Link (), link_ptr->Shape ());
		} else {
			shape_ptr = NULL;
		}

		//---- AB direction ----

		if (link_ptr->AB_Dir () > 0) {
			dir_ptr = dir_data [link_ptr->AB_Dir ()];

			fprintf (file, "\n<LINK ID=\"%d%c\" ", link_ptr->Link (), ((flip) ? 'B' : 'A'));

			fprintf (file, "FROMNODENO=\"%d\" TONODENO=\"%d\" NAME=\"%s\" ",
				link_ptr->Anode (), link_ptr->Bnode (), link_ptr->Name ());

			fprintf (file, "LINKTYPENO=\"%d\" SPEED=\"%.1lf\" NUMLANES=\"%d\" ",
				link_ptr->Type (), 3.6 * UnRound (dir_ptr->Speed ()), dir_ptr->Thru ());
			
			if (link_ptr->BA_Dir () > 0) {
				fprintf (file, "REVERSELINK=\"%d%c\">", link_ptr->Link (), ((flip) ? 'A' : 'B'));
			} else {
				fprintf (file, ">");
			}
			if (shape_ptr != NULL) {
				fprintf (file, "\n<LINKPOLY>");
				i = 1;
				node_ptr = node_data.Get (link_ptr->Anode ());

				fprintf (file, "\n<POINT INDEX=\"%d\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />",
					i++, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()));

				for (point = shape_ptr->First_Point (); point; point = shape_ptr->Next_Point ()) {
					fprintf (file, "\n<POINT INDEX=\"%d\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />",
						i++, UnRound (point->x), UnRound (point->y));
				}
				node_ptr = node_data.Get (link_ptr->Bnode ());

				fprintf (file, "\n<POINT INDEX=\"%d\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />",
					i++, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()));

				fprintf (file, "\n</LINKPOLY>");
			}
			fprintf (file, "\n</LINK>");
		}

		//---- BA direction ----

		if (link_ptr->BA_Dir () > 0) {
			dir_ptr = dir_data [link_ptr->BA_Dir ()];

			fprintf (file, "\n<LINK ID=\"%d%c\" ", link_ptr->Link (), ((flip) ? 'A' : 'B'));

			fprintf (file, "FROMNODENO=\"%d\" TONODENO=\"%d\" NAME=\"%s\" ",
				link_ptr->Bnode (), link_ptr->Anode (), link_ptr->Name ());

			fprintf (file, "LINKTYPENO=\"%d\" SPEED=\"%.1lf\" NUMLANES=\"%d\" ",
				link_ptr->Type (), 3.6 * UnRound (dir_ptr->Speed ()), dir_ptr->Thru ());
			
			if (link_ptr->AB_Dir () > 0) {
				fprintf (file, "REVERSELINK=\"%d%c\">", link_ptr->Link (), ((flip) ? 'B' : 'A'));
			} else {
				fprintf (file, ">");
			}
			if (shape_ptr != NULL) {
				fprintf (file, "\n<LINKPOLY>");
				i = 1;
				node_ptr = node_data.Get (link_ptr->Bnode ());

				fprintf (file, "\n<POINT INDEX=\"%d\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />",
					i++, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()));

				for (point = shape_ptr->Last_Point (); point; point = shape_ptr->Previous_Point ()) {
					fprintf (file, "\n<POINT INDEX=\"%d\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />",
						i++, UnRound (point->x), UnRound (point->y));
				}
				node_ptr = node_data.Get (link_ptr->Anode ());

				fprintf (file, "\n<POINT INDEX=\"%d\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />",
					i++, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()));

				fprintf (file, "\n</LINKPOLY>");
			}
			fprintf (file, "\n</LINK>");
		}
	}
	fprintf (file, "\n</LINKS>");
}
