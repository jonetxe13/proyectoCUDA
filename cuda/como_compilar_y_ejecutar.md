# Como compilar y ejecutar
## Secuencial
### analogia:
nvcc analogy_cuda.cu biblioteca_funciones_cuda.cu -o analogy_paralelo.out
./analogy_paralelo.out ~/PAR/proyecto/embeddings.dat ~/PAR/proyecto/dictionary.dat 
los 2 ultimos parametros son el numero de bloques y el numero de hilos por bloques, aun no esta implementado
### knn:
gcc -O2 biblioteca_funciones_secuencial.c knn_secuencial.c -o knn_secuencial.out -lm
./kmeans_secuencial.out ~/PAR/proyecto/embeddings.dat ~/PAR/proyecto/dictionary.dat ./micluster.dat
### kmeans
nvcc -rdc=true kmeans_cuda.cu biblioteca_funciones_cuda.cu -o kmeans_cuda.out

kmeans_cuda.out ../secuencial/embeddings.dat ../secuencial/dictionary.dat micluster.dat 1000
