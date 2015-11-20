//*********************************************************
//	Read_Location.cpp - Read the Activity Location File
//*********************************************************

#include "LocationData.hpp"

#include "In_Polygon.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Location
//---------------------------------------------------------

void LocationData::Read_Location (void)
{
	int i, j, field, code, id, count, num_in, num_out, weight, weight1, weight2, distance, runs;
	int x, y, zone, zone_field, time, nzone;
	double dx, dy, total, value;
	bool range_flag;

	Db_Index_Array *data;
	Location_Data *location_ptr;
	Access_Data *access_ptr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Link_Data *link_ptr;
	Subzone *sub_ptr;
	Subzone_Data *subzone_ptr;
	Zone_Map map_rec, *map_ptr;
	Index_Array *polygon;
	Boundary *ptr;
	Weight *weight_ptr, weight_rec;
	Subzone_Map subzone_map_rec, *subzone_map_ptr;
	Loc_Weight loc_weight_rec, *loc_weight_ptr;
	Zone_Field_Data *zone_data_ptr;

	num_in = num_out = nzone = 0;

	//---- initialize the location data array ----

	if (walk_access_flag && location_array == NULL) {
		location_array = new Location_Array ();
		if (!location_data.Max_Records (input_file->Num_Records ())) goto mem_error;
		if (subzone_map_flag) {
			if (!loc_weight.Max_Records (input_file->Num_Records ())) goto mem_error;
		}
	}
	range_flag = (zone_range.Num_Ranges () > 0);
	zone_field = input_file->Zone_Field ();

	//---- read the activity location file to count records ----

	Show_Message ("Reading %s -- Record", input_file->File_Type ());
	Set_Progress (1000);

	while (input_file->Read ()) {
		Show_Progress ();

		id = input_file->Location ();
		if (id == 0) continue;

		zone = input_file->Zone ();
		dx = input_file->X ();
		dy = input_file->Y ();
		x = Round (dx);
		y = Round (dy);

		//---- update the zone number ----

		if (zone_flag || coverage_flag) {
			if (zone_flag && (!range_flag || zone_range.In_Range (zone))) {

				//---- check current zone number ----

				ptr = (Boundary *) zone_boundary.Get (&zone);

				if (ptr == NULL || !In_Polygon (dx, dy, ptr->points)) {
					zone = -1;

					for (ptr = (Boundary *) zone_boundary.First (); ptr; ptr = (Boundary *) zone_boundary.Next ()) {
						if (In_Polygon (dx, dy, ptr->points)) {
							zone = ptr->zone;
							break;
						}
					}
					if (zone < 0) {
						zone = input_file->Zone ();
						Warning ("Location %d was not within a Zone Polygon", id);
					}
				}
			}
			map_rec.location = id;
			map_rec.zone = zone;

			if (!zone_map.Add (&map_rec)) {
				Error ("Adding Zone Map Record");
			}
		}

		//---- collect the activity location data ----

		if (walk_access_flag) {
			location_ptr = location_data.New_Record ();
			if (location_ptr == NULL) goto mem_error;

			location_ptr->Location (id);
			location_ptr->X (x);
			location_ptr->Y (y);
			location_ptr->Weight (0);
			location_ptr->Offset (0);
			location_ptr->Count (0);

			if (!location_data.Add ()) goto mem_error;
		}

		//---- count the records for the data file -----

		for (i=0; i < num_data_files; i++) {
			data = data_db [i];
			if (data == NULL) continue;

			field = location_field [i];

			if (zone_flag && field == zone_field) {
				id = zone;
			} else {
				input_file->Get_Field (field, &id);
			}
			if (id <= 0) continue;

			if (data->Get_Record (&id)) {
				data->Get_Field (2, &count);
				data->Put_Field (2, ++count);
				data->Write_Record ();
			}
		}

		//---- calculate the subzone distances ----

		if (subzone_map_flag) {

			//---- get a subzone data for coordinates ----

			for (i=0; i < num_subzone; i++) {
				sub_ptr = subzone [i];
				if (sub_ptr == NULL) continue;
				if (sub_ptr->location_field == 0 || sub_ptr->max_distance > 0) continue;
				break;
			}
			subzone_map_rec.zone = zone;
			subzone_map_rec.subzone = 0;

			//---- create working space for location weights ----

			loc_weight_rec.location = id;
			loc_weight_rec.weight = 0.0;

			if (!loc_weight.Add (&loc_weight_rec)) {
				Error ("Adding Location Weight Record");
			}
			if (zone > nzone) nzone = zone;

			//---- create a zone map ----

			if (!zone_flag && !coverage_flag) {
				map_rec.location = id;
				map_rec.zone = zone;

				if (!zone_map.Add (&map_rec)) {
					Error ("Adding Zone Map Record");
				}
			}

			//---- distance from the activity location to the subzone ----

			for (subzone_map_ptr = (Subzone_Map *) subzone_map.Get_GE (&subzone_map_rec); subzone_map_ptr; subzone_map_ptr = (Subzone_Map *) subzone_map.Next_Key ()) {
				if (subzone_map_ptr->zone != zone) break;
			
				weight_rec.subzone = subzone_map_ptr->subzone;
				weight_rec.location = id;

				if (subzone_weight.Get (&weight_rec) == NULL) {
					subzone_ptr = sub_ptr->data.Get (0, subzone_map_ptr->subzone);

					if (subzone_ptr == NULL) continue;

					dx = UnRound (subzone_ptr->X () - x);
					dy = UnRound (subzone_ptr->Y () - y);

					weight_rec.weight = sqrt (dx * dx + dy * dy);

					if (!subzone_weight.Add (&weight_rec)) {
						Error ("Adding Subzone Weight");
					}
				}
			}
		} else {
			for (i=0; i < num_subzone; i++) {
				sub_ptr = subzone [i];
				if (sub_ptr == NULL) continue;

				if (sub_ptr->location_field == 0 || sub_ptr->max_distance > 0) continue;

				if (zone > nzone) nzone = zone;

				for (subzone_ptr = sub_ptr->data.Get_GE (zone); subzone_ptr; subzone_ptr = sub_ptr->data.Next_Key ()) {
					if (subzone_ptr->Zone () != zone) break;

					weight_rec.subzone = subzone_ptr->ID ();
					weight_rec.location = id;

					if (subzone_weight.Get (&weight_rec) == NULL) {

						dx = UnRound (subzone_ptr->X () - x);
						dy = UnRound (subzone_ptr->Y () - y);

						weight_rec.weight = sqrt (dx * dx + dy * dy);

						if (!subzone_weight.Add (&weight_rec)) {
							Error ("Adding Subzone Weight");
						}
					}
				}
			}
		}
	}
	End_Progress ();

	input_file->Rewind ();

	//---- initialize the stop access locations ----

	if (walk_access_flag) {

		//---- calculate the number of runs serving each stop ----

		for (line_ptr = line_data.First (); line_ptr; line_ptr = line_data.Next ()) {
			for (i=1; i <= line_ptr->Stops (); i++) {
				stop_ptr = stop_data.Get (line_ptr->Stop (i));
				if (stop_ptr == NULL) continue;

				runs = line_ptr->Runs ();

				if (walk_time_flag) {
					count = 0;

					for (j=1; j <= runs; j++) {
						time = Resolve (line_ptr->Schedule (j, i));
						if (walk_time.Period (time) != 0) count++;
					}
					runs = count;
				}
				if (runs > 0) {
					stop_ptr->Add_Runs (runs);
				}
			}
		}

		//---- find activity locations with access to transit ----

		for (access_ptr = access_data.First (); access_ptr; access_ptr = access_data.Next ()) {
			if (access_ptr->From_Type () == LOCATION_ID && access_ptr->To_Type () == STOP_ID) {
				location_ptr = location_data.Get (access_ptr->From_ID ());
				stop_ptr = stop_data.Get (access_ptr->To_ID ());

				if (location_ptr != NULL && stop_ptr != NULL) {
					if (stop_ptr->Num_Runs () > 0) {
						distance = walk_distance - Resolve (access_ptr->Time ());

						if (distance > 0) {
							location_ptr->Add_Weight (distance * stop_ptr->Num_Runs ());
							location_ptr->Add_Offset (distance);
							location_ptr->Add_Count (1);
						}
					}
				}
			}
		}

		//---- normalize the location weights ----

		for (location_ptr = location_data.First (); location_ptr; location_ptr = location_data.Next ()) {
			if (location_ptr->Count () > 0) {
				distance = (location_ptr->Offset () + location_ptr->Count () / 2) / location_ptr->Count ();

				location_ptr->Offset (walk_distance - distance);

				if (distance > 0) {
					weight = (location_ptr->Weight () + distance / 2) / distance;

					location_ptr->Weight (weight);
				}
			}
		}
	}

	//---- calculate the subzone weights ----

	if (nzone > 0) {
		if (subzone_map_flag) {
			Show_Message ("Calculating Subzone Allocation Weights -- Zone");
			Set_Progress (100);

			for (zone=1; zone <= nzone; zone++) {
				Show_Progress ();

				subzone_map_rec.zone = zone;
				subzone_map_rec.subzone = 0;

				for (subzone_map_ptr = (Subzone_Map *) subzone_map.Get_GE (&subzone_map_rec); subzone_map_ptr; subzone_map_ptr = (Subzone_Map *) subzone_map.Next_Key ()) {
					if (subzone_map_ptr->zone != zone) break;
				
					weight_rec.subzone = subzone_map_ptr->subzone;
					weight_rec.location = 0;
					
					dx = -1.0;

					for (weight_ptr = (Weight *) subzone_weight.Get_GE (&weight_rec); weight_ptr; weight_ptr = (Weight *) subzone_weight.Next_Key ()) {
						if (weight_ptr->subzone != weight_rec.subzone) break;
						if (weight_ptr->weight > dx) dx = weight_ptr->weight;
					}
					if (dx < 0.0) continue;
					dy = 0.0;

					for (weight_ptr = (Weight *) subzone_weight.Get_GE (&weight_rec); weight_ptr; weight_ptr = (Weight *) subzone_weight.Next_Key ()) {
						if (weight_ptr->subzone != weight_rec.subzone) break;
						weight_ptr->weight = dx - weight_ptr->weight;
						if (weight_ptr->weight < 1.0) weight_ptr->weight = 1.0;
						dy += weight_ptr->weight;
					}
					for (weight_ptr = (Weight *) subzone_weight.Get_GE (&weight_rec); weight_ptr; weight_ptr = (Weight *) subzone_weight.Next_Key ()) {
						if (weight_ptr->subzone != weight_rec.subzone) break;
						weight_ptr->weight /= dy;
					}
				}
			}
			End_Progress ();

			Show_Message ("Distribute Zone Attributes -- Zone");
			Set_Progress (100);

			for (i=0; i < num_subzone; i++) {
				sub_ptr = subzone [i];
				if (sub_ptr == NULL) continue;

				if (sub_ptr->location_field == 0 || sub_ptr->max_distance > 0) continue;
				Set_Progress (100);

				//---- process each zone ----

				for (zone_data_ptr = (Zone_Field_Data *) sub_ptr->zone_field_data.First_Key (); zone_data_ptr; zone_data_ptr = (Zone_Field_Data *) sub_ptr->zone_field_data.Next_Key ()) {
					Show_Progress (zone_data_ptr->zone);
		
					if (zone_data_ptr->data == 0) continue;

					//---- zero the location weights ----

					for (loc_weight_ptr = (Loc_Weight *) loc_weight.First (); loc_weight_ptr; loc_weight_ptr = (Loc_Weight *) loc_weight.Next ()) {
						loc_weight_ptr->weight = 0.0;
					}
					total = 0.0;
					subzone_map_rec.zone = zone = zone_data_ptr->zone;
					subzone_map_rec.subzone = 0;

					for (subzone_map_ptr = (Subzone_Map *) subzone_map.Get_GE (&subzone_map_rec); subzone_map_ptr; subzone_map_ptr = (Subzone_Map *) subzone_map.Next_Key ()) {
						if (subzone_map_ptr->zone != zone) break;

						subzone_ptr = sub_ptr->data.Get (0, subzone_map_ptr->subzone);
						if (subzone_ptr == 0 || subzone_ptr->Data () == 0) continue;

						weight = (int) (subzone_ptr->Data () * subzone_map_ptr->factor + 0.5);
						if (weight == 0) continue;

						weight_rec.subzone = subzone_map_ptr->subzone;
						weight_rec.location = 0;
		
						for (weight_ptr = (Weight *) subzone_weight.Get_GE (&weight_rec); weight_ptr; weight_ptr = (Weight *) subzone_weight.Next_Key ()) {
							if (weight_ptr->subzone != weight_rec.subzone) break;

							map_ptr = (Zone_Map *) zone_map.Get (&weight_ptr->location);
							if (map_ptr == NULL || map_ptr->zone != zone) continue;

							loc_weight_ptr = (Loc_Weight *) loc_weight.Get (&weight_ptr->location);
							if (loc_weight_ptr == 0) continue;

							value = weight_ptr->weight * weight;

							total += value;
							loc_weight_ptr->weight += value;
						}
					}

					//---- use distance weights ----

					if (total <= 0.01) {
						for (subzone_map_ptr = (Subzone_Map *) subzone_map.Get_GE (&subzone_map_rec); subzone_map_ptr; subzone_map_ptr = (Subzone_Map *) subzone_map.Next_Key ()) {
							if (subzone_map_ptr->zone != zone) break;

							if (subzone_map_ptr->factor == 0.0) continue;

							weight_rec.subzone = subzone_map_ptr->subzone;
							weight_rec.location = 0;
			
							for (weight_ptr = (Weight *) subzone_weight.Get_GE (&weight_rec); weight_ptr; weight_ptr = (Weight *) subzone_weight.Next_Key ()) {
								if (weight_ptr->subzone != weight_rec.subzone) break;

								map_ptr = (Zone_Map *) zone_map.Get (&weight_ptr->location);
								if (map_ptr == NULL || map_ptr->zone != zone) continue;

								loc_weight_ptr = (Loc_Weight *) loc_weight.Get (&weight_ptr->location);
								if (loc_weight_ptr == 0) continue;

								value = weight_ptr->weight * subzone_map_ptr->factor;

								total += value;
								loc_weight_ptr->weight += value;
							}
						}
					}

					//---- normalize the weight ----

					if (total > 0.0) {
						value = 0.5;

						for (loc_weight_ptr = (Loc_Weight *) loc_weight.First (); loc_weight_ptr; loc_weight_ptr = (Loc_Weight *) loc_weight.Next ()) {
							if (loc_weight_ptr->weight == 0.0) continue;

							loc_weight_ptr->weight *= zone_data_ptr->data / total;
							weight = (int) (loc_weight_ptr->weight += value);
							value = loc_weight_ptr->weight - weight;
							if (weight > 0) {
								loc_weight_ptr->weight = weight;
								sub_ptr->loc_weight.Add (loc_weight_ptr);
							}
						}
					}
				}
			}
			End_Progress ();
		} else {
			for (i=0; i < num_subzone; i++) {
				sub_ptr = subzone [i];
				if (sub_ptr == NULL) continue;

				if (sub_ptr->location_field == 0 || sub_ptr->max_distance > 0) continue;

				Show_Message ("Calculating Subzone Allocation Weights -- Zone");
				Set_Progress (100);

				for (zone=1; zone <= nzone; zone++) {
					Show_Progress ();

					for (subzone_ptr = sub_ptr->data.Get_GE (zone); subzone_ptr; subzone_ptr = sub_ptr->data.Next_Key ()) {
						if (subzone_ptr->Zone () != zone) break;

						weight_rec.subzone = subzone_ptr->ID ();
						weight_rec.location = 0;
						
						dx = -1.0;

						for (weight_ptr = (Weight *) subzone_weight.Get_GE (&weight_rec); weight_ptr; weight_ptr = (Weight *) subzone_weight.Next_Key ()) {
							if (weight_ptr->subzone != weight_rec.subzone) break;
							if (weight_ptr->weight > dx) dx = weight_ptr->weight;
						}
						if (dx < 0.0) continue;
						dy = 0.0;

						for (weight_ptr = (Weight *) subzone_weight.Get_GE (&weight_rec); weight_ptr; weight_ptr = (Weight *) subzone_weight.Next_Key ()) {
							if (weight_ptr->subzone != weight_rec.subzone) break;
							weight_ptr->weight = dx - weight_ptr->weight;
							if (weight_ptr->weight < 1.0) weight_ptr->weight = 1.0;
							dy += weight_ptr->weight;
						}
						for (weight_ptr = (Weight *) subzone_weight.Get_GE (&weight_rec); weight_ptr; weight_ptr = (Weight *) subzone_weight.Next_Key ()) {
							if (weight_ptr->subzone != weight_rec.subzone) break;
							weight_ptr->weight /= dy;
						}
					}
				}
				End_Progress ();
				break;
			}
		}
	}

	//---- copy the activity locations to the output file ----

	Show_Message ("Processing Activity Locations -- Record");
	Set_Progress (1000);

	while (input_file->Read ()) {
		Show_Progress ();

		id = input_file->Location ();
		if (id == 0) continue;

		num_in++;

		output_file->Copy_Fields (input_file);

		//---- get the zone number ----

		if (zone_flag) {
			map_ptr = (Zone_Map *) zone_map.Get (&id);

			if (map_ptr == NULL) {
				zone = input_file->Zone ();
			} else {
				zone = map_ptr->zone;
			}
		} else {
			zone = input_file->Zone ();
		}

		//---- copy standard field types -----

		output_file->Location (id);
		output_file->Link (input_file->Link ());
		output_file->Node (input_file->Node ());
		output_file->Offset (input_file->Offset ());
		output_file->X (input_file->X ());
		output_file->Y (input_file->Y ());
		output_file->Z (input_file->Z ());
		output_file->Zone (zone);

		//---- set the link use flag field ----

		if (use_flag) {
			link_ptr = link_data.Get (output_file->Link ());

			field = use_field.First ();
			code = use_code.First ();

			for (; field > 0; ) {
				if ((link_ptr->Use () & code) > 0) {
					output_file->Put_Field (field, 1);
				} else {
					output_file->Put_Field (field, 0);
				}
				field = use_field.Next ();
				code = use_code.Next ();
			}
		}

		//---- calculate the walk access field ----

		if (walk_access_flag) {
			link_ptr = link_data.Get (output_file->Link ());
			weight = 0;

			//---- check for walk permissions ----

			if (Use_Permission (link_ptr->Use (), WALK)) {
				location_ptr = location_data.Get (id);
				x = location_ptr->X ();
				y = location_ptr->Y ();

				//---- find the best transit access points ----

				weight1 = weight2 = 0;

				for (location_ptr = location_data.First (); location_ptr; location_ptr = location_data.Next ()) {
					if (location_ptr->Weight () == 0 || location_ptr->Count () == 0) continue;

					dx = UnRound (location_ptr->X () - x);
					dy = UnRound (location_ptr->Y () - y);

					distance = (int) (sqrt (dx * dx + dy * dy) + 0.5);

					distance += location_ptr->Offset ();

					if (distance < walk_distance) {
						weight = location_ptr->Weight () * (walk_distance - distance);
						if (weight > weight1) {
							weight2 = weight1;
							weight1 = weight;
						} else if (weight > weight2) {
							weight2 = weight;
						}
					}
				}
				weight = (weight1 + weight2 + 1) / 2;
			}
			output_file->Put_Field (walk_access_field, weight);
		}

		//---- calculate the subzone fields ----

		for (i=0; i < num_subzone; i++) {
			sub_ptr = subzone [i];
			if (sub_ptr == NULL) continue;

			if (sub_ptr->location_field == 0) continue;

			zone = output_file->Zone ();
			output_file->Put_Field (sub_ptr->location_field, 0);

			//---- allocate subzone data ----

			if (subzone_map_flag) {
				loc_weight_ptr = (Loc_Weight *) sub_ptr->loc_weight.Get (&id);
				if (loc_weight_ptr != NULL) {
					weight = (int) (loc_weight_ptr->weight + 0.5);
				} else {
					weight = 0;
				}

			} else if (sub_ptr->max_distance == 0) {
				weight = 0;

				for (subzone_ptr = sub_ptr->data.Get_GE (zone); subzone_ptr; subzone_ptr = sub_ptr->data.Next_Key ()) {
					if (subzone_ptr->Zone () != zone) break;

					weight_rec.subzone = subzone_ptr->ID ();
					weight_rec.location = id;

					weight_ptr = (Weight *) subzone_weight.Get (&weight_rec);
					if (weight_ptr != NULL && subzone_ptr->Data () > 0) {
						weight += (int) (subzone_ptr->Data () * weight_ptr->weight + 0.5);
					}
				}
				if (weight < 0) weight = 0;

			} else {

				//---- distance weighted attribute ----

				x = Round (output_file->X ());
				y = Round (output_file->Y ());

				weight1 = weight2 = 0;

				for (subzone_ptr = sub_ptr->data.Get_GE (zone); subzone_ptr; subzone_ptr = sub_ptr->data.Next_Key ()) {
					if (subzone_ptr->Zone () != zone) break;

					dx = UnRound (subzone_ptr->X () - x);
					dy = UnRound (subzone_ptr->Y () - y);

					distance = (int) (sqrt (dx * dx + dy * dy) + 0.5);

					if (distance < sub_ptr->max_distance) {
						weight = subzone_ptr->Data () * (sub_ptr->max_distance - distance);
						if (weight > weight1) {
							weight2 = weight1;
							weight1 = weight;
						} else if (weight > weight2) {
							weight2 = weight;
						}
					}
				}
				weight = (weight1 + weight2 + 1) / 2;
			}
			output_file->Put_Field (sub_ptr->location_field, weight);
		}

		//---- set the data field id ----

		if (script_flag) {

			for (i=0; i < num_data_files; i++) {
				data = data_db [i];
				if (data == NULL) continue;

				field = location_field [i];

				if (zone_flag && field == zone_field) {
					id = output_file->Zone ();
				} else {
					input_file->Get_Field (field, &id);
				}

				if (!data->Get_Record (&id)) {
					data->Reset_Record ();
				}
			}

			//---- set the polygon record index ----

			if (polygon_flag) {
				dx = input_file->X ();
				dy = input_file->Y ();
			}

			for (i=0; i < num_polygons; i++) {
				data = polygon_db [i];
				if (data == NULL) continue;

				polygon = polygons [i];

				//---- find the polygon index ----

				id = 0;

				for (ptr = (Boundary *) polygon->First (); ptr; ptr = (Boundary *) polygon->Next ()) {
					if (In_Polygon (dx, dy, ptr->points)) {
						id = ptr->zone;
						break;
					}
				}
				if (!id || !data->Get_Record (&id)) {
					data->Reset_Record ();
				}
			}

			//---- execute the conversion script ----

			if (program.Execute (num_in) == 0) continue;
		}

		//---- save the output fields ----

		if (!output_file->Write ()) {
			Error ("Writing %s Record %d", output_file->File_Type (), id);
		}
		num_out++;
	}
	End_Progress ();

	input_file->Close ();
	output_file->Close ();

	Write (2, "Number of Activity Location Records Read = %d", num_in);
	Write (1, "Number of Activity Location Records Written = %d", num_out);
	return;

mem_error:
	Error ("Insufficient Memory for Activity Location Data");
}
