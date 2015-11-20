//*********************************************************
//	Write_Veh_Type.cpp - convert the vehicle type data
//*********************************************************

#include "VISSIMNet.hpp"

//---------------------------------------------------------
//	Write_Link
//---------------------------------------------------------

void VISSIMNet::Write_Vehicle_Types (void)
{
	int type;
	char *name;

	Veh_Type_Data *type_ptr;

	//---- write the vehicle types ----

	fprintf (file, "\n<VEHTYPES>");

	for (type_ptr = veh_type_data.First (); type_ptr; type_ptr = veh_type_data.Next ()) {
		type = type_ptr->Use ();

		name = Veh_Use_Code ((Use_Type) type);

		fprintf (file, "\n<VEHTYPE NO=\"%d%d\" NAME=\"%s\" ", 
			type_ptr->Type (), type_ptr->Sub_Type (), name);

		if (type == BUS) {
			name = "Bus";
		} else if (type == RAIL) {
			name = "Tram";
		} else if (type == TRUCK || type == LIGHTTRUCK || type == HEAVYTRUCK) {
			name = "HGV";
		} else {
			name = "Car";
		}
		fprintf (file, "VEHCATEGORY=\"%s\" />", name);
	}
	fprintf (file, "\n</VEHTYPES>");

	//---- write the vehicle classes ----

	fprintf (file, "\n<VEHCLASSES>");

	//---- auto vehicles ----
			
	fprintf (file, "\n<VEHCLASS ID=\"C\">");

	for (type_ptr = veh_type_data.First (); type_ptr; type_ptr = veh_type_data.Next ()) {
		type = type_ptr->Use ();

		if (type == CAR || type == SOV || type == HOV2 || type == HOV3 || 
			type == HOV4 || type == TAXI) {

			fprintf (file, "\n<VEHTYPEID NO=\"%d%d\" />", 
				type_ptr->Type (), type_ptr->Sub_Type ());
		}
	}
	fprintf (file, "\n</VEHCLASS>");

	//---- transit vehicles ----
			
	fprintf (file, "\n<VEHCLASS ID=\"PtDefault\">");

	for (type_ptr = veh_type_data.First (); type_ptr; type_ptr = veh_type_data.Next ()) {
		type = type_ptr->Use ();

		if (type == BUS || type == RAIL) {
			fprintf (file, "\n<VEHTYPEID NO=\"%d%d\" />", 
				type_ptr->Type (), type_ptr->Sub_Type ());
		}
	}
	fprintf (file, "\n</VEHCLASS>");

	//---- truck vehicles ----
			
	fprintf (file, "\n<VEHCLASS ID=\"T\">");

	for (type_ptr = veh_type_data.First (); type_ptr; type_ptr = veh_type_data.Next ()) {
		type = type_ptr->Use ();

		if (type == TRUCK || type == LIGHTTRUCK || type == HEAVYTRUCK) {
			fprintf (file, "\n<VEHTYPEID NO=\"%d%d\" />", 
				type_ptr->Type (), type_ptr->Sub_Type ());
		}
	}
	fprintf (file, "\n</VEHCLASS>");

	fprintf (file, "\n</VEHCLASSES>");
}
