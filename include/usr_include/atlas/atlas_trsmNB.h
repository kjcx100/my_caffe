#ifndef ATLAS_TRSMNB_H
   #define ATLAS_TRSMNB_H

   #ifdef SREAL
      #define TRSM_NB 24
   #elif defined(DREAL)
      #define TRSM_NB 24
   #elif defined(SCPLX)
      #define TRSM_NB 12
   #elif defined(DCPLX)
      #define TRSM_NB 12
   #endif

#endif
