
// EXEKUTATZEKO: kmeans embeddings.dat dictionary.dat myclusters.dat [numwords]    // numwords: matrize txikiekin probak egiteko 

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include "biblioteca_funciones.h" 
struct clusterinfo	 // clusterrei buruzko informazioa -- informacion de los clusters
{
	int  elements[VOCAB_SIZE]; 	// osagaiak -- elementos
	int  number;       		// osagai kopurua -- número de elementos
};


// Bi bektoreen arteko biderketa eskalarra kalkulatzeko funtzioa


// Distantzia euklidearra: bi hitzen kenketa ber bi, eta atera erro karratua
// Distancia euclidea: raiz cuadrada de la resta de dos palabras elevada al cuadrado
// Adi: double
double word_distance (float *word1, float *word2)
{
	/****************************************************************************************
	  OSATZEKO - PARA COMPLETAR
	  ????????????????????????????????? no se si es correcto
	 ****************************************************************************************/

	double distancia = 0;
	for(int i = 0; i < EMB_SIZE; i++){
		distancia += pow((word1[i] - word2[i]), 2);
	}
	return sqrt(distancia);
}

// Zentroideen hasierako balioak ausaz -- Inicializar centroides aleatoriamente
void initialize_centroids(float *words, float *centroids, int n, int numclusters, int dim) {
	int i, j, random_index;
	for (i = 0; i < numclusters; i++) {
		random_index = rand() % n;
		for (j = 0; j < dim; j++) {
			centroids[i*dim+j] = words[random_index*dim+j];
		}
	}
}

// Zentroideak eguneratu -- Actualizar centroides
void update_centroids(float *words, float *centroids, int *wordcent, int numwords, int numclusters, int dim, int *cluster_sizes) {

	int i, j, cluster;

	for (int i = 0; i < numclusters; i++) {//reseteo la info de los clusters
		cluster_sizes[i]=0;
		for (int j = 0; j < dim; j++) {
			centroids[i*dim+j] = 0.0; // Zentroideak berrasieratu -- Reinicia los centroides
		}
	}
	//estos 2 fors hacen la media entre todos los elementos de un cluster para actualizar el valor del centroide
	for (i = 0; i < numwords; i++) {//por cada palabra
		cluster = wordcent[i];
		cluster_sizes[cluster]++;
		for (j = 0; j < dim; j++) {
			centroids[cluster*dim+j] += words[i*dim+j];
		}
	}

	for (i = 0; i < numclusters; i++) {
		if (cluster_sizes[i] > 0) {
			for (j = 0; j < dim; j++) {
				centroids[i * dim + j] = centroids[i * dim + j] / cluster_sizes[i];
			}
		}
	}
}

// K-Means funtzio nagusia -- Función principal de K-Means
void k_means_calculate(float *words, int numwords, int dim, int numclusters, int *wordcent, float *centroids, int *changed) 
{  
	/****************************************************************************************    
	  OSATZEKO - PARA COMPLETAR
	  - Hitz bakoitzari cluster gertukoena esleitu cosine_similarity funtzioan oinarrituta
	  - Asignar cada palabra al cluster más cercano basandose en la función cosine_similarity       
	 ****************************************************************************************/
	int i,j,centroideFinal;
	double minDist;
	double dist;
	for (i = 0; i < numwords; i++) {//por cada palabra...
		minDist =DBL_MAX;
		centroideFinal=wordcent[i];//asignamos la palabra anterior
		for (j = 0; j < numclusters; j++) {// y por cada centroide
			dist=word_distance (&words[i*dim],&centroids[j*dim]);//miro la distancia...
			if ( dist < minDist){//y me quedo con el que este mas cerca
				minDist=dist;	
				centroideFinal=j;	
			}
		}
		if(	wordcent[i]!=centroideFinal)//despues me quedo el mas cerca y lo guardo en wordcent
		{
			* changed = 1;
			wordcent[i]=centroideFinal;
		}
	}
}

double cluster_homogeneity(float *words, struct clusterinfo *members, int i, int numclusters, int number)
{
	/****************************************************************************************
	  OSATZEKO - PARA COMPLETAR
	  Media de las distancias entre los elementos del cluster
	  En cada cluster, las distancias entre todos los pares de elementos
	  Ojo, una vez calculado el par i-j no hay que calcular el j-i
	 ****************************************************************************************/
	int *resultados = calloc(members[i].number*members[i].number,sizeof(int));//matriz para que no se pisen los calculos
	int k,j;
	double adevolver= 0.0;
	for(k=0;k<members[i].number;k++){//por cada palabra en el cluster...
		for(j=0;j<members[i].number;j++){ //por cada palabra en el cluster
			if(!resultados[k*members[i].number + j]&& j!=k)//juraria que esto funciona, lo hago para no repetir
			{	
				resultados[j*members[i].number + k]+=1;//marcamos "el contrario", si hemos hecho distancia (k,j), marcamos para no hacer (j,k)
				adevolver += word_distance(
						&words[members[i].elements[k]*EMB_SIZE],
						&words[members[i].elements[j]*EMB_SIZE]);
			}
		}
	}

	free(resultados);
	return adevolver;
}




double centroid_homogeneity(float *centroids, int i, int numclusters)
{
	/****************************************************************************************
	  OSATZEKO - PARA COMPLETAR
	 ****************************************************************************************/
	// completado, creo que bien
	int j; 
	double distancia = 0.0;
	for(j = 0;j < numclusters ; j++){
		if(i!=j)	
			distancia += word_distance (&centroids[i*EMB_SIZE],&centroids[j*EMB_SIZE]);
	}
	return distancia;
}


double validation (float *words, struct clusterinfo *members, float *centroids, int numclusters)
{
	int     i, j, k, number, word1, word2;
	float   cent_homog[numclusters];
	double  disbat, max, cvi;
	float   clust_homog[numclusters];	// multzo bakoitzeko trinkotasuna -- homogeneidad de cada cluster


	// Kalkulatu clusterren trinkotasuna -- Calcular la homogeneidad de los clusters
	// Cluster bakoitzean, hitz bikote guztien arteko distantzien batezbestekoa. Adi, i - j neurtuta, ez da gero j - i neurtu behar
	// En cada cluster las distancias entre todos los pares de palabras. Ojo, una vez calculado i - j, no hay que calcular el j - i

	for (i=0; i<numclusters; i++)
	{
		disbat = 0.0;
		number = members[i].number; 
		if (number > 1)     // min 2 members in the cluster
		{
			disbat = cluster_homogeneity(words, members, i, numclusters, number);
			clust_homog[i] = disbat/(number*(number-1)/2);	// zati bikote kopurua -- div num de parejas
		}
		else clust_homog[i] = 0;


		// Calcular la homogeneidad de los centroides
		// dist. media del centroide del cluster al resto de centroides

		disbat = centroid_homogeneity(centroids, i, numclusters);
		cent_homog[i] = disbat / (numclusters-1);	// si son 5 clusters, se han sumado 4 dist.
	}

	//cambio no se si estará bien 
	cvi = 0;
	for(i = 0; i < numclusters; i++){
		if(clust_homog[i]>cent_homog[i]){
			cvi += (clust_homog[i]-cent_homog[i])/clust_homog[i];
		}
		else {
			cvi += (clust_homog[i]-cent_homog[i])/cent_homog[i];
		}
	}
	cvi = cvi/numclusters;

	// cvi index
	/****************************************************************************************
	  OSATZEKO - PARA COMPLETAR
fmaxf: max of 2 floats --> maximoa kalkulatzeko -- para calcular el máximo
	 ****************************************************************************************/
	return (cvi);
}


int main(int argc, char *argv[]) 
{
	int		i, j, numwords, k, iter, changed, end_classif;
	int		cluster, zenb, numclusters = 20;
	double  	cvi, cvi_old, dif;
	float 	*words;
	FILE    	*f1, *f2, *f3;
	char 	**hiztegia;  
	int     	*wordcent;

	struct clusterinfo  members[NUMCLUSTERSMAX];

	struct timespec  t0, t1;
	double tej;


	if (argc < 4) {
		printf("\nCall: kmeans embeddings.dat dictionary.dat myclusters.dat [numwords]\n\n");
		printf("\t(in) embeddings.dat and dictionary.dat\n");
		printf("\t(out) myclusters.dat\n");
		printf("\t(numwords optional) prozesatu nahi den hitz kopurua -- num de palabras a procesar\n\n");
		exit (-1);;
	}  

	// Irakurri datuak sarrea-fitxategietatik -- Leer los datos de los ficheros de entrada
	// =================================================================================== 

	f1 = fopen (argv[1], "r");
	if (f1 == NULL) {
		printf ("Errorea %s fitxategia irekitzean -- Error abriendo fichero\n", argv[1]);
		exit (-1);
	}

	f2 = fopen (argv[2], "r");
	if (f2 == NULL) {
		printf ("Errorea %s fitxategia irekitzean -- Error abriendo fichero\n", argv[2]);
		exit (-1);
	}

	fscanf (f1, "%d", &numwords);	       
	if (argc == 5) numwords = atoi (argv[4]);  
	printf ("numwords = %d\n", numwords);

	words = (float*)malloc (numwords*EMB_SIZE*sizeof(float));
	hiztegia = (char**)malloc (numwords*sizeof(char*));
	for (i=0; i<numwords;i++){
		hiztegia[i] = (char*)malloc(TAM*sizeof(char));
	}

	for (i=0; i<numwords; i++) {
		fscanf (f2, "%s", hiztegia[i]);
		for (j=0; j<EMB_SIZE; j++) {
			fscanf (f1, "%f", &(words[i*EMB_SIZE+j]));
		}
	}
	printf ("Embeddingak eta hiztegia irakurrita -- Embeddings y dicionario leidos\n");

	wordcent = (int *)malloc(numwords * sizeof(int));
	for (int i = 0; i < numwords; i++) wordcent[i] = -1;

	k = NUMCLUSTERSMAX;   // hasierako kluster kopurua (20) -- numero de clusters inicial
	end_classif = 0; 
	cvi_old= -1;

	float *centroids = (float *)malloc(k * EMB_SIZE * sizeof(float));
	int *cluster_sizes = (int *)calloc(k, sizeof(int));



	/******************************************************************/
	// A. kmeans kalkulatu -- Calcular kmeans
	// =========================================================
	printf("K_means\n");
	clock_gettime (CLOCK_REALTIME, &t0);

	//while (numclusters < NUMCLUSTERSMAX && end_classif == 0)
	while (numclusters < 31  && end_classif == 0)
	{
		for (i = 0; i < numwords; i++) {
			wordcent[i] =-1;
		}
		initialize_centroids(words, centroids, numwords, numclusters, EMB_SIZE);
		cvi_old=-1;
		for (iter = 0; iter < MAX_ITER; iter++) {
			changed = 0;
			/****************************************************************************************
			  OSATZEKO - PARA COMPLETAR
			  deitu k_means_calculate funtzioari -- llamar a la función k_means_calculate
			 ****************************************************************************************/
			k_means_calculate(words, numwords, EMB_SIZE, numclusters,wordcent,centroids,&changed) ;
			if (changed==0) break; // Aldaketarik ez bada egon, atera -- Si no hay cambios, salir
			//printf("borrame, pero ha habido cambios\n");
			update_centroids(words, centroids, wordcent, numwords, numclusters, EMB_SIZE, cluster_sizes);
		}  


		// B. Sailkatzearen "kalitatea" -- "Calidad" del cluster
		// =====================================================
		//printf("Kalitatea -- Calidad\n");   
		for (i=0; i<numclusters; i++)  members[i].number = 0;

		// cluster bakoitzeko hitzak (osagaiak) eta kopurua -- palabras de cada clusters y cuantas son
		for (i=0; i<numwords; i++)  {
			cluster = wordcent[i];
			zenb = members[cluster].number;
			members[cluster].elements[zenb] = i;	// clusterreko hitza -- palabra del cluster
			members[cluster].number ++; 
		}

		/****************************************************************************************
		  OSATZEKO - PARA COMPLETAR
		  cvi = validation (OSATZEKO - PARA COMPLETAR);
		  if (cvi appropriate) end classification;
		  else  continue classification;	
		 ****************************************************************************************/
		cvi = validation (words, members, centroids ,numclusters );
		printf("El indice de calidad en la iteracion con %i clusters es: %1.6f, \n",numclusters,cvi);   
		if(cvi-cvi_old<DELTA) end_classif =1;
		cvi_old=cvi;
		//numclusters+=10;
		numclusters++;
	} 

	clock_gettime (CLOCK_REALTIME, &t1);
	/******************************************************************/
	int auxi=0;
	for (i=0; i<numclusters; i++){
		printf ("%d. cluster, %d words \n", i, cluster_sizes[i]);
		auxi += cluster_sizes[i];
	}
	printf ("Num palabras total:%i\n", auxi);

	tej = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / (double)1e9;
	printf("\n Tej. (serie) = %1.3f ms\n\n", tej*1000);

	// Idatzi clusterrak fitxategietan -- Escribir los clusters en el fichero
	f3 = fopen (argv[3], "w");
	if (f3 == NULL) {
		printf ("Errorea %s fitxategia irekitzean -- Error abriendo fichero\n", argv[3]);
		exit (-1);
	}

	for (i=0; i<numwords; i++)
		fprintf (f3, "%s \t\t -> %d cluster\n", hiztegia[i], wordcent[i]);
	printf ("clusters written\n");

	fclose (f1);
	fclose (f2);
	fclose (f3);

	free(words);
	for (i=0; i<numwords;i++) free (hiztegia[i]);
	free(wordcent);
	free(hiztegia); 
	free(cluster_sizes);
	free(centroids);
	return 0;
}

