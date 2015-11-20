//******************************************************** 
//	Problem_Link_File.hpp - Problem Link File Input/Output
//********************************************************

#ifndef PROBLEM_LINK_FILE_HPP
#define PROBLEM_LINK_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Problem_Link_File Class definition
//---------------------------------------------------------

class  Problem_Link_File : public Db_Header
{
public:

	Problem_Link_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Problem_Link_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Problem_Link_File (void);

	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	char * Start_Time (void)		{ Get_Field (start, svalue); return (svalue); }
	char * End_Time (void)			{ Get_Field (end, svalue); return (svalue); }
	int Problem (void)				{ Get_Field (problem, &lvalue); return (lvalue); }
	int Count (void)				{ Get_Field (count, &lvalue); return (lvalue); }

	void Link (int value)			{ Put_Field (link, value); }
	void Node (int value)			{ Put_Field (node, value); }
	void Start_Time (char *value)	{ Put_Field (start, value); }
	void End_Time (char *value)		{ Put_Field (end, value); }
	void Problem (int value)		{ Put_Field (problem, value); }
	void Count (int value)			{ Put_Field (count, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int link, node, start, end, problem, count;
};

#endif
