#ifndef BUSCA_PROFUNDIDADE_H
#define BUSCA_PROFUNDIDADE_H

#include "busca_largura.h"

/* Pilha LIFO */
typedef struct {
    int *dados;
    int topo;
    int capacidade;
} Pilha;

/* DFS recursiva com tempos de entrada e saida */
void dfs_recursiva(
    GrafoLista *g,
    int u,
    int *visitado,
    int *tempo_entrada,
    int *tempo_saida,
    int *tempo
);

/* Aplicacoes de DFS */
int contar_componentes(GrafoLista *g);
int tem_ciclo(GrafoLista *g);

#endif