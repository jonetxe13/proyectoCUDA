
// EXEC: knn embeddins.dat similarities.dat [numwords]   

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "biblioteca_funciones.h"

int main(int argc, char *argv[]) 
{
	int		i, j, numwords, row, col;
	float 	*words;
	FILE    	*f1, *f2;
	float 	*similarities;

	struct timespec  t0, t1;
	double tej;


	if (argc < 3) {
		printf("Deia-Llamada: knn embeddins.dat similarities.dat [numwords]\n");
		printf("\t(in) embeddings.dat\n");
		printf("\t(out) similarities.dat\n");
		printf("\t(hautazkoa) prozesatu nahi den hitz kopurua -- (opcional) num de palabras que se quieren procesar\n");
		exit (-1);;
	}  

	// Irakurri datuak sarrea-fitxategietatik
	// ====================================== 

	f1 = fopen (argv[1], "r");
	if (f1 == NULL) {
		printf ("Errorea %s fitxategia irekitzean -- Error abriendo fichero\n", argv[1]);
		exit (-1);
	}

	fscanf (f1, "%d", &numwords);	       
	if (argc == 4) numwords = atoi (argv[3]); 
	printf ("numwords = %d\n", numwords);

	/******************************************************************
	// Asignar memoria dinámica a las estructuras de datos words y similarities

	//    OSATZEKO - PARA COMPLETAR
	 ******************************************************************/

	words = (float *)malloc(numwords*EMB_SIZE*sizeof(float));
	similarities = (float *)malloc(numwords*numwords*sizeof(float));

	for (i=0; i<numwords; i++) {
		for (j=0; j<EMB_SIZE; j++) {
			fscanf (f1, "%f", &(words[i*EMB_SIZE+j]));
		}
	}
	printf ("Embeddingak irakurrita\n");

	printf("Hitz guztien auzokideak kalkulatzera zoaz\n");
	clock_gettime (CLOCK_REALTIME, &t0);
	/******************************************************************
	// Deitu funtzioari
	// Llamar a la función

	//    OSATZEKO - PARA COMPLETAR
	 ******************************************************************/
	knn_complet(words, numwords, similarities);
	clock_gettime (CLOCK_REALTIME, &t1);

	tej = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / (double)1e9;
	printf("\n Tej. (serie) = %1.3f ms\n\n", tej*1000);

	// Idatzi antzekotasunak similarities fitxategietan -- Escribe las similitudes en el fichero similarities
	f2 = fopen (argv[2], "w");
	if (f2 == NULL) {
		printf ("Errorea %s fitxategia irekitzean -- Error abriendo fichero\n", argv[2]);
		exit (-1);
	}

	fprintf (f2, "%d\n", numwords);
	for (i=0; i<numwords; i++) {
		for (j=0; j<numwords; j++) {
			fprintf (f2, "%f\t", similarities[i*numwords+j]);
		} 
		fprintf  (f2, "\n"); 
	}
	printf ("antzekotasunak idatzita fitxategietan -- similitudes escritas en el fichero\n");

	fclose (f1);
	fclose (f2);

	free(words);
	free(similarities); 

	return 0;
}

