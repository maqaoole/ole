#ifndef OLE_H
#define OLE_H

void OLE_inject_cacheline(uint64_t loopID, uint64_t baddr, unsigned inst, /* uint64_t stream_id, */ char *asmline, uint64_t iaddr);

#endif
