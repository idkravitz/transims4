//******************************************************** 
//	Timing_File.hpp - Timing Plan File Input/Output
//********************************************************

#ifndef TIMING_FILE_HPP
#define TIMING_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Timing_File Class definition
//---------------------------------------------------------

class  Timing_File : public Db_Header
{
public:

	Timing_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Timing_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Timing_File (void);

	int Timing (void)				{ Get_Field (timing, &lvalue); return (lvalue); }
	int Phase (void)				{ Get_Field (phase, &lvalue); return (lvalue); }
	char * Next_Phase (void)		{ Get_Field (next, svalue); return (svalue); }
	int Min_Green (void)			{ Get_Field (minimum, &lvalue); return (lvalue); }
	int Max_Green (void)			{ Get_Field (maximum, &lvalue); return (lvalue); }
	int Ext_Green (void)			{ Get_Field (extend, &lvalue); return (lvalue); }
	int Yellow (void)				{ Get_Field (yellow, &lvalue); return (lvalue); }
	int Red_Clear (void)			{ Get_Field (red, &lvalue); return (lvalue); }
	int Ring (void)					{ Get_Field (ring, &lvalue); return (lvalue); }
	int Group (void)				{ Get_Field (group, &lvalue); return (lvalue); }

	void Timing (int value)			{ Put_Field (timing, value); }
	void Phase (int value)			{ Put_Field (phase, value); }
	void Next_Phase (char * value)	{ Put_Field (next, value); }
	void Min_Green (int value)		{ Put_Field (minimum, value); }
	void Max_Green (int value)		{ Put_Field (maximum, value); }
	void Ext_Green (int value)		{ Put_Field (extend, value); }
	void Yellow (int value)			{ Put_Field (yellow, value); }
	void Red_Clear (int value)		{ Put_Field (red, value); }
	void Ring (int value)			{ Put_Field (ring, value); }
	void Group (int value)			{ Put_Field (group, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int timing, phase, next, minimum, maximum, extend, yellow, red, ring, group;
};

#endif
