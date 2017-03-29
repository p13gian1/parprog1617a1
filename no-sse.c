//**********************************************************************************************
//**********************************************************************************************
//******** 1η Ε Ρ Γ Α Σ Ι Α  Σ Τ Ο Ν  Π Α Ρ Α Λ Λ Η Λ Ο  Π Ρ Ο Γ Ρ Α Μ Μ Α Τ Ι Σ Μ Ο  **********
//********									      **********
//********         29/2/2017   ΓΙΑΝΝΙΟΣ ΑΝΤΩΝΙΟΣ   ΑM:Π2013153                        **********
//********                                                                            **********
//********                  noSSE.c - ΥΠΟΛΟΓΙΣΜΟΣ EIKONAΣ ΕΞΟΔΟΥ ΜΕ ΧΡΗΣΗ SSE2        **********
//********                                                                            **********
//**********************************************************************************************
//********************************************************************************************** 
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define R 1
#define N 60
#define M 120

//function for calculating time
void get_walltime(double *wct)//wct: pointer
{
	struct timeval tp;
	gettimeofday(&tp,NULL);
	*wct=(double)(tp.tv_sec+tp.tv_usec/1000000.0);//tv_sec & tv_usec
}

int main()// main function
{
float *a,*b;
int i,j;
double ts,te,mflops;//ts: time_start, te: time_end
const float K0=0.5, K1=0.5, K2=0.5, K3=0.5, K4=5.0, K5=0.5, K6=0.5, K7=0.5, K8=0.5;//co-efficients

//allocating arrays and checking
a = (float *)malloc(N*M*sizeof(float));
if (a==NULL) exit(1);
b = (float *)malloc(N*M*sizeof(float));
if (b==NULL)
{
	free(a);
	exit(1);
}

//initialize all arrays - cache warm-up

for (i=0;i<N*M;i++)
{
	a[i]=0.0; b[i]=0.0;
	
}

for (i=1;i<N-1;i++)		
{
	for (j=1;j<M-1;j++)
	{
		*(a+((i*M)+j))=rand()+0.0;//random float values for image input
	}

}

/*for (i=0;i<N*M;i++)//printing input image (just for testing)
{
	printf("%d ",i+1);
	printf("%f\n",a[i]);
	
}*/
	
get_walltime(&ts);// picking start time 

//load code
for (i=1;i<N-1;i++)//scanning image input using i and j	except upper and lower boundary	
		
{
	for (j=1;j<M-1;j++)
	{
		b[i*M+j]=K0*a[(i-1)*M+(j-1)]+K1*a[(i-1)*M+j]+K2*a[(i-1)*M+(j+1)]+K3*a[i*M+(j-1)]+K4*a[i*M+j]+K5*a[i*M+(j+1)]+K6*a[(i+1)*M+(j-1)]+K7*a[(i+1)*M+j]+K8*a[(i+1)*M+(j+1)];
		
	}
}

get_walltime(&te);//picking end time

/*for (i=0;i<N*M;i++) //printing output image (just for testing)
{
	printf("%d ",i+1);
	printf("%f\n",b[i]);
}*/
  
//calculation of mflops
mflops = (M*R*2.0)/((te-ts)*1e6);
printf("MFLOPS/sec = %f\n",mflops);
  
// freeing memory used by arrays
free(a);
free(b);

return 0;//end of main function - end of the program
}