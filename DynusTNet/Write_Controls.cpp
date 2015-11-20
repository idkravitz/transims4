//*********************************************************
//	Write_Controls.cpp - convert the traffic controls
//*********************************************************

#include "DynusTNet.hpp"

#include "Integer_Array.hpp"

//---------------------------------------------------------
//	Write_Controls
//---------------------------------------------------------

void DynusTNet::Write_Controls (void)
{
	int i, j, nrec, dir, start, type, nphase, cycle, yield, nstop, no_sign, green;
	int nmajor, nminor, major [10], minor [10], anode, bnode;
	int num_in, num_out [10], approach [10], depart [10] [10];

	Integer_Sort start_time;

	Node_List *node_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ptr;
	Timing_Data *timing_ptr, timing_rec;
	Signal_Data *signal_ptr;
	Sign_Data *sign_ptr;
	Phasing_Data *phase_ptr;

	FILE *file = ctl_file.File ();

	Show_Message ("Writing %s -- Record", ctl_file.File_Type ());
	Set_Progress ();

	nrec = 0;

	//---- timing plan start times ----

	for (signal_ptr = signal_data.First_Key (); signal_ptr; signal_ptr = signal_data.Next_Key ()) {
		start = signal_ptr->Start () + 1;

		if (!start_time.Get_Index (start)) {
			if (!start_time.Add (start)) {
				Error ("Adding Signal Start Time");
			}
		}
	}
	fprintf (file, "%d\n", start_time.Num_Records ());
	nrec++;

	for (start = start_time.First_Key (); start > 0; start = start_time.Next_Key ()) {
		start--;
		fprintf (file, "%6.2lf", start / 60.0);
	}
	fprintf (file, "\n");
	nrec++;

	//---- loop through start times ----

	for (start = start_time.First_Key (); start > 0; start = start_time.Next_Key ()) {
		start--;
		
		//---- write the node records ----

		for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
			Show_Progress ();
			type = 1;
			nphase = cycle = 0;

			signal_ptr = signal_data.Get_LE (node_ptr->Node (), start);

			if (signal_ptr != NULL && signal_ptr->Node () == node_ptr->Node ()) {
				if (signal_ptr->Type () == TIMED) {
					type = 4;
				} else {
					type = 5;
				}
				timing_rec.Timing (signal_ptr->Timing ());
				timing_rec.Phase (1);

				for (timing_ptr = timing_data.Get (&timing_rec); timing_ptr; timing_ptr = timing_data.Next_Key ()) {
					if (timing_ptr->Timing () != signal_ptr->Timing ()) break;
					nphase++;
					green = (timing_ptr->Min_Green () > timing_ptr->Max_Green ()) ? timing_ptr->Min_Green () : timing_ptr->Max_Green ();
					cycle += green + timing_ptr->Yellow () + timing_ptr->Red_Clear ();
				}
			} else {
				nstop = yield = no_sign = 0;

				for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
					dir_ptr = dir_data [dir];

					sign_ptr = sign_data.Get (dir_ptr->Link_Dir ());
					
					if (sign_ptr != NULL) {
						if (sign_ptr->Sign () == STOP_SIGN) {
							nstop++;
						} else if (sign_ptr->Sign () == YIELD_SIGN) {
							yield = 1;
						} else {
							no_sign++;
						}
					}
				}
				if (nstop > 0) {
					if (no_sign == 0) {
						type = 3;
					} else {
						type = 6;
					}
				} else if (yield) {
					type = 2;
				}
			}
			fprintf (file, "%d %d %d %d\n", node_ptr->Node (), type, nphase, cycle);
			nrec++;
		}

		//---- write the phase records ----

		for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
			Show_Progress ();

			signal_ptr = signal_data.Get_LE (node_ptr->Node (), start);

			if (signal_ptr != NULL && signal_ptr->Node () == node_ptr->Node ()) {
				if (signal_ptr->Type () == TIMED) {
					type = 4;
				} else {
					type = 5;
				}
				timing_rec.Timing (signal_ptr->Timing ());
				timing_rec.Phase (1);
				cycle = 0;

				for (timing_ptr = timing_data.Get (&timing_rec); timing_ptr; timing_ptr = timing_data.Next_Key ()) {
					if (timing_ptr->Timing () != signal_ptr->Timing ()) break;

					//---- gather the phase movement data ----

					num_in = 0;
					num_out [0] = 0;
					memset (approach, '\0', sizeof (approach));

					for (phase_ptr = phasing_data.First (); phase_ptr; phase_ptr = phasing_data.Next ()) {
						if (phase_ptr->Node () == node_ptr->Node () && phase_ptr->Timing () == signal_ptr->Timing () &&
							phase_ptr->Phase () == timing_ptr->Phase ()) {

							link_ptr = link_data.Get (phase_ptr->In_Link ());

							if (phase_ptr->In_Dir () == 0) {
								anode = link_ptr->Anode ();
							} else {
								anode = link_ptr->Bnode ();
							}
							for (i=0; i < num_in; i++) {
								if (anode == approach [i]) break;
							}
							if (i >= num_in) {
								i = num_in;
								num_out [i] = 0;
								approach [num_in++] = anode;
							}

							link_ptr = link_data.Get (phase_ptr->Out_Link ());

							if (phase_ptr->Out_Dir () == 0) {
								bnode = link_ptr->Bnode ();
							} else {
								bnode = link_ptr->Anode ();
							}
							for (j=0; j < num_out [i]; j++) {
								if (bnode == depart [i] [j]) break;
							}
							if (j >= num_out [i]) {
								depart [i] [num_out [i]++] = bnode;
							}
						}
					}

					//---- save the phase timing and approach nodes ----

					green = (timing_ptr->Min_Green () > timing_ptr->Max_Green ()) ? timing_ptr->Min_Green () : timing_ptr->Max_Green ();
					if (num_in > 4) num_in = 4;

					if (type == 4) {
						fprintf (file, "%7d%7d%7d%7d%7d%7d", node_ptr->Node (), timing_ptr->Phase (),
							cycle, green, timing_ptr->Yellow () + timing_ptr->Red_Clear (), num_in);

						cycle += green + timing_ptr->Yellow () + timing_ptr->Red_Clear ();
					} else {
						if (timing_ptr->Max_Green () == 0) {
							green = timing_ptr->Min_Green () + timing_ptr->Ext_Green ();
						}
						fprintf (file, "%7d%7d%7d%7d%7d%7d", node_ptr->Node (), timing_ptr->Phase (), 
							green, timing_ptr->Min_Green (),
							timing_ptr->Yellow () + timing_ptr->Red_Clear (), num_in);
					}
					for (i=0; i < 4; i++) {
						fprintf (file, "%7d", approach [i]);
					}
					fprintf (file, "\n");
					nrec++;

					//---- phase movements ----

					for (i=0; i < num_in; i++) {

						fprintf (file, "%7d%7d%7d%7d", approach [i], node_ptr->Node (), 
							timing_ptr->Phase (), num_out [i]);

						for (j=0; j < num_out [i]; j++) {
							fprintf (file, "%7d", depart [i] [j]);
						}
						fprintf (file, "\n");
						nrec++;
					}
				}
			}
		}
		fprintf (file, "=======Two Way Stop Signs/Yield Signs Below =======\n");

		//---- write the sign records ----

		for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
			Show_Progress ();

			signal_ptr = signal_data.Get_LE (node_ptr->Node (), start);

			if (signal_ptr == NULL || signal_ptr->Node () != node_ptr->Node ()) {
				nstop = yield = no_sign = nmajor = nminor = 0;

				for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
					dir_ptr = dir_data [dir];

					sign_ptr = sign_data.Get (dir_ptr->Link_Dir ());
					
					if (sign_ptr != NULL) {
						if (sign_ptr->Sign () == STOP_SIGN) {
							nstop++;
							minor [nminor++] = dir;
						} else if (sign_ptr->Sign () == YIELD_SIGN) {
							yield = 1;
							minor [nminor++] = dir;
						} else {
							no_sign++;
							major [nmajor++] = dir;
						}
					} else {
						major [nmajor++] = dir;
						no_sign++;
					}
				}
				if ((nstop > 0 && no_sign == 0) || (nstop == 0 && yield == 0)) continue;
				if (nstop > 0) {
					type = 6;
				} else if (yield) {
					type = 2;
				}
				fprintf (file, "%7d%7d%7d\n", node_ptr->Node (), nmajor, nminor);
				nrec++;

				for (i=0; i < nmajor; i++) {
					dir_ptr = dir_data [major [i]];
					link_ptr = link_data.Get (dir_ptr->Link ());

					if (dir_ptr->Dir () == 0) {
						anode = link_ptr->Anode ();
						bnode = link_ptr->Bnode ();
					} else {
						anode = link_ptr->Bnode ();
						bnode = link_ptr->Anode ();
					}
					fprintf (file, "%7d%7d", anode, bnode);
				}
				fprintf (file, "\n");
				nrec++;

				for (i=0; i < nminor; i++) {
					dir_ptr = dir_data [minor [i]];
					link_ptr = link_data.Get (dir_ptr->Link ());

					if (dir_ptr->Dir () == 0) {
						anode = link_ptr->Anode ();
						bnode = link_ptr->Bnode ();
					} else {
						anode = link_ptr->Bnode ();
						bnode = link_ptr->Anode ();
					}
					fprintf (file, "%7d%7d", anode, bnode);
				}
				fprintf (file, "\n");
				nrec++;
			}
		}
	}
	End_Progress ();

	ctl_file.Close ();

	Print (2, "Number of %s Records = %d", ctl_file.File_Type(), nrec);
}
