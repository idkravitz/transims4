//*********************************************************
//	Draw_Vehicle.cpp - draw the vehicle shape
//*********************************************************

#include "ArcSnapshot.hpp"

#include "Vehicle_Shape.hpp"

#define MAX_VEH_SEG		10

//---------------------------------------------------------
//	Draw_Vehicle
//---------------------------------------------------------

void ArcSnapshot::Draw_Vehicle (void)
{
	int i, center, link, dir, lane, type, num_pts, num;
	double offset, side, veh_width, veh_len;

	XYZ_Point pt1, pt2, pts [MAX_VEH_SEG];
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	arcview_snapshot.Get_Field (link_field, &link);
	arcview_snapshot.Get_Field (dir_field, &dir);

	if (link < 0) {
		link = -link;
		dir = 1;
	}
	link_ptr = link_data.Get (link);

	if (link_ptr == NULL) {
		Warning ("Snapshot Link %d was Not Found", link);
		return;
	}

	//---- check the subarea flag ----

	if (link_ptr->Use () == 0) return;

	arcview_snapshot.Get_Field (type_field, &type);
	if (type == 0) type = 1;

	arcview_snapshot.Get_Field (offset_field, &offset);
	arcview_snapshot.Get_Field (lane_field, &lane);

	if (shape_flag) {
		if (type >= max_type) type = 1;
		veh_len = veh_length [type] - 1.0;

		if (veh_len > offset) {
			veh_len = offset;
		}
		num_pts = (int) (veh_len / (2 * cell_size)) + 2;
		if (num_pts > MAX_VEH_SEG) num_pts = MAX_VEH_SEG;
	} else {
		veh_len = 1.5;
	}
	offset -= veh_len;

	if (arcview_snapshot.LinkDir_Type () == LINK_NODE) {		//***** from node *****
		if (dir == link_ptr->Anode ()) {
			dir = 0;
		} else if (dir == link_ptr->Bnode ()) {
			dir = 1;
		} else {
			Warning ("Snapshot Node %d not on Link %d", dir, link);
			return;
		}
	}	
	if (dir == 1) {
		offset = UnRound (link_ptr->Length ()) - offset;
	}
	if (center_flag) {
		if (link_ptr->BA_Dir () == 0) {
			dir_ptr = dir_data [link_ptr->AB_Dir ()];
			center = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right () + 1;
		} else if (link_ptr->AB_Dir () == 0) {
			dir_ptr = dir_data [link_ptr->BA_Dir ()];
			center = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right () + 1;
		} else {
			center = 1;
		}
	} else {
		center = 1;
	}
	side = (2 * lane - center) * lane_width / 2.0;

	if (shape_flag) {
		Link_Shape (link_ptr, dir, &points, offset, veh_len, side);

		veh_width = lane_width * 0.80;

		pt1 = *(points [points.Num_Points ()]);
		pt2 = *(points [1]);

		if (snapshot_flag) {
			if (num_pts > 2) {
				num = num_pts - 1;

				pts [0] = pt1;
				pts [num] = pt2;

				for (i=1; i < num; i++) {
					Link_Shape (link_ptr, dir, &points, offset, veh_len * (num - i) / num, side);

					pts [i] = *(points [points.Num_Points ()]);
				}
				Vehicle_Shape (pts, num_pts, veh_width, &arcview_snapshot.points);
			} else {
				Vehicle_Shape (pt1, pt2, veh_width, &arcview_snapshot.points);
			}
		}
	} else {
		if (snapshot_flag) {
			Link_Shape (link_ptr, dir, &points, offset, 0.0, side);

			arcview_snapshot.points.Set (1, points [1]);
		}
		if (output_flag) {
			Link_Shape (link_ptr, dir, &points, offset, veh_len, side);

			pt1 = *(points [points.Num_Points ()]);
			pt2 = *(points [1]);
		}
	}
	if (snapshot_flag && !arcview_snapshot.Write_Record ()) {
		Error ("Writing ArcView Snapshot File");
	}
	nsaved++;

	if (output_flag) {
		double dx, dy, bear;

		new_snapshot_file.Copy_Fields (&arcview_snapshot);

		dx = pt1.x - pt2.x;
		dy = pt1.y - pt2.y;
		bear = compass.Direction (dx, dy);

		if (new_snapshot_file.Dbase_Format () == VERSION3) {
			bear = 90.0 - bear;
			if (bear > 180) {
				bear = bear - 360.0;
			} else if (bear < -179.0) {
				bear = 360.0 + bear;
			}
		}
		if (project_flag) {
			project_coord.Convert (&(pt1.x), &(pt1.y));
		}
		new_snapshot_file.X (pt1.x);
		new_snapshot_file.Y (pt1.y);
		new_snapshot_file.Z (0.0);
		new_snapshot_file.Bearing (bear);

		if (!new_snapshot_file.Write ()) {
			Error ("Writing %s", new_snapshot_file.File_Type ());
		}
	}
}

