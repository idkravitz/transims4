//*********************************************************
//	Write_Node.cpp - convert the node data
//*********************************************************

#include "VISSIMNet.hpp"

//---------------------------------------------------------
//	Write_Node
//---------------------------------------------------------

void VISSIMNet::Write_Node (void)
{
	int i, j, nlane, dir, link1, link2, num_sign, lane, num_right [20];
	char buffer [FIELD_BUFFER], dir_char, out_char, *control_type, *name;
	bool signal_type;

	Node_List *node_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ptr;
	Connect_Data *connect_ptr;
	Signal_Data *signal_ptr;
	Sign_Data *sign_ptr;
	Phasing_Data *phase_ptr;
	Pocket_Data *pocket_ptr;

	fprintf (file, "\n<NODES>");

	for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
		buffer [0] = '\0';

		//---- construct the intersection name ----

		link1 = link2 = num_sign = 0;

		for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];

			if (dir_ptr->Link () == link1 || dir_ptr->Link () == link2) continue;

			sign_ptr = sign_data.Get (dir_ptr->Link_Dir ());
			if (sign_ptr != NULL && sign_ptr->Sign () == STOP_SIGN) num_sign++;

			link_ptr = link_data.Get (dir_ptr->Link ());
			name = link_ptr->Name ();
			if (name == NULL || strlen (link_ptr->Name ()) <= 1) continue;

			if (link1 == 0) {
				link1 = link_ptr->Link ();
				str_cpy (buffer, sizeof (buffer), link_ptr->Name ());
			} else if (link2 == 0) {
				link2 = link_ptr->Link ();
				str_cat (buffer, sizeof (buffer), " and ");
				str_cat (buffer, sizeof (buffer), link_ptr->Name ());
			}
		}

		if (link2 == 0) {
			for (dir = node_ptr->From_Dir (); dir; dir = dir_ptr->From_List ()) {
				dir_ptr = dir_data [dir];
			
				if (dir_ptr->Link () == link1 || dir_ptr->Link () == link2) continue;

				link_ptr = link_data.Get (dir_ptr->Link ());
				name = link_ptr->Name ();
				if (name == NULL || strlen (link_ptr->Name ()) <= 1) continue;

				if (link1 == 0) {
					link1 = link_ptr->Link ();
					str_cpy (buffer, sizeof (buffer), link_ptr->Name ());
				} else {
					link2 = link_ptr->Link ();
					str_cat (buffer, sizeof (buffer), " and ");
					str_cat (buffer, sizeof (buffer), link_ptr->Name ());
					break;
				}
			}	
		}

		//---- node record ----

		signal_type = false;
		if (num_sign == 0) {
			signal_ptr = signal_data.Get (node_ptr->Node (), 0);
			if (signal_ptr == NULL) {
				control_type = "Unknown";
			} else {
				control_type = "Signalized";
				signal_type = true;
			}
		} else if (num_sign > 2) {
			control_type = "AllWayStop";
		} else {
			control_type = "TwoWayStop";
		}

		fprintf (file, "\n<NODE NO=\"%d\" NAME=\"%s\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" CONTROLTYPE=\"%s\">",
			node_ptr->Node (), buffer, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), control_type);

		//---- lanes in and out of the node ----

		fprintf (file, "\n<LANES>");

		//---- approach lanes ----

		memset (num_right, '\0', sizeof (num_right));

		for (j=0, dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List (), j++) {
			dir_ptr = dir_data [dir];

			link_ptr = link_data.Get (dir_ptr->Link ());
			
			nlane = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right ();

			if (dir_ptr->Dir () == 0) {
				dir_char = (link_ptr->Anode () > link_ptr->Bnode ()) ? 'B' : 'A';
			} else {
				dir_char = (link_ptr->Anode () > link_ptr->Bnode ()) ? 'A' : 'B';
			}

			sign_ptr = sign_data.Get (dir_ptr->Link_Dir ());

			if (sign_ptr != NULL && sign_ptr->Sign () == STOP_SIGN) {
				control_type = " TRAFFICSIGN=\"Stop\" ";
			} else {
				control_type = " ";
			}
			lane = 1;

			for (i=nlane; i > 0; i--) {

				if (i > (dir_ptr->Left () + dir_ptr->Thru ())) {

					//---- write right pocket lanes ----

					for (pocket_ptr = pocket_data.First (); pocket_ptr; pocket_ptr = pocket_data.Next ()) {
						if (pocket_ptr->Link_Dir () == dir_ptr->Link_Dir () && pocket_ptr->Lane () == i &&
							pocket_ptr->Type () == TURN_LANE) {

							fprintf (file, "\n<LANE LINKID=\"%d%c\" INDEX=\"%d\" POCKET=\"true\" POCKETLENGTH=\"%.1lf\" WIDTH=\"3.66\"%s/>",
								link_ptr->Link (), dir_char, lane++, UnRound (pocket_ptr->Length ()), control_type);
							num_right [j]++;
						}
					}

				} else if (i <= dir_ptr->Left ()) {

					//---- write left pocket lanes ----

					for (pocket_ptr = pocket_data.First (); pocket_ptr; pocket_ptr = pocket_data.Next ()) {
						if (pocket_ptr->Link_Dir () == dir_ptr->Link_Dir () && pocket_ptr->Lane () == i &&
							pocket_ptr->Type () == TURN_LANE) {

							fprintf (file, "\n<LANE LINKID=\"%d%c\" INDEX=\"%d\" POCKET=\"true\" POCKETLENGTH=\"%.1lf\" WIDTH=\"3.66\"%s/>",
								link_ptr->Link (), dir_char, lane++, UnRound (pocket_ptr->Length ()), control_type);
						}
					}

				} else {

					//---- write thru lanes ----

					fprintf (file, "\n<LANE LINKID=\"%d%c\" INDEX=\"%d\" POCKET=\"false\" POCKETLENGTH=\"0.0\" WIDTH=\"3.66\"%s/>",
						link_ptr->Link (), dir_char, lane++, control_type);
				}
			}
		}

		//---- departure lanes ----

		for (dir = node_ptr->From_Dir (); dir; dir = dir_ptr->From_List ()) {
			dir_ptr = dir_data [dir];

			link_ptr = link_data.Get (dir_ptr->Link ());

			nlane = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right ();

			if (dir_ptr->Dir () == 0) {
				dir_char = (link_ptr->Anode () > link_ptr->Bnode ()) ? 'B' : 'A';
			} else {
				dir_char = (link_ptr->Anode () > link_ptr->Bnode ()) ? 'A' : 'B';
			}
			lane = 1;

			for (i=nlane; i > 0; i--) {

				if (i > (dir_ptr->Left () + dir_ptr->Thru ())) {

					//---- write right pocket lanes ----

					for (pocket_ptr = pocket_data.First (); pocket_ptr; pocket_ptr = pocket_data.Next ()) {
						if (pocket_ptr->Link_Dir () == dir_ptr->Link_Dir () && pocket_ptr->Lane () == i &&
							pocket_ptr->Type () == MERGE_LANE) {

							fprintf (file, "\n<LANE LINKID=\"%d%c\" INDEX=\"%d\" POCKET=\"true\" POCKETLENGTH=\"%.1lf\" WIDTH=\"3.66\" />",
								link_ptr->Link (), dir_char, lane++, UnRound (pocket_ptr->Length ()));
						}
					}

				} else if (i <= dir_ptr->Left ()) {

					//---- write left pocket lanes ----

					for (pocket_ptr = pocket_data.First (); pocket_ptr; pocket_ptr = pocket_data.Next ()) {
						if (pocket_ptr->Link_Dir () == dir_ptr->Link_Dir () && pocket_ptr->Lane () == i &&
							pocket_ptr->Type () == MERGE_LANE) {

							fprintf (file, "\n<LANE LINKID=\"%d%c\" INDEX=\"%d\" POCKET=\"true\" POCKETLENGTH=\"%.1lf\" WIDTH=\"3.66\" />",
								link_ptr->Link (), dir_char, lane++, UnRound (pocket_ptr->Length ()));
						}
					}

				} else {

					//---- write thru lanes ----

					fprintf (file, "\n<LANE LINKID=\"%d%c\" INDEX=\"%d\" POCKET=\"false\" POCKETLENGTH=\"0.0\" WIDTH=\"3.66\" />",
						link_ptr->Link (), dir_char, lane++);
				}
			}
		}
		fprintf (file, "\n</LANES>");

		//---- lane connectivity options ----

		fprintf (file, "\n<LANETURNS>");

		//---- approach lanes ----

		for (j=0, dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List (), j++) {
			dir_ptr = dir_data [dir];

			link_ptr = link_data.Get (dir_ptr->Link ());
			
			nlane = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right () + 1;
			lane = dir_ptr->Right () - num_right [j];

			if (dir_ptr->Dir () == 0) {
				dir_char = (link_ptr->Anode () > link_ptr->Bnode ()) ? 'B' : 'A';
			} else {
				dir_char = (link_ptr->Anode () > link_ptr->Bnode ()) ? 'A' : 'B';
			}
			connect_ptr = connect_data.Get_GE (dir_ptr->Link_Dir (), 0);

			for (; connect_ptr; connect_ptr = connect_data.Next_Key ()) {
				if (connect_ptr->In_Link_Dir () != dir_ptr->Link_Dir ()) break;

				link_ptr = link_data.Get (connect_ptr->Out_Link ());

				if (connect_ptr->Out_Dir () == 0) {
					out_char = (link_ptr->Anode () > link_ptr->Bnode ()) ? 'B' : 'A';
				} else {
					out_char = (link_ptr->Anode () > link_ptr->Bnode ()) ? 'A' : 'B';
				}
				buffer [0] = ' ';
				buffer [1] = '\0';

				if (signal_type) {
					for (phase_ptr = phasing_data.First (); phase_ptr; phase_ptr = phasing_data.Next ()) {
						if (phase_ptr->Node () == node_ptr->Node () &&
							phase_ptr->In_Link_Dir () == connect_ptr->In_Link_Dir () &&
							phase_ptr->Out_Link_Dir () == connect_ptr->Out_Link_Dir ()) {

							str_fmt (buffer, sizeof (buffer), " SCNO=\"%d\" SGNO=\"%d\" ",
								phase_ptr->Timing (), phase_ptr->Phase ());
							break;
						}
					}
				}

				for (i=connect_ptr->In_Lane_Low (); i <= connect_ptr->In_Lane_High (); i++) {
					fprintf (file, "\n<LANETURN FROMLINKID=\"%d%c\" FROMLANEINDEX=\"%d\" TOLINKID=\"%d%c\"%s/>",
						connect_ptr->In_Link (), dir_char, nlane - i - lane, connect_ptr->Out_Link (), out_char, buffer);
				}
			}
		}

		fprintf (file, "\n</LANETURNS>");

		fprintf (file, "\n<CROSSWALKS />");

		fprintf (file, "\n</NODE>");
	}
	fprintf (file, "\n</NODES>");
}
