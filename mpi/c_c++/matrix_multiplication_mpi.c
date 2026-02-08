
#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

#define world MPI_COMM_WORLD

int main(int argc, char ** argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(world, &rank);
  MPI_Comm_size(world, &size);

  int k = 100, m = 50, n = 50, p = 50;

  MPI_Bcast(&k, 1, MPI_INT, 0, world);
  MPI_Bcast(&m, 1, MPI_INT, 0, world);
  MPI_Bcast(&n, 1, MPI_INT, 0, world);
  MPI_Bcast(&p, 1, MPI_INT, 0, world);

  int a[k][m][n], b[k][n][p], r[k][m][p];

  if(rank == 0) {
    for(int i = 0;i<k;i++) {
      for(int j = 0;j < m; j++) {
        for(int l = 0; l< n; l++) {
          a[i][j][l] = rand() % 100;
        }
      }

      for(int j = 0;j < n; j++) {
        for(int l = 0; l< p; l++) {
          b[i][j][l] = rand() % 100;
        }
      }
    }
  }

  int chunk = k / size;
  int locacA[chunk][m][n], localB[chunk][n][p], localR[chunk][m][p];

  int sizeA = chunk * m * n;
  int sizeB = chunk * n * p;
  int sizeR = chunk * m * p;

  MPI_Scatter(a, sizeA, MPI_INT, locacA, sizeA, MPI_INT, 0, world);
  MPI_Scatter(b, sizeB, MPI_INT, localB, sizeB, MPI_INT, 0, world);

  MPI_Barrier(world);

  double start = MPI_Wtime();
  for(int i = 0;i < k; i++) {
    for(int j = 0; j < m; j++) {
      for(int l = 0; l < p; l++) {
        localR[i][j][l] = 0;
        for(int h = 0; h < n; h++) {
          localR[i][j][l] += (locacA[i][j][h] * localB[i][h][j]) % 100;
        }
        localR[i][j][l] %= 100;
      }
    }
  }
  double end = MPI_Wtime();
  MPI_Gather(localR, sizeR, MPI_INT, r, sizeR, MPI_INT, 0, world);

  printf("process : %d time: %fs\n", rank, end-start);
  MPI_Finalize();
  return 0;
}
