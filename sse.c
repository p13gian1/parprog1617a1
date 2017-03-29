//**********************************************************************************************
//**********************************************************************************************
//******** 1η Ε Ρ Γ Α Σ Ι Α  Σ Τ Ο Ν  Π Α Ρ Α Λ Λ Η Λ Ο  Π Ρ Ο Γ Ρ Α Μ Μ Α Τ Ι Σ Μ Ο  **********
//********									      **********
//********         29/2/2017  ΓΙΑΝΝΙΟΣ ΑΝΤΩΝΙΟΣ   ΑM:Π2013153                         **********
//********                                                                            **********
//********                  SIMD.c - ΥΠΟΛΟΓΙΣΜΟΣ EIKONAΣ ΕΞΟΔΟΥ ΜΕ ΧΡΗΣΗ SSE2         **********
//********                                                                            **********
//**********************************************************************************************
//********************************************************************************************** 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <emmintrin.h>
#define R 1
#define N 60
#define M 120


//function for calculating time
void get_walltime(double *wct) //wct: pointer
{
	struct timeval tp;
	gettimeofday (&tp, NULL);
	*wct= (double)(tp.tv_sec + tp.tv_usec/10000000.0); //tv_sec & tv_usec 
}

int main()// main function
{
double ts, te; //ts: time_start, te: time_end
double mflops;

int i,j,f;
float *a;//array image input
float *b;//array image output
float *mask1,*mask2,*mask3, *mask4, *mask5,*e1,*e2,*e3,*e4,*e5,*e6;//arrays used as masks etc


//pointers we need
__m128 *vfa1, *vfa2, *vfa3, *vfb, *tempmul1, *tempmul2, *tempmul3;
__m128 *tempmul4,*tempsum1, *tempsum2, *vmask1, *vmask2, *vmask3, *vmask4, *vmask5;

f=posix_memalign((void**)&a,16,(N*M)*sizeof(float));//allingment and checking of all our arrays
if (f!=0)
{
	exit(1);
}

f=posix_memalign((void**)&b,16,(N*M)*sizeof(float));
if (f!=0)
{
	free(a);
	exit(1);
}

f=posix_memalign((void**)&mask1,16,4*sizeof(float));
if (f!=0)
{
	free(a);
	free(b);
	exit(1);
}
f=posix_memalign((void**)&mask2,16,4*sizeof(float));
if (f!=0)
{
	free(a);
	free(b);
	free(mask1);
	exit(1);
}

f=posix_memalign((void**)&mask3,16,4*sizeof(float));
if (f!=0)
{
	free(a);
	free(b);
	free(mask1);	
	free(mask2);
	exit(1);
}

f=posix_memalign((void**)&mask4,16,4*sizeof(float));
if (f!=0)
{
	free(a);
	free(b);
	free(mask1);	
	free(mask2);
	free(mask3);	
	exit(1);
}

f=posix_memalign((void**)&mask5,16,4*sizeof(float));
if (f!=0)
{
	free(a);
	free(b);
	free(mask1);	
	free(mask2);
	free(mask3);
	free(mask4);	
	exit(1);
}

f=posix_memalign((void**)&e1,16,4*sizeof(float));

if (f!=0)
{
	free(a);
	free(b);
	free(mask1);
	free(mask2);
	free(mask3);
	free(mask4);
	free(mask5);
	exit(1);
}

f=posix_memalign((void**)&e2,16,4*sizeof(float));

if (f!=0)
{
	free(a);
	free(b);
	free(mask1);
	free(mask2);
	free(mask3);
	free(mask4);
	free(mask5);
	free(e1);
	exit(1);
}
f=posix_memalign((void**)&e3,16,4*sizeof(float));

if (f!=0)
{
	free(a);
	free(b);
	free(mask1);
	free(mask2);
	free(mask3);
	free(mask4);
	free(mask5);
	free(e1);
	free(e2);
	exit(1);
}

f=posix_memalign((void**)&e4,16,4*sizeof(float));

if (f!=0)
{
	free(a);
	free(b);
	free(mask1);
	free(mask2);
	free(mask3);
	free(mask4);
	free(mask5);
	free(e1);
	free(e2);
	free(e3);
	exit(1);
}

f=posix_memalign((void**)&e5,16,4*sizeof(float));

if (f!=0)
{
	free(a);
	free(b);
	free(mask1);
	free(mask2);
	free(mask3);
	free(mask4);
	free(mask5);
	free(e1);
	free(e2);
	free(e3);
	free(e4);
	exit(1);
}

f=posix_memalign((void**)&e6,16,4*sizeof(float));

if (f!=0)
{
	free(a);
	free(b);
	free(mask1);
	free(mask2);
	free(mask3);
	free(mask4);
	free(mask5);
	free(e1);
	free(e2);
	free(e3);
	free(e4);
	free(e5);
	exit(1);
}

//initialize all arrays - cache warm-up

for (i = 0; i < 4; i++)
{
	*(e1 + i) = 0.0;
	*(e2 + i) = 0.0;
	*(e3 + i) = 0.0;
	*(e4 + i) = 0.0;
	*(e5 + i) = 0.0;
	*(e6 + i) = 0.0;
	*(mask1 + i) = 0.5;
	*(mask2 + i) = 0.5;
	*(mask3 + i) = 0.5;
	*(mask4 + i) = 0.5;
	*(mask5 + i) = 0.5;
}
mask1[0]=5.0;//array mask1 is [5.0 , 0.5 , 0.5 , 0.5]
mask2[1]=5.0;//array mask2 is [0.5 , 5.0 , 0.5 , 0.5]	
mask3[2]=5.0;//array mask3 is [0.5 , 0.5 , 5.0 , 0.5]
mask4[3]=5.0;//array mask4 is [0.5 , 0.5 , 0.5 , 5.0]
	     //array mask5 is [0.5 , 0.5 , 0.5 , 0.5]

for (i=0;i<N*M;i++)
{
	*(a+i)=0.0; *(b+i)=0.0; 
	
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
	if (i % M == 0)
	printf("\n");
	printf("%8.1f\t",a[i]);   
}*/

printf("\n");


get_walltime(&ts);//picking start time

//load code
vmask1=(__m128*)mask1;//arranging pointers _m128
vmask2=(__m128*)mask2;
vmask3=(__m128*)mask3;
vmask4=(__m128*)mask4;
vmask5=(__m128*)mask5;
tempmul1=(__m128*)e1;
tempmul2=(__m128*)e2;
tempmul3=(__m128*)e3;
tempmul4=(__m128*)e4;
tempsum1=(__m128*)e5;
tempsum2=(__m128*)e6;

vfa1=(__m128*)&a[0];//initialize starting point of upper 4 cells 
vfa2=(__m128*)&a[M];//    -""-         -""-       middle 4 cells
vfa3=(__m128*)&a[2*M];//  -""-         -""-        lower 4 cells

for (i=1;i<N-1;i++)//scanning image input using i and j	except upper and lower boundary	
{
	for (j=0;j<M;j+=4)//although, code scans left and right boundary
	{
			
		*tempmul1=_mm_mul_ps(*vfa1,*vmask5);//calculating 1st pixel on the left...
		*tempmul2=_mm_mul_ps(*vfa2,*vmask1);//
		*tempmul3=_mm_mul_ps(*vfa3,*vmask5);//
		*tempsum1=_mm_add_ps(*tempmul1,_mm_add_ps(*tempmul2,*tempmul3));
		b[(i*M)+j]=e5[0]+e5[1]+e5[2];//end of calculation - summing result of 4 cells
	
		*tempmul1=_mm_mul_ps(*vfa1,*vmask5);//calculating 2nd pixel on the left...
		*tempmul2=_mm_mul_ps(*vfa2,*vmask2);//
		*tempmul3=_mm_mul_ps(*vfa3,*vmask5);//
		*tempsum1=_mm_add_ps(*tempmul1,_mm_add_ps(*tempmul2,*tempmul3));
		b[((i*M)+j+1)]=e5[0]+e5[1]+e5[2];//end of calculation - summing result of 4 cells

		*tempmul1=_mm_mul_ps(*vfa1,*vmask5);//calculating 3rd pixel on the left...
		*tempmul2=_mm_mul_ps(*vfa2,*vmask3);//
		*tempmul3=_mm_mul_ps(*vfa3,*vmask5);//
		*tempsum1=_mm_add_ps(*tempmul1,_mm_add_ps(*tempmul2,*tempmul3));
		b[(i*M)+j+2]=e5[1]+e5[2]+e5[3];//end of calculation - summing result of 4 cells

		*tempmul1=_mm_mul_ps(*vfa1,*vmask5);//calculating 4th pixel on the left...
		*tempmul2=_mm_mul_ps(*vfa2,*vmask4);//
		*tempmul3=_mm_mul_ps(*vfa3,*vmask5);//
		*tempsum1=_mm_add_ps(*tempmul1,_mm_add_ps(*tempmul2,*tempmul3));
		b[(i*M)+j+3]=e5[1]+e5[2]+e5[3];//end of calculation - summing result of 4 cells
	
		vfa1++; vfa2++; vfa3++;//increasing our pointers, showing to next 4 cells of image	
	}
	
}

get_walltime(&te);//picking end time

/*for (i=0;i<N*M;i++)//printing output image (just for testing)
{
	if (i % M == 0)
	printf("\n");
	printf("%8.1f\t",b[i]);   
}*/

//calculation of mflops
mflops= (N*R*2.0)/((te-ts)*1e6); //1e6: 1 ekatommyrio
printf ("%f\n", mflops);

//freeing memory used by arrays
free(a);
free(b);
free(mask1);
free(mask2);
free(mask3);
free(mask4);
free(mask5);
free(e1);
free(e2);
free(e3);
free(e4);
free(e5);
free(e6);

return 0;//end of main function - end of the program
}

