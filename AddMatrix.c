#include<stdio.h>
 
int main() 
{
   int i, j, mat1[10][10], mat2[10][10], mat3[10][10];
   int row1=3, col1=4, row2=3, col2=4;
 
   for (i = 0; i < row1; i++) {
      for (j = 0; j < col1; j++) {
         printf("Enter the Element a[%d][%d] : ", i, j);
         scanf("%d", &mat1[i][j]);
      }
   }
 
   for (i = 0; i < row2; i++)
      for (j = 0; j < col2; j++) 
	  {
         printf("Enter the Element b[%d][%d] : ", i, j);
         scanf("%d", &mat2[i][j]);
      }
 
   for (i = 0; i < row1; i++)
      for (j = 0; j < col1; j++) 
	  {
         mat3[i][j] = mat1[i][j] + mat2[i][j];
      }
 
   printf("\nThe Addition of two Matrices is : \n");
   for (i = 0; i < row1; i++) 
   {
      for (j = 0; j < col1; j++) 
	  {
         printf("%d  ", mat3[i][j]);
      }
      printf("\n");
   }
 
   return (0);
}
