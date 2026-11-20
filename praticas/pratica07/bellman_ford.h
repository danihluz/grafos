#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <limits.h>

/* INT_MAX fica reservado para vertices inalcancaveis. */
#define INFINITO INT_MAX

typedef struct NoAresta {
    int destino;
    int peso;
    struct NoAresta *prox;
} NoAresta;

typedef struct {
    int n;
    NoAresta **adj;
    /* Ultimo vertice relaxado na n-esima passagem, ou -1 se nao ha ciclo. */
    int vertice_ciclo;
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo_ponderado(int n);

/* Insere uma aresta direcionada origem -> destino; aceita pesos negativos. */
int inserir_aresta_ponderada(
    GrafoListaPonderado *g, int origem, int destino, int peso
);

void liberar_grafo_ponderado(GrafoListaPonderado *g);

/*
 * Retorna 1 em caso de sucesso, 0 se ha ciclo negativo alcancavel da origem,
 * ou -1 em caso de entrada invalida, falta de memoria ou custo fora de int.
 * Em caso de ciclo negativo, dist[] nao representa distancias minimas;
 * use pred[] e g->vertice_ciclo para reconstruir o ciclo.
 */
int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/* Retrocede n vezes por pred[] e imprime o ciclo na direcao das arestas.
 * Retorna 1 se imprimiu um ciclo, ou 0 se os dados sao invalidos.
 */
int reconstruir_ciclo_negativo(int n, int vertice, const int *pred);

#endif
