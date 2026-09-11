#ifndef DAG_H
#define DAG_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

/* Funcoes basicas do grafo */
GrafoLista *criar_grafo_lista(int n);
void inserir_aresta(GrafoLista *g, int origem, int destino);
void liberar_grafo(GrafoLista *g);

/* Funcoes da pratica */
int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho);
int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho);
int eh_dag(GrafoLista *g);

#endif