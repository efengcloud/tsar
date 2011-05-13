#ifndef _COMMON_H
#define _COMMON_H
/*
 * convert data to array
 */
int convert_record_to_array(U_64 *array, int l_array, char *record);
void get_mod_hdr(char hdr[], struct module *mod);
int strtok_next_item(char item[], char *record, int *start);
int merge_mult_item_to_array(U_64 *array, struct module *mod);
int get_strtok_num(char *str, char *split);
#endif
