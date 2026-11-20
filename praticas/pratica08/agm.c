#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "agm.h"

DSU *criar_dsu(int n) {
    if (n <= 0) {
        return NULL;
    }

    DSU *d = malloc(sizeof(*d));
    if (d == NULL) {
        return NULL;
    }
    d->pai = malloc((size_t)n * sizeof(*d->pai));
    d->rank = calloc((size_t)n, sizeof(*d->rank));
    if (d->pai == NULL || d->rank == NULL) {
        liberar_dsu(d);
        return NULL;
    }

    d->n = n;
    for (int x = 0; x < n; x++) {
        d->pai[x] = x;
    }
    return d;
}

int find(DSU *d, int x) {
    if (d == NULL || x < 0 || x >= d->n) {
        return -1;
    }

    int raiz = x;
    while (d->pai[raiz] != raiz) {
        raiz = d->pai[raiz];
    }
    /* Compressao de caminho: todos os vertices visitados apontam a raiz. */
    while (d->pai[x] != x) {
        int proximo = d->pai[x];
        d->pai[x] = raiz;
        x = proximo;
    }
    return raiz;
}

int unir(DSU *d, int x, int y) {
    int raiz_x = find(d, x);
    int raiz_y = find(d, y);
    if (raiz_x < 0 || raiz_y < 0 || raiz_x == raiz_y) {
        return 0;
    }

    /* Uniao por rank: a arvore de menor rank fica sob a de maior rank. */
    if (d->rank[raiz_x] < d->rank[raiz_y]) {
        int temporario = raiz_x;
        raiz_x = raiz_y;
        raiz_y = temporario;
    }
    d->pai[raiz_y] = raiz_x;
    if (d->rank[raiz_x] == d->rank[raiz_y]) {
        d->rank[raiz_x]++;
    }
    return 1;
}

void liberar_dsu(DSU *d) {
    if (d != NULL) {
        free(d->pai);
        free(d->rank);
        free(d);
    }
}

static int validar_entrada(int n, const Aresta *arestas, int m) {
    if (n <= 0 || m < 0 || (m > 0 && arestas == NULL)) {
        return 0;
    }
    for (int i = 0; i < m; i++) {
        if (arestas[i].u < 0 || arestas[i].u >= n ||
            arestas[i].v < 0 || arestas[i].v >= n) {
            return 0;
        }
    }
    return 1;
}

static int preparar_resultado(int n, ResultadoAGM *resultado) {
    resultado->arestas = malloc((size_t)(n > 1 ? n - 1 : 1) *
                               sizeof(*resultado->arestas));
    return resultado->arestas != NULL;
}

void liberar_agm(ResultadoAGM *resultado) {
    if (resultado != NULL) {
        free(resultado->arestas);
        *resultado = (ResultadoAGM){0};
    }
}

static int comparar_arestas(const void *primeira, const void *segunda) {
    const Aresta *a = primeira;
    const Aresta *b = segunda;
    /* Comparacoes, em vez de subtracao, evitam overflow com INT_MIN/MAX. */
    if (a->peso != b->peso) {
        return (a->peso > b->peso) - (a->peso < b->peso);
    }
    if (a->u != b->u) {
        return (a->u > b->u) - (a->u < b->u);
    }
    return (a->v > b->v) - (a->v < b->v);
}

int kruskal(int n, const Aresta *arestas, int m, ResultadoAGM *resultado) {
    if (resultado == NULL) {
        return -1;
    }
    *resultado = (ResultadoAGM){0};
    if (!validar_entrada(n, arestas, m) || !preparar_resultado(n, resultado)) {
        return -1;
    }

    DSU *d = criar_dsu(n);
    Aresta *ordenadas = m > 0 ? malloc((size_t)m * sizeof(*ordenadas)) : NULL;
    if (d == NULL || (m > 0 && ordenadas == NULL)) {
        liberar_dsu(d);
        free(ordenadas);
        liberar_agm(resultado);
        return -1;
    }
    if (m > 0) {
        memcpy(ordenadas, arestas, (size_t)m * sizeof(*ordenadas));
        qsort(ordenadas, (size_t)m, sizeof(*ordenadas), comparar_arestas);
    }

    resultado->componentes = n;
    for (int i = 0; i < m && resultado->quantidade < n - 1; i++) {
        Aresta a = ordenadas[i];
        if (unir(d, a.u, a.v)) {
            resultado->arestas[resultado->quantidade++] = a;
            resultado->custo_total += a.peso;
            resultado->componentes--;
        }
    }

    liberar_dsu(d);
    free(ordenadas);
    return resultado->componentes == 1;
}

typedef struct NoAdj {
    int destino, peso;
    struct NoAdj *prox;
} NoAdj;

static void liberar_adjacencias(NoAdj **adj, int n) {
    if (adj == NULL) {
        return;
    }
    for (int u = 0; u < n; u++) {
        NoAdj *a = adj[u];
        while (a != NULL) {
            NoAdj *proxima = a->prox;
            free(a);
            a = proxima;
        }
    }
    free(adj);
}

static NoAdj **criar_adjacencias(int n, const Aresta *arestas, int m) {
    NoAdj **adj = calloc((size_t)n, sizeof(*adj));
    if (adj == NULL) {
        return NULL;
    }
    for (int i = 0; i < m; i++) {
        NoAdj *ida = malloc(sizeof(*ida));
        NoAdj *volta = malloc(sizeof(*volta));
        if (ida == NULL || volta == NULL) {
            free(ida);
            free(volta);
            liberar_adjacencias(adj, n);
            return NULL;
        }
        int u = arestas[i].u;
        int v = arestas[i].v;
        *ida = (NoAdj){v, arestas[i].peso, adj[u]};
        adj[u] = ida;
        *volta = (NoAdj){u, arestas[i].peso, adj[v]};
        adj[v] = volta;
    }
    return adj;
}

typedef struct {
    int vertice;
    long long chave;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int *posicao;
    int tamanho;
} MinHeap;

static int menor_no(NoHeap a, NoHeap b) {
    return a.chave < b.chave || (a.chave == b.chave && a.vertice < b.vertice);
}

static void trocar(MinHeap *h, int i, int j) {
    NoHeap temporario = h->dados[i];
    h->dados[i] = h->dados[j];
    h->dados[j] = temporario;
    h->posicao[h->dados[i].vertice] = i;
    h->posicao[h->dados[j].vertice] = j;
}

static void diminuir_chave(MinHeap *h, int vertice, long long chave) {
    int i = h->posicao[vertice];
    h->dados[i].chave = chave;
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (!menor_no(h->dados[i], h->dados[pai])) {
            break;
        }
        trocar(h, i, pai);
        i = pai;
    }
}

static NoHeap extrair_minimo(MinHeap *h) {
    NoHeap minimo = h->dados[0];
    trocar(h, 0, h->tamanho - 1);
    h->tamanho--;
    h->posicao[minimo.vertice] = -1;

    int i = 0;
    while (i < h->tamanho / 2) {
        int menor = 2 * i + 1;
        int direita = menor + 1;
        if (direita < h->tamanho && menor_no(h->dados[direita], h->dados[menor])) {
            menor = direita;
        }
        if (!menor_no(h->dados[menor], h->dados[i])) {
            break;
        }
        trocar(h, i, menor);
        i = menor;
    }
    return minimo;
}

int prim(int n, const Aresta *arestas, int m, int origem, ResultadoAGM *resultado) {
    if (resultado == NULL) {
        return -1;
    }
    *resultado = (ResultadoAGM){0};
    if (!validar_entrada(n, arestas, m) || origem < 0 || origem >= n ||
        !preparar_resultado(n, resultado)) {
        return -1;
    }

    NoAdj **adj = criar_adjacencias(n, arestas, m);
    int *pai = malloc((size_t)n * sizeof(*pai));
    MinHeap h = {
        malloc((size_t)n * sizeof(*h.dados)),
        malloc((size_t)n * sizeof(*h.posicao)), n
    };
    if (adj == NULL || pai == NULL || h.dados == NULL || h.posicao == NULL) {
        liberar_adjacencias(adj, n);
        free(pai);
        free(h.dados);
        free(h.posicao);
        liberar_agm(resultado);
        return -1;
    }

    for (int v = 0; v < n; v++) {
        /* Sentinela em long long permite arestas de peso INT_MAX. */
        h.dados[v] = (NoHeap){v, LLONG_MAX};
        h.posicao[v] = v;
        pai[v] = -1;
    }
    diminuir_chave(&h, origem, 0);

    while (h.tamanho > 0) {
        NoHeap minimo = extrair_minimo(&h);
        int u = minimo.vertice;
        if (pai[u] == -1) {
            /* Uma nova raiz inicia outra componente, se o grafo e desconexo. */
            resultado->componentes++;
        } else {
            resultado->arestas[resultado->quantidade++] =
                (Aresta){pai[u], u, (int)minimo.chave};
            resultado->custo_total += minimo.chave;
        }

        for (const NoAdj *a = adj[u]; a != NULL; a = a->prox) {
            int posicao = h.posicao[a->destino];
            if (posicao != -1 && a->peso < h.dados[posicao].chave) {
                pai[a->destino] = u;
                diminuir_chave(&h, a->destino, a->peso);
            }
        }
    }

    liberar_adjacencias(adj, n);
    free(pai);
    free(h.dados);
    free(h.posicao);
    return resultado->componentes == 1;
}
