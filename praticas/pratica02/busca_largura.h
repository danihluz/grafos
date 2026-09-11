#ifndef BUSCA_LARGURA_H
#define BUSCA_LARGURA_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

/* Fila FIFO para BFS */
typedef struct {
    int *dados;
    int capacidade;
    int inicio;
    int fim;
    int tamanho;
} Fila;

/* Funcoes basicas do grafo */
GrafoLista *criar_grafo_lista(int n);
void inserir_aresta(GrafoLista *g, int origem, int destino);
void liberar_grafo(GrafoLista *g);

/* BFS */
void bfs(GrafoLista *g, int origem, int *dist, int *pred);

/* Biparticao */
int eh_bipartido(GrafoLista *g);

#endif