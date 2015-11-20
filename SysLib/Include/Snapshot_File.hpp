//******************************************************** 
//	Snapshot_File.hpp - Snapshot File Input/Output
//********************************************************

#ifndef SNAPSHOT_FILE_HPP
#define SNAPSHOT_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Snapshot_File Class definition
//---------------------------------------------------------

class  Snapshot_File : public Db_Header
{
public:

	Snapshot_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Snapshot_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Snapshot_File (void);

	int Vehicle (void)				{ Get_Field (vehicle, &lvalue); return (lvalue); }
	char *Time (void)				{ Get_Field (time, svalue); return (svalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	int Lane (void)					{ Get_Field (lane, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	double Speed (void)				{ Get_Field (speed, &dvalue); return (dvalue); }
	int VehType (void)				{ Get_Field (vehtype, &lvalue); return (lvalue); }
	double Accel (void)				{ Get_Field (accel, &dvalue); return (dvalue); }
	int Driver (void)				{ Get_Field (driver, &lvalue); return (lvalue); }
	int Passengers (void)			{ Get_Field (pass, &lvalue); return (lvalue); }
	double X (void)					{ Get_Field (x, &dvalue); return (dvalue); }
	double Y (void)					{ Get_Field (y, &dvalue); return (dvalue); }
	double Z (void)					{ Get_Field (z, &dvalue); return (dvalue); }
	double Bearing (void)			{ Get_Field (bearing, &dvalue); return (dvalue); }
	int User (void)					{ Get_Field (user, &lvalue); return (lvalue); }

	void Vehicle (int value)		{ Put_Field (vehicle, value); }
	void Time (char *value)			{ Put_Field (time, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }
	void Lane (int value)			{ Put_Field (lane, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Speed (double value)		{ Put_Field (speed, value); }
	void VehType (int value)		{ Put_Field (vehtype, value); }
	void Accel (double value)		{ Put_Field (accel, value); }
	void Driver (int value)			{ Put_Field (driver, value); }
	void Passengers (int value)		{ Put_Field (pass, value); }
	void X (double value)			{ Put_Field (x, value); }
	void Y (double value)			{ Put_Field (y, value); }
	void Z (double value)			{ Put_Field (z, value); }
	void Bearing (double value)		{ Put_Field (bearing, value); }
	void User (int value)			{ Put_Field (user, value); }

	virtual bool Create_Fields (void);

	int  Link_Field (void)			{ return (link); }
	int  Dir_Field (void)			{ return (dir); }
	int  Lane_Field (void)			{ return (lane); }
	int  Offset_Field (void)		{ return (offset); }
	int  Type_Field (void)			{ return (vehtype); }

	bool Location_Flag (void)		{ return (location_flag); }
	void Location_Flag (bool flag)	{ location_flag = flag; }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int vehicle, time, link, dir, lane, offset, speed, vehtype;
	int accel, driver, pass, x, y, z, bearing, user;
	bool location_flag;
};

#endif
