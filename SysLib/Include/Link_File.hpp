//******************************************************** 
//	Link_File.hpp - Node File Input/Output
//********************************************************

#ifndef LINK_FILE_HPP
#define LINK_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Link_File Class definition
//---------------------------------------------------------

class  Link_File : public Db_Header
{
public:

	Link_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Link_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Link_File (void);

	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	char * Street (void)            { Get_Field (street, svalue); return (svalue); }
	int Anode (void)				{ Get_Field (anode, &lvalue); return (lvalue); }
	int Bnode (void)				{ Get_Field (bnode, &lvalue); return (lvalue); }
	double Length (void)			{ Get_Field (length, &dvalue); return (dvalue); }
	double Setback_A (void)			{ Get_Field (setback_a, &dvalue); return (dvalue); }
	double Setback_B (void)			{ Get_Field (setback_b, &dvalue); return (dvalue); }
	int Bearing_A (void)			{ Get_Field (bearing_a, &lvalue); return (lvalue); }
	int Bearing_B (void)			{ Get_Field (bearing_b, &lvalue); return (lvalue); }
	char * Use (void)				{ Get_Field (use, svalue); return (svalue); }
	char * Type (void)				{ Get_Field (type, svalue); return (svalue); }
	int Lanes_AB (void)				{ Get_Field (lanes_ab, &lvalue); return (lvalue); }
	int Left_AB (void)				{ Get_Field (left_ab, &lvalue); return (lvalue); }
	int Right_AB (void)				{ Get_Field (right_ab, &lvalue); return (lvalue); }
	double Speed_AB (void)			{ Get_Field (speed_ab, &dvalue); return (dvalue); }
	double Fspd_AB (void)			{ Get_Field (fspd_ab, &dvalue); return (dvalue); }
	int Cap_AB (void)				{ Get_Field (cap_ab, &lvalue); return (lvalue); }
	int Lanes_BA (void)				{ Get_Field (lanes_ba, &lvalue); return (lvalue); }
	int Left_BA (void)				{ Get_Field (left_ba, &lvalue); return (lvalue); }
	int Right_BA (void)				{ Get_Field (right_ba, &lvalue); return (lvalue); }
	double Speed_BA (void)			{ Get_Field (speed_ba, &dvalue); return (dvalue); }
	double Fspd_BA (void)			{ Get_Field (fspd_ba, &dvalue); return (dvalue); }
	int Cap_BA (void)				{ Get_Field (cap_ba, &lvalue); return (lvalue); }

	void Link (int value)			{ Put_Field (link, value); }
	void Street (char * value)      { Put_Field (street, value); }
	void Anode (int value)			{ Put_Field (anode, value); }
	void Bnode (int value)			{ Put_Field (bnode, value); }
	void Length (double value)		{ Put_Field (length, value); }
	void Setback_A (double value)	{ Put_Field (setback_a, value); }
	void Setback_B (double value)	{ Put_Field (setback_b, value); }
	void Bearing_A (int value)		{ Put_Field (bearing_a, value); }
	void Bearing_B (int value)		{ Put_Field (bearing_b, value); }
	void Use (char * value)			{ Put_Field (use, value); }
	void Type (char * value)		{ Put_Field (type, value); }
	void Lanes_AB (int value)		{ Put_Field (lanes_ab, value); }
	void Left_AB (int value)		{ Put_Field (left_ab, value); }
	void Right_AB (int value)		{ Put_Field (right_ab, value); }
	void Speed_AB (double value)	{ Put_Field (speed_ab, value); }
	void Fspd_AB (double value)		{ Put_Field (fspd_ab, value); }
	void Cap_AB (int value)			{ Put_Field (cap_ab, value); }
	void Lanes_BA (int value)		{ Put_Field (lanes_ba, value); }
	void Left_BA (int value)		{ Put_Field (left_ba, value); }
	void Right_BA (int value)		{ Put_Field (right_ba, value); }
	void Speed_BA (double value)	{ Put_Field (speed_ba, value); }
	void Fspd_BA (double value)		{ Put_Field (fspd_ba, value); }
	void Cap_BA (int value)			{ Put_Field (cap_ba, value); }

	bool Name_Flag (void)           { return (name_flag); }
	void Name_Flag (bool flag)      { name_flag = flag; }
	
	bool Bearing_Flag (void)		{ return (bearing_a != 0 || bearing_b != 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (bool flag);

	bool name_flag;
	int lvalue;
	double dvalue;

	int link, anode, bnode, length, setback_a, setback_b, bearing_a, bearing_b, use, type, street;
	int lanes_ab, left_ab, right_ab, speed_ab, fspd_ab, cap_ab;
	int lanes_ba, left_ba, right_ba, speed_ba, fspd_ba, cap_ba;
};

#endif
