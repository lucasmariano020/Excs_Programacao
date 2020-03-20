#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float c;
float b;
float a;
float x1;
float x2;
float dta;
float delta(float a,float b,float c);

int main()
{

	printf("Digite o coeficiente A: ");
	scanf("%f",&a);
	
	printf("Digite o coeficiente B: ");
	scanf("%f",&b);
	
	printf("Digite o coeficiente C: ");
	scanf("%f",&c);
	
	if(delta(a,b,c)>0)
	{
		x1=((-b+sqrt (delta(a,b,c)))/(2*a));
		x2=((-b-sqrt (delta(a,b,c)))/(2*a));
		printf("X1 = %f\nX2 = %f\n",x1,x2);
	}
	else
	{
		if(delta(a,b,c)==0)
		{
			x1=((-b+sqrt (delta(a,b,c)))/(2*a));
			printf("X1 = %f\n",x1);	
		}
		else
		{	
			printf("Delta negativo! Nenhuma raiz real.");
		}
	}
	
	//printf("%f\n",dta);
	//int j=sqrt(9);
	//printf("%d",j);
	
	return (0);
}

float delta(float a,float b,float c)
{
	dta=((b*b)+(-4*a*c));
	return (dta);
}

