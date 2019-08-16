#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define num_of_messages 50

int main(int argc, char** argv)
{
    int my_rank, size;
    double t_start, t_end,t;
    float buffer[1];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    t_start = MPI_Wtime();

    for(int i = 0; i < num_of_messages; i++)
    {
        if(my_rank == 0)
        {
            MPI_Send(buffer, 1, MPI_FLOAT, 1, 17, MPI_COMM_WORLD);
            MPI_Recv(buffer, 1, MPI_FLOAT, 1, 23, MPI_COMM_WORLD, &status);
        }
        else if (my_rank == 1)
        {
            MPI_Recv(buffer, 1, MPI_FLOAT, 0, 17, MPI_COMM_WORLD, &status);
            MPI_Send(buffer, 1, MPI_FLOAT, 0, 23, MPI_COMM_WORLD);
        }
    }

    t_end = MPI_Wtime();
    t = t_end - t_start;

    if(my_rank == 0)
        printf("Total Time: %f seconds.",t);

    MPI_Finalize();

    return 0;
}
