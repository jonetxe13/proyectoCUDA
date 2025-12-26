#include <math.h>
#include <string.h>
#include <stdio.h>
#include "biblioteca_funciones.h"
#include <stdlib.h>

//-------------------
// COMPARTIDAS:
//-------------------

double dot_product(float* a, float* b, int size) {
	double result = 0;
	for (int i = 0; i < size; i++) {
		result += a[i] * b[i];
	}
	return result;
}

float magnitude(float* vec, int size) {
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += vec[i] * vec[i];
	}
	return sqrt(sum);
}

float cosine_similarity(float* vec1, float* vec2, int size) {
	float mag1, mag2;

	mag1 = magnitude(vec1, size);
	mag2 = magnitude(vec2, size);
	return dot_product(vec1, vec2, size) / (mag1 * mag2);
}

//-------------------
// Analogy:
//-------------------

int word2ind(char* word, char** dictionary, int numwords) {
    for (int i = 0; i < numwords; i++) {
        if (strcmp(word, dictionary[i]) == 0) {
            return i;
        }
    }
    return -1;  // if the word is not found
}

// Función para calcular la analogía
//no creo que merezca la pena ser paralelizada ya que es muy pequepa
void perform_analogy(float *words, int idx1, int idx2, int idx3, float *result_vector) {
	int i;
	for(i=0;i<EMB_SIZE;i++){
		result_vector[i]=words[idx1*EMB_SIZE+i]-words[idx2*EMB_SIZE+i]+words[idx3*EMB_SIZE+i];
	}
	return;
}

// Función para encontrar la palabra más cercana al vector resultante
void find_closest_word(float *result_vector, float *words, int numwords, int idx1, int idx2, int idx3, int *closest_word_idx, float *max_similarity,int numBloques,int numHilos) {
	int i,j;
	float simil;
	* max_similarity = 0.0;
	* closest_word_idx = -1;	//copiamos el nuevo indice	
	float * resultados =(float*) calloc(sizeof(float)*numBloques);
	int * indices =(int*)malloc(sizeof(int)*numBloques);
	//pasar los datos a el device:
	//TODO
	//crear y pasar vector maxBloques 
	//crear y pasar vector indicesBloques
	//crear y pasar result_vector
	//crear y pasar words 
	// llamada a kernel:
	reduc_analogy<<<numBloques,numHilos>>> (idx_1,idx_2,idx_3,d_vec, d_reduc);
	//procesar reslutados:
	max_similarity = -1;
	for(i=0;i<numWords;i++){
		if (max_similarity<maxBloques[i]){
			max_similarity = maxBloques[i];	
			*closest_word_idx = indicesBloques[i];
		}
	}

	//TODO
	free(resultados);
	free(indices);
	return;
}

__global__ void reduc_analogy(int idx_1,int idx_2,int idx_3int numWords,int * indicesBloques, int * maximosBloques)
{
	int tid = threadIdx.x;
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	int i;
	int stride=blockDim.x*gridDim.x;
	int indice;
	float simil;
	float maxSimil=-1;
	__shared__ int indices[blockDim.x];
	__shared__ float maximos[blockDim.x];
	for (i = idx;i<numWords;i+=stride){
		simil = cosine_similarity(result_vector,&words[i*EMB_SIZE],EMB_SIZE);
		if (simil>maxSimil&& i !=idx_1&& i !=idx_2&& i !=idx_3){
			maxSimil = simil;
			indice = i;
		}
	}
	indices[tid]=indice;
	maximos[tid]=maxSimil;	
	//ahora tenemos los mas grandes entre los que le ha tocado procesar a este thread
	__syncthreads ();  
	for (stride=1; stride<blockDim.x; stride*=2)
	{
		if ((tid % (2*stride)) == 0)
		{
			if(maximos[tid+stride]>maximos[tid]){
				maximos[tid]=maximos[tid+stride];
				indices[tid]=indices[tid+stride]:
			}
			//esto no hace obviamente falta obviamente, pero lo dejo porque se entiende un poco mejor la logica:
			//else{
			//maximos[tid]=maximos[tid];
			//indices[tid]=indices[tid]:
			//}
		}
		__syncthreads (); // sincronización de los hilos de un bloque
	}
	if (tid == 0){
		indicesBloques[blockIdx.x] = maximos[0];
		maximosBloques[blockIdx.x] = indices[0];
	}
}

//-------------------
// KNN:
//-------------------

void knn_complet(float *words, int numwords, float *similarities) {
	int i,j;
	for(i = 0; i < numwords; i++){
		for(j = 0; j < numwords; j++){
			similarities[i*numwords+j] = cosine_similarity(&words[i*EMB_SIZE], &words[j*EMB_SIZE], EMB_SIZE);
		}
	}
}
//-------------------
// Debuging:
//-------------------
void imprimir_vector_float(float *vector, int tamano){
	printf("\n");
	for (int i = 0; i < EMB_SIZE; i++)	printf(" %2.3f",vector[i]);
	printf("\n");
}

