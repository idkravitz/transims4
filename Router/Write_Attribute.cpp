//*********************************************************
//	Write_Attribute.cpp - write the attribute record
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Write_Attribute
//---------------------------------------------------------

void Router::Write_Attribute (void)
{
	if (attribute_flag && attribute_data.traveler > 0) {
		if (wait_leg_flag) {
			attribute_data.transit -= (attribute_data.first_wait + attribute_data.xfer_wait);
		} else {
			attribute_data.walk -= (attribute_data.first_wait + attribute_data.xfer_wait);
		}
		fprintf (attribute_file.File (), "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", attribute_data.traveler, attribute_data.trip,
			Resolve (attribute_data.impedance), Resolve (attribute_data.walk), Resolve (attribute_data.first_wait), 
			Resolve (attribute_data.xfer_wait), (attribute_data.num_xfer-1), Resolve (attribute_data.penalty), 
			Resolve (attribute_data.transit), Resolve (attribute_data.drive), attribute_data.cost);

		memset (&attribute_data, '\0', sizeof (attribute_data));
	}
}
