//******************************************************** 
//	Traveler_File.hpp - Traveler File Input/Output
//********************************************************

#ifndef TRAVELER_FILE_HPP
#define TRAVELER_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Traveler_File Class definition
//---------------------------------------------------------

class  Traveler_File : public Db_Header
{
public:

	Traveler_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Traveler_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Traveler_File (void);

	int Traveler (void)				{ Get_Field (traveler, &lvalue); return (lvalue); }
	int Vehicle (void)				{ Get_Field (vehicle, &lvalue); return (lvalue); }
	char *Time (void)				{ Get_Field (time, svalue); return (svalue); }
	double Distance (void)			{ Get_Field (distance, &dvalue); return (dvalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	int Lane (void)					{ Get_Field (lane, &lvalue); return (lvalue); }
	double Speed (void)				{ Get_Field (speed, &dvalue); return (dvalue); }

	void Traveler (int value)		{ Put_Field (traveler, value); }
	void Vehicle (int value)		{ Put_Field (vehicle, value); }
	void Time (char *value)			{ Put_Field (time, value); }
	void Distance (double value)	{ Put_Field (distance, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Lane (int value)			{ Put_Field (lane, value); }
	void Speed (double value)		{ Put_Field (speed, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int traveler, vehicle, time, distance, link, dir, offset, lane, speed;
};

#endif
