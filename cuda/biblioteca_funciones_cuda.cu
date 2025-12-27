#include "biblioteca_funciones.h"
#include <driver_types.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <texture_types.h>

//-------------------
// COMPARTIDAS:
//-------------------

__device__ double dot_product(float *a, float *b, int size) {
  double result = 0;
  for (int i = 0; i < size; i++) {
    result += a[i] * b[i];
  }
  return result;
}

__device__ float magnitude(float *vec, int size) {
  float sum = 0;
  for (int i = 0; i < size; i++) {
    sum += vec[i] * vec[i];
  }
  return sqrt(sum);
}

__device__ float cosine_similarity(float *vec1, float *vec2, int size) {
  float mag1, mag2;

  mag1 = magnitude(vec1, size);
  mag2 = magnitude(vec2, size);
  return dot_product(vec1, vec2, size) / (mag1 * mag2);
}

//-------------------
// Analogy:
//-------------------

int word2ind(char *word, char **dictionary, int numwords) {
  for (int i = 0; i < numwords; i++) {
    if (strcmp(word, dictionary[i]) == 0) {
      return i;
    }
  }
  return -1; // if the word is not found
}

// Función para calcular la analogía
// no creo que merezca la pena ser paralelizada ya que es muy pequepa
void perform_analogy(float *words, int idx1, int idx2, int idx3,
                     float *result_vector) {
  int i;
  for (i = 0; i < EMB_SIZE; i++) {
    result_vector[i] = words[idx1 * EMB_SIZE + i] - words[idx2 * EMB_SIZE + i] +
                       words[idx3 * EMB_SIZE + i];
  }
  return;
}

// // Función para encontrar la palabra más cercana al vector resultante
// void find_closest_word(float *result_vector, float *words, int numWords, int
// idx1, int idx2, int idx3, int *closest_word_idx, float *max_similarity,int
// numBloques,int numHilos) { 	int i,j; 	*max_similarity = 0.0;
// *closest_word_idx = -1;	//copiamos el nuevo indice 	float
// *maxBloques = (float*) calloc(numBloques, sizeof(float)); 	int * indices
// =(int*)malloc(sizeof(int)*numBloques); 	int *cu_ind; 	float *
// cu_max,*cu_words,*cu_vector_res;
// 	//pasar los datos a el device:
// 	//TODO
// 	//crear vector maxBloques
// 	cudaMalloc(&cu_max,numBloques*sizeof(float));
// 	//crear y pasar vector indicesBloques
// 	cudaMalloc(&cu_ind,numBloques*sizeof(int));
// 	//crear y pasar result_vector
// 	cudaMalloc(&cu_vector_res, EMB_SIZE*sizeof(float));
// 	cudaMemcpy (cu_vector_res,result_vector,EMB_SIZE*sizeof(float),
// cudaMemcpyHostToDevice);
// 	//crear y pasar words
// 	cudaMalloc(&cu_words,numWords*EMB_SIZE*sizeof(float));
// 	cudaMemcpy (cu_words,words, numWords*EMB_SIZE*sizeof(float),
// cudaMemcpyHostToDevice);
//
// 	// llamada a kernel:
// 	reduc_analogy<<<numBloques, numHilos>>>(idx1, idx2, idx3, numWords,
// cu_ind, cu_max, cu_words, cu_vector_res); reduc_analogy(int idx1,int idx2,int
// idx3,int numWords,int * indicesBloques, float * maximosBloques,float *
// words,float * result_vector)
// 	//traerse los resultados del kernel:
// 	cudaMemcpy(maxBloques, cu_max, numBloques*sizeof(float),
// cudaMemcpyDeviceToHost); 	cudaMemcpy(indices, cu_ind,
// numBloques*sizeof(int), cudaMemcpyDeviceToHost);
// 	//procesar reslutados:
// 	*max_similarity = -1.0;
// 	for(i=0;i<numBloques;i++){
// 		if (*max_similarity<maxBloques[i]){
// 			*max_similarity = maxBloques[i];
// 			*closest_word_idx = indices[i];
// 		}
// 	}
//
// 	//TODO algun free mas?
// 	free(maxBloques);
// 	free(indices);
// 	cudaFree(cu_max);
// 	cudaFree(cu_ind);
// 	cudaFree(cu_vector_res);
// 	cudaFree(cu_words);
// 	return;
// }
//
// __global__ void reduc_analogy(int idx1,int idx2,int idx3,int numWords,int *
// indicesBloques, int * maximosBloques,float * words,float * result_vector)
// {
// 	int tid = threadIdx.x;
// 	int idx = blockIdx.x * blockDim.x + threadIdx.x;
// 	int i;
// 	int stride=blockDim.x*gridDim.x;
// 	int indice;
// 	float simil;
// 	float maxSimil=-1;
// 	__shared__ int indices[blockDim.x];
// 	__shared__ float maximos[blockDim.x];
// 	for (i = idx;i<numWords;i+=stride){
// 		simil =
// cosine_similarity(result_vector,&words[i*EMB_SIZE],EMB_SIZE); if
// (simil>maxSimil&& i !=idx1&& i !=idx2&& i !=idx3){ 			maxSimil
// = simil; 			indice = i;
// 		}
// 	}
// 	indices[tid]=indice;
// 	maximos[tid]=maxSimil;
// 	//ahora tenemos los mas grandes entre los que le ha tocado procesar a
// este thread
// 	__syncthreads ();
// 	for (stride=1; stride<blockDim.x; stride*=2)
// 	{
// 		if ((tid % (2*stride)) == 0)
// 		{
// 			if(maximos[tid+stride]>maximos[tid]){
// 				maximos[tid]=maximos[tid+stride];
// 				indices[tid]=indices[tid+stride];
// 			}
// 			//esto no hace obviamente falta obviamente, pero lo dejo
// porque se entiende un poco mejor la logica:
// 			//else{
// 			//maximos[tid]=maximos[tid];
// 			//indices[tid]=indices[tid]:
// 			//}
// 		}
// 		__syncthreads (); // sincronización de los hilos de un bloque
// 	}
// 	if (tid == 0){
// 		indicesBloques[blockIdx.x] = maximos[0];
// 		maximosBloques[blockIdx.x] = indices[0];
// 	}
// }

//-------------------
// KNN:
//-------------------

__global__ void precalculate_norms(float *words, float *norms, int numwords) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < numwords) {
    norms[idx] = magnitude(&words[idx * EMB_SIZE], EMB_SIZE);
  }
}

__global__ void knn_cuda(float *words, int numwords, float *similarities,
                         float *norms) {
  int tid = threadIdx.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  int i, j;
  int nwords = numwords;
  float norm_i;

  for (i = idx; i < nwords; i += stride) {
    norm_i = norms[i];
    for (j = 0; j < nwords; j++) {
      float dot =
          dot_product(&words[i * EMB_SIZE], &words[j * EMB_SIZE], EMB_SIZE);
      similarities[i * nwords + j] = dot / (norm_i * norms[j]);
    }
  }
}

void knn_complet(float *words, int numwords, float *similarities) {
  float *wordsCuda, *similaritiesCuda, *normsCuda;
  int numBloques = (numwords + 255) / 256, tamBloques = 256;

  float Tex;
  cudaEvent_t t0, t1;

  cudaMalloc(&wordsCuda, numwords * EMB_SIZE * sizeof(float));
  cudaMemcpy(wordsCuda, words, numwords * EMB_SIZE * sizeof(float),
             cudaMemcpyHostToDevice);
  cudaMalloc(&similaritiesCuda, numwords * numwords * sizeof(float));

  cudaMalloc(&normsCuda, numwords * sizeof(float));

  precalculate_norms<<<numBloques, tamBloques>>>(wordsCuda, normsCuda,
                                                 numwords);
  cudaDeviceSynchronize();

  // cudaMalloc(&numwordsCuda, sizeof(int));
  // cudaMemcpy(numwordsCuda, &numwords, sizeof(int), cudaMemcpyHostToDevice);

  cudaEventCreate(&t0); // crear objeto
  cudaEventCreate(&t1);
  cudaEventRecord(t0); // tiempo en t

  knn_cuda<<<numBloques, tamBloques>>>(wordsCuda, numwords, similaritiesCuda,
                                       normsCuda);

  cudaEventRecord(t1);      // tiempo en t1
  cudaEventSynchronize(t1); // esperar hasta que
  // t1 esté listo
  cudaEventElapsedTime(&Tex, t0, t1); // calcula el tiempo
  // entre t0 y t1
  printf(" Tex (kernela): %fms\n", Tex); // milisegundos
  cudaEventDestroy(t0);                  // borrar objeto
  cudaEventDestroy(t1);

  cudaMemcpy(words, wordsCuda, numwords * EMB_SIZE * sizeof(float),
             cudaMemcpyDeviceToHost);
  cudaMemcpy(similarities, similaritiesCuda,
             numwords * numwords * sizeof(float), cudaMemcpyDeviceToHost);
  cudaFree(wordsCuda);
  cudaFree(similaritiesCuda);
  cudaFree(normsCuda);
}

//-------------------
// Debuging:
//-------------------
void imprimir_vector_float(float *vector, int tamano) {
  printf("\n");
  for (int i = 0; i < EMB_SIZE; i++)
    printf(" %2.3f", vector[i]);
  printf("\n");
}
