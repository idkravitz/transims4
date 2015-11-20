//******************************************************** 
//	Pocket_File.hpp - Pocket File Input/Output
//********************************************************

#ifndef POCKET_FILE_HPP
#define POCKET_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Pocket_File Class definition
//---------------------------------------------------------

class  Pocket_File : public Db_Header
{
public:

	Pocket_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Pocket_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Pocket_File (void);

	int ID (void)					{ Get_Field (id, &lvalue); return (lvalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	int Lane (void)					{ Get_Field (lane, &lvalue); return (lvalue); }
	char * Type (void)				{ Get_Field (type, svalue); return (svalue); }
	double Length (void)			{ Get_Field (length, &dvalue); return (dvalue); }

	void ID (int value)				{ Put_Field (id, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Node (int value)			{ Put_Field (node, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Lane (int value)			{ Put_Field (lane, value); }
	void Type (char * value)		{ Put_Field (type, value); }
	void Length (double value)		{ Put_Field (length, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int id, link, node, offset, lane, type, length;
};

#endif
