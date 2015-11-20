//*********************************************************
//	Write_Coord.cpp - convert the node coordinates
//*********************************************************

#include "DynusTNet.hpp"

//---------------------------------------------------------
//	Write_Coordinates
//---------------------------------------------------------

void DynusTNet::Write_Coordinates (void)
{
	double x, y;

	Node_List *node_ptr;

	FILE *file = xy_file.File ();

	Show_Message ("Writing %s -- Record", xy_file.File_Type ());
	Set_Progress ();

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		if (node_ptr->X () < min_x) min_x = node_ptr->X ();
		if (node_ptr->Y () < min_y) min_y = node_ptr->Y ();
	}
	for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
		Show_Progress ();

		x = UnRound (node_ptr->X () - min_x) * METERTOFEET;
		y = UnRound (node_ptr->Y () - min_y) * METERTOFEET;

		fprintf (file, "%d\t%.2lf\t%.2lf\n", node_ptr->Node (), x, y);
	}
	End_Progress ();

	xy_file.Close ();

	Print (2, "Number of %s Records = %d", xy_file.File_Type(), Progress_Count ());
}
