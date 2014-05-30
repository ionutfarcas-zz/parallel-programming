#include <stdlib.h>
#include "prime_count.h"

unsigned long testPrime(unsigned long a)
{
    unsigned long j, flag;
    
    flag = 1;
    for(j = 2 ; j <= a/2 ; j++)
    {
        if(a%j == 0)
        {
            flag = 0;
            break;
        }
    }

    return flag;
}

unsigned long prime_count(unsigned long a, unsigned long b, unsigned long num_threads, unsigned long chunk_size)
{
    unsigned long i;
    unsigned count;
    count = 0;
    for(i = a + 1 ; i <= b ; i++)
    {
    
        if(testPrime(i) == 1)
            count++;
    }

    return count;
}
