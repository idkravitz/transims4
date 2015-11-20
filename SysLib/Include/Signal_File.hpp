//******************************************************** 
//	Signal_File.hpp - Signal File Input/Output
//********************************************************

#ifndef SIGNAL_FILE_HPP
#define SIGNAL_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Signal_File Class definition
//---------------------------------------------------------

class  Signal_File : public Db_Header
{
public:

	Signal_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Signal_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Signal_File (void);

	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	char* Type (void)				{ Get_Field (type, svalue); return (svalue); }
	int Timing (void)				{ Get_Field (timing, &lvalue); return (lvalue); }
	int Offset (void)				{ Get_Field (offset, &lvalue); return (lvalue); }
	char *Start (void)				{ Get_Field (start, svalue); return (svalue); }
	int Coordinator (void)			{ Get_Field (coord, &lvalue); return (lvalue); }
	char *Ring (void)				{ Get_Field (ring, svalue); return (svalue); }

	void Node (int value)			{ Put_Field (node, value); }
	void Type (char *value)			{ Put_Field (type, value); }
	void Timing (int value)			{ Put_Field (timing, value); }
	void Offset (int value)			{ Put_Field (offset, value); }
	void Start (char *value)		{ Put_Field (start, value); }
	void Coordinator (int value)	{ Put_Field (coord, value); }
	void Ring (char *value)			{ Put_Field (ring, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int node, type, timing, offset, start, coord, ring;
};

#endif
