#ifndef COBERTURA_H
#define COBERTURA_H

typedef struct {
    int u, v;
} Aresta;

/*
 * Grafo nao direcionado, com vertices de 0 a n-1.
 * cobertura[v] vale 1 se o vertice foi escolhido, ou 0 caso contrario.
 * O chamador fornece um vetor de n inteiros; a entrada nao e modificada.
 * Os algoritmos retornam o tamanho da cobertura, ou -1 em caso de erro.
 * O grafo vazio (n = m = 0) admite cobertura NULL e tem solucao de tamanho 0.
 */
int cobertura_gulosa(int n, const Aresta *arestas, int m, int *cobertura);
int cobertura_minima_bt(int n, const Aresta *arestas, int m, int *cobertura);

/* Retorna 1 se todas as arestas estao cobertas, ou 0 se nao/entrada invalida. */
int eh_cobertura(int n, const Aresta *arestas, int m, const int *cobertura);

#endif
