//*********************************************************
//	Projection_Service.cpp - Coordinate Projection Service
//*********************************************************

#include "Projection_Service.hpp"

#include <stdlib.h>
#include <string.h>
#include <math.h>

char * Projection_Service::INPUT_COORDINATE_SYSTEM			= "INPUT_COORDINATE_SYSTEM";
char * Projection_Service::INPUT_COORDINATE_ADJUSTMENT		= "INPUT_COORDINATE_ADJUSTMENT";
char * Projection_Service::OUTPUT_COORDINATE_SYSTEM			= "OUTPUT_COORDINATE_SYSTEM";
char * Projection_Service::OUTPUT_COORDINATE_ADJUSTMENT		= "OUTPUT_COORDINATE_ADJUSTMENT";
char * Projection_Service::OUTPUT_XYZ_SHAPES				= "OUTPUT_XYZ_SHAPES";
char * Projection_Service::OUTPUT_XYM_SHAPES				= "OUTPUT_XYM_SHAPES";

#define ELLIPSE_A		6378137.0
#define ELLIPSE_B		6356752.3141403561

//---- type codes ----

char * Projection_Service::type_text [] = {
	"LATLONG", "STATEPLANE", "UTM", NULL
};

Projection_Service::Projection_Type  Projection_Service::type_code [] = {
	LATLONG, STATEPLANE, UTM
};

//---- units codes ----

char * Projection_Service::units_text [] = {
	"DEGREES", "FEET", "METERS", "MILES", "MILLION DEGREES", "KILOMETERS", "MILLION_DEGREES", NULL
};

Projection_Service::Projection_Units  Projection_Service::units_code [] = {
	DEGREES, FEET, METERS, MILES, MILLION_DEG, KILOMETERS, MILLION_DEG
};

char Projection_Service::projcs [] = "";

Projection_Service::SP_Data Projection_Service::sp_data [] = {
//-----------------------------------------------------------------------------------------------------------------------------------------------
//	Lambert Conformal Conic (LCC) Projection, NAD 1983, GRS 80
//	{FIPS, Lat of Origin, Central Meridian, Std Parallel #1, Std Parallel #2, False Easting, False Northing, 0, Name}	 
//-----------------------------------------------------------------------------------------------------------------------------------------------
	{5010,			 51,			-176,	51.83333333,	53.83333333,	3280833.333,			0.0,			0.0, "Alaska_10"},
	{ 301,	34.33333333,			 -92,	34.93333333,	36.23333333,	1312333.333,			0.0,			0.0, "Arkansas_North"},
	{ 302,	32.66666667,			 -92,		   33.3,	34.76666667,	1312333.333,	1312333.333,			0.0, "Arkansas_South"},
	{ 401,	39.33333333,			-122,			 40,	41.66666667,	6561666.667,	1640416.667,			0.0, "California_I"},
	{ 402,	37.66666667,			-122,	38.33333333,	39.83333333,	6561666.667,	1640416.667,			0.0, "California_II"},
	{ 403,		   36.5,		  -120.5,	37.06666667,	38.43333333,	6561666.667,	1640416.667,			0.0, "California_III"},
	{ 404,	35.33333333,			-119,			 36,		  37.25,	6561666.667,	1640416.667,			0.0, "California_IV"},
	{ 405,		   33.5,			-118,	34.03333333,	35.46666667,	6561666.667,	1640416.667,			0.0, "California_V"},
	{ 406,	32.16666667,		 -116.25,	32.78333333,	33.88333333,	6561666.667,	1640416.667,			0.0, "California_VI"},
	{ 502,	37.83333333,		  -105.5,		  38.45,		  39.75,		3000000,		1000000,			0.0, "Colorado_Central"},
	{ 501,	39.33333333,		  -105.5,	39.71666667,	40.78333333,		3000000,		1000000,			0.0, "Colorado_North"},
	{ 503,	36.66666667,		  -105.5,	37.23333333,	38.43333333,		3000000,		1000000,			0.0, "Colorado_South"},
	{ 600,	40.83333333,		  -72.75,		   41.2,	41.86666667,		1000000,		 500000,			0.0, "Connecticut"},
	{ 903,			 29,		   -84.5,	29.58333333,		  30.75,		1968500,			0.0,			0.0, "Florida_North"},
	{1401,		   41.5,		   -93.5,	42.06666667,	43.26666667,		4921250,	3280833.333,			0.0, "Iowa_North"},
	{1402,			 40,		   -93.5,	40.61666667,	41.78333333,	1640416.667,			0.0,			0.0, "Iowa_South"},
	{1501,	38.33333333,			 -98,	38.71666667,	39.78333333,	1312333.333,			0.0,			0.0, "Kansas_North"},
	{1502,	36.66666667,		   -98.5,	37.26666667,	38.56666667,	1312333.333,	1312333.333,			0.0, "Kansas_South"},
	{1600,	36.33333333,		  -85.75,	37.08333333,	38.66666667,		4921250,	3280833.333,			0.0, "Kentucky"},
	{1601,		   37.5,		  -84.25,	37.96666667,	38.96666667,	1640416.667,			0.0,			0.0, "Kentucky_North"},
	{1602,	36.33333333,		  -85.75,	36.73333333,	37.93333333,	1640416.667,	1640416.667,			0.0, "Kentucky_South"},
	{1701,		   30.5,		   -92.5,	31.16666667,	32.66666667,	3280833.333,			0.0,			0.0, "Louisiana_North"},
	{1702,		   28.5,	-91.33333333,		   29.3,		   30.7,	3280833.333,			0.0,			0.0, "Louisiana_South"},
	{1900,	37.66666667,			 -77,		   38.3,		  39.45,	1312333.333,			0.0,			0.0, "Maryland"},
	{2002,			 41,		   -70.5,	41.28333333,	41.48333333,	1640416.667,			0.0,			0.0, "Massachusetts_Island"},
	{2001,			 41,		   -71.5,	41.71666667,	42.68333333,	656166.6667,		2460625,			0.0, "Massachusetts_Mainland"},
	{2202,	43.31666667,	-84.36666667,	44.18333333,		   45.7,	   19685000,			0.0,			0.0, "Michigan_Central"},
	{2111,	44.78333333,			 -87,	45.48333333,	47.08333333,	26246666.67,			0.0,			0.0, "Michigan_North"},
	{2113,		   41.5,	-84.36666667,		   42.1,	43.66666667,	13123333.33,			0.0,			0.0, "Michigan_South"},
	{2202,			 45,		  -94.25,	45.61666667,		  47.05,	2624666.667,	328083.3333,			0.0, "Minnesota_Central"},
	{2201,		   46.5,		   -93.1,	47.03333333,	48.63333333,	2624666.667,	328083.3333,			0.0, "Minnesota_North"},
	{2203,			 43,			 -94,	43.78333333,	45.21666667,	2624666.667,	328083.3333,			0.0, "Minnesota_South"},
	{2500,		  44.25,		  -109.5,			 45,			 49,		1968500,			0.0,			0.0, "Montana"},
	{2600,	39.83333333,			-100,			 40,			 43,	1640416.667,			0.0,			0.0, "Nebraska"},
	{3104,	40.16666667,			 -74,	40.66666667,	41.03333333,		 984250,			0.0,			0.0, "New_York_Long_Island"},
	{3200,		  33.75,			 -79,	34.33333333,	36.16666667,	2000000.003,			0.0,			0.0, "North_Carolina"},
	{3301,			 47,		  -100.5,	47.43333333,	48.73333333,		1968500,			0.0,			0.0, "North_Dakota_North"},
	{3302,	45.66666667,		  -100.5,	46.18333333,	47.48333333,		1968500,			0.0,			0.0, "North_Dakota_South"},
	{3401,	39.66666667,		   -82.5,	40.43333333,		   41.7,		1968500,			0.0,			0.0, "Ohio_North"},
	{3402,			 38,		   -82.5,	38.73333333,	40.03333333,		1968500,			0.0,			0.0, "Ohio_South"},
	{3501,			 35,			 -98,	35.56666667,	36.76666667,		1968500,			0.0,			0.0, "Oklahoma_North"},
	{3502,	33.33333333,			 -98,	33.93333333,	35.23333333,		1968500,			0.0,			0.0, "Oklahoma_South"},
	{3601,	43.66666667,		  -120.5,	44.33333333,			 46,	8202083.333,			0.0,			0.0, "Oregon_North"},
	{3602,	41.66666667,		  -120.5,	42.33333333,			 44,		4921250,			0.0,			0.0, "Oregon_South"},
	{5200,	17.83333333,	-66.43333333,	18.03333333,	18.43333333,	656166.6667,	656166.6667,			0.0, "PR_Virgin_Islands"},
	{3701,	40.16666667,		  -77.75,	40.88333333,		  41.95,		1968500,			0.0,			0.0, "Pennsylvania_North"},
	{3702,	39.33333333,		  -77.75,	39.93333333,	40.96666667,		1968500,			0.0,			0.0, "Pennsylvania_South"},
	{3900,	31.83333333,			 -81,		   32.5,	34.83333333,		1999996,			0.0,			0.0, "South_Carolina"},
	{4001,	43.83333333,			-100,	44.41666667,	45.68333333,		1968500,			0.0,			0.0, "South_Dakota_North"},
	{4002,	42.33333333,	-100.3333333,	42.83333333,		   44.4,		1968500,			0.0,			0.0, "South_Dakota_South"},
	{4100,	34.33333333,			 -86,		  35.25,	36.41666667,		1968500,			0.0,			0.0, "Tennessee"},
	{4203,	29.66666667,	-100.3333333,	30.11666667,	31.88333333,	2296583.333,		9842500,			0.0, "Texas_Central"},
	{4202,	31.66666667,		   -98.5,	32.13333333,	33.96666667,		1968500,	6561666.667,			0.0, "Texas_North_Central"},
	{4201,			 34,		  -101.5,		  34.65,	36.18333333,	656166.6667,	3280833.333,			0.0, "Texas_North"},
	{4204,	27.83333333,			 -99,	28.38333333,	30.28333333,		1968500,	13123333.33,			0.0, "Texas_South_Central"},
	{4205,	25.66666667,		   -98.5,	26.16666667,	27.83333333,		 984250,	16404166.67,			0.0, "Texas_South"},
	{4302,	38.33333333,		  -111.5,	39.01666667,		  40.65,	1640416.667,	6561666.667,			0.0, "Utah_Central"},
	{4301,	40.33333333,		  -111.5,	40.71666667,	41.78333333,	1640416.667,	3280833.333,			0.0, "Utah_North"},
	{4303,	36.66666667,		  -111.5,	37.21666667,		  38.35,	1640416.667,		9842500,			0.0, "Utah_South"},
	{4501,	37.66666667,		   -78.5,	38.03333333,		   39.2,	11482916.67,	6561666.667,			0.0, "Virginia_North"},
	{4502,	36.33333333,		   -78.5,	36.76666667,	37.96666667,	11482916.67,	3280833.333,			0.0, "Virginia_South"},
	{4601,			 47,	-120.8333333,		   47.5,	48.73333333,	1640416.667,			0.0,			0.0, "Washington_North"},
	{4602,	45.33333333,		  -120.5,	45.83333333,	47.33333333,	1640416.667,			0.0,			0.0, "Washington_South"},
	{4701,		   38.5,		   -79.5,			 39,		  40.25,		1968500,			0.0,			0.0, "West_Virginia_North"},
	{4702,			 37,			 -81,	37.48333333,	38.88333333,		1968500,			0.0,			0.0, "West_Virginia_South"},
	{4802,	43.83333333,			 -90,		  44.25,		   45.5,		1968500,			0.0,			0.0, "Wisconsin_Central"},
	{4801,	45.16666667,			 -90,	45.56666667,	46.76666667,		1968500,			0.0,			0.0, "Wisconsin_North"},
	{4803,			 42,			 -90,	42.73333333,	44.06666667,		1968500,			0.0,			0.0, "Wisconsin_South"},
//-----------------------------------------------------------------------------------------------------------------------------------------------
//	Tranverse Mercator (TM) Projection, NAD 1983, GRS 80
//	{FIPS, Lat of Origin, Central Meridian, 0, 0, False Easting, False Northing, Scale Factor, Name}	 
//-----------------------------------------------------------------------------------------------------------------------------------------------
	{101,		   30.5,	-85.83333333,			  0,			  0,	656166.6667,			  0,		0.99996, "Alabama_East"},
	{102,			 30,		   -87.5,			  0,			  0,		1968500,			  0,	0.999933333, "Alabama_West"},
	{5002,			 54,			-142,			  0,			  0,	1640416.667,			  0,	 	 0.9999, "Alaska_2"},
	{5003,			 54,			-146,			  0,			  0,	1640416.667,			  0,		 0.9999, "Alaska_3"},
	{5004,			 54,			-150,			  0,			  0,	1640416.667,			  0,		 0.9999, "Alaska_4"},
	{5005,			 54,			-154,			  0,			  0,	1640416.667,			  0,		 0.9999, "Alaska_5"},
	{5006,			 54,			-158,			  0,			  0,	1640416.667,			  0,		 0.9999, "Alaska_6"},
	{5007,			 54,			-162,			  0,			  0,	1640416.667,			  0,		 0.9999, "Alaska_7"},
	{5008,			 54,			-166,			  0,			  0,	1640416.667,			  0,		 0.9999, "Alaska_8"},
	{5009,			 54,			-170,			  0,			  0,	1640416.667,			  0,		 0.9999, "Alaska_9"},
	{202,			 31,	-111.9166667,			  0,			  0,	   699998.6,			  0,		 0.9999, "Arizona_Central"},
	{201,			 31,	-110.1666667,			  0,			  0,	   699998.6,			  0,		 0.9999, "Arizona_East"},
	{203,			 31,		 -113.75,			  0,			  0,	   699998.6,			  0,	0.999933333, "Arizona_West"},
	{700,			 38,	-75.41666667,			  0,			  0,	656166.6667,			  0,	   0.999995, "Delaware"},
	{901,	24.33333333,	  		 -81,			  0,			  0,	656166.6667,		  	  0,	0.999941176, "Florida_East"},
	{902,	24.33333333,			 -82,			  0,			  0,	656166.6667,			  0,	0.999941176, "Florida_West"},
	{1001,			 30,	-82.16666667,			  0,			  0,	656166.6667,			  0,		 0.9999, "Georgia_East"},
	{1002,			 30,	-84.16666667,			  0,			  0,	2296583.333,			  0,		 0.9999, "Georgia_West"},
	{5101,	18.83333333,		  -155.5,			  0,			  0,	1640416.667,			  0,	0.999966667, "Hawaii_1"},
	{5102,	20.33333333,	-156.6666667,			  0,			  0,	1640416.667,			  0,	0.999966667, "Hawaii_2"},
	{5103,	21.16666667,			-158,			  0,			  0,	1640416.667,			  0,		0.99999, "Hawaii_3"},
	{5104,	21.83333333,		  -159.5,			  0,			  0,	1640416.667,			  0,		0.99999, "Hawaii_4"},
	{5105,	21.66666667,	-160.1666667,			  0,			  0,	1640416.667,			  0,			  1, "Hawaii_5"},
	{1102,	41.66666667,			-114,			  0,			  0,	1640416.667,			  0,	0.999947368, "Idaho_Central"},
	{1101,	41.66666667,	-112.1666667,			  0,			  0,	656166.6667,			  0,	0.999947368, "Idaho_East"},
	{1103,	41.66666667,		 -115.75,			  0,			  0,	2624666.667,			  0,	0.999933333, "Idaho_West"},
	{1201,	36.66666667,	-88.33333333,			  0,			  0,		 984250,			  0,	   0.999975, "Illinois_East"},
	{1202,	36.66666667,	-90.16666667,			  0,			  0,	2296583.333,			  0,	0.999941176, "Illinois_West"},
	{1301,		   37.5,	-85.66666667,			  0,			  0,	328083.3333,	820208.3333,	0.999966667, "Indiana_East"},
	{1302,		   37.5,	-87.08333333,			  0,			  0,		2952750,	820208.3333,	0.999966667, "Indiana_West"},
	{1801,	43.66666667,	   	   -68.5,			  0,			  0,	     984250,			  0,		 0.9999, "Maine_East"},
	{1802,	42.83333333,	-70.16666667,			  0,			  0,		2952750,			  0,	0.999966667, "Maine_West"},
	{2301,	       29.5,	-88.83333333,			  0,			  0,		 984250,			  0,		0.99995, "Mississippi_East"},
	{2302,		   29.5,	-90.33333333,			  0,			  0,	2296583.333,			  0,		0.99995, "Mississippi_West"},
	{2402,	35.83333333,		   -92.5,			  0,			  0,	1640416.667,			  0,	0.999933333, "Missouri_Central"},
	{2401,	35.83333333,		   -90.5,			  0,			  0,	820208.3333,			  0,	0.999933333, "Missouri_East"},
	{2403,	36.16666667,	  	   -94.5,			  0,			  0,	2788708.333,			  0,	0.999941176, "Missouri_West"},
	{2702,		  34.75,	-116.6666667,			  0,			  0,	1640416.667,	   19685000,		 0.9999, "Nevada_Central"},
	{2701,		  34.75,	-115.5833333,			  0,			  0,	656166.6667,	26246666.67,		 0.9999, "Nevada_East"},
	{2703,		  34.75,	-118.5833333,			  0,			  0,	2624666.667,	13123333.33,		 0.9999, "Nevada_West"},
	{2800,		   42.5,	-71.66666667,			  0,			  0,		 984250,			  0,	0.999966667, "New_Hampshire"},
	{2900,	38.83333333,		   -74.5,			  0,			  0,		 492125,			  0,		 0.9999, "New_Jersey"},
	{3002,	  		 31,		 -106.25,			  0,			  0,	1640416.667,			  0,		 0.9999, "New_Mexico_Central"},
	{3001,			 31,	-104.3333333,			  0,			  0,	   541337.5,			  0,	0.999909091, "New_Mexico_East"},
	{3003,			 31,	-107.8333333,			  0,			  0,	2723091.667,			  0,	0.999916667, "New_Mexico_West"},
	{3102,			 40,	-76.58333333,			  0,			  0,	820208.3333,			  0,	  0.9999375, "New_York_Central"},
	{3101,	38.83333333,	       -74.5,			  0,			  0,	  	 492125,			  0,		 0.9999, "New_York_East"},
	{3103,			 40,	-78.58333333,			  0,			  0,	1148291.667,			  0,	  0.9999375, "New_York"},
	{3800,	41.08333333,		   -71.5,			  0,			  0,	328083.3333,			  0,	 0.99999375, "Rhode_Island"},
	{4400,	       42.5,		   -72.5,			  0,			  0,	1640416.667,			  0,	0.999964286, "Vermont"},
	{4902,		   40.5,	-107.3333333,			  0,			  0,	1312333.333,	328083.3333,	  0.9999375, "Wyoming_East_Central"},
	{4901,		   40.5,	-105.1666667,			  0,			  0,	656166.6667,			  0,	  0.9999375, "Wyoming_East"},
	{4903,		   40.5,	   	 -108.75,			  0,			  0,		1968500,			  0,	  0.9999375, "Wyoming_West_Central"},
	{4904,		   40.5,	-110.0833333,			  0,			  0,	2624666.667,	328083.3333,	  0.9999375, "Wyoming_West"},
	{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, ""}
};

Projection_Service::UTM_Data Projection_Service::utm_data [] = {
//-----------------------------------------------------------------------------------------------------------------------------------------------
//	Universal Transverse Mercator Projection, NAD 1983, GRS 80
//	{UTM Zone, Lat of Origin, Central Meridian,	Scale Factor, False Easting, False Northing, Name}
//-----------------------------------------------------------------------------------------------------------------------------------------------
	{ 1, 0.0, -177.0, 0.9996, 500000.0, 0.0, "1N"},
	{ 2, 0.0, -171.0, 0.9996, 500000.0, 0.0, "2N"},
	{ 3, 0.0, -165.0, 0.9996, 500000.0, 0.0, "3N"},
	{ 4, 0.0, -159.0, 0.9996, 500000.0, 0.0, "4N"},
	{ 5, 0.0, -153.0, 0.9996, 500000.0, 0.0, "5N"},
	{ 6, 0.0, -147.0, 0.9996, 500000.0, 0.0, "6N"},
	{ 7, 0.0, -141.0, 0.9996, 500000.0, 0.0, "7N"},
	{ 8, 0.0, -135.0, 0.9996, 500000.0, 0.0, "8N"},
	{ 9, 0.0, -129.0, 0.9996, 500000.0, 0.0, "9N"},
	{10, 0.0, -123.0, 0.9996, 500000.0, 0.0, "10N"},
	{11, 0.0, -117.0, 0.9996, 500000.0, 0.0, "11N"},
	{12, 0.0, -111.0, 0.9996, 500000.0, 0.0, "12N"},
	{13, 0.0, -105.0, 0.9996, 500000.0, 0.0, "13N"},
	{14, 0.0,  -99.0, 0.9996, 500000.0, 0.0, "14N"},
	{15, 0.0,  -93.0, 0.9996, 500000.0, 0.0, "15N"},
	{16, 0.0,  -87.0, 0.9996, 500000.0, 0.0, "16N"},
	{17, 0.0,  -81.0, 0.9996, 500000.0, 0.0, "17N"},
	{18, 0.0,  -75.0, 0.9996, 500000.0, 0.0, "18N"},
	{19, 0.0,  -69.0, 0.9996, 500000.0, 0.0, "19N"},
	{20, 0.0,  -63.0, 0.9996, 500000.0, 0.0, "20N"},
	{21, 0.0,  -57.0, 0.9996, 500000.0, 0.0, "21N"},
	{22, 0.0,  -51.0, 0.9996, 500000.0, 0.0, "22N"},
	{23, 0.0,  -45.0, 0.9996, 500000.0, 0.0, "23N"},
	{59, 0.0,  171.0, 0.9996, 500000.0, 0.0, "59N"},
	{60, 0.0,  177.0, 0.9996, 500000.0, 0.0, "60N"},
	{0, 0.0, 0.0, 0.0, 0.0, 0.0, ""}
};

//---------------------------------------------------------
//	Projection_Service constructor
//---------------------------------------------------------

Projection_Service::Projection_Service (void) : Static_Service (), Projection_Code ()
{
}

//---------------------------------------------------------
//	Projection_System 
//---------------------------------------------------------

bool Projection_Service::Projection_System (Projection_Data *data, char *string)
{
	int i;
	char *str_ptr, field [FIELD_BUFFER];

	i = 0;
	str_ptr = string;
	data->Type (LATLONG);
	data->Code (0);
	data->Units (DEGREES);

	while (str_ptr != NULL) {
		str_ptr = Get_Token (str_ptr, field);

		if (field [0] == '\0') break;

		if (!i) {
			data->Type (Type_Code (field));

			if (data->Type () == STATEPLANE) {
				data->Units (FEET);
			} else if (data->Type () == UTM) {
				data->Units (METERS);
			}
		} else if (data->Type () == LATLONG) {
			data->Units (Units_Code (field));

			if (data->Units () != MILLION_DEG && data->Units () != DEGREES) {
				if (Send_Messages ()) {
					exe->Error ("Lat-Long Projection with %s Units is Illogical", field);
				}
				return (false);
			}
		} else if (i == 1) {
			if (field [0] < '0' || field [0] > '9') {
				if (Send_Messages ()) {
					exe->Error ("%s Projection Code was Not Provided", Type_Code (data->Type ()));
				}
				return (false);
			}
			data->Code (atol (field));
		} else {
			data->Units (Units_Code (field));
		}
		i++;
	}
	return (true);
}

//---------------------------------------------------------
//	Projection_Adjustment 
//---------------------------------------------------------

bool Projection_Service::Projection_Adjustment (Projection_Data *data, char *string)
{
	int i;
	char *str_ptr, field [FIELD_BUFFER];

	i = 0;
	str_ptr = string;
	data->X_Offset (0.0);
	data->Y_Offset (0.0);
	data->X_Factor (1.0);
	data->Y_Factor (1.0);

	while (str_ptr != NULL) {
		str_ptr = Get_Token (str_ptr, field);

		if (field [0] == '\0') break;
			
		if ((field [0] < '0' || field [0] > '9') && 
			field [0] != '.' && field [0] != '-' && field [0] != '+') return (false);

		switch (i++) {
			case 0:
				data->X_Offset (atof (field));
				break;
			case 1:
				data->Y_Offset (atof (field));
				break;
			case 2:
				data->X_Factor (atof (field));
				break;
			case 3:
				data->Y_Factor (atof (field));
				break;
			default:
				return (false);
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Projection_Service::Add_Keys (void)
{
	char *keys [] = {
		INPUT_COORDINATE_SYSTEM,
		INPUT_COORDINATE_ADJUSTMENT,
		OUTPUT_COORDINATE_SYSTEM,
		OUTPUT_COORDINATE_ADJUSTMENT,
		OUTPUT_XYZ_SHAPES,
		OUTPUT_XYM_SHAPES,
		NULL
	};
	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Projection_Service::Read_Control (void)
{
	if (exe == NULL) return (false);

	char *str_ptr;
	bool first = true;
	bool defined = false;

	//---- coordination system ----

	str_ptr = exe->Get_Control_String (INPUT_COORDINATE_SYSTEM);

	if (str_ptr != NULL) {
		if (!Projection_System (&input, str_ptr)) {
			exe->Error ("Input Coordinate System");
		}
		if (first) {
			exe->Print (1);
			first = false;
		}
		exe->Print (1, "Input Coordinate System = %s", str_ptr);
		defined = true;
	}
	str_ptr = exe->Get_Control_String (INPUT_COORDINATE_ADJUSTMENT);

	if (str_ptr != NULL) {
		if (!Projection_Adjustment (&input, str_ptr)) {
			exe->Error ("Input Coordinate Adjustment");
		}
		if (first) {
			exe->Print (1);
			first = false;
		}
		exe->Print (1, "Input Coordinate Adjustment = %s", str_ptr);
		defined = true;
	}
	str_ptr = exe->Get_Control_String (OUTPUT_COORDINATE_SYSTEM);
	
	if (str_ptr != NULL) {
		if (!Projection_System (&output, str_ptr)) {
			exe->Error ("Output Coordinate System");
		}
		if (first) {
			exe->Print (1);
			first = false;
		}
		exe->Print (1, "Output Coordinate System = %s", str_ptr);
		defined = true;
	}
	str_ptr = exe->Get_Control_String (OUTPUT_COORDINATE_ADJUSTMENT);

	if (str_ptr != NULL) {
		if (!Projection_Adjustment (&output, str_ptr)) {
			exe->Error ("Output Coordinate System");
		}
		if (first) {
			exe->Print (1);
			first = false;
		}
		exe->Print (1, "Output Coordinate Adjustment = %s", str_ptr);
		defined = true;
	}

	//---- coordinates flags ----

	m_flag = exe->Get_Control_Flag (OUTPUT_XYM_SHAPES);
	z_flag = (m_flag || exe->Get_Control_Flag (OUTPUT_XYZ_SHAPES));
	first = true;

	//---- show the z coordinates flags ----

	str_ptr = exe->Get_Control_String (OUTPUT_XYZ_SHAPES);
	if (str_ptr != NULL) {
		if (first) {
			exe->Print (1);
			first = false;
		}
		exe->Print (1, "Output XYZ Shapes = %s", str_ptr);
	}

	str_ptr = exe->Get_Control_String (OUTPUT_XYM_SHAPES);
	if (str_ptr != NULL) {
		if (first) {
			exe->Print (1);
			first = false;
		}
		exe->Print (1, "Output XYM Shapes = %s", str_ptr);
	}
	Initialize (&input);
	Initialize (&output);

	return (defined);
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

bool Projection_Service::Initialize (Projection_Data *data)
{
	int i;
	double temp1, temp2, lat_origin, first_parallel, second_parallel;

	data->adjust = (data->X_Offset () != 0.0 || data->Y_Offset () != 0.0 || 
		data->X_Factor () != 1.0 || data->Y_Factor () != 1.0);

	data->Status (false);
	
	switch (data->Type ()) {
		case STATEPLANE:
			for (i=0; ; i++) {
				if (sp_data [i].code == 0) {
					if (Send_Messages ()) {
						exe->Error ("State Plane Projection Code %d was Unrecognized", data->Code ());
					}
					return (false);
				}
				if (sp_data [i].code == data->Code ()) break;
			}
			data->index = i;
			data->scale_factor = sp_data [i].scale_factor;

			if (data->scale_factor == 0) {
				lat_origin = sp_data [i].lat_origin * DEG2RAD;
				data->long_origin = sp_data [i].long_origin * DEG2RAD;
				first_parallel = sp_data [i].first_parallel * DEG2RAD;
				second_parallel = sp_data [i].second_parallel * DEG2RAD;
				data->false_easting = sp_data [i].false_easting;	
				data->false_northing = sp_data [i].false_northing;

				data->a = ELLIPSE_A * METERTOFEET;
				data->b = ELLIPSE_B * METERTOFEET;

				data->ef = (data->a - data->b) / data->a;
				data->e_square = 2 * data->ef - pow (data->ef, 2.0);
				data->e = sqrt (data->e_square);
				data->e_prime_sq = data->e_square / (1 - data->e_square);

				data->m1 = cos (first_parallel) /
					sqrt (1 - data->e_square * (sin (first_parallel) *
					sin (first_parallel)));

				data->m2 = cos (second_parallel) /
					sqrt (1 - data->e_square * (sin (second_parallel) *
					sin (second_parallel)));
				
				data->t1 = tan ((PI / 4) - (first_parallel) / 2) / 
					pow (((1 - data->e * sin (first_parallel)) /
					(1 + data->e * sin (first_parallel))),
					(data->e / 2));

				data->t2 = tan ((PI / 4) - (second_parallel) / 2) / 
					pow (((1 - data->e * sin (second_parallel)) /
					(1 + data->e * sin (second_parallel))),
					(data->e / 2));

				data->tf = tan ((PI / 4) - (lat_origin / 2)) / 
					pow (((1 - data->e * sin (lat_origin)) /
					(1 + data->e * sin (lat_origin))),
					(data->e / 2));

				data->n = (log (data->m1) - log (data->m2)) / 
					(log (data->t1) - log (data->t2));
				
				data->sf = data->m1 / (data->n * pow (data->t1, data->n));

				data->rf = data->a * data->sf * pow (data->tf, data->n);
			} else {	
				lat_origin = sp_data [i].lat_origin * DEG2RAD;
				data->long_origin = sp_data [i].long_origin * DEG2RAD;
				data->scale_factor = sp_data [i].scale_factor;
				data->false_easting = sp_data [i].false_easting / METERTOFEET;	
				data->false_northing = sp_data [i].false_northing / METERTOFEET;

				data->a = ELLIPSE_A;
				data->b = ELLIPSE_B;

				data->ef = (data->a - data->b) / data->a;
				data->e_square = 2 * data->ef - pow (data->ef, 2.0);
				data->e = sqrt (data->e_square);
				data->e_prime_sq = data->e_square / (1 - data->e_square);
		
				temp1 = pow (data->e_square, 2.0);
				temp2 = pow (data->e_square, 3.0);

				data->a1 = (1.0 - (data->e_square / 4.0) - (3.0 * temp1 / 64.0) -
					(5.0 * temp2 / 256.0));

				data->a2 = ((3.0 * data->e_square / 8.0) + (3.0 * temp1 / 32.0) +
					(45.0 * temp2 / 1024.0));

				data->a3 = ((15.0 * temp1 / 256.0) + (45.0 * temp2 / 1024.0));
				
				data->a4 = (35.0 * temp2) / 3072.0;

				data->mo = data->a * 
					((data->a1 * lat_origin) - 
					(data->a2 * sin (2.0 * lat_origin)) +
					(data->a3 * sin (4.0 * lat_origin)) - 
					(data->a4 * sin (6.0 * lat_origin)));

				data->e1 = (1 - sqrt (1 - data->e_square)) / (1 + sqrt (1 - data->e_square));
            }
			break;

		case UTM:
			for (i=0; ; i++) {
				if (utm_data [i].code == 0) {
					if (Send_Messages ()) {
						exe->Error ("UTM Projection Code %d was Not Recognized", data->Code ());
					}
					return (false);
				}
				if (utm_data [i].code == data->Code ()) break;
			}
			data->index = i;

			lat_origin = utm_data [i].lat_origin * DEG2RAD;
			data->long_origin = utm_data [i].long_origin * DEG2RAD;
			data->scale_factor = utm_data [i].scale_factor;
			data->false_easting = utm_data [i].false_easting;	
			data->false_northing = utm_data [i].false_northing;

			data->a = ELLIPSE_A;
			data->b = ELLIPSE_B;

			data->ef = (data->a - data->b) / data->a;
			data->e_square = 2 * data->ef - pow (data->ef, 2.0);
			data->e = sqrt (data->e_square);
			data->e_prime_sq = data->e_square / (1 - data->e_square);
			
			temp1 = pow (data->e_square, 2.0);
			temp2 = pow (data->e_square, 3.0);

			data->a1 = (1.0 - (data->e_square / 4.0) - (3.0 * temp1 / 64.0) -
				(5.0 * temp2 / 256.0));

			data->a2 = ((3.0 * data->e_square / 8.0) + (3.0 * temp1 / 32.0) +
				(45.0 * temp2 / 1024.0));

			data->a3 = ((15.0 * temp1 / 256.0) + (45.0 * temp2 / 1024.0));
			
			data->a4 = (35.0 * temp2) / 3072.0;

			data->mo = data->a * 
				((data->a1 * lat_origin) - 
				(data->a2 * sin (2.0 * lat_origin)) +
				(data->a3 * sin (4.0 * lat_origin)) - 
				(data->a4 * sin (6.0 * lat_origin)));

			data->e1 = (1 - sqrt (1 - data->e_square)) / (1 + sqrt (1 - data->e_square));
			break;

		default:
			break;
	}
	data->Status (true);

	return (true);
}

//---------------------------------------------------------
//	 Retrieve the GIS projection string for the output projection
//---------------------------------------------------------

char * Projection_Service::Get_Projection_String (Projection_Data *data)
{
	double proj_factor = 0.0;
	char *unit_app, *unit_str;

	char *projcs1 = "PROJCS[\"NAD_1983_StatePlane_";
	char *projcs2 = "PROJCS[\"NAD_1983_UTM_Zone_";

	char *geogcs1 = "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\"";
	char *geogcs2 = "GEOGCS[\"GCS_North_American_1983\",DATUM[\"D_North_American_1983\"";

	char *speriod1 = "SPHEROID[\"WGS_1984\",6378137,298.257223563]]";
	char *speriod2 = "SPHEROID[\"GRS_1980\",6378137.0,298.257222101]]";

	char *primem = "PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";

	char *proj1 = "PROJECTION[\"Transverse_Mercator\"]"; 
	char *proj2 = "PROJECTION[\"Lambert_Conformal_Conic\"]";

	char *param1 = "PARAMETER[\"False_Easting\"";
	char *param2 = "PARAMETER[\"False_Northing\"";
	char *param3 = "PARAMETER[\"Central_Meridian\"";
	char *param4 = "PARAMETER[\"Scale_Factor\"";
	char *param5 = "PARAMETER[\"Latitude_Of_Origin\"";
	char *param6 = "PARAMETER[\"Standard_Parallel_1\"";
	char *param7 = "PARAMETER[\"Standard_Parallel_2\"";

	projcs [0] = '\0';

	if (data->Type () == LATLONG) {
		if (data->Units () == DEGREES) {
			str_fmt (projcs, sizeof (projcs), "%s,%s,%s", geogcs1, speriod1, primem);
		}
	} else if (data->index >= 0) {

		switch (data->Units ()) {
			default:
				proj_factor = 0.0;
				unit_app = unit_str = "";
				break;
			case FEET:
				proj_factor = 1.0 / METERTOFEET;
				unit_app = "_Feet";
				unit_str = "Foot_US";
				break;
			case METERS:
				proj_factor = 1.0;
				unit_app = "";
				unit_str = "Meter";
				break;
			case MILES:
				proj_factor = MILETOMETER;
				unit_app = "_Mile";
				unit_str = "Mile_US";
				break;
			case KILOMETERS:
				proj_factor = 1000.0;
				unit_app = "_Kilometer";
				unit_str = "Kilometer";
				break;
		}

		if (data->Type () == UTM) {
			UTM_Data *utm = utm_data + data->index;

			str_fmt (projcs, sizeof (projcs), 
				"%s%s%s\",%s,%s,%s,%s,%s,%g],%s,%g],%s,%g],%s,%g],%s,%g],UNIT[\"%s\",%g]]", 
				projcs2, utm->name, unit_app, 
				geogcs2, speriod2, primem, proj1, 
				param1, utm->false_easting / proj_factor, 
				param2, utm->false_northing / proj_factor,
				param3, utm->long_origin,
				param4, utm->scale_factor,
				param5, utm->lat_origin,
				unit_str, proj_factor);

		} else if (data->Type () == STATEPLANE) {
			SP_Data *sp = sp_data + data->index;

			if (sp->scale_factor == 0.0) {

				//---- Lambert Conformal Conic ----

				str_fmt (projcs, sizeof (projcs),
					"%s%s_FIPS_%04d%s\",%s,%s,%s,%s,%s,%g],%s,%g],%s,%g],%s,%g],%s,%g],%s,%g],UNIT[\"%s\",%g]]",
					projcs1, sp->name, sp->code, unit_app,
					geogcs2, speriod2, primem, proj2, 
					param1, sp->false_easting / METERTOFEET / proj_factor,
					param2, sp->false_northing / METERTOFEET / proj_factor,
					param3, sp->long_origin,
					param6, sp->first_parallel,
					param7, sp->second_parallel,
					param5, sp->lat_origin,
					unit_str, proj_factor);

			} else {

				//---- Transverse Mercator ----

				str_fmt (projcs, sizeof (projcs),
					"%s%s_FIPS_%04d%s\",%s,%s,%s,%s,%s,%g],%s,%g],%s,%g],%s,%g],%s,%g],UNIT[\"%s\",%g]]",
					projcs1, sp->name, sp->code, unit_app,
					geogcs2, speriod2, primem, proj1, 
					param1, sp->false_easting / METERTOFEET / proj_factor,
					param2, sp->false_northing / METERTOFEET / proj_factor,
					param3, sp->long_origin,
					param4, sp->scale_factor,
					param5, sp->lat_origin,
					unit_str, proj_factor);
			}
		}
	}
	return (projcs);
}

//---------------------------------------------------------
//	Type_Code
//---------------------------------------------------------

Projection_Data::Projection_Type  Projection_Service::Type_Code (char *text)
{
	for (int i=0; type_text [i] != NULL; i++) {
		if (str_cmp (text, type_text [i]) == 0) {
			return (type_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Projection Type %s", text);
	}
	return (Projection_Data::NO_PROJECTION);
}

char * Projection_Service::Type_Code (Projection_Data::Projection_Type code)
{
	for (int i=0; type_text [i] != NULL; i++) {
		if (code == type_code [i]) {
			return (type_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Projection Type %d", code);
	}
	return (NULL);
}

//---------------------------------------------------------
//	Units_Code
//---------------------------------------------------------

Projection_Data::Projection_Units  Projection_Service::Units_Code (char *text)
{
	for (int i=0; units_text [i] != NULL; i++) {
		if (str_cmp (text, units_text [i]) == 0) {
			return (units_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Projection Units %s", text);
	}
	return (Projection_Data::NO_UNITS);
}

char * Projection_Service::Units_Code (Projection_Data::Projection_Units code)
{
	for (int i=0; units_text [i] != NULL; i++) {
		if (code == units_code [i]) {
			return (units_text [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Projection Units %d", code);
	}
	return (NULL);
}
