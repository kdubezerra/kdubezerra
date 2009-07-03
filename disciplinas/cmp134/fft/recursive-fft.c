 
/* Demonstration program for Fast Fourier Transform */
/* implemented JWDunn */
/* compile using -lm option */
/* this version uses global allocation of working storage for speed */
 
#include <stdio.h>
#include <math.h>
char *malloc();
 
#define twopi 6.2831854
int size;
float *w,*a,*A,*work;
 
main()
{
        int i;
        size = 16;      /* size is a global variable used by FFT() */
        initdata();     /* sets up the data and work areas - change to suit */
        initw();        /* initialises an array of cos and sin */
        FFT(size,a,A,2,work);   /* note the parameter 2 */
        for (i=0;i<(size << 1);i+=2)
        /* normalisation is not included in FFT */
        {
                A[i] /= size; A[i+1] /= size;
        }
/*      FFT2(size,A,a,2,work); */ /* call of the inverse function */
        result();       /* just for demo purposes */
}
 
/* complex input and output data are stored in FORTRAN fashion */
/* as an array of floats - reals in even locations, imaginary in */
/* succeeding odds */
 
initw()
{
/* the complex w array prestores values of w^k; w = cos 2pi/N + jsin 2pi/N */
/* so that w^k is obtained by simple look-up when needed */
        int i;
        float *fp,k,theta;
        w = (float *) malloc(2*(size+1)*sizeof(float));
        i = 0;
        k = twopi/size;
        theta = k*i;
        fp = w;
        while (i<(size+1))
        {
                *fp++ = cos(theta); *fp++ = sin(theta);
                i++; theta = k*i;
        }
}
 
initdata()
{
        float *fp,k;
        int i;
 
/* allocate some storage */
        a = (float *) malloc(2*size*sizeof(float)); /* for input data */
        A = (float *) malloc(2*size*sizeof(float)); /* ditto for output */
        work = (float *) malloc(4*size*sizeof(float)); /* 4*size is enough */
        i = 0;
        fp = a;
        while (i<size)  /* some real data for demo purposes */
        {
/* alter to suit current use, read from file etc. */
                if ((i < size/4) || (i > size*3/4))
                *fp++ = 900.0;  /* real part */
                *fp++ = 0.0;    /* imaginary part */
                i++;
        }
}
 
FFT(N,f,F,step,warea)
int N;
float *f,*F;
int step;
float *warea;
/* Fast Fourier routine */
/* from algorithm described in LIPSON Elements of Algebra and Algebraic */
/* Computing */
/* Implemented by J.W.Dunn */
/* Some more optimisation - particularly register variables use */
/* is certainly possible */
/* N is the complex size of the array f - ie half the actual size */
/* in float and must be 2^n */
/* F points to an array of float of same size as f, for output */
/* step must be 2 in initial call; it is used in looking up w[k] */
{
        if (N == 1)     /* end recursion */
        {
                *F = *f;        /* real */
                *(F+1) = *(f+1);        /* imaginary */
        }
        else
        {
                float *B,*C,*wa;
                register float *Bp,*Cp,tfr,tfi;
                int n,s;
                register int k;
                /* grab some temporary working space */
                Bp = B = warea;
                Cp = C = warea + N;
                wa = warea + (N << 1);  /* new bottom to work area */
                n = N >> 1;     /* halve problem size */
                s = step + step;        /* for use by w[k] later */
                FFT(n,f,B,s,wa);                /* recursive calls */
                FFT(n,f+step,C,s,wa);   /* to set up B & C */
/* note that b and c are accessed from f implicitly via step */
/* ie the original input data */
/* now put the output together - note symmetry use */
                for (k=0;k<size;k += step)      /* optimised! */
/* size must be declared and initialised globally */
                {
                        tfr = w[k]*(*Cp); /* reals used twice */
                        tfi = w[k+1]*(*Cp) + w[k]*(*(Cp + 1));
                        Cp++;
                        tfr -= w[k+1]*(*Cp++);
                        /* complex mult. = 4 floating mults. */
                        *F = *Bp  + tfr;/* only adds & subs needed now */
                        *(F+1) = *(Bp+1) + tfi;
                        *(F+N) = *Bp++ - tfr;
                        *(++F + N) = *Bp++ - tfi;
                        F++;
                }
        }
}
 
FFT2(N,f,F,step,warea)
int N;
float *f,*F;
int step;
float *warea;
/* Fast Fourier routine - reverse transform */
/* The only difference from FFT() is a few signs near the end */
{
        if (N == 1)
        {
                *F = *f;        /* real */
                *(F+1) = *(f+1);        /* imaginary */
        }
        else
        {
                float *B,*C,*wa;
                register float *Bp,*Cp,tfr,tfi;
                int n,s;
                register int k;
                Bp = B = warea;
                Cp = C = warea + N;
                wa = warea + (N << 1);
                n = N >> 1;     /* halve problem size */
                s = step + step;
                FFT2(n,f,B,s,wa);
                FFT2(n,f+step,C,s,wa);
/* now put the output together - note symmetry use */
                for (k=2*size;k>size;k -= step) /* optimised! */
                {
                        tfr = w[k]*(*Cp); /* reals used twice */
                        tfi = w[k+1]*(*Cp) + w[k]*(*(Cp + 1));
                        Cp++;
                        tfr -= w[k+1]*(*Cp++);
                        /* complex mult. = 4 floating mults. */
                        *F = *Bp  + tfr;/* only adds & subs needed now */
                        *(F+1) = *(Bp+1) + tfi;
                        *(F+N) = *Bp++ - tfr;
                        *(++F + N) = *Bp++ - tfi;
                        F++;
                }
        }
}
 
result()
{
/* just for demonstration */
        int i;
        printf("Results follow\n");
        printf("\n    i   Re(f)    Re(F)    Im(F)      |F|\n");
        for (i=0;i<2*size;i += 2)
        {
                printf("%4d %8.1f %8.1f ",i/2,a[i],A[i]);
                printf("%8.1f ",A[i+1]);
                printf("%8.1f \n",sqrt(A[i]*A[i] + A[i+1]*A[i+1]));
        }
}
