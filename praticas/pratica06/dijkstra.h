#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <limits.h>

/* INT_MAX representa vertices inalcancaveis; custos finitos devem ser menores. */
#define INFINITO INT_MAX

typedef struct NoAresta {
    int destino;
    int peso;
    struct NoAresta *prox;
} NoAresta;

typedef struct {
    int n;
    NoAresta **adj;
} GrafoListaPonderado;

/* Estruturas obrigatorias do heap */
typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade;
    int tamanho;
    int *posicao;
} MinHeap;

/* Grafo nao direcionado, com pesos nao negativos. */
GrafoListaPonderado *criar_grafo_ponderado(int n);
void inserir_aresta_ponderada(
    GrafoListaPonderado *g,
    int origem,
    int destino,
    int peso
);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

/* Min-Heap */
MinHeap *criar_min_heap(int capacidade);
void inserir_heap(
    MinHeap *heap,
    int vertice,
    int distancia
);
NoHeap extrair_minimo(MinHeap *heap);
void diminuir_chave(
    MinHeap *heap,
    int vertice,
    int nova_distancia
);
void liberar_min_heap(MinHeap *heap);

/* Dijkstra */
void dijkstra(
    GrafoListaPonderado *g,
    int origem,
    int *dist,
    int *pred
);

/* Os vertices e o vetor pred devem pertencer a uma execucao de dijkstra. */
void reconstruir_caminho(
    int origem,
    int destino,
    int *pred
);

#endif
