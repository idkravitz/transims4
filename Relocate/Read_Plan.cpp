//*********************************************************
//	Read_Plan.cpp - Read the Plan File
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

bool Relocate::Read_Plan (int file_num)
{
	int hhold, last_hh, num_out, num_update, num_hhold, location;
	int *path, num_path, link, dir, offset, offset2, p1, p2, length;
	char buffer [FIELD_BUFFER];
	bool flag, park_flag;

	Integer_List node_list, flip_list;

	Map_Data *map_ptr, *park_ptr;
	Parking_Data *parking_ptr;
	Link_Data *link_ptr, *park_link, *park_link2;
	AB_Data *ab_ptr;
		
	if (file_num > 0) {
		if (!plan_file.Open (file_num)) return (false);

		if (!new_plan_file.Open (file_num)) {
			Error ("Creating %s", new_plan_file.File_Type ());
		}
	}
	if (hhlist_flag) {
		if (!hhlist_file.Open (file_num)) {
			Error ("Creating %s", hhlist_file.File_Type ());
		}
	}

	//---- process each trip ----

	if (plan_file.Extend ()) {
		Show_Message ("Reading %s %s -- Record", plan_file.File_Type (), plan_file.Extension ());
	} else {
		Show_Message ("Reading %s -- Record", plan_file.File_Type ());
	}
	Set_Progress (10000);
	last_hh = num_update = num_out = num_hhold = 0;

	while (plan_file.Read ()) {
		Show_Progress ();

		hhold = plan_file.Household ();
		if (hhold == 0) continue;

		flag = false;

		new_plan_file.Plan (plan_file.Plan ());

		if (new_plan_file.Start_Type () == LOCATION_ID) {
			location = new_plan_file.Start_ID ();

			map_ptr = (Map_Data *) activity_map.Get (&location);

			if (map_ptr != NULL && map_ptr->new_id != location) {
				new_plan_file.Start_ID (map_ptr->new_id);
				flag = true;
			}
		} else if (new_plan_file.Start_Type () == PARKING_ID) {
			location = new_plan_file.Start_ID ();

			park_ptr = (Map_Data *) parking_map.Get (&location);

			if (park_ptr != NULL && park_ptr->new_id != location) {
				new_plan_file.Start_ID (park_ptr->new_id);
				flag = true;
			}
		}

		if (new_plan_file.End_Type () == LOCATION_ID) {
			location = new_plan_file.End_ID ();

			map_ptr = (Map_Data *) activity_map.Get (&location);

			if (map_ptr != NULL && map_ptr->new_id != location) {
				new_plan_file.End_ID (map_ptr->new_id);
				flag = true;
			}
		} else if (new_plan_file.End_Type () == PARKING_ID) {
			location = new_plan_file.End_ID ();

			park_ptr = (Map_Data *) parking_map.Get (&location);

			if (park_ptr != NULL && park_ptr->new_id != location) {
				new_plan_file.End_ID (park_ptr->new_id);
				flag = true;
			}
		}

		//---- check for path updates ----

		if (network_flag && plan_file.Start_Type () == PARKING_ID && 
			plan_file.End_Type () == PARKING_ID) {

			path = new_plan_file.Path (&num_path);

			node_list.Reset ();

			if (num_path > 1 || (type_flag && num_path > 0)) {

				//---- get the new parking link ----

				parking_ptr = parking_data.Get (new_plan_file.Start_ID ());

				if (parking_ptr != NULL) {
					park_link = link_data.Get (parking_ptr->Link ());
				} else {
					park_link = NULL;
				}

				//---- origin parking lot ----

				parking_ptr = old_park_data.Get (plan_file.Start_ID ());
				link = dir = 0;

				if (parking_ptr != NULL && park_link != NULL) {
					link = parking_ptr->Link ();
					offset = parking_ptr->Offset ();

					link_ptr = old_link_data.Get (link);

					if (link_ptr == NULL) {
						Warning ("Parking %d Link %d was Not Found in the Old Link File", parking_ptr->ID (), link);
					} else {
						if ((type_flag && *path == link_ptr->Anode ()) || *path < 0) {
							dir = 1;
						} else {
							dir = 0;
						}

						//---- add new link information ----

						map_ptr = (Map_Data *) old_link_map.Get (&link);

						if (map_ptr == NULL) {
							Warning ("Old Link %d could not be Mapped to a New Link", link);
						} else {

							link = map_ptr->new_id;
							length = 0;

							if (dir == 1) {
								flip_list.Reset ();

								do {
									if (link < 0) {
										link = -link;
										dir = 1;
									} else {
										dir = 0;
									}
									link_ptr = link_data [link];

									if (link_ptr == NULL) {
										Warning ("Plan Link %d was Not Found in the Link File", link);
									} else {
										if (link_ptr->Link () == park_link->Link ()) {
											if (type_flag) {
												p1 = (dir == 0) ? link_ptr->Anode () : link_ptr->Bnode ();
											} else {
												p1 = (dir == 0) ? -link_ptr->Link () : link_ptr->Link ();
											}
											if (!flip_list.Add (p1)) goto mem_error;
											break;
										} else if (dir == 0) {
											if (link_ptr->Bnode () == park_link->Anode () ||
												link_ptr->Bnode () == park_link->Bnode ()) {
												if (type_flag) {
													if (!flip_list.Add (link_ptr->Anode ())) goto mem_error;
													if (!flip_list.Add (link_ptr->Bnode ())) goto mem_error;
												} else {
													if (!flip_list.Add (-link_ptr->Link ())) goto mem_error;
													if (link_ptr->Bnode () == park_link->Anode ()) {
														p1 = -park_link->Link ();
													} else {
														p1 = park_link->Link ();
													}
													if (!flip_list.Add (p1)) goto mem_error;
												}
												break;
											}
										} else {
											if (link_ptr->Anode () == park_link->Anode () ||
												link_ptr->Anode () == park_link->Bnode ()) {
												if (type_flag) {
													if (!flip_list.Add (link_ptr->Bnode ())) goto mem_error;
													if (!flip_list.Add (link_ptr->Anode ())) goto mem_error;
												} else {
													if (!flip_list.Add (link_ptr->Link ())) goto mem_error;
													if (link_ptr->Anode () == park_link->Anode ()) {
														p1 = -park_link->Link ();
													} else {
														p1 = park_link->Link ();
													}
													if (!flip_list.Add (p1)) goto mem_error;
												}
												break;
											}
										}
										if (length <= offset) {
											if (type_flag) {
												p1 = (dir == 0) ? link_ptr->Anode () : link_ptr->Bnode ();
											} else {
												p1 = (dir == 0) ? -link_ptr->Link () : link_ptr->Link ();
											}
											if (!flip_list.Add (p1)) goto mem_error;
										}
										length += link_ptr->Length ();
									}
									link = new_link_list [link];
								} while (link != 0);

								for (p1 = flip_list.Last (); p1 != 0; p1 = flip_list.Previous ()) {
									if (!node_list.Add (p1)) goto mem_error;
								}

							} else {

								do {
									if (link < 0) {
										link = -link;
										dir = 1;
									} else {
										dir = 0;
									}
									link_ptr = link_data [link];

									if (link_ptr == NULL) {
										Warning ("Plan Link %d was Not Found in the Link File", link);
									} else {
										length += link_ptr->Length ();
										park_flag = false;

										if (link_ptr->Link () == park_link->Link ()) {
											if (type_flag) {
												p1 = (dir == 0) ? link_ptr->Bnode () : link_ptr->Anode ();
											} else {
												p1 = (dir == 0) ? link_ptr->Link () : -link_ptr->Link ();
											}
											if (!node_list.Add (p1)) goto mem_error;
											park_flag = true;
										} else if (dir == 0) {
											if (link_ptr->Bnode () == park_link->Anode ()) {
												if (type_flag) {
													if (!node_list.Add (link_ptr->Bnode ())) goto mem_error;
												} else {
													if (!node_list.Add (-park_link->Link ())) goto mem_error;
												}
												park_flag = true;
											} else if (link_ptr->Bnode () == park_link->Bnode ()) {
												if (type_flag) {
													if (!node_list.Add (link_ptr->Bnode ())) goto mem_error;
												} else {
													if (!node_list.Add (park_link->Link ())) goto mem_error;
												}
												park_flag = true;
											}
										} else {
											if (link_ptr->Anode () == park_link->Anode ()) {
												if (type_flag) {
													if (!node_list.Add (link_ptr->Anode ())) goto mem_error;
												} else {
													if (!node_list.Add (-park_link->Link ())) goto mem_error;
												}
												park_flag = true;
											} else if (link_ptr->Anode () == park_link->Bnode ()) {
												if (type_flag) {
													if (!node_list.Add (link_ptr->Anode ())) goto mem_error;
												} else {
													if (!node_list.Add (park_link->Link ())) goto mem_error;
												}
												park_flag = true;
											}
										}
										if (length >= offset && !park_flag) {
											if (type_flag) {
												p1 = (dir == 0) ? link_ptr->Bnode () : link_ptr->Anode ();
											} else {
												p1 = (dir == 0) ? link_ptr->Link () : -link_ptr->Link ();
											}
											if (!node_list.Add (p1)) goto mem_error;
										}
										link = new_link_list [link];
									}
								} while (link != 0);
							}
						}
					}
				}

				//---- trace the path ----

				p1 = 0;

				while (num_path-- > 0) {
					p2 = *path++;

					if (p1 != 0 && (type_flag || num_path > 0)) {

						//---- get the link direction ----

						if (type_flag) {
							ab_ptr = old_ab_key.Get (p1, p2);

							if (ab_ptr == NULL) {
								dir = -1;
							} else if (ab_ptr->dir < 0) {
								link = -ab_ptr->dir;
								dir = 1;
							} else {
								link = ab_ptr->dir;
								dir = 0;
							}
						} else {
							if (p2 < 0) {
								link = -p2;
								dir = 1;
							} else {
								link = p2;
								dir = 0;
							}
						}

						//---- convert the link data ----

						if (dir >= 0) {
							map_ptr = (Map_Data *) old_link_map.Get (&link);

							if (map_ptr == NULL) {
								Warning ("Old Link %d could not be Mapped to a New Link");
							} else {
								link = map_ptr->new_id;

								if (dir == 1) {
									flip_list.Reset ();

									do {
										if (link < 0) {
											link = -link;
											dir = 1;
										} else {
											dir = 0;
										}
										link_ptr = link_data [link];

										if (link_ptr == NULL) {
											Warning ("Plan Link %d was Not Found in the New Link File", link);
										} else {
											if (type_flag) {
												p1 = (dir == 0) ? link_ptr->Anode () : link_ptr->Bnode ();
											} else {
												p1 = (dir == 0) ? -link_ptr->Link () : link_ptr->Link ();
											}
											if (!flip_list.Add (p1)) goto mem_error;
										}
										link = new_link_list [link];
									} while (link != 0);

									for (p1 = flip_list.Last (); p1 != 0; p1 = flip_list.Previous ()) {
										if (!node_list.Add (p1)) goto mem_error;
									}
								} else {
									do {
										if (link < 0) {
											link = -link;
											dir = 1;
										} else {
											dir = 0;
										}
										link_ptr = link_data [link];

										if (link_ptr == NULL) {
											Warning ("Plan Link %d was Not Found in the New Link File", link);
										} else {
											if (type_flag) {
												p1 = (dir == 0) ? link_ptr->Bnode () : link_ptr->Anode ();
											} else {
												p1 = (dir == 0) ? link_ptr->Link () : -link_ptr->Link ();
											}
											if (!node_list.Add (p1)) goto mem_error;
										}
										link = new_link_list [link];
									} while (link != 0);
								}
							}
						}
					}
					p1 = p2;
				}

				//---- get the new parking link ----

				parking_ptr = parking_data.Get (new_plan_file.End_ID ());

				if (parking_ptr != NULL) {
					park_link = link_data.Get (parking_ptr->Link ());
				} else {
					park_link = NULL;
				}

				//---- destination parking lot ----

				parking_ptr = old_park_data.Get (plan_file.End_ID ());
				link = dir = 0;

				if (parking_ptr != NULL && park_link != NULL) {
					link = parking_ptr->Link ();
					offset = parking_ptr->Offset ();

					link_ptr = old_link_data.Get (link);

					if (link_ptr == NULL) {
						Warning ("Parking %d Link %d was Not Found in the Old Link File", parking_ptr->ID (), link);
					} else {
						if ((type_flag && p1 == link_ptr->Bnode ()) || p1 < 0) {
							dir = 1;
						} else {
							dir = 0;
						}

						//---- add new link information ----

						map_ptr = (Map_Data *) old_link_map.Get (&link);

						if (map_ptr == NULL) {
							Warning ("Old Link %d was not Mapped to a New Link", link);
						} else {
							link = map_ptr->new_id;
							length = 0;

							if (dir == 1) {
								flip_list.Reset ();

								do {
									if (link < 0) {
										link = -link;
										dir = 1;
									} else {
										dir = 0;
									}
									link_ptr = link_data [link];
									park_flag = false;

									if (link_ptr == NULL) {
										Warning ("Plan Link %d was Not Found in the New Link File", link);
									} else {
										if (link_ptr->Link () == park_link->Link ()) {
											if (!type_flag) {
												p1 = (dir == 0) ? -link_ptr->Link () : link_ptr->Link ();
												if (!flip_list.Add (p1)) goto mem_error;
											}
											park_flag = true;
										} else if (dir == 0) {
											if (link_ptr->Bnode () == park_link->Anode ()) {
												if (!type_flag) {
													if (!flip_list.Add (park_link->Link ())) goto mem_error;
												}
												park_flag = true;
											} else if (link_ptr->Bnode () == park_link->Bnode ()) {
												if (!type_flag) {
													if (!flip_list.Add (-park_link->Link ())) goto mem_error;
												}
												park_flag = true;
											}
										} else {
											if (link_ptr->Anode () == park_link->Anode ()) {
												if (!type_flag) {
													if (!flip_list.Add (park_link->Link ())) goto mem_error;
												}
												park_flag = true;
											} else if (link_ptr->Anode () == park_link->Bnode ()) {
												if (type_flag) {
													if (!flip_list.Add (-park_link->Link ())) goto mem_error;
												}
												park_flag = true;
											}
										}
										if (park_flag) offset = 0;

										if (length >= offset && !park_flag) {
											if (type_flag) {
												p1 = (dir == 0) ? link_ptr->Anode () : link_ptr->Bnode ();
											} else {
												p1 = (dir == 0) ? -link_ptr->Link () : link_ptr->Link ();
											}
											if (!flip_list.Add (p1)) goto mem_error;
										}
										length += link_ptr->Length ();
									}
									link = new_link_list [link];
								} while (link != 0);

								for (p1 = flip_list.Last (); p1 != 0; p1 = flip_list.Previous ()) {
									if (!node_list.Add (p1)) goto mem_error;
								}

							} else {

								do {
									if (link < 0) {
										link = -link;
										dir = 1;
									} else {
										dir = 0;
									}
									link_ptr = link_data [link];

									if (link_ptr == NULL) {
										Warning ("Plan Link %d was Not Found in the New Link File", link);
									} else {
										length += link_ptr->Length ();

										if (link_ptr->Link () == park_link->Link ()) {
											if (!type_flag) {
												p1 = (dir == 0) ? link_ptr->Link () : link_ptr->Link ();
												if (!node_list.Add (p1)) goto mem_error;
											}
											break;
										} else if (dir == 0) {
											if (link_ptr->Bnode () == park_link->Anode () ||
												link_ptr->Bnode () == park_link->Bnode ()) {
												if (type_flag) {
													if (!node_list.Add (link_ptr->Bnode ())) goto mem_error;
												} else {
													if (!node_list.Add (link_ptr->Link ())) goto mem_error;
													if (link_ptr->Bnode () == park_link->Anode ()) {
														p1 = park_link->Link ();
													} else {
														p1 = -park_link->Link ();
													}
													if (!node_list.Add (p1)) goto mem_error;
												}
												break;
											}
										} else {
											if (link_ptr->Anode () == park_link->Anode () ||
												link_ptr->Anode () == park_link->Bnode ()) {
												if (type_flag) {
													if (!node_list.Add (link_ptr->Anode ())) goto mem_error;
												} else {
													if (!node_list.Add (-link_ptr->Link ())) goto mem_error;
													if (link_ptr->Anode () == park_link->Anode ()) {
														p1 = park_link->Link ();
													} else {
														p1 = -park_link->Link ();
													}
													if (!node_list.Add (p1)) goto mem_error;
												}
												break;
											}
										}
										if (length <= offset) {
											if (type_flag) {
												p1 = (dir == 0) ? link_ptr->Bnode () : link_ptr->Anode ();
											} else {
												p1 = (dir == 0) ? link_ptr->Link () : -link_ptr->Link ();
											}
											if (!node_list.Add (p1)) goto mem_error;
										}
									}
									link = new_link_list [link];
								} while (link != 0);
							}
						}
					}
				}

				//---- add the new path to the plan ----

				num_path = node_list.Num_Records () + 2;

				new_plan_file.Tokens (num_path);
				new_plan_file.Check_Size (num_path);

				path = new_plan_file.Path (&num_path);

				for (p1 = node_list.First (); p1 != 0; p1 = node_list.Next ()) {
					*path++ = p1;
				}

			} else {

				//---- origin and destination on the same link ----

				//---- get the new parking link ----

				parking_ptr = parking_data.Get (new_plan_file.Start_ID ());

				if (parking_ptr != NULL) {
					park_link = link_data.Get (parking_ptr->Link ());
				} else {
					park_link = NULL;
				}

				//---- origin parking lot ----

				parking_ptr = old_park_data.Get (plan_file.Start_ID ());
				link = dir = 0;

				if (parking_ptr != NULL && park_link != NULL) {
					link = parking_ptr->Link ();
					offset = parking_ptr->Offset ();

					link_ptr = old_link_data.Get (link);

					if (link_ptr == NULL) {
						Warning ("Parking %d Link %d was Not Found in the Old Link File", parking_ptr->ID (), link);
					} else {

						//---- get the new parking link ----

						parking_ptr = parking_data.Get (new_plan_file.End_ID ());

						if (parking_ptr != NULL) {
							park_link2 = link_data.Get (parking_ptr->Link ());
						} else {
							park_link2 = NULL;
						}

						//---- destination parking lot ----

						parking_ptr = old_park_data.Get (plan_file.End_ID ());

						if (parking_ptr != NULL && park_link != NULL) {
							offset2 = parking_ptr->Offset ();

							if (offset > offset2) {
								dir = 1;
							} else {
								dir = 0;
							}

							//---- add new link information ----

							map_ptr = (Map_Data *) old_link_map.Get (&link);

							if (map_ptr == NULL) {
								Warning ("Old Link %s was not Mapped to a New Link", link);
							} else {
								link = map_ptr->new_id;
								length = 0;

								if (dir == 1) {
									flip_list.Reset ();

									do {
										if (link < 0) {
											link = -link;
											dir = 1;
										} else {
											dir = 0;
										}
										link_ptr = link_data [link];

										if (link_ptr == NULL) {
											Warning ("Plan Link %s was Not Found in the New Link File", link);
										} else {
											park_flag = false;

											if (link_ptr->Link () == park_link->Link ()) {
												if (type_flag) {
													p1 = (dir == 0) ? link_ptr->Anode () : link_ptr->Bnode ();
												} else {
													p1 = (dir == 0) ? -link_ptr->Link () : link_ptr->Link ();
												}
												if (!flip_list.Add (p1)) goto mem_error;
												break;
											} else if (link_ptr->Link () == park_link2->Link ()) {
												if (!type_flag) {
													p1 = (dir == 0) ? -link_ptr->Link () : link_ptr->Link ();
													if (!flip_list.Add (p1)) goto mem_error;
												}
												park_flag = true;
											} else if (dir == 0) {
												if (link_ptr->Bnode () == park_link->Anode () ||
													link_ptr->Bnode () == park_link->Bnode ()) {

													if (link_ptr->Bnode () == park_link2->Anode () ||
														link_ptr->Bnode () == park_link2->Bnode ()) {

														if (!type_flag) {
															if (link_ptr->Bnode () == park_link2->Anode ()) {
																p1 = park_link2->Link ();
															} else {
																p1 = -park_link2->Link ();
															}
															if (!flip_list.Add (p1)) goto mem_error;

															if (link_ptr->Bnode () == park_link->Anode ()) {
																p1 = -park_link->Link ();
															} else {
																p1 = park_link->Link ();
															}
															if (!flip_list.Add (p1)) goto mem_error;

														}
													} else if (type_flag) {
														if (!flip_list.Add (link_ptr->Anode ())) goto mem_error;
														if (!flip_list.Add (link_ptr->Bnode ())) goto mem_error;
													} else {
														if (!flip_list.Add (-link_ptr->Link ())) goto mem_error;
														if (link_ptr->Bnode () == park_link->Anode ()) {
															p1 = -park_link->Link ();
														} else {
															p1 = park_link->Link ();
														}
														if (!flip_list.Add (p1)) goto mem_error;
													}
													break;
												} else if (link_ptr->Bnode () == park_link2->Anode ()) {
													if (!type_flag) {
														if (!flip_list.Add (park_link2->Link ())) goto mem_error;
													}
													park_flag = true;
												} else if (link_ptr->Bnode () == park_link2->Bnode ()) {
													if (!type_flag) {
														if (!flip_list.Add (-park_link2->Link ())) goto mem_error;
													}
													park_flag = true;
												}
											} else {
												if (link_ptr->Anode () == park_link->Anode () ||
													link_ptr->Anode () == park_link->Bnode ()) {

													if (link_ptr->Anode () == park_link2->Anode () ||
														link_ptr->Anode () == park_link2->Bnode ()) {

														if (!type_flag) {
															if (link_ptr->Anode () == park_link2->Anode ()) {
																p1 = park_link2->Link ();
															} else {
																p1 = -park_link2->Link ();
															}
															if (!flip_list.Add (p1)) goto mem_error;

															if (link_ptr->Anode () == park_link->Anode ()) {
																p1 = -park_link->Link ();
															} else {
																p1 = park_link->Link ();
															}
															if (!flip_list.Add (p1)) goto mem_error;
														}
													} else if (type_flag) {
														if (!flip_list.Add (link_ptr->Bnode ())) goto mem_error;
														if (!flip_list.Add (link_ptr->Anode ())) goto mem_error;
													} else {
														if (!flip_list.Add (link_ptr->Link ())) goto mem_error;
														if (link_ptr->Anode () == park_link->Anode ()) {
															p1 = -park_link->Link ();
														} else {
															p1 = park_link->Link ();
														}
														if (!flip_list.Add (p1)) goto mem_error;
													}
													break;
												} else if (link_ptr->Anode () == park_link2->Anode ()) {
													if (!type_flag) {
														if (!flip_list.Add (park_link2->Link ())) goto mem_error;
													}
													park_flag = true;
												} else if (link_ptr->Anode () == park_link2->Bnode ()) {
													if (type_flag) {
														if (!flip_list.Add (-park_link2->Link ())) goto mem_error;
													}
													park_flag = true;
												}
											}
											if (park_flag) offset2 = 0;

											if (length <= offset && length >= offset2 && !park_flag) {
												if (type_flag) {
													p1 = (dir == 0) ? link_ptr->Anode () : link_ptr->Bnode ();
												} else {
													p1 = (dir == 0) ? -link_ptr->Link () : link_ptr->Link ();
												}
												if (!flip_list.Add (p1)) goto mem_error;
											}
											length += link_ptr->Length ();
										}
										link = new_link_list [link];
									} while (link != 0);

									for (p1 = flip_list.Last (); p1 != 0; p1 = flip_list.Previous ()) {
										if (!node_list.Add (p1)) goto mem_error;
									}

								} else {

									do {
										if (link < 0) {
											link = -link;
											dir = 1;
										} else {
											dir = 0;
										}
										link_ptr = link_data [link];

										if (link_ptr == NULL) {
											Warning ("Plan Link %d was Not Found in the New Link File", link);
										} else {
											length += link_ptr->Length ();
											park_flag = false;

											if (link_ptr->Link () == park_link2->Link ()) {
												if (!type_flag) {
													p1 = (dir == 0) ? link_ptr->Link () : link_ptr->Link ();
													if (!node_list.Add (p1)) goto mem_error;
												}
												break;
											} else if (link_ptr->Link () == park_link->Link ()) {
												if (type_flag) {
													p1 = (dir == 0) ? link_ptr->Bnode () : link_ptr->Anode ();
												} else {
													p1 = (dir == 0) ? link_ptr->Link () : -link_ptr->Link ();
												}
												if (!node_list.Add (p1)) goto mem_error;
												park_flag = true;
											} else if (dir == 0) {
												if (link_ptr->Bnode () == park_link2->Anode () ||
													link_ptr->Bnode () == park_link2->Bnode ()) {

													if (link_ptr->Bnode () == park_link->Anode () ||
														link_ptr->Bnode () == park_link->Bnode ()) {

														if (!type_flag) {
															if (link_ptr->Bnode () == park_link->Anode ()) {
																p1 = -park_link->Link ();
															} else {
																p1 = park_link->Link ();
															}
															if (!node_list.Add (p1)) goto mem_error;

															if (link_ptr->Bnode () == park_link2->Anode ()) {
																p1 = park_link2->Link ();
															} else {
																p1 = -park_link2->Link ();
															}
															if (!node_list.Add (p1)) goto mem_error;
														}
													} else if (type_flag) {
														if (!node_list.Add (link_ptr->Bnode ())) goto mem_error;
													} else {
														if (!node_list.Add (link_ptr->Link ())) goto mem_error;
														if (link_ptr->Bnode () == park_link2->Anode ()) {
															p1 = park_link2->Link ();
														} else {
															p1 = -park_link2->Link ();
														}
														if (!node_list.Add (p1)) goto mem_error;
													}
													break;
												} else if (link_ptr->Bnode () == park_link->Anode ()) {
													if (type_flag) {
														if (!node_list.Add (link_ptr->Bnode ())) goto mem_error;
													} else {
														if (!node_list.Add (-park_link->Link ())) goto mem_error;
													}
													park_flag = true;
												} else if (link_ptr->Bnode () == park_link->Bnode ()) {
													if (type_flag) {
														if (!node_list.Add (link_ptr->Bnode ())) goto mem_error;
													} else {
														if (!node_list.Add (park_link->Link ())) goto mem_error;
													}
													park_flag = true;
												}
											} else {
												if (link_ptr->Anode () == park_link2->Anode () ||
													link_ptr->Anode () == park_link2->Bnode ()) {

													if (link_ptr->Anode () == park_link->Anode () ||
														link_ptr->Anode () == park_link->Bnode ()) {

														if (!type_flag) {
															if (link_ptr->Anode () == park_link->Anode ()) {
																p1 = -park_link->Link ();
															} else {
																p1 = park_link->Link ();
															}
															if (!node_list.Add (p1)) goto mem_error;

															if (link_ptr->Anode () == park_link2->Anode ()) {
																p1 = park_link2->Link ();
															} else {
																p1 = -park_link2->Link ();
															}
															if (!node_list.Add (p1)) goto mem_error;
														}
													} else if (type_flag) {
														if (!node_list.Add (link_ptr->Anode ())) goto mem_error;
													} else {
														if (!node_list.Add (-link_ptr->Link ())) goto mem_error;
														if (link_ptr->Anode () == park_link->Anode ()) {
															p1 = park_link2->Link ();
														} else {
															p1 = -park_link2->Link ();
														}
														if (!node_list.Add (p1)) goto mem_error;
													}
													break;
												} else if (link_ptr->Anode () == park_link->Anode ()) {
													if (type_flag) {
														if (!node_list.Add (link_ptr->Anode ())) goto mem_error;
													} else {
														if (!node_list.Add (-park_link->Link ())) goto mem_error;
													}
													park_flag = true;
												} else if (link_ptr->Anode () == park_link->Bnode ()) {
													if (type_flag) {
														if (!node_list.Add (link_ptr->Anode ())) goto mem_error;
													} else {
														if (!node_list.Add (park_link->Link ())) goto mem_error;
													}
													park_flag = true;
												}
											}
											if (length >= offset && !park_flag && length <= offset2) {
												if (type_flag) {
													p1 = (dir == 0) ? link_ptr->Bnode () : link_ptr->Anode ();
												} else {
													p1 = (dir == 0) ? link_ptr->Link () : -link_ptr->Link ();
												}
												if (!node_list.Add (p1)) goto mem_error;
											}
										}
										link = new_link_list [link];
									} while (link != 0);
								}
							}
						}
					}

					//---- add the new path to the plan ----

					num_path = node_list.Num_Records () + 2;

					new_plan_file.Tokens (num_path);
					new_plan_file.Check_Size (num_path);

					path = new_plan_file.Path (&num_path);

					for (p1 = node_list.First (); p1 != 0; p1 = node_list.Next ()) {
						*path++ = p1;
					}
				}
			}
		}

		//---- save the plan ----

		if (!new_plan_file.Write ()) {
			Error ("Writing %s", new_plan_file.File_Type ());
		}
		num_out++;

		if (flag) {
			num_update++;

			if (hhold != last_hh) {
				last_hh = hhold;
				num_hhold++;

				if (hhlist_flag) {
					str_fmt (buffer, sizeof (buffer), "%d", hhold);
					if (!hhlist_file.Write (buffer)) {
						Error ("Writing %s", hhlist_file.File_Type ());
					}
				}
			}
		} else if (plan_file.Tokens () != new_plan_file.Tokens ()) {
			num_update++;
		}
	}
	End_Progress ();

	total_in += Progress_Count ();
	if (plan_file.Extend ()) {
		Print (2, "Number of Plan Records Read from %s = %d", plan_file.Extension (), Progress_Count ());
	} else {
		Print (2, "Number of Plan Records Read = %d", Progress_Count ());
	}
	total_out += num_out;
	if (new_plan_file.Extend ()) {
		Print (1, "Number of Plan Records Written to %s = %d", new_plan_file.Extension (), num_out);
	} else {
		Print (1, "Number of Plan Records Written = %d", num_out);
	}
	total_update += num_update;
	Print (1, "Number of Plan Records Updated = %d", num_update);

	if (hhlist_flag) {
		total_hhold += num_hhold;
		Print (1, "Number of Updated Households = %d", num_hhold);
	}
	return (true);

mem_error:
	Error ("Path Memory Allocation");
	return (false);
}
