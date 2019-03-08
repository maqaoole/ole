#ifndef CONSTS_H
#define CONSTS_H

#define DBG_MODE          0 //Debug mode switch
#define TRC_MODE          0 //Trace mode switch

#define MAX_STR           369   //Homage to Tesla (why not?)
#define MAX_INSN          1000  //Maximum number of  
#define LEN_ADDR          64    //64 bit addresses

//

#define MAX_INSTANCES     100     //Catching the first instances
#define MAX_ITER          10000   //Max iterations (Change to 9,223,372,036,854,775,807 = 2^64 - 1)
#define MAX_4K_EVENTS     1024

#define MAX_ADDRESSES     8

//Replacement policies in ASCII base 16
#define BLRU 0x424c5255
#define PLRU 0x504c5255
//#define BIPP bimodal insertion policy (set dueling)
//#define RAND random 

#define HIT       0
#define COLD_MISS 1
#define CONF_MISS 2
#define CAPA_MISS 3
 
#define NB_COLS 9 //Number of hit/miss stats columns

//Explicit column index -- Very Intel-friendly :)

#define MAX_CACHE_LEVELS  3      

#define L1_HIT  0
#define L1_COLD 1
#define L1_CONF 2

#define L2_HIT  3
#define L2_COLD 4
#define L2_CONF 5

#define L3_HIT  6
#define L3_COLD 7
#define L3_CONF 8

#define L1 0
#define L2 1
#define L3 2

#endif
