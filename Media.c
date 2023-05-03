#include <stdio.h>
int i = 0;
float valor[5] = {0,0,0,0,0}, media = 0;
int main()
 {
    for (i=0;i<5;i++)
        {
            scanf("%f", &valor[i]);
            media = valor[i] + media;
        }
        
    media = media/5;
    printf("Media 5 numeros: %.2f", media);
    
 }
