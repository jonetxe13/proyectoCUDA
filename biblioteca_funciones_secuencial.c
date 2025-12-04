#include <math.h>
#include <string.h>
#include "biblioteca_funciones.h"

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
	int i;
	for(i;i<EMB_SIZE;i++){
		result_vector[i]=words[idx1*EMB_SIZE+i]-words[idx2*EMB_SIZE+i]+words[idx3*EMB_SIZE+i];
	}
	return;
}

// Función para encontrar la palabra más cercana al vector resultante
void find_closest_word(float *result_vector, float *words, int numwords, int idx1, int idx2, int idx3, int *closest_word_idx, float *max_similarity) {
	/*******************************************************
	 *       OSATZEKO - PARA COMPLETAR
	 *             find closest word using cosine_similarity function
	                  ********************************************************/
	int i,j;
	float max = 0.0;
	float simil;

	for(i = 0; i<numwords;i++){
		simil = cosine_similarity(result_vector,words[i*EMB_SIZE],EMB_SIZE);
		if ( simil > max){
			max=simil;
			for(j=0;j<EMB_SIZE;j++)
				result_vector[i]=(words[i*EMB_SIZE+j]);
		}
	}
	return;
}



//-------------------
// KNN:
//-------------------

void knn_complet(float *words, int numwords, float *similarities) {
	int i,j;
	for(int i = 0; i < numwords; i++){
		for(int j = 0; j < numwords; j++){
			similarities[i*numwords+j] = cosine_similarity(&words[i*EMB_SIZE], &words[j*EMB_SIZE], EMB_SIZE);
		}
	}
}

