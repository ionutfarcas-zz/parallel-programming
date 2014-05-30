#include "prime_count.h"

unsigned long testPrime(unsigned long a)
{
    unsigned long j, flag;
    
    flag = 1;

    if(a == 0 || a == 1)
    {
        flag = 0;
    }
    else if(a == 2)
    {
        flag = 1;
    }
    else
    {
        for(j = 2 ; j <= a/2 ; j++)
        {
            if(a%j == 0)
            {
                flag = 0;
                break;
            }
        }
    }
    return flag;
}

unsigned long prime_count(unsigned long a, unsigned long b, unsigned long num_threads, unsigned long chunk_size)
{
    unsigned long i;
    unsigned count;
    count = 0;

    for(i = a ; i <= b ; i++)
    {
        count = count + testPrime(i);
    }

    return count;
}
