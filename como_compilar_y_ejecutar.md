# Como compilar y ejecutar
## Secuencial
### analogia:
gcc -O2 biblioteca_funciones_secuencial.c analogy_todo.c -o analogy_secuencial.out -lm
./analogy_secuencial.out ~/PAR/proyecto/embeddings.dat ~/PAR/proyecto/dictionary.dat 
### knn:
gcc -O2 biblioteca_funciones_secuencial.c knn_secuencial.c -o knn_secuencial.out -lm
./kmeans_secuencial.out ~/PAR/proyecto/embeddings.dat ~/PAR/proyecto/dictionary.dat ./micluster.dat
### kmeans
gcc kmeans_todo.c -o kmeans -lm -O2
