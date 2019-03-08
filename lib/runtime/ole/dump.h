#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

void OLE_dump_loop_all_stats(FILE *fd, _OLE_loop_t *loop);
void OLE_dump_config_(FILE *fd, _OLE_config_t *config);
void OLE_dump_config_H(FILE *fd, _OLE_config_t *config);
void OLE_dump_config_V(FILE *fd, _OLE_config_t *config);
void OLE_print_config(_OLE_config_t *config);
void OLE_dump_global_loop_stats();
void OLE_dump_groups(FILE *fd, _OLE_loop_t *loop);

#endif
