//*********************************************************
//	Write_Shapes.cpp - convert the shape coordinates
//*********************************************************

#include "DynusTNet.hpp"

//---------------------------------------------------------
//	Write_Shapes
//---------------------------------------------------------

void DynusTNet::Write_Shapes (void)
{
	int npts;
	double x, y;

	Node_List *node_ptr;
	AB_Data *ab_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ptr;
	Shape_Data *shape_ptr;
	XYZ *point;

	FILE *file = shp_file.File ();

	Show_Message ("Writing %s -- Record", shp_file.File_Type ());
	Set_Progress ();

	for (ab_ptr = ab_key.First_Key (); ab_ptr; ab_ptr = ab_key.Next_Key ()) {
		Show_Progress ();

		dir_ptr = dir_data [ab_ptr->dir];

		link_ptr = link_data.Get (dir_ptr->Link ());

		if (link_ptr->Shape () > 0) {
			shape_ptr = shape_data [link_ptr->Shape ()];
		} else {
			shape_ptr = NULL;
		}
		if (shape_ptr == NULL) {
			npts = 2;
		} else {
			npts = shape_ptr->Points () + 2;
		}
		fprintf (file, "%d %d %d", ab_ptr->anode, ab_ptr->bnode, npts);

		node_ptr = node_data.Get (ab_ptr->anode);

		x = UnRound (node_ptr->X () - min_x) * METERTOFEET;
		y = UnRound (node_ptr->Y () - min_y) * METERTOFEET;

		fprintf (file, " %.2lf, %.2lf", x, y);

		if (shape_ptr != NULL) {
			if (dir_ptr->Dir () == 0) {
				for (point = shape_ptr->First_Point (); point; point = shape_ptr->Next_Point ()) {

					x = UnRound (point->x - min_x) * METERTOFEET;
					y = UnRound (point->y - min_y) * METERTOFEET;

					fprintf (file, " %.2lf, %.2lf", x, y);
				}
			} else {
				for (point = shape_ptr->Last_Point (); point; point = shape_ptr->Previous_Point ()) {

					x = UnRound (point->x - min_x) * METERTOFEET;
					y = UnRound (point->y - min_y) * METERTOFEET;

					fprintf (file, " %.2lf, %.2lf", x, y);
				}
			}
		}
		node_ptr = node_data.Get (ab_ptr->bnode);

		x = UnRound (node_ptr->X () - min_x) * METERTOFEET;
		y = UnRound (node_ptr->Y () - min_y) * METERTOFEET;

		fprintf (file, " %.2lf, %.2lf\n", x, y);
	}
	End_Progress ();

	shp_file.Close ();

	Print (2, "Number of %s Records = %d", shp_file.File_Type(), Progress_Count ());
}
