// gcc pi.c -O2 -lm --openmp -o pi
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

#define SEED     42
#define SAMPLES 50000000
int main(int argc, char* argv[])
{
    int count = 0;
    double x, y, z, pi;
    
    srand(SEED);
    double start=omp_get_wtime();
    for (int i = 0; i < SAMPLES; i++) // pri tem more sodelovat več processov ki si potem delijo count in na koncu en izračuna pi
    {
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x*x) + (y*y));
        
        if (z <= 1.0) // preveri če je razdalja do izhodišča večja ali manjša od 1
        {
            count++;
        }
    }
    
    pi = ((double)count / SAMPLES) * 4.0;
    double end=omp_get_wtime();
    printf("PI = %f | computed in %.2f s\n", pi, end-start);
    
    return 0;
}