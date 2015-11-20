//*********************************************************
//	Read_SpdCap.cpp - convert the speed-capacity file
//*********************************************************

#include "TPPlusNet.hpp"

//---------------------------------------------------------
//	Read_SpdCap
//---------------------------------------------------------

void TPPlusNet::Read_SpdCap (void)
{
	int at, at1, at2, fc, fc1, fc2, ln, ln1, ln2, lvalue; 
	int at1_fld, at2_fld, fc1_fld, fc2_fld, ln1_fld, ln2_fld, cap_fld, cap_flg, spd_fld, spd_flg;
	char cap_flag [10], spd_flag [10];
	double cap, spd, dvalue;
	bool flag = units_flag;

	//---- initialize the data fields ----

	if (spdcap_file.Num_Fields () == 0) {
		spdcap_file.Add_Field ("FROM_AT", INTEGER, 2);
		spdcap_file.Add_Field ("TO_AT", INTEGER, 2);
		spdcap_file.Add_Field ("FROM_FC", INTEGER, 2);
		spdcap_file.Add_Field ("TO_FC", INTEGER, 2);
		spdcap_file.Add_Field ("FROM_LANE", INTEGER, 2);
		spdcap_file.Add_Field ("TO_LANE", INTEGER, 2);
		spdcap_file.Add_Field ("CAP_FLAG", CHAR, 1);
		spdcap_file.Add_Field ("CAPACITY", DOUBLE, 5);
		spdcap_file.Add_Field ("SPD_FLAG", CHAR, 1);
		spdcap_file.Add_Field ("SPEED", DOUBLE, 4);
	}

	at1_fld = spdcap_file.Optional_Field ("FROM_AT", "FROMAT", "AT1", "AT", "AREATYPE");
	at2_fld = spdcap_file.Optional_Field ("TO_AT", "TOAT", "AT2", "AT", "AREATYPE");
	fc1_fld = spdcap_file.Optional_Field ("FROM_FC", "FROMFC", "FC1", "FC", "FUNCL");
	fc2_fld = spdcap_file.Optional_Field ("TO_FC", "TOFC", "FC2", "FC", "FUNCL");
	ln1_fld = spdcap_file.Optional_Field ("FROM_LANE", "FROMLANE", "LN1", "LN", "LANE");
	ln2_fld = spdcap_file.Optional_Field ("TO_LANE", "TOLANE", "LN2", "LN", "LANE");
	cap_flg = spdcap_file.Optional_Field ("CAP_FLAG");
	cap_fld = spdcap_file.Optional_Field ("CAPACITY", "CAP");
	spd_flg = spdcap_file.Optional_Field ("SPD_FLAG");
	spd_fld = spdcap_file.Optional_Field ("SPEED", "SPD");

	memset (cap_map, '\0', sizeof (cap_map));
	memset (spd_map, '\0', sizeof (spd_map));

	//---- read each node record----

	Show_Message ("Reading %s -- Record", spdcap_file.File_Type ());
	Set_Progress ();

	while (spdcap_file.Read_Record ()) {
		Show_Progress ();

		//---- read the data fields ----

		spdcap_file.Get_Field (at1_fld, &at1);
		spdcap_file.Get_Field (at2_fld, &at2);
		spdcap_file.Get_Field (fc1_fld, &fc1);
		spdcap_file.Get_Field (fc2_fld, &fc2);
		spdcap_file.Get_Field (ln1_fld, &ln1);
		spdcap_file.Get_Field (ln2_fld, &ln2);
		spdcap_file.Get_Field (cap_flg, cap_flag);
		spdcap_file.Get_Field (cap_fld, &cap);
		spdcap_file.Get_Field (spd_flg, spd_flag);
		spdcap_file.Get_Field (spd_fld, &spd);

		if (spd >= 40) flag = true;

		//---- save the data ----

		for (at=at1; at <= at2; at++) {
			if (at < 0 || at >= MAX_AT) {
				Error ("Area Type %d is Out of Range (0..%d)", at, MAX_AT-1);
			}
			for (fc=fc1; fc <= fc2; fc++) {
				if (fc < 0 || fc >= MAX_FUNCL) {
					Error ("Facility Type %d is Out of Range (0..%d)", fc, MAX_FUNCL-1);
				}
				for (ln=ln1; ln <= ln2; ln++) {
					if (ln < 0 || ln >= MAX_LANE) {
						Error ("Lanes %d is Out of Range (0..%d)", ln, MAX_LANE-1);
					}
					if (cap_flag [0] == ' ') {
						lvalue = (int) cap;
					} else {
						lvalue = cap_map [at] [fc] [ln];

						if (cap_flag [0] == '*') {
							lvalue = (int) (lvalue * cap + 0.5);
						} else if (cap_flag [0] == '+') {
							lvalue = (int) (lvalue + cap + 0.5);
						} else if (cap_flag [0] == '-') {
							lvalue = (int) (lvalue - cap + 0.5);
						}
					}
					cap_map [at] [fc] [ln] = lvalue;

					if (spd_flag [0] == ' ') {
						dvalue = spd;
					} else {
						dvalue = UnRound (spd_map [at] [fc] [ln]);

						if (spd_flag [0] == '*') {
							dvalue *= spd;
						} else if (spd_flag [0] == '+') {
							dvalue += spd;
						} else if (spd_flag [0] == '-') {
							dvalue -= spd;
						}
					}
					spd_map [at] [fc] [ln] = Round (dvalue);
				}
			}
		}
	}
	End_Progress ();

	spdcap_file.Close ();

	//---- convert speeds from mph to meters/second ----

	if (flag) {
		for (at=1; at < MAX_AT; at++) {
			for (fc=1; fc < MAX_FUNCL; fc++) {
				for (ln=1; ln < MAX_LANE; ln++) {
					spd_map [at] [fc] [ln] = Round (UnRound (spd_map [at] [fc] [ln]) * MPHTOMPS);
				}
			}
		}
	}
	Print (2, "Number of %s Records = %d", spdcap_file.File_Type (), Progress_Count ());
}
