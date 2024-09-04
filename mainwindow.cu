
#include <iostream>

__device__ int mandelbrot(double a, double b, int maxIteration) {
    double real = a;
    double imag = b;
    int iteration = 0;
    while (real * real + imag * imag <= 4.0 && iteration < maxIteration) {
        double tempReal = real * real - imag * imag + a;
        imag = 2 * real * imag + b;
        real = tempReal;
        ++iteration;
    }
    return iteration;
}


__global__ void mandelbrotKernel(int width, int height, double minX, double minY,
                                 double scaleX, double scaleY, int offsetX, int offsetY,
                                 int maxIteration, double* output) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height) {
        double a = minX + (x + offsetX) * scaleX;
        double b = minY + (y + offsetY) * scaleY;

        int iteration = mandelbrot(a, b, maxIteration);
        output[y * width + x] = iteration;
    }
}

extern "C"
double* CudaExec(int width, int height, double minX, double minY, double scaleX, double scaleY,
              int offsetX, int offsetY, int maxIteration){

    double* h_output = new double[width * height];
    double* d_output;

    cudaMalloc(&d_output, width * height * sizeof(double));

    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((width + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (height + threadsPerBlock.y - 1) / threadsPerBlock.y);
    mandelbrotKernel<<<blocksPerGrid, threadsPerBlock>>>(width,height, minX, minY, scaleX, scaleY,offsetX, offsetY, maxIteration, d_output);
    cudaDeviceSynchronize();
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        std::cout << "CUDA error: " << cudaGetErrorString(err);
    }

    cudaMemcpy( h_output, d_output, width * height * sizeof(double), cudaMemcpyDeviceToHost);
    cudaFree(d_output);


    return h_output;
}
