#include <stdio.h>
int i = 0;
float valor[10] = {0,0,0,0,0,0,0,0,0,0};
int main()
 {
    for (i=0;i<10;i++)
            scanf("%f", &valor[i]);
    for(i=10;i>0;i--)
        printf("%f ", valor);
 }