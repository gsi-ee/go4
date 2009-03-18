#ifndef F_FIND_PEAKS
#define F_FIND_PEAKS

#define TYPE__SHORT  0
#define TYPE__INT    1
#define TYPE__FLOAT  2
#define TYPE__DOUBLE 3
void f_find_peaks(
        void *,    // pointer to data
        int,       // data type: use definitions above
        int,       // first channel (0,...)
        int,       // number of channels
        int,       // channels to sum up (bin size in channels)
        double,    // noise factor to square root
        double,    // noise minimum (absolut)
        double *,  // array with noise (original channels)
        int,       // Maximum number of peaks. Arrays below must have this size.
        int *,     // returns number of peaks found
        int *,     // pointer to array of minima
        double *,  // pointer to array of minima values
        int *,     // pointer to array of maxima
                  int *,     // pointer to array of width
        double *); // pointer to array of integrals

#endif
