#ifndef _TFILEIO_H
	#define _TFILEIO_H
	
Boolean save_scenario(char * argv[]);
Boolean load_scenario(char * argv[]);
void port_scenario();
void port_item_list();
void flip_spec_node(special_node_type *spec);
void flip_short(short *s);
void alter_rect(RECT *r);
void flip_rect(RECT *s);
void flip_rect(RECT16 *s);
void alter_rect(RECT16 *r);
#endif
