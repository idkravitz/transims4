//*************************************************** 
//	Projection_Service.hpp - Coordinate Projection Service
//***************************************************

#ifndef PROJECTION_SERVICE_HPP
#define PROJECTION_SERVICE_HPP

#include "Projection_Data.hpp"
#include "Projection_Code.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Projection_Service Class definition
//---------------------------------------------------------

class Projection_Service : public Static_Service, public Projection_Code
{
public:
	Projection_Service (void);

	void Add_Keys (void);

	bool Read_Control (void);

	void Input_Projection (Projection_Data value)	{ input = value; }
	void Output_Projection (Projection_Data value)	{ output = value; }

	Projection_Data Input_Projection (void)			{ return (input); }
	Projection_Data Output_Projection (void)		{ return (output); }

	bool Z_Flag (void)								{ return (z_flag); }
	bool M_Flag (void)								{ return (m_flag); }

	bool Initialize (Projection_Data *data);
	bool Projection_System (Projection_Data *data, char *string);
	bool Projection_Adjustment (Projection_Data *data, char *string);

	char * Get_Projection_String (Projection_Data *value);

protected:
	Projection_Type Type_Code (char *text);
	char * Type_Code (Projection_Type code);

	Projection_Units Units_Code (char *text);
	char * Units_Code (Projection_Units code);

private:

	static char *type_text [];
	static Projection_Type type_code [];

	static char *units_text [];
	static Projection_Units units_code [];

	static char * INPUT_COORDINATE_SYSTEM;
	static char * INPUT_COORDINATE_ADJUSTMENT;
	static char * OUTPUT_COORDINATE_SYSTEM;
	static char * OUTPUT_COORDINATE_ADJUSTMENT;
	static char * OUTPUT_XYZ_SHAPES;
	static char * OUTPUT_XYM_SHAPES;

	Projection_Data input, output;

	bool z_flag, m_flag;

	typedef struct {
		int code;
		double lat_origin;
		double long_origin;
		double first_parallel;
		double second_parallel;
		double false_easting;
		double false_northing;
		double scale_factor;
		char name [32];
	} SP_Data;

	typedef struct {
		int code;
		double lat_origin;
		double long_origin;
		double scale_factor;
		double false_easting;
		double false_northing;
		char name [4];
	} UTM_Data;

	static char projcs [2048];

	static SP_Data  sp_data [];
	static UTM_Data utm_data [];
};
#endif
