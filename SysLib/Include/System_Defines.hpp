//*********************************************************
//	System_Defines.hpp - global type definitions
//*********************************************************

#ifndef SYSTEM_DEFINES_HPP
#define SYSTEM_DEFINES_HPP

#define BLANK	' '

#define FILE_NAME_SIZE	256
#define STRING_BUFFER	512
#define FIELD_BUFFER	255
#define TIME_BUFFER		16

#define MAX(a,b)	((a >= b) ? a : b)
#define MIN(a,b)	((a <= b) ? a : b)

#define MIDNIGHT		86400
#define MAX_INTEGER		0x7FFFFFFF
#define MAX_USHORT		0xFFFF

#define PI				3.1415926535897932384626433832795
#define DEG2RAD			(PI / 180.0)
#define RAD2DEG			(180.0 / PI)

#define METERTOFEET		(39.37/12)				// US Survey Foot
#define	MILETOFEET		(5280.009842519685)		// US Survey Foot
#define MILETOMETER		(MILETOFEET / METERTOFEET)
#define MPSTOMPH		(3600.0 / MILETOMETER)
#define MPHTOMPS		(MILETOMETER / 3600.0)
	 
#define NAN				(-1.79769e+308)

//----	XY_Point -----

typedef  struct {
	double  x;
	double  y;
} XY_Point;

typedef  struct {
	double  x;
	double  y;
	double  z;
} XYZ_Point;

typedef  struct {
	int  x;
	int  y;
	int  z;
} XYZ;

#endif
