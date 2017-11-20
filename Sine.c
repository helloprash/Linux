#include <stdio.h>
int main()
{
int input;
float pi=3.1415926535897;
double degree;
double sinx;
double powerseven;
double powerfive;
double powerthree;

printf("Enter a number:");
scanf("%d",&input);
degree= (input*pi)/180;

powerseven=(degree*degree*degree*degree*degree*degree*degree);
powerfive=(degree*degree*degree*degree*degree);
powerthree=(degree*degree*degree);

sinx = (degree - (powerthree/6) + (powerfive/120) - (powerseven/5040));
printf("%f", sinx);
}
