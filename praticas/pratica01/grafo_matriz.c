#include <stdio.h>
#include <stdlib.h>
#include "grafo_matriz.h"

static int vertice_valido(GrafoMatriz *grafo, int vertice) {
    return grafo != NULL && vertice >= 0 && vertice < grafo->n;
}

GrafoMatriz *criar_grafo_matriz(int n) {
    if (n <= 0) {
        return NULL;
    }

    GrafoMatriz *grafo = malloc(sizeof(GrafoMatriz));

    if (grafo == NULL) {
        return NULL;
    }

    grafo->n = n;

    grafo->adj = malloc(n * sizeof(int *));

    if (grafo->adj == NULL) {
        free(grafo);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        grafo->adj[i] = calloc(n, sizeof(int));

        if (grafo->adj[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(grafo->adj[j]);
            }

            free(grafo->adj);
            free(grafo);

            return NULL;
        }
    }

    return grafo;
}

void inserir_aresta_matriz(GrafoMatriz *grafo, int origem, int destino) {
    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino)) {
        return;
    }

    grafo->adj[origem][destino] = 1;
    grafo->adj[destino][origem] = 1;
}

void remover_aresta_matriz(GrafoMatriz *grafo, int origem, int destino) {
    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino)) {
        return;
    }

    grafo->adj[origem][destino] = 0;
    grafo->adj[destino][origem] = 0;
}

int grau_matriz(GrafoMatriz *grafo, int vertice) {
    if (!vertice_valido(grafo, vertice)) {
        return -1;
    }

    int grau = 0;

    for (int i = 0; i < grafo->n; i++) {
        if (grafo->adj[vertice][i] == 1) {
            grau++;
        }
    }

    return grau;
}

int sao_adjacentes_matriz(GrafoMatriz *grafo, int origem, int destino) {
    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino)) {
        return 0;
    }

    return grafo->adj[origem][destino];
}

void liberar_grafo_matriz(GrafoMatriz *grafo) {
    if (grafo == NULL) {
        return;
    }

    for (int i = 0; i < grafo->n; i++) {
        free(grafo->adj[i]);
    }

    free(grafo->adj);
    free(grafo);
}