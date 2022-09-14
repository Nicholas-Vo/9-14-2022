/* File: mpi_array.c
 Processing array using point-to-point communication.
 IT 388 - Fall 2022
 Nick Voss
 */

#include <stdio.h>
#include <mpi.h>

void printArray(int a[], int N);

int main(int argc, char *argv[]) {
    int N = 12; // array length
    int array[N], array_new[N];
    for (int i = 0; i < N; i++) {
        array[i] = 0;
        array_new[i] = 0;
    }
    int my_rank, nproc;
    int offset, n_local, dest, source;

    /* Start MPI*/
    MPI_Init(&argc, &argv);
    MPI_Comm comm;
    MPI_Status status;
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &nproc);
    MPI_Comm_rank(comm, &my_rank);
    // Get the name of the processor
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Get_processor_name(proc_name, &len);

    if (my_rank == 0) { // Manager core
        /* 1. Define local amount of items each processor will work on */
        int items = N / nproc;
        /* 2.1 Manager initializing array */
        for (int i = 0; i < N; i++) {
            array[i] = i + 1;
        }
        /* 2.2 Manager prints initial array*/
        printArray(array, N);
        /* 2.3 manager sends subarrays to each process */
        for (int i = 1; i < nproc - 1; i++) {
            MPI_Send(&items, // buffer
                     1, // number of elements in buffer
                     MPI_INT, // data type
                     i, // destination
                     0, // message tag
                     comm);
        }
        /* 2.4. manager works on its part */
        for (int i = 0; i < items; i++) {
            // Not sure what should go here
        }
        /* 2.5. manager receives calculations from workers */
        int recv;
        for (int i = 1; i < nproc - 1; i++) {
            MPI_Recv(&recv, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm, &status);
            printf("recv: %d\n", recv);
            array_new[i] += recv;
        }
        /* 2.6. manager prints the processed array */
        printf("Final array = ");
        printArray(array_new, N);
    } else { // not manager core
        /* 3.0 Workers receive offset, and its portion of the data */
        int items = N / nproc;
        int temp[items];
        MPI_Recv(&items, 1, MPI_INT, 0, 0, comm, &status);
        //MPI_Recv(&array_new, items, MPI_INT, 0, 0, comm, &status);
        /* 3.1 Worker prints processor name, its rank and received array*/
        printf("Name: %s Rank: %d", proc_name, my_rank);
        printArray(temp, items);
        /* 3.2. Worker works on array, doing some calculation on it */
        for (int i = 0; i < items; i++) {
            temp[i] += 1;
        }
        /* 3.3. Workers send their work back to manager */
        for (int i = 0; i < items; i++) {
            MPI_Send(&temp[i], 1, MPI_INT, 0, 0, comm);
        }
    }

    MPI_Finalize();
    return 0;
}

/*--------------------
 Print array to screen
 */
void printArray(int a[], int N) {
    printf("[");
    for (int i = 0; i < N - 1; i++) {
        printf("%d,", a[i]);
    }
    printf("%d]\n", a[N - 1]);
}
