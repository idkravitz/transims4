//*********************************************************
//	Merge_Delay.cpp - merge arcview delay files
//*********************************************************

#include "ArcDiff.hpp"

#include "Point_Array.hpp"
#include "Db_Field.hpp"

//---------------------------------------------------------
//	Merge_Delay
//---------------------------------------------------------

void ArcDiff::Merge_Delay (Delay_Group *arcview)
{
	int i, link, dir, key1, key2, nfield1, nfield2, len, lvalue1, lvalue2;
	int link_field1, link_field2, dir_field1, dir_field2, *map1, *map2, *map3, *map4, *type;
	int start_field1, start_field2, end_field1, end_field2, seg_field1;
	int anode_field1, anode_field2, bnode_field1, bnode_field2, seg_field2;
	double dvalue1, dvalue2, percent;
	bool flag1, flag2, binary, node_flag;
	char buffer [FIELD_BUFFER];

	Db_Field *fld;

	//---- get key fields ----

	link_field1 = arcview->delay1.Optional_Field ("LINK");
	dir_field1 = arcview->delay1.Optional_Field ("DIR", "NODE");
	seg_field1 = arcview->delay1.Optional_Field ("SEGMENT");
	anode_field1 = arcview->delay1.Optional_Field ("ANODE", "NODEA", "A", "AN");
	bnode_field1 = arcview->delay1.Optional_Field ("BNODE", "NODEB", "B", "BN");
	end_field1 = arcview->delay1.Optional_Field ("PERIOD", "TIME");
	if (end_field1 == 0) {
		start_field1 = arcview->delay1.Optional_Field ("START", "START_TIME", "STARTTIME");
		end_field1 = arcview->delay1.Optional_Field ("END", "END_TIME", "ENDTIME", "TIME");
	} else {
		start_field1 = 0;
	}

	link_field2 = arcview->delay2.Optional_Field ("LINK");
	dir_field2 = arcview->delay2.Optional_Field ("DIR", "NODE");
	seg_field2 = arcview->delay2.Optional_Field ("SEGMENT");
	anode_field2 = arcview->delay2.Optional_Field ("ANODE", "NODEA", "A", "AN");
	bnode_field2 = arcview->delay2.Optional_Field ("BNODE", "NODEB", "B", "BN");
	end_field2 = arcview->delay2.Optional_Field ("PERIOD", "TIME");
	if (end_field2 == 0) {
		start_field2 = arcview->delay2.Optional_Field ("START", "START_TIME", "STARTTIME");
		end_field2 = arcview->delay2.Optional_Field ("END", "END_TIME", "ENDTIME", "TIME");
	} else {
		start_field2 = 0;
	}

	//---- check the key type ----

	if (link_field1 == 0 && link_field2 == 0) {
		node_flag = true;
	} else if (link_field1 == 0 || link_field2 == 0) {
		Error ("ArcView Files 1 and 2 have Incompatible Keys");
	} else {
		node_flag = false;
	}

	//----- create the fields ----

	nfield1 = arcview->delay1.Num_Fields ();
	binary = (arcview->delay1.Record_Format () == Db_Code::BINARY);

	for (i=1; i <= nfield1; i++) {
		fld = arcview->delay1.Field (i);

		arcview->diff.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
	}

	nfield2 = arcview->delay2.Num_Fields ();
	binary = (arcview->delay2.Record_Format () == Db_Code::BINARY);

	map1 = new int [nfield2+1];
	map2 = new int [nfield2+1];
	map3 = new int [nfield2+1];
	map4 = new int [nfield2+1];
	type = new int [nfield2+1];

	for (i=1; i <= nfield2; i++) {
		map1 [i] = map2 [i] = 0;

		if (i == link_field2 || i == dir_field2 || i == start_field2 || i == end_field2 ||
			i == anode_field2 || i == bnode_field2 || i == seg_field2) continue;

		fld = arcview->delay2.Field (i);

		map1 [i] = arcview->diff.Field_Number (fld->Name ());

		str_cpy (buffer, sizeof (buffer), fld->Name ());

		len = (int) strlen (buffer);
		if (len > 9) len = 9;
		buffer [len] = '2';
		buffer [len+1] = '\0';

		arcview->diff.Add_Field (buffer, fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
		map2 [i] = arcview->diff.Num_Fields ();

		if (fld->Type () == Db_Code::INTEGER) {
			type [i] = 1;
		} else if (fld->Type () == Db_Code::DOUBLE) {
			type [i] = 2;
		} else {
			type [i] = 0;
		}
	}

	for (i=1; i <= nfield2; i++) {
		map3 [i] = map4 [i] = 0;

		if (map2 [i] == 0 || type [i] == 0) continue;

		fld = arcview->delay2.Field (i);

		str_cpy (buffer, sizeof (buffer), fld->Name ());

		len = (int) strlen (buffer);
		if (len > 9) len = 9;
		buffer [len] = '3';
		buffer [len+1] = '\0';

		arcview->diff.Add_Field (buffer, fld->Type (), fld->Size (), fld->Decimal (), -1, binary);	
		map3 [i] = arcview->diff.Num_Fields ();

		if (percent_flag) {
			buffer [len] = '4';

			arcview->diff.Add_Field (buffer, Db_Code::DOUBLE, 12, 1);
			map4 [i] = arcview->diff.Num_Fields ();
		}
	}
	arcview->diff.Write_Header ();

	//---- merge the files ----

	if (arcview->group_num > 0) {
		Show_Message ("Merging Arcview Files #%d -- Record", arcview->group_num);
	} else {
		Show_Message ("Merging Arcview Files -- Record");
	}
	Set_Progress ();

	key1 = key2 = 0;
	flag1 = flag2 = false;

	//---- read the first record in the delay file 1 ----

	if (arcview->delay1.Read_Record ()) {
		arcview->delay1.Get_Field (dir_field1, &dir);

		if (node_flag) {
			key1 = dir;
		} else {
			arcview->delay1.Get_Field (link_field1, &link);
			key1 = (link << 1) + dir;
		}
	} else {
		key1 = MAX_INTEGER;
	}

	//---- read the first record in the delay file 2 ----

	if (arcview->delay2.Read_Record ()) {
		arcview->delay2.Get_Field (dir_field2, &dir);

		if (node_flag) {
			key2 = dir;
		} else {
			arcview->delay2.Get_Field (link_field2, &link);
			key2 = (link << 1) + dir;
		}
	} else {
		key2 = MAX_INTEGER;
	}

	//---- interleaf plan records based on traveler ID ----

	for (;;) {
		if (key1 == MAX_INTEGER && key2 == MAX_INTEGER) break;

		Show_Progress ();

		arcview->diff.parts.Reset ();
		arcview->diff.points.Reset ();
		arcview->diff.Reset_Record ();

		//---- set the processing flags ----

		if (key2 < key1) {
			flag1 = false;
			flag2 = true;
		} else if (key2 > key1) {
			flag1 = true;
			flag2 = false;
		} else {
			flag1 = flag2 = true;
		}

		//---- process file 1 ----

		if (flag1) {

			arcview->diff.Copy_Fields (&arcview->delay1);

			for (i=1; i <= arcview->delay1.Num_Parts (); i++) {
				if (!arcview->diff.parts.Add (arcview->delay1.parts [i])) goto point_error;
			}
			for (i=1; i <= arcview->delay1.Num_Points (); i++) {
				if (!arcview->diff.points.Add (arcview->delay1.points [i])) goto point_error;
			}

			//---- read the next record in delay file #1 ----

			if (arcview->delay1.Read_Record ()) {
				arcview->delay1.Get_Field (dir_field1, &dir);

				if (node_flag) {
					key1 = dir;
				} else {
					arcview->delay1.Get_Field (link_field1, &link);
					key1 = (link << 1) + dir;
				}
			} else {
				key1 = MAX_INTEGER;
			}
		}

		//---- process file 2 ----

		if (flag2) {

			if (!flag1) {

				//---- add header fields ----

				arcview->delay2.Get_Field (link_field2, &lvalue2);
				arcview->diff.Put_Field (link_field1, lvalue2);

				arcview->delay2.Get_Field (dir_field2, &lvalue2);
				arcview->diff.Put_Field (dir_field1, lvalue2);

				arcview->delay2.Get_Field (start_field2, buffer);
				arcview->diff.Put_Field (start_field1, buffer);

				arcview->delay2.Get_Field (end_field2, buffer);
				arcview->diff.Put_Field (end_field1, buffer);
				
				//---- add shape points ----

				for (i=1; i <= arcview->delay2.Num_Parts (); i++) {
					if (!arcview->diff.parts.Add (arcview->delay2.parts [i])) goto point_error;
				}
				for (i=1; i <= arcview->delay2.Num_Points (); i++) {
					if (!arcview->diff.points.Add (arcview->delay2.points [i])) goto point_error;
				}
			}

			//---- add file 2 fields ----

			for (i=1; i <= nfield2; i++) {
				if (map2 [i] == 0) continue;

				if (type [i] == 1) {
					arcview->delay2.Get_Field (i, &lvalue2);
					arcview->diff.Put_Field (map2 [i], lvalue2);
				} else if (type [i] == 2) {
					arcview->delay2.Get_Field (i, &dvalue2);
					arcview->diff.Put_Field (map2 [i], dvalue2);
				} else {
					arcview->delay2.Get_Field (i, buffer);
					arcview->diff.Put_Field (map3 [i], buffer);
				}
				if (percent_flag) {
					arcview->diff.Put_Field (map4 [i], 100.0);
				}
			}

			//---- read the next record in the delay file 2 ----

			if (arcview->delay2.Read_Record ()) {
				arcview->delay2.Get_Field (dir_field2, &dir);

				if (node_flag) {
					key2 = dir;
				} else {
					arcview->delay2.Get_Field (link_field2, &link);
					key2 = (link << 1) + dir;
				}
			} else {
				key2 = MAX_INTEGER;
			}
		}

		//---- calculate the difference ----

		for (i=1; i <= nfield2; i++) {
			if (map2 [i] == 0) continue;

			if (type [i] == 1) {
				arcview->diff.Get_Field (map1 [i], &lvalue1);
				arcview->diff.Get_Field (map2 [i], &lvalue2);
				arcview->diff.Put_Field (map3 [i], (lvalue1 - lvalue2));

				if (percent_flag) {
					if (lvalue2 > 0) {
						percent = 100.0 * (lvalue1 - lvalue2) / lvalue2;
					} else {
						percent = 100.0;
					}
					arcview->diff.Put_Field (map4 [i], percent);
				}
			} else if (type [i] == 2) {
				arcview->diff.Get_Field (map1 [i], &dvalue1);
				arcview->diff.Get_Field (map2 [i], &dvalue2);
				arcview->diff.Put_Field (map3 [i], (dvalue1 - dvalue2));

				if (percent_flag) {
					if (dvalue2 > 0.0) {
						percent = 100.0 * (dvalue1 - dvalue2) / dvalue2;
					} else {
						percent = 100.0;
					}
					arcview->diff.Put_Field (map4 [i], percent);
				}
			}
		}

		//---- save the diff record ----

		if (!arcview->diff.Write_Record ()) {
			Error ("Writing %s", arcview->diff.File_Type ());
		}
	}
	End_Progress ();

	arcview->delay1.Close ();
	arcview->delay2.Close ();
	arcview->diff.Close ();

	delete [] map1;
	delete [] map2;
	delete [] map3;
	delete [] type;
	return;

point_error:
	Error ("Insufficient Memory for Link Shape Points");
	return;
}
