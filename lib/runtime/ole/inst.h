#ifndef INST_H
#define INST_H

uint64_t OLE_get_nb_instructions(_OLE_loop_t *loop);
_OLE_instruction_t *OLE_lookup_instruction(_OLE_instruction_t *root, uint64_t key);
_OLE_instruction_t *OLE_new_instruction(_OLE_instruction_t **root, uint64_t bin_addr, unsigned char type, uint64_t nb_caches, /* uint64_t stream_id, */ char *asm_line, uint64_t iter);

#endif
