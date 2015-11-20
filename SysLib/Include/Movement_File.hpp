//******************************************************** 
//	Movement_File.hpp - Turn Movement File Input/Output
//********************************************************

#ifndef MOVEMENT_FILE_HPP
#define MOVEMENT_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Movement_File Class definition
//---------------------------------------------------------

class  Movement_File : public Db_Header
{
public:

	Movement_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Movement_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Movement_File (void);

	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int In_Link (void)				{ Get_Field (in_link, &lvalue); return (lvalue); }
	int Out_Link (void)				{ Get_Field (out_link, &lvalue); return (lvalue); }
	char * Start (void)				{ Get_Field (start, svalue); return (svalue); }
	char * End (void)				{ Get_Field (end, svalue); return (svalue); }
	int Volume (void)				{ Get_Field (volume, &lvalue); return (lvalue); }

	void Node (int value)			{ Put_Field (node, value); }
	void In_Link (int value)		{ Put_Field (in_link, value); }
	void Out_Link (int value)		{ Put_Field (out_link, value); }
	void Start (char * value)		{ Put_Field (start, value); }
	void End (char * value)			{ Put_Field (end, value); }
	void Volume (int value)			{ Put_Field (volume, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int node, in_link, out_link, start, end, volume;
};

#endif
