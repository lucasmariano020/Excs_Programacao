#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char mes[30];
int num=0;
char* correspondente()
{
	switch(num)
	{
		case 1:strcpy(mes,"Janeiro");
		break;
		case 2:strcpy(mes,"Fevereiro");
		break;
		case 3:strcpy(mes,"Março");
		break;
		case 4:strcpy(mes,"Abril");
		break;
		case 5:strcpy(mes,"Maio");
		break;
		case 6:strcpy(mes,"Junho");
		break;
		case 7:strcpy(mes,"Julho");
		break;
		case 8:strcpy(mes,"Agosto");
		break;
		case 9:strcpy(mes,"Setembro");
		break;
		case 10:strcpy(mes,"Outubro");
		break;
		case 11:strcpy(mes,"Novembro");
		break;
		case 12:strcpy(mes,"Dezembro");
		break;
		default:strcpy(mes, "Erro");
		break;
	}
	return(mes);
}
int main(){
	printf("Digite um numero correspondente a um mes: ");
	scanf("%d",&num);
	printf("Mes correspondente: %s",correspondente());
	}
