#define VOCAB_SIZE  10000       // Hitz kopuru maximoa -- Maximo num. de palabras
#define EMB_SIZE    50      // Embedding-en kopurua hitzeko -- Nº de embedding-s por palabra
#define TAM         25      // Hiztegiko hitzen tamaina maximoa -- Tamaño maximo del diccionario
#define MAX_ITER    1000        // konbergentzia: iterazio kopuru maximoa -- Convergencia: num maximo de iteraciones
#define K       20      // kluster kopurua -- numero de clusters
#define DELTA       0.5     // konbergentzia (cvi) -- convergencia (cvi)
#define NUMCLUSTERSMAX 100  // cluster kopuru maximoa -- numero máximo de clusters

//-------------------
// COMPARTIDAS:
//-------------------

// Función para calcular el producto escalar entre dos vectores
double dot_product(float* a, float* b, int size);

// Función para calcular la norma (magnitud) de un vector
float magnitude(float* vec, int size);

// Función para calcular la similitud coseno entre dos vectores
//
float cosine_similarity(float* vec1, float* vec2, int size);

//-------------------
// Analogy:
//-------------------

// Función para calcular el indice de una palabra 
int word2ind(char* word, char** dictionary, int numwords);

// Función para calcular la analogía
void perform_analogy(float *words, int idx1, int idx2, int idx3, float *result_vector);

// Función para encontrar la palabra más cercana al vector resultante
void find_closest_word(float *result_vector, float *words, int numwords, int idx1, int idx2, int idx3, int *closest_word_idx, float *max_similarity, int numBloques, int numHilos);


__global__ void reduc_analogy(int idx1,int idx2,int idx3,int numWords,int * indicesBloques, int * maximosBloques,float * words,float * result_vector);

//-------------------
// KNN:
//-------------------

// kNN para todas las palabras
void knn_complet(float *words, int numwords, float *similarities);
//-------------------
// Debuging:
//-------------------
void imprimir_vector_float(float *vector, int tamano);
