//******************************************************** 
//	Route_Nodes_File.hpp - Route Nodes File Input/Output
//********************************************************

#ifndef ROUTE_NODES_FILE_HPP
#define ROUTE_NODES_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Route_Nodes_File Class definition
//---------------------------------------------------------

class  Route_Nodes_File : public Db_Header
{
public:

	Route_Nodes_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Route_Nodes_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Route_Nodes_File (void);

	int Route (void)				{ Get_Field (route, &lvalue); return (lvalue); }
	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int Dwell (void)				{ Get_Field (dwell, &lvalue); return (lvalue); }
	int Time (void)					{ Get_Field (time, &lvalue); return (lvalue); }
	double Speed (void)				{ Get_Field (speed, &dvalue); return (dvalue); }

	void Route (int value)			{ Put_Field (route, value); }
	void Node (int value)			{ Put_Field (node, value); }
	void Dwell (int value)			{ Put_Field (dwell, value); }	//---- seconds ----
	void Time (int value)			{ Put_Field (time, value); }	//---- seconds ----
	void Speed (double value)		{ Put_Field (speed, value); }	//---- mps ----

	virtual bool Create_Fields (void);

	bool Dwell_Flag (void)			{ return (dwell_flag); }
	void Dwell_Flag (bool flag)		{ dwell_flag = flag; }

	bool Time_Flag (void)			{ return (time_flag); }
	void Time_Flag (bool flag)		{ time_flag = flag; }

	bool Speed_Flag (void)			{ return (speed_flag); }
	void Speed_Flag (bool flag)		{ speed_flag = flag; }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	bool dwell_flag, time_flag, speed_flag;
	int route, node, dwell, time, speed;
};

#endif
