//*********************************************************
//	Map_Network.cpp - map the links and nodes
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Map_Network
//---------------------------------------------------------

void Relocate::Map_Network (void)
{
	int node, x, y, best_node, xmin, xmax, ymin, ymax;
	int nnode, nlink, link, anode, bnode, *list, an, bn, use_code, index;
	int *distance, *next_node, *from_node, *link_dir, *first, *last, best_len, dist_a, dist_b;
	double dx, dy, best_dist;
	Use_Type use;
	char *str_ptr;
	bool ab_flag;

	List_Data *list_ptr;
	Map_Data map_rec, *map_ptr;

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	AB_Data *ab_ptr;
	Parking_Data *park_ptr;

	//---- read the old node file ----

	if (!node_map.Max_Records (old_node_file.Num_Records ())) {
		Error ("Insufficient Memory for Old Node Map");
	}
	Show_Message ("Reading %s -- Record", old_node_file.File_Type ());
	Set_Progress ();	

	while (old_node_file.Read ()) {
		Show_Progress ();

		map_rec.old_id = node = old_node_file.Node ();

		x = Round (old_node_file.X ());
		y = Round (old_node_file.Y ());
		
		xmin = x - MIN_DIFF;
		xmax = x + MIN_DIFF;
		ymin = y - MIN_DIFF;
		ymax = y + MIN_DIFF;

		node_ptr = node_data.Get (node);

		if (node_ptr != NULL && node_ptr->X () <= xmax && node_ptr->X () >= xmin && 
			node_ptr->Y () <= ymax && node_ptr->Y () >= ymin) {

			map_rec.new_id = node;
		} else {
			best_node = 0;
			best_dist = 0.0;

			for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {

				dx = (double) node_ptr->X () - x;
				dy = (double) node_ptr->Y () - y;

				dx = dx * dx + dy * dy;

				if (best_node == 0 || best_dist > dx) {
					best_node = node_ptr->Node ();
					best_dist = dx;
					if (best_dist == 0.0) break;
				}
			}
			map_rec.new_id = best_node;
		}
		if (!node_map.Add (&map_rec)) {
			Error ("Adding Node Map Record");
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", old_node_file.File_Type (), Progress_Count ());
	
	//---- allocate link and path building data ----

	nlink = old_link_file.Num_Records ();

	if (!old_link_map.Max_Records (nlink)) goto mem_error;
	if (!old_link_data.Max_Records (nlink)) goto mem_error;
	if (!old_ab_key.Max_Records (2 * nlink)) goto mem_error;

	nnode = node_data.Num_Records ();
	nlink = link_data.Num_Records ();

	if (!new_link_list.Num_Records (nlink)) goto mem_error;
	if (!node_list.Num_Records (nnode)) goto mem_error;
	if (!node_link.Num_Records (nlink)) goto mem_error;

	nnode++;

	distance = new int [nnode];
	next_node = new int [nnode];
	from_node = new int [nnode];
	link_dir = new int [nnode];

	if (distance == NULL || next_node == NULL || from_node == NULL, link_dir == NULL) goto mem_error;

	//---- build the link list ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		link = link_data.Record_Index ();

		list_ptr = node_link [link];

		node = node_data.Get_Index (link_ptr->Anode ());

		list = node_list.Record (node);

		list_ptr->From_List (*list);
		*list = link;

		node = node_data.Get_Index (link_ptr->Bnode ());

		list = node_list.Record (node);

		list_ptr->To_List (*list);
		*list = link;
	}

	//---- read the old link file ----

	Show_Message ("Reading %s -- Record", old_link_file.File_Type ());
	Set_Progress ();

	while (old_link_file.Read ()) {
		Show_Progress ();

		map_rec.old_id = link = old_link_file.Link ();

		anode = old_link_file.Anode ();
		bnode = old_link_file.Bnode ();

		//---- save the old link data ----

		link_ptr = old_link_data.New_Record (true);

		link_ptr->Link (link);
		link_ptr->Anode (anode);
		link_ptr->Bnode (bnode);
		link_ptr->Length (Round (old_link_file.Length ()));
		link_ptr->AB_Dir (old_link_file.Lanes_AB ());
		link_ptr->BA_Dir (old_link_file.Lanes_BA ());

		if (!old_link_data.Add ()) goto mem_error;

		if (link_ptr->AB_Dir () > 0) {
			ab_ptr = (AB_Data *) old_ab_key.New_Record (true);

			ab_ptr->anode = anode;
			ab_ptr->bnode = bnode;
			ab_ptr->dir = link;

			if (!old_ab_key.Add ()) goto mem_error;
		}
		if (link_ptr->BA_Dir () > 0) {
			ab_ptr = (AB_Data *) old_ab_key.New_Record (true);

			ab_ptr->anode = bnode;
			ab_ptr->bnode = anode;
			ab_ptr->dir = -link;

			if (!old_ab_key.Add ()) goto mem_error;
		}

		//---- convert the nodes ----

		map_ptr = (Map_Data *) node_map.Get (&anode);
		anode = map_ptr->new_id;

		map_ptr = (Map_Data *) node_map.Get (&bnode);
		bnode = map_ptr->new_id;

		//---- check for one-to-one mapping ----

		link_ptr = link_data.Get (link);

		if (link_ptr != NULL) {
			if (link_ptr->Anode () == anode && link_ptr->Bnode () == bnode) {
				map_rec.new_id = link_data.Record_Index ();

				if (!old_link_map.Add (&map_rec)) goto mem_error;
				continue;
			}
		} else {
			ab_ptr = ab_key.Get (anode, bnode);

			if (ab_ptr != NULL) {
				dir_ptr = dir_data [ab_ptr->dir];

				link_ptr = link_data.Get (dir_ptr->Link ());

				if (link_ptr->Anode () == anode && link_ptr->Bnode () == bnode) {
					map_rec.new_id = link_data.Record_Index ();

					if (!old_link_map.Add (&map_rec)) goto mem_error;
					continue;
				}
			}
		}

		//---- determine the use restrictions -----

		str_ptr = old_link_file.Use ();

		if (*str_ptr == '\0') {
			str_ptr = "ANY";
		}
		use_code = Use_Code (str_ptr);

		use = CAR;
		if (!Use_Permission (use_code, use)) {
			use = HOV3;
			if (!Use_Permission (use_code, use)) {
				use = BUS;
				if (!Use_Permission (use_code, use)) {
					use = RAIL;
					if (!Use_Permission (use_code, use)) {
						use = WALK;
						if (!Use_Permission (use_code, use)) {
							use = BIKE;
						} else {
							use = ANY;
						}
					}
				}
			}
		}

		//---- build the path ----

		an = node_data.Get_Index (anode);
		bn = node_data.Get_Index (bnode);

		memset (distance, '\0', nnode * sizeof (int));
		memset (next_node, '\0', nnode * sizeof (int));
		memset (from_node, '\0', nnode * sizeof (int));
		memset (link_dir, '\0', nnode * sizeof (int));

		first = next_node;
		*first = an;
		last = next_node + an;

		best_node = an;
		best_len = Round (old_link_file.Length () * 1.25);

		for (;;) {
			an = *first;
			if (an <= 0) break;

			*first = -1;
			first = next_node + an;

			//---- check the distance ----

			dist_a = distance [an];

			if (dist_a >= best_len) continue;

			//---- process each link leaving the node ----

			for (link = node_list [an]; link; link = list_ptr->Next (ab_flag)) {

				list_ptr = node_link [link];

				link_ptr = link_data [link];

				node = node_data.Get_Index (link_ptr->Bnode ());

				ab_flag = (node != an);

				if (!Use_Permission (link_ptr->Use (), use)) continue;

				if (!ab_flag) {
					node = node_data.Get_Index (link_ptr->Anode ());
					link = -link;
				}

				//---- check the cumulative distance ----

				dist_b = dist_a + link_ptr->Length ();
				if (dist_b > best_len) continue;

				if (dist_b >= distance [node] && distance [node] > 0) continue;

				if (node == bn) {
					best_len = dist_b;
				}

				//---- add to the tree ----

				if (next_node [node] == -1) {
					next_node [node] = *first;
					first = next_node + node;
				} else if (next_node [node] == 0 && last != next_node + node) {
					*last = node;
					last = next_node + node;
				}
				distance [node] = dist_b;
				from_node [node] = an;
				link_dir [node] = link;
			}
		}

		//---- trace the path ----

		an = bn;
		link = 0;

		while (an != best_node && an > 0) {
			bn = an;
			an = from_node [bn];
			if (an == 0) break;

			index = abs (link_dir [bn]);

			list = new_link_list.Record (index);
			*list = link;

			link = link_dir [bn];
		}
		map_rec.new_id = link;

		if (!old_link_map.Add (&map_rec)) goto mem_error;
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", old_link_file.File_Type (), Progress_Count ());

	//---- read the old parking file ----

	if (!old_park_data.Max_Records (old_park_file.Num_Records ())) {
		Error ("Insufficient Memory for Old Parking Datap");
	}
	Show_Message ("Reading %s -- Record", old_park_file.File_Type ());
	Set_Progress ();

	ab_flag = (old_park_file.Field_Number ("DIR") != 0);

	while (old_park_file.Read ()) {
		Show_Progress ();

		park_ptr = old_park_data.New_Record (true);

		park_ptr->ID (old_park_file.ID ());
		park_ptr->Offset (Round (old_park_file.Offset ()));

		link = old_park_file.Link ();
		node = old_park_file.Node ();

		if (link < 0) {
			link = -link;
			node = 1;
		} else if (!ab_flag) {
			link_ptr = old_link_data.Get (link);

			if (link_ptr->Anode () == node) {
				node = 1;
			} else {
				node = 0;
			}
		}
		park_ptr->Link (link);
		park_ptr->Dir (node);

		if (park_ptr->Dir () == 0) {		//---- offset from A node ----
			link_ptr = old_link_data.Get (link);
			park_ptr->Offset (link_ptr->Length () - park_ptr->Offset ());
		}

		if (!old_park_data.Add ()) {
			Error ("Adding Parking Map Record");
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", old_park_file.File_Type (), Progress_Count ());
	return;

mem_error:
	Error ("Adding Link Map Record");
}
