#ifndef AGM_H
#define AGM_H

typedef struct {
    int *pai, *rank;
    int n;
} DSU;

typedef struct {
    int u, v, peso;
} Aresta;

typedef struct {
    Aresta *arestas;
    int quantidade;
    int componentes;
    long long custo_total;
} ResultadoAGM;

DSU *criar_dsu(int n);
int find(DSU *d, int x);
/* Retorna 1 se uniu duas componentes, ou 0 se ja unidas/entrada invalida. */
int unir(DSU *d, int x, int y);
void liberar_dsu(DSU *d);

/*
 * As arestas sao nao direcionadas e podem ter qualquer peso do tipo int.
 * Retorno: 1 para AGM, 0 para floresta (grafo desconectado), -1 para erro.
 * Inicialize resultado com {0} e libere-o antes de reutiliza-lo.
 * O vetor de entrada nao e modificado. O custo total usa long long.
 */
int kruskal(int n, const Aresta *arestas, int m, ResultadoAGM *resultado);
int prim(int n, const Aresta *arestas, int m, int origem, ResultadoAGM *resultado);
void liberar_agm(ResultadoAGM *resultado);

#endif
