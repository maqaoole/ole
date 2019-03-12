nbcores = 8

L1 = {
   size = 32 * 1024,
   cost = 2,
   level = 1,
   assoc = 8,
   linesize = 64,
   policy = "BLRU"
}

L2 = {
   size = 256 * 1024,
   cost = 5,
   level = 2,
   assoc = 8,
   linesize = 64,
   policy = "BLRU"
}

L3 = {
   size = 8 * 1024 * 1024,
   cost = 47,
   level = 3,
   assoc = 16,
   linesize = 64,
   policy = "BLRU"
}
