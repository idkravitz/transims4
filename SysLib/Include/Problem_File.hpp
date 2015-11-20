//******************************************************** 
//	Problem_File.hpp - Problem File Input/Output
//********************************************************

#ifndef PROBLEM_FILE_HPP
#define PROBLEM_FILE_HPP

#include "Ext_Header.hpp"

//---------------------------------------------------------
//	Problem_File Class definition
//---------------------------------------------------------

class  Problem_File : public Ext_Header
{
public:

	Problem_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Problem_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Problem_File (void);

	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Person (void)				{ Get_Field (person, &lvalue); return (lvalue); }
	int Trip (void)					{ Get_Field (trip, &lvalue); return (lvalue); }
	int Leg (void)					{ Get_Field (leg, &lvalue); return (lvalue); }
	int Mode (void)					{ Get_Field (mode, &lvalue); return (lvalue); }
	int Problem (void)				{ Get_Field (problem, &lvalue); return (lvalue); }
	char * Start (void)				{ Get_Field (start, svalue); return (svalue); }
	int Origin (void)				{ Get_Field (origin, &lvalue); return (lvalue); }
	char * Arrive (void)			{ Get_Field (arrive, svalue); return (svalue); }
	int Destination (void)			{ Get_Field (destination, &lvalue); return (lvalue); }
	char * Time (void)				{ Get_Field (time, svalue); return (svalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)                  { Get_Field (dir, &lvalue); return (lvalue); }
	int Lane (void)					{ Get_Field (lane, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	int Activity (void)				{ return (Trip ()); }
	int Location (void)				{ return (Origin ()); }
	char * End (void)				{ return (Arrive ()); }
	int Vehicle (void)				{ Get_Field (vehicle, &lvalue); return (lvalue); }
	int Survey (void)				{ Get_Field (survey, &lvalue); return (lvalue); }
	int Route (void)				{ Get_Field (route, &lvalue); return (lvalue); }

	void Household (int value)		{ Put_Field (hhold, value); }
	void Person (int value)			{ Put_Field (person, value); }
	void Trip (int value)			{ Put_Field (trip, value); }
	void Leg (int value)			{ Put_Field (leg, value); }
	void Mode (int value)			{ Put_Field (mode, value); }
	void Problem (int value)		{ Put_Field (problem, value); }
	void Start (char * value)		{ Put_Field (start, value); }
	void Origin (int value)			{ Put_Field (origin, value); }
	void Arrive (char * value)		{ Put_Field (arrive, value); }
	void Destination (int value)	{ Put_Field (destination, value); }
	void Time (char * value)		{ Put_Field (time, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)            { Put_Field (dir, value); }
	void Lane (int value)			{ Put_Field (lane, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Activity (int value)		{ Trip (value); }
	void Location (int value)		{ Origin (value); }
	void End (char * value)			{ Arrive (value); }
	void Vehicle (int value)		{ Put_Field (vehicle, value); }
	void Survey (int value)			{ Put_Field (survey, value); }
	void Route (int value)			{ Put_Field (route, value); }

	void PopSyn_Flag (void)			{ program_code = POPSYN_CODE; }
	void ActGen_Flag (void)			{ program_code = ACTGEN_CODE; }
	void Router_Flag (void)			{ program_code = ROUTER_CODE; }
	void Microsimulator_Flag (void) { program_code = MSIM_CODE; }

	virtual bool Create_Fields (void);

	int  Link_Field (void)			{ return (link); }
	int  Time_Field (void)          { return (time); }
	int  Destination_Field (void)   { return (destination); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	enum Problem_Code { POPSYN_CODE = 1, ACTGEN_CODE, ROUTER_CODE, MSIM_CODE };

	void Setup (void);

	int lvalue;
	double dvalue;

	int program_code;

	int hhold, person, trip, leg, mode, problem, start, origin, arrive, destination;
	int time, link, dir, lane, vehicle, offset, survey, route;
};

#endif
