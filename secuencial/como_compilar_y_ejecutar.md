# Como compilar y ejecutar
## Secuencial
### analogia:
gcc -O2 biblioteca_funciones_secuencial.c analogy_secuencial.c -o analogy_secuencial.out -lm
./analogy_secuencial.out ~/PAR/proyecto/embeddings.dat ~/PAR/proyecto/dictionary.dat 
### knn:
gcc -O2 biblioteca_funciones_secuencial.c knn_secuencial.c -o knn_secuencial.out -lm
./knn_secuencial.out ~/PAR/proyecto/embeddings.dat ./similarities.dat
knn_test ./similarities.dat ~/PAR/proyecto/dictionary.dat 
### kmeans
gcc kmeans_secuencial.c -o kmeans -lm -O2 biblioteca_funciones_secuencial.c
./kmeans embeddings.dat dictionary.dat micluster.dat 1000 

