#ifndef TRACE_H
#define TRACE_h

#include <stdint.h>
#include <inttypes.h>

#include "consts.h"

_OLE_trace_t *OLE_trace_init();
_OLE_trace_t *OLE_lookup_trace_address(_OLE_trace_t *root, uint64_t addr);
_OLE_trace_meta_t *OLE_add_new_meta_entry(_OLE_trace_meta_t **node, uint64_t bin_addr, uint64_t iter);
_OLE_trace_t *OLE_add_new_trace_entry(_OLE_trace_t **root, uint64_t addr, uint64_t bin_addr, uint64_t iter);

#endif
