//*********************************************************
//	Write_Image.cpp - write the arcview traffic image
//*********************************************************

#include "ArcDelay.hpp"

#include "Vehicle_Shape.hpp"

#define MAX_LANE	20
#define MAX_CELL	2000
#define MAX_TYPE	20

//---------------------------------------------------------
//	Write_Image
//---------------------------------------------------------

void ArcDelay::Write_Image (void)
{
	int lane, nlanes, center, dir, end, period, nperiods, volume, type, ntype, start, right, left;
	int cell_size, half_cell, cell, cells, ncells, tod, rec, seconds, count, num_veh, spacing, space;
	int lane_cells;
	double ttime, density, speed, length, side, veh_width, offset, start_offset, share, vehicles, total, veh_len;
	char start_time [FIELD_BUFFER], end_time [FIELD_BUFFER], buffer [STRING_BUFFER];
	bool version3_flag, offset_flag, distrib_flag, first;

	int cell_grid [MAX_LANE] [MAX_CELL];
	int lane_use [MAX_LANE], veh_length [MAX_TYPE], veh_cells [MAX_TYPE];
	double lane_veh [MAX_TYPE], lane_share [MAX_LANE] [MAX_TYPE];

	XYZ_Point pt1, pt2;
	Use_Type use_type [MAX_TYPE];
	Image_Data *image_ptr;

	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Pocket_Data *pocket_ptr;
	Pocket_Index index_rec, *index_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;
	Veh_Type_Data *veh_type_ptr;

	version3_flag = (delay_file->Dbase_Format () == VERSION3);
	offset_flag = (link_offset != 0.0);

	//---- process the vehicle type data ----

	type = ntype = 1;
	veh_width = lane_width * 0.80;
	cell_size = 1000;

	for (veh_type_ptr = veh_type_data.First (); veh_type_ptr; veh_type_ptr = veh_type_data.Next ()) {
		type = veh_type_ptr->Type ();
		veh_length [type] = veh_type_ptr->Length ();
		use_type [type] = veh_type_ptr->Use ();
		if (type > ntype) ntype = type;

		if (veh_length [type] < cell_size) {
			cell_size = veh_length [type];
			half_cell = (cell_size + 1) / 2;
		}
	}
	for (type = 1; type <= ntype; type++) {
		veh_cells [type] = (veh_length [type] + half_cell) / cell_size;
	}

	//---- process each time period ----

	nperiods = time_period.Num_Ranges ();

	for (period = 1; period <= nperiods; period++) {

		range_ptr = time_period [period];

		end = range_ptr->High () + 1;
		seconds = end - range_ptr->Low ();

		tod = Round ((range_ptr->Low () + end) / 2);

		str_cpy (start_time, sizeof (start_time), time_period.Format_Step (range_ptr->Low ()));
		str_cpy (end_time, sizeof (end_time), time_period.Format_Step (end));

		//---- create a each shape file ----

		first = true;

		for (image_ptr = (Image_Data *) image_data.First (); image_ptr; image_ptr = (Image_Data *) image_data.Next ()) {
			str_fmt (buffer, sizeof (buffer), "%s_%s.shp", image_ptr->name, time_period.Range_Label (period));
			Print (1);

			if (!image_ptr->file->Open (buffer)) {
				File_Error ("Opening ArcView Traffic Image", image_ptr->file->Shape_Filename ());
			}
			image_ptr->file->Write_Header ();

			if (first) {
				Show_Message ("Writing %s %s -- Link", image_ptr->file->File_Type (), end_time);
				first = false;
			}
		}
		Set_Progress (1000);

		//---- process each link ----

		for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {
			Show_Progress ();

			length = UnRound (link_ptr->Length ());

			//---- process each direction ----

			for (dir = 0; dir < 2; dir++) {

				if (dir == 1) {
					perf_ptr = performance_data [link_ptr->BA_Dir ()];
				} else {
					perf_ptr = performance_data [link_ptr->AB_Dir ()];
				}
				if (perf_ptr == NULL || perf_ptr->Periods () == 0) continue;

				//---- save the link direction data ----

				if (delay_file->LinkDir_Type () == LINK_SIGN) {
					if (dir == 1) {
						delay_file->Link (-link_ptr->Link ());
					} else {
						delay_file->Link (link_ptr->Link ());
					}
				} else {
					delay_file->Link (link_ptr->Link ());	

					if (delay_file->LinkDir_Type () == LINK_NODE) {
						delay_file->Dir ((dir == 1) ? link_ptr->Bnode () : link_ptr->Anode ());
					} else {
						delay_file->Dir (dir);
					}
				}

				//---- calculate the performance ----

				volume = perf_ptr->Volume (period);
				ttime = perf_ptr->TTime (period);
				speed = perf_ptr->Avg_Speed (period);
				density = perf_ptr->Density (period);

				//---- save the Version 3 fields ----

				if (version3_flag) {

					delay_file->Time (end);
					delay_file->Lane (perf_ptr->Left () + 1);
					delay_file->Turn (0);

					delay_file->Volume (volume);
					delay_file->TTime (volume * ttime);
					delay_file->TTime2 (volume * ttime * ttime);

					delay_file->Vehicles ((int) (density + 0.5));
					delay_file->Speed (density * speed);
					delay_file->Speed2 (density * speed * speed);

				} else {

					delay_file->Start_Time (start_time);
					delay_file->End_Time (end_time);

					delay_file->Volume (volume);
					delay_file->Enter (perf_ptr->Enter (period));
					delay_file->Exit (perf_ptr->Exit (period));

					delay_file->Speed (speed);
					delay_file->TTime (ttime);
					delay_file->Delay (perf_ptr->Delay (period));

					delay_file->Density (density);
					delay_file->Max_Density (perf_ptr->Max_Density (period));

					delay_file->Time_Ratio (perf_ptr->Time_Ratio (period));
					delay_file->Avg_Queue (perf_ptr->Queue (period));
					delay_file->Max_Queue (perf_ptr->Max_Queue (period));
					delay_file->Cycle_Failure (perf_ptr->Failure (period));

					delay_file->VMT (perf_ptr->Distance (period));
					delay_file->VHT (perf_ptr->Time (period));
				}
				delay_file->Num_Connect (0);

				//---- set the grid cells ----

				memset (cell_grid, '\0', sizeof (cell_grid));

				if (dir == 0) {
					start_offset = UnRound (link_ptr->Boffset ());
					start = (link_ptr->Boffset () + half_cell) / cell_size + 1;
					ncells = (link_ptr->Length () - link_ptr->Aoffset () + half_cell) / cell_size - 1;
				} else {
					start_offset = UnRound (link_ptr->Aoffset ());
					start = (link_ptr->Aoffset () + half_cell) / cell_size + 1;
					ncells = (link_ptr->Length () - link_ptr->Boffset () + half_cell) / cell_size - 1;
				}
				if (ncells <= start) ncells = start + 1;
				if (ncells >= MAX_CELL) ncells = MAX_CELL - 1;

				//---- initialize the pocket lanes and access restrictions ----

				left = perf_ptr->Left ();
				right = perf_ptr->Right ();

				nlanes = left + perf_ptr->Thru () + right;
				
				right = nlanes - right;
				if (nlanes >= MAX_LANE) nlanes = MAX_LANE;

				for (lane=1; lane <= nlanes; lane++) {
					if (lane <= left || lane > right) {
						for (cell=start; cell <= ncells; cell++) {
							cell_grid [lane] [cell] = -1;
						}
					}
					lane_use [lane] = link_ptr->Use ();
				}

				//---- retrieve pocket lane information ----

				index_rec.link_dir = perf_ptr->Link_Dir ();
				index_rec.pocket_id = 0;

				for (index_ptr = (Pocket_Index *) pocket_index.Get_GE (&index_rec);
					index_ptr != NULL && index_ptr->link_dir == index_rec.link_dir;
					index_ptr = (Pocket_Index *) pocket_index.Next_Key ()) {

					pocket_ptr = pocket_data.Get (index_ptr->pocket_id);
					if (pocket_ptr == NULL) continue;

					lane = pocket_ptr->Lane ();
					if ((lane > left && lane <= right) || lane > nlanes) continue;

					cells = pocket_ptr->Length () / cell_size;

					if (pocket_ptr->Type () == TURN_LANE) {
						cell = start;
						cells = MIN (start + cells - 1, ncells);
					} else if (pocket_ptr->Type () == MERGE_LANE) {
						cell = MAX (ncells - cells, start);
						cells = ncells;
					} else {
						cell = (pocket_ptr->Offset () + half_cell) / cell_size;
						cell = MAX (ncells - cell, start);
						cells = MIN (cell + cells, ncells);
					}
					for (; cell <= cells; cell++) {
						cell_grid [lane] [cell] = 0;
					}
				}

				//---- set lane use restrictions ----

				for (rec = perf_ptr->TOD_List (); rec; rec = use_ptr->TOD_List ()) {
					use_ptr = link_use_data [rec]; 

					if (use_ptr->Offset () > 0 || use_ptr->Length () > 0) continue;

					if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
						for (lane=use_ptr->Low_Lane (); lane <= use_ptr->High_Lane (); lane++) {
							if (lane > 0 && lane <= nlanes) {
								lane_use [lane] = use_ptr->Use ();
							}
						}
					}
				}

				//---- count cells per lane ----

				for (lane = 1; lane <= nlanes; lane++) {
					count = 0;

					for (cell = 1; cell <= ncells; cell++) {
						if (cell_grid [lane] [cell] == 0) count++;
					}
					cell_grid [lane] [0] = count;
				}

				//---- find the center point ----

				if (center_flag) {
					if (link_ptr->BA_Dir () == 0 || link_ptr->AB_Dir () == 0) {
						center = nlanes + 1;
					} else {
						center = 1;
					}
				} else {
					center = 1;
				}

				//---- allocate the vehicles types to lanes ----

				memset (lane_share, '\0', sizeof (lane_share));
				total = 0.0;
					
				for (type = 1; type <= ntype; type++) {
					count = type_share [type];
					if (count == 0) continue;
					share = count;
					cells = 0;

					for (lane = 1; lane <= nlanes; lane++) {
						if (Use_Permission (lane_use [lane], use_type [type])) {
							cells += cell_grid [lane] [0];
						}
					}
					if (cells == 0) continue;

					for (lane = 1; lane <= nlanes; lane++) {
						if (Use_Permission (lane_use [lane], use_type [type])) {
							density = share * cell_grid [lane] [0] / cells;

							lane_share [lane] [type] = density;
							total += density;
						}
					}
				}

				//---- normalize the vehicle shares ----

				if (total > 0.0) {
					share = 1.0 / total;

					for (lane = 1; lane <= nlanes; lane++) {
						for (type = 1; type <= ntype; type++) {
							lane_share [lane] [type] *= share;
						}
					}
				}

				//---- process each attribute ----

				for (image_ptr = (Image_Data *) image_data.First (); image_ptr; 
					image_ptr = (Image_Data *) image_data.Next ()) {

					//---- check the attribute ----

					switch (image_ptr->type) {
						case VOLUME_DATA:
							distrib_flag = true;
							vehicles = perf_ptr->Volume (period) * perf_ptr->TTime (period) / seconds;
							break;
						case DENSITY_DATA:
							distrib_flag = true;
							vehicles = perf_ptr->Density (period) * perf_ptr->Thru () * length / 1000.0;
							break;
						case MAX_DENSITY_DATA:
							distrib_flag = true;
							vehicles = perf_ptr->Max_Density (period) * perf_ptr->Thru () * length / 1000.0;
							break;
						case QUEUE_DATA:
							distrib_flag = false;
							vehicles = perf_ptr->Queue (period);
							break;
						case MAX_QUEUE_DATA:
							distrib_flag = false;
							vehicles = perf_ptr->Max_Queue (period);
							break;
						case FAILURE_DATA:
							distrib_flag = false;
							vehicles = perf_ptr->Failure (period) * 100.0 / seconds;  //---- assume 100 second cycle ----
	//						vehicles = perf_ptr->Failure (period);
							break;
					}
					num_veh = (int) (vehicles + 0.9);
					if (num_veh <= 0) continue;

					//---- copy the data fields ----

					image_ptr->file->Copy_Fields (delay_file);

					//---- assign the vehicles to lanes ---

					for (lane = 1; lane <= nlanes; lane++) {
						if (cell_grid [lane] [0] == 0) continue;

						//---- calculate the lane volume ----

						total = 0.0;
						lane_cells = 0;
						memset (lane_veh, '\0', sizeof (lane_veh));

						for (type = 1; type <= ntype; type++) {
							total += share = lane_share [lane] [type] * vehicles;

							lane_veh [type] = share;
							lane_cells += (int) (share * veh_cells [type] + 0.5);
						}
						volume = (int) (total + 0.5);
						if (volume == 0) continue;

						lane_veh [0] = total;

						side = (2 * lane - center) * lane_width / 2.0;

						//---- calculate vehicle spacing ----

						if (distrib_flag) {
							if (lane_cells > 0) {
								spacing = cell_grid [lane] [0] / lane_cells;
								if (spacing > 2) spacing = 2;

								cells = lane_cells + (volume - 1) * spacing;
								density = (double) cells / cell_grid [lane] [0];
							} else {
								spacing = 0;
								density = 1.0;
							}
							space = spacing;
						}

						//---- assign vehicle types to available cells ----

						for (cell = start; cell <= ncells && volume > 0; cell++) {
							if (cell_grid [lane] [cell] != 0) continue;

							if (distrib_flag) {
								if (spacing > 0) {
									if (space < spacing) {
										space++;
										continue;
									}
								}
								if (random.Probability () > density) continue;
								space = 0;
							}
							total = 0.0;
							share = lane_veh [0] * random.Probability ();

							for (type = 1; type <= ntype; type++) {
								total += lane_veh [type];
								if (share < total) break;
							}

							//---- draw the vehicle ----

							if (shape_flag) {
								veh_len = UnRound (veh_length [type]);

								offset = (cell - 1) * UnRound (cell_size) + veh_len;

								if (dir == 0) {
									offset = length - offset;
								}
								Link_Shape (link_ptr, dir, &points, offset, veh_len, side);

								pt1 = *(points [points.Num_Points ()]);
								pt2 = *(points [1]);

								Vehicle_Shape (pt1, pt2, veh_width, &image_ptr->file->points);
							} else {
								offset = (cell - 1) * UnRound (cell_size) + 1.5;

								if (dir == 0) {
									offset = length - offset;
								}
								Link_Shape (link_ptr, dir, &points, offset, 0.0, side);

								image_ptr->file->points.Set (1, points [1]);
							}

							//---- save the vehicle image ----

							if (!image_ptr->file->Write_Record ()) {
								Error ("Writing ArcView Traffic Image");
							}
							volume--;
							cell += veh_cells [type] - 1;
						}
					}
				}
			}
		}
		End_Progress ();

		//---- close the files ----

		for (image_ptr = (Image_Data *) image_data.First (); image_ptr; image_ptr = (Image_Data *) image_data.Next ()) {
			image_ptr->file->Close ();
		}
	}
}
