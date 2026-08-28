#ifndef GRAFO_MATRIZ_H
#define GRAFO_MATRIZ_H

typedef struct {
    int n;
    int **adj;
} GrafoMatriz;

GrafoMatriz *criar_grafo_matriz(int n);

void inserir_aresta_matriz(GrafoMatriz *grafo, int origem, int destino);

void remover_aresta_matriz(GrafoMatriz *grafo, int origem, int destino);

int grau_matriz(GrafoMatriz *grafo, int vertice);

int sao_adjacentes_matriz(GrafoMatriz *grafo, int origem, int destino);

void liberar_grafo_matriz(GrafoMatriz *grafo);

#endif