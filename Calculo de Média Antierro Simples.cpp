#include <stdio.h>
#include <string.h>

float media=0;
float nota1;
float nota2;
char nome[30];
char situacao[30];

int main()
	{
		printf("Digite o nome do aluno: ");
		scanf("%s",&nome);
	
		printf("Digite a primeira nota: ");
		scanf("%f",&nota1);
	
		printf("Digite a segunda nota: ");
		scanf("%f",&nota2);
	
		if(nota1>0 && nota2>0)
		{
			if(nota1<10 && nota2<10)
			{
				media=((nota1+nota2)/2);
				if(media>7.00)
				{
					strcpy(situacao, "aprovado");
				}	
				else
				{
					strcpy(situacao, "reprovado");
				}	
				printf("\nO resultado e: \n");
				printf("Nome do aluno: %s\nMedia: %.2f\nSituacao: %s",nome,media,situacao);	
			}
		}		
		else 
		{
			printf("Digite apenas numeros positivos menores que 10");
		}
		getchar();
	}
