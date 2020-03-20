#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char nome[30];	
int tamanho=0;

int main()	
	{
		printf("Digite um nome: ");
		scanf("%[^\n]c",&nome);
		tamanho = strlen(nome);
	  
		for(tamanho>0;tamanho--;)
		{
			if(nome[tamanho]== ' ')
			{
				tamanho--;
			}
			printf("%s\n",nome);		
		}
	}
	
	
