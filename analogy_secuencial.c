// KONPILATZEKO - PARA COMPILAR: (C: -lm) (CUDA: -arch=sm_61)
// EXEC: analogy embeddings.dat dictionary.dat 
// Ej., king – man + woman = queen

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "biblioteca_funciones.h"

int main(int argc, char *argv[]) 
{
	int		i, j, numwords, idx1, idx2, idx3;
	int 	closest_word_idx;
	float	max_similarity;
	float 	*words;
	FILE    	*f1, *f2;
	char 	**dictionary;  
	char	target_word1[TAM], target_word2[TAM], target_word3[TAM];
	float	*result_vector;
	float	*sim_cosine;

	struct timespec  t0, t1;
	double tej;


	if (argc < 3) {
		printf("Deia: analogia embedding_fitx hiztegi_fitx\n");
		exit (-1);;
	}  


	// Irakurri datuak sarrea-fitxategietatik
	// ====================================== 
	f1 = fopen (argv[1], "r");
	if (f1 == NULL) {
		printf ("Errorea %s fitxategia irekitzean\n", argv[1]);
		exit (-1);
	}

	f2 = fopen (argv[2], "r");
	if (f1 == NULL) {
		printf ("Errorea %s fitxategia irekitzean\n", argv[2]);
		exit (-1);
	}


	fscanf (f1, "%d", &numwords);	       // prozesatu behar den hitz kopurua fitxategitik jaso
	if (argc == 4) numwords = atoi (argv[3]);   // 3. parametroa = prozesatu behar diren hitzen kopurua  
	printf ("numwords = %d\n", numwords);

	words = (float*)malloc (numwords*EMB_SIZE*sizeof(float));
	dictionary = (char**)malloc (numwords*sizeof(char*));
	for (i=0; i<numwords;i++){
		dictionary[i] = (char*)malloc(TAM*sizeof(char));
	}
	sim_cosine = (float*)malloc (numwords*sizeof(float));
	result_vector = (float*)malloc (EMB_SIZE*sizeof(float));

	for (i=0; i<numwords; i++) {
		fscanf (f2, "%s", dictionary[i]);
		for (j=0; j<EMB_SIZE; j++) {
			fscanf (f1, "%f", &(words[i*EMB_SIZE+j]));
		}
	}
	printf("Sartu analogoak diren bi hitzak eta analogia bilatu nahi diozun hitza: \n");
	printf("Introduce las dos palabras analogas y la palabra a la que le quieres buscar la analogia: \n");
	scanf ("%s %s %s",target_word1, target_word2, target_word3);

	//calculamos los indices y lanzamos errores si no estan en los diccionarios:
	idx1 = word2ind(target_word1,dictionary,numwords);
	idx2 = word2ind(target_word2,dictionary,numwords);
	idx3 = word2ind(target_word3,dictionary,numwords);

	if (idx1 == -1 ) {
		printf(" No se encontró la palabra \"%s\". en el dicionario\n", target_word1);
		return -1;
	}
	if (idx2 == -1 ) {
		printf(" No se encontró la palabra \"%s\". en el dicionario\n", target_word2);
		return -1;
	}
	if (idx3 == -1 ) {
		printf(" No se encontró la palabra \"%s\". en el dicionario\n", target_word3);
		return -1;
	}
	printf("se han encontrado correctamente las 3 palabras en el diccionario");

	clock_gettime (CLOCK_REALTIME, &t0);
	/***************************************************/  
	//operamos los vectores de las respectivas palabras para obtener el vector de la palabra que mas se pareceria
	perform_analogy(words,idx1,idx2,idx3,result_vector);
	//printf("el resultado de la analolgia de vectores es: %f,%f,%f...\n",result_vector[0],result_vector[1],result_vector[2]);
	find_closest_word(result_vector,words,numwords,idx1,idx2,idx3,&closest_word_idx,&max_similarity);

	clock_gettime (CLOCK_REALTIME, &t1);   
	if (closest_word_idx != -1) {
		printf("\nClosest_word: %s (%d), sim = %f \n", dictionary[closest_word_idx],closest_word_idx, max_similarity);
	} else printf("No close word found.\n");


	tej = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / (double)1e9;
	printf("\n Tej. (serie) = %1.3f ms\n\n", tej*1000);

	fclose (f1);
	fclose (f2);

	free(words);
	free(sim_cosine);
	free(result_vector);
	for (i=0; i<numwords;i++) free (dictionary[i]);
	free(dictionary); 

	return 0;
}

