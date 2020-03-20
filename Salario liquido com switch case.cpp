#include <stdio.h>
#include <stdlib.h>

	
int main()
{
	float salariob=0;
	float salariol=0;
	int x=0;
	
	printf("Digite o seu salario bruto: ");
	scanf("%float",&salariob);	
	
	salariob = x;
		
	switch(x)
	{
	
		case(0): salariol=(salariob-(salariob*0,12)+150);
		break;	
		
		case(2000): salariol=(salariob-(salariob*(12/100)));
		break;
		
		case(10000): salariol=(salariob-(salariob*(15/100)));
		break;
					
		
		
		default: printf("Erro! Valor invalido.");
		break;
	}
	printf("Valor do salario liquido: %0.2fR$ \n",salariol);
	
	
}
