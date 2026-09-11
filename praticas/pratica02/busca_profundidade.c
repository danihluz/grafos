#include <stdlib.h>
#include "busca_profundidade.h"

/* =========================
   DFS RECURSIVA
   ========================= */

void dfs_recursiva(
    GrafoLista *g,
    int u,
    int *visitado,
    int *tempo_entrada,
    int *tempo_saida,
    int *tempo
) {
    if (g == NULL ||
        visitado == NULL ||
        tempo_entrada == NULL ||
        tempo_saida == NULL ||
        tempo == NULL ||
        u < 0 ||
        u >= g->n) {
        return;
    }

    visitado[u] = 1;

    (*tempo)++;
    tempo_entrada[u] = *tempo;

    No *atual = g->adj[u];

    while (atual != NULL) {
        int v = atual->destino;

        if (!visitado[v]) {
            dfs_recursiva(
                g,
                v,
                visitado,
                tempo_entrada,
                tempo_saida,
                tempo
            );
        }

        atual = atual->prox;
    }

    (*tempo)++;
    tempo_saida[u] = *tempo;
}


/* =========================
   COMPONENTES CONEXOS
   ========================= */

static void marcar_componente(
    GrafoLista *g,
    int u,
    int *visitado
) {
    visitado[u] = 1;

    No *atual = g->adj[u];

    while (atual != NULL) {
        int v = atual->destino;

        if (!visitado[v]) {
            marcar_componente(g, v, visitado);
        }

        atual = atual->prox;
    }
}

int contar_componentes(GrafoLista *g) {
    if (g == NULL) {
        return 0;
    }

    int *visitado =
        calloc((size_t)g->n, sizeof(int));

    if (visitado == NULL) {
        return 0;
    }

    int componentes = 0;

    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            componentes++;
            marcar_componente(g, i, visitado);
        }
    }

    free(visitado);

    return componentes;
}


/* =========================
   DETECCAO DE CICLO
   ========================= */

static int dfs_ciclo(
    GrafoLista *g,
    int u,
    int pai,
    int *visitado
) {
    visitado[u] = 1;

    No *atual = g->adj[u];

    while (atual != NULL) {
        int v = atual->destino;

        if (!visitado[v]) {
            if (dfs_ciclo(g, v, u, visitado)) {
                return 1;
            }
        } else if (v != pai) {
            /*
               Vizinho ja visitado que nao e o pai:
               encontramos um ciclo.
            */
            return 1;
        }

        atual = atual->prox;
    }

    return 0;
}

int tem_ciclo(GrafoLista *g) {
    if (g == NULL) {
        return 0;
    }

    int *visitado =
        calloc((size_t)g->n, sizeof(int));

    if (visitado == NULL) {
        return 0;
    }

    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            if (dfs_ciclo(g, i, -1, visitado)) {
                free(visitado);
                return 1;
            }
        }
    }

    free(visitado);

    return 0;
}