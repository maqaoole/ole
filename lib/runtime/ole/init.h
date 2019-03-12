#ifndef INIT_H
#define INIT_H

#include <lua.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <lualib.h>
#include <lauxlib.h>
#include <inttypes.h>
#include <pthread.h>

#include "structs.h"

char xpath[MAX_STR];
char cfg_path[MAX_STR];
char out_path[MAX_STR];
char bfname[MAX_STR];

const char _dbg_levels[3][3]; 
const char _dbg_events[4][10];

//Pattern extraction
uint64_t nb_addresses;
uint64_t addresses[MAX_ADDRESSES];

int global_length;
_OLE_cpu_t *global_cpu;
uint64_t global_loopID;
_OLE_trace_t *global_trace;
_OLE_config_t *global_config;
_OLE_loop_t *global_tmp_loop;
_OLE_loop_t *global_root_loop;
_OLE_4K_alias_event_t alias_event_tab[MAX_4K_EVENTS];

short dumped;
uint64_t nb_calls;
uint64_t nb_groups;
uint64_t nb_instances;
unsigned registered_dump_call;

uint64_t last_store_i;
uint64_t last_store_b;
uint64_t curr_load_i;
uint64_t curr_load_b;
unsigned char events_overload; 
uint64_t nb_4K_alias_events;

FILE *new_groups_fd;

lua_State *OLE_open_lua_config(char *fname);
int OLE_close_lua_config(lua_State *L);
int OLE_lua_get_int_field(lua_State *L, const char *key);
char *OLE_lua_get_str_field(lua_State *L, const char *key);
int OLE_get_socket_config(); 
int OLE_get_cpu_config(lua_State *L, _OLE_config_t *config);
int OLE_get_core_config(lua_State *L, _OLE_config_t *config);
_OLE_config_t *OLE_load_config(char *cfgfname);
_OLE_cpu_t *OLE_init(char *bin, char *xp, uint64_t loopID);
void OLE_loop_record();
void OLE_next_iter();
void OLE_close_new_groups_fd();
void OLE_analyze_pattern();

//
void OLE_exit() __attribute__((destructor));

#endif
