#include<bits/stdc++.h>
#include<cuda_runtime.h>
using namespace std;

__global__ void multiply(float* A, float* B, float* C,int M, int N, int P, int offset) {
  int k = threadIdx.x + offset;

  float* a = A + k * M * N;
  float* b = B + k * N * P;
  float* c = C + k * M * P;

  int i, j, l;

  for(i = 0; i < M; i++) {
    for(j = 0; j < P; j++) {
      for(l = 0; l < N; l++) {
        c[i * P + j] += a[i * N + l] * b[l * P + j];
      }
    }
  }
}

int main(int argc, char* argv[]) {
  if(argc < 3) {
    cout<<"provide thread limit and number of matrics"<<endl;
    return 0;
  }

  int threadLimit = atoi(argv[2]);
  int k = atoi(argv[1]);

  int M = 400, N = 400, P = 400;
  int sizeA = k * M * N;
  int sizeB = k * N * P;
  int sizeC = k * M * P;

  float* h_a = new float[sizeA];
  float* h_b = new float[sizeB];
  float* h_c = new float[sizeC];

  for(int i = 0; i < sizeA; i++) {
    h_a[i] = rand();
  }

  for(int i = 0; i < sizeB; i++) {
    h_b[i] = rand();
  }

  float* d_a;
  float* d_b;
  float* d_c;

  cudaMalloc(&d_a, sizeA * sizeof(float));
  cudaMemcpy(d_a, h_a, sizeA * sizeof(float), cudaMemcpyHostToDevice);

  cudaMalloc(&d_b, sizeB * sizeof(float));
  cudaMemcpy(d_b, h_b, sizeB * sizeof(float), cudaMemcpyHostToDevice);


  int remaining = k;
  int offset = 0;
  while(remaining > 0) {
    int batchSize = min(remaining, threadLimit);

    multiply<<<1,batchSize>>>(d_a, d_b, d_c, M, N, P, offset);
    cudaDeviceSynchronize();

    remaining -= batchSize;
    offset += batchSize;
  }

  cudaMemcpy(h_c, d_c, sizeC * sizeof(float), cudaMemcpyDeviceToHost);
  cout<<"ALL Operation Done"<<endl;
  return 0;
}
