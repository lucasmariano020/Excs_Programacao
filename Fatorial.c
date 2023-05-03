#include <stdio.h>
int N = 0, fatorial = 0, i = 0;
int main()
 {
    scanf("%d", &N);
    for(i=0; i<=N; i++)
     {
        fatorial = i*fatorial;
     }

    if(N == 0)
        printf("Fatorial: 0");
    else if(N > 0)
     {
        printf("Fatorial: %d", fatorial);
     }
     
 }