//******************************************************** 
//	Count_File.hpp - Traffic Count File Input/Output
//********************************************************

#ifndef COUNT_FILE_HPP
#define COUNT_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Count_File Class definition
//---------------------------------------------------------

class  Count_File : public Db_Header
{
public:

	Count_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Count_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Count_File (void);

	int    Anode (void)				{ Get_Field (anode, &lvalue); return (lvalue); }
	int    Bnode (void)				{ Get_Field (bnode, &lvalue); return (lvalue); }
	char * Start_Time (void)		{ Get_Field (start, svalue); return (svalue); }
	char * End_Time (void)			{ Get_Field (end, svalue); return (svalue); }
	int    Count (void)				{ Get_Field (count, &lvalue); return (lvalue); }

	void Anode (int value)			{ Put_Field (anode, value); }
	void Bnode (int value)          { Put_Field (bnode, value); }
	void Start_Time (char *value)	{ Put_Field (start, value); }
	void End_Time (char *value)		{ Put_Field (end, value); }
	void Count (int value)			{ Put_Field (count, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	char svalue [20];
	int lvalue;

	int anode, bnode, start, end, count;
};

#endif
