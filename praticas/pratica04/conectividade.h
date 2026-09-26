#ifndef CONECTIVIDADE_H
#define CONECTIVIDADE_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

/* Estrutura basica do grafo */
GrafoLista *criar_grafo_lista(int n);
void inserir_aresta(GrafoLista *g, int u, int v);
int sao_adjacentes(GrafoLista *g, int u, int v);
void liberar_grafo(GrafoLista *g);

/* Tarjan */
void dfs_articulacoes(
    GrafoLista *g,
    int u,
    int *visitado,
    int *descoberta,
    int *low,
    int *pai,
    int *articulacao,
    int *tempo
);

void detectar_articulacoes(GrafoLista *g);
void detectar_pontes(GrafoLista *g);

#endif