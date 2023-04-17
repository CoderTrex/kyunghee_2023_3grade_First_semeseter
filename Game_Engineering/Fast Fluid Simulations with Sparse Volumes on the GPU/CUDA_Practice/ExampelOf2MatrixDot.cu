#include "device_launch_parameters.h" 
#include <cuda_runtime.h> 
#include <stdlib.h> 
#include <stdio.h> 
#define SIZE (2048 * 2048)
#define THREADS_PER_BLOCK 512

// // __global__을 통해서 커널임을 표시한다. host에서 호출된다. 
// __global__ void dot(int *a, int *b, int *c, int n){ 
//     // 수많은 블록과 스레드가 동시에 처리한다. 
//     // 위에서 정의한 index를 통해서 스레드들을 구별한다. 
//     int index = threadIdx.x + blockIdx.x * blockDim.x;
// 	c[index] = a[index] * b[index];
//     printf("%d = %d * %d\n", c[index], a[index], b[index]); 
// } 

__global__ void dot(int *a, int *b, int *c)
{
    __shared__ int      temp[THREADS_PER_BLOCK];
    int                 index = threadIdx.x + blockIdx.x * blockDim.x;
    
    temp[threadIdx.x] = a[index] * b[index];
    
    __syncthreads();

    if(threadIdx.x == 0){
        int sum = 0;
        for(int i = 0 ; i < THREADS_PER_BLOCK ; i++){
            sum += temp[i];
        }
        atomicAdd(c, sum);
    }
}

int main(){ 
    int *a, *b, *c; 
    int *d_a, *d_b, *d_c;
    int size = N * sizeof(int);
    
    // 호스트의 메모리에 할당한다. 
    a = (int *)malloc(size);
    b = (int *)malloc(size); 
    c = (int *)malloc(size); 
    
    // cudaMalloc(destination, number of byte)로 device의 메모리를 할당한다.
    cudaMalloc(&d_a, size); 
    cudaMalloc(&d_b, size); 
    cudaMalloc(&d_c, size); 
    
    // 초기화 
    for (int i = 0; i<SIZE; ++i) { 
        a[i] = i; 
        b[i] = i; 
        c[i] = 0; 
    } 
    // cudaMemcpy(destination, source, number of byte, cudaMemcpyHostToDevice)로 호스트에서 디바이스로 메모리를 카피한다. 
    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_c, c, size, cudaMemcpyHostToDevice); 
    
    // 함수 호출을 위해서 새로운 신텍스 요소를 추가할 필요가 있다. 
    // 첫번째 parameter는 블럭의 수이다. 예제에서는 스레드 블럭이 하나이다. 
    // SIZE는 1024개의 스레드를 의미한다. 
    dot <<< SIZE / THREADS_PER_BLOCK , THREADS_PER_BLOCK >>>(d_a, d_b, d_c, SIZE); 
    
    //cudaMemcpy(source, destination, number of byte, cudaMemDeviceToHost)로 디바이스의 메모리(연산 결과 데이터)를 호스트에 카피한다. 
    cudaMemcpy(a, d_a, size, cudaMemcpyDeviceToHost); 
    cudaMemcpy(b, d_b, size, cudaMemcpyDeviceToHost); 
    cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost); 
    
    for (int i = 0; i<SIZE; ++i) {
        printf("c[%d] = %d\n", i, c[i]);
	}
    
    // 호스트의 메모리 할당 해제 
    free(a); 
    free(b); 
    free(c); 
    // cudaFree(d_a)를 통해 디바이스의 메모리를 할당 해제 
    cudaFree(d_a); 
    cudaFree(d_b); 
    cudaFree(d_c); 
    
    return 0;
}