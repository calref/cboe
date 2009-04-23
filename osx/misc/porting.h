/*
 *  porting.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "oldstructs.h"

void flip_long(long *s);
void flip_spec_node(legacy::special_node_type *spec);
void flip_short(short *s);
void flip_rect(Rect *s);

void port_t_d(legacy::big_tr_type* old);
void port_ave_t(legacy::ave_tr_type* old);
void port_tiny_t(legacy::tiny_tr_type* old);
void port_scenario(legacy::scenario_data_type* old);
void port_item_list(legacy::scen_item_data_type* old);
void port_out(legacy::outdoor_record_type* out);
void port_talk_nodes(legacy::talking_record_type* dummy_talk_ptr);
void port_dummy_t_d(short size,char *buffer);
void port_town(legacy::town_record_type* dummy_town_ptr);