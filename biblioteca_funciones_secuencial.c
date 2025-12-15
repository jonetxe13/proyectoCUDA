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
void perform_analogy(float *words, int idx1, int idx2, int idx3, float *result_vector) {
	//printf("se va a calcular la analogia\n");
	//printf("vector con indice %i\n",idx1);
	//printf("vector con indice %i\n",idx2);
	//printf("vector con indice %i\n",idx3);
	//imprimir_vector_float(&words[idx1*EMB_SIZE],EMB_SIZE);
	//imprimir_vector_float(&words[idx2*EMB_SIZE],EMB_SIZE);
	//imprimir_vector_float(&words[idx3*EMB_SIZE],EMB_SIZE);
	int i;
	for(i=0;i<EMB_SIZE;i++){
		result_vector[i]=words[idx1*EMB_SIZE+i]-words[idx2*EMB_SIZE+i]+words[idx3*EMB_SIZE+i];
	}
	//imprimir_vector_float(result_vector,EMB_SIZE);
	return;
}

// Función para encontrar la palabra más cercana al vector resultante
void find_closest_word(float *result_vector, float *words, int numwords, int idx1, int idx2, int idx3, int *closest_word_idx, float *max_similarity) {
	int i,j;
	float simil;
	* max_similarity = 0.0;
	* closest_word_idx = -1;	//copiamos el nuevo indice	
	//imprimir_vector_float(result_vector,EMB_SIZE);
	for(i = 0; i<numwords;i++){
		simil = cosine_similarity(result_vector,&words[i*EMB_SIZE],EMB_SIZE);
		if ( simil > * max_similarity && i != idx1 && i != idx2 && i != idx3){
			* max_similarity=simil;	//copiamos el nuevo mas
			for(j=0;j<EMB_SIZE;j++)	//copiamos el nuevo vector result_vector[i]=(words[i*EMB_SIZE+j]);
			* closest_word_idx = i;	//copiamos el nuevo indice	
		}
	}
	return;
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

