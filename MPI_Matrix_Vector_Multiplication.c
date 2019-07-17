#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define n 16
#define seed 5

void master(int np, int blk_size, int rem_size);
void slave (int np, int blk_size);

int main(int argc, char** argv)
{
    int my_rank, np;
    int blk_size, rem_size;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    blk_size = n / (np-1);
    rem_size = n - blk_size*(np-1);

    if (my_rank == 0)
        master(np, blk_size, rem_size);
    else
        slave(np, blk_size);

    MPI_Finalize();

    return 0;
}

void master(int np, int blk_size, int rem_size)
{
    int A[n][n], B[n], C[n];
    int i, j, sum;
    int proc;

    MPI_Status status;
    MPI_Request request;

    srand48(seed);

    for(i = 0; i < n; i++)
    {
        B[i] = 1 + floor(drand48() * 100);
        for(j = 0; j < n;j++)
        {
         A[i][j] = 1 + floor(drand48() * 100);
        }
    }

    for(proc = 1; proc < np; proc++)
    {
        MPI_Isend(A+blk_size*(proc-1), blk_size*n, MPI_INT, proc, 0, MPI_COMM_WORLD, &request);
        MPI_Send(B, n, MPI_INT, proc, 1, MPI_COMM_WORLD);
        MPI_Wait(&request, &status);
    }

    if(rem_size != 0)
    {
        for(i = blk_size*(np-1); i < n; i++)
        {
            sum = 0;
            for(j = 0; j < n; j++)
            {
                sum += A[i][j] * B[j];
            }
            C[i] = sum;
        }
    }

    for(proc = 1; proc < np; proc++)
        MPI_Revc(C+blk_size*(proc - 1), blk_size, MPI_INT, proc, 2, MPI_COMM_WORLD, &status);

    for(i = 0; i < n ; i++)
    {
        for(j = 0; j < n; j++)
        {
            printf("%3d ", A[i][j]);
            printf("%3d ", B[i]);
            printf("%8d \n", C[i]);
        }
    }
}

void slave(int np, int blk_size)
{
    int B[n], C[n], *D;
    int i, j, sum;
    MPI_Status status1, status2;
    MPI_Request request;

    D = (int*) malloc (blk_size*n*sizeof(int));

    MPI_Irecv(D, blk_size*n, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
    MPI_Recv(B, n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status1);
    MPI_Wait(&request, &status2);

    for(i = 0; i < blk_size; i++)
    {
        sum = 0;
        for(j = 0; j < n ; j++)
        {
            sum += D[i*n+j] * B[j];
        }
        C[i] = sum;
    }

    MPI_Send(C, blk_size, MPI_INT, 0, 2, MPI_COMM_WORLD);

}


