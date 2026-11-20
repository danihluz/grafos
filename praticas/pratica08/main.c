#include <stdio.h>
#include <stdlib.h>
#include "agm.h"

static void imprimir_resultado(const char *nome, const ResultadoAGM *resultado) {
    printf("%s: %s\n", nome,
           resultado->componentes == 1 ? "arvore geradora minima" :
                                        "floresta geradora minima");
    for (int i = 0; i < resultado->quantidade; i++) {
        Aresta a = resultado->arestas[i];
        printf("  %d -- %d (peso %d)\n", a.u, a.v, a.peso);
    }
    printf("Arestas: %d | Componentes: %d | Custo total: %lld\n",
           resultado->quantidade, resultado->componentes, resultado->custo_total);
}

static int executar_exemplo(const char *titulo, int n, const Aresta *arestas, int m) {
    ResultadoAGM resultado_kruskal = {0};
    ResultadoAGM resultado_prim = {0};
    int status_kruskal = kruskal(n, arestas, m, &resultado_kruskal);
    int status_prim = prim(n, arestas, m, 0, &resultado_prim);
    int sucesso = 0;

    if (status_kruskal < 0 || status_prim < 0) {
        fprintf(stderr, "Erro ao calcular AGM: entrada invalida ou falta de memoria.\n");
        goto finalizar;
    }

    printf("=== %s ===\n", titulo);
    imprimir_resultado("Kruskal", &resultado_kruskal);
    printf("\n");
    imprimir_resultado("Prim", &resultado_prim);

    if (resultado_kruskal.custo_total != resultado_prim.custo_total ||
        resultado_kruskal.componentes != resultado_prim.componentes ||
        resultado_kruskal.quantidade != resultado_prim.quantidade) {
        fprintf(stderr, "Erro: os algoritmos produziram resultados inconsistentes.\n");
        goto finalizar;
    }
    printf("Os dois algoritmos encontraram o mesmo custo minimo.\n");
    sucesso = 1;

finalizar:
    liberar_agm(&resultado_kruskal);
    liberar_agm(&resultado_prim);
    return sucesso;
}

int main(void) {
    /* AGM com quatro arestas e custo 16. */
    const Aresta conectado[] = {
        {0, 1, 2}, {0, 3, 6}, {1, 2, 3}, {1, 3, 8},
        {1, 4, 5}, {2, 4, 7}, {3, 4, 9}
    };
    /* Duas componentes: a floresta minima tem tres arestas e custo 2. */
    const Aresta desconectado[] = {{0, 1, -2}, {1, 2, 0}, {3, 4, 4}};

    if (!executar_exemplo("GRAFO CONECTADO", 5, conectado,
                          (int)(sizeof(conectado) / sizeof(conectado[0])))) {
        return EXIT_FAILURE;
    }
    printf("\n");
    if (!executar_exemplo("GRAFO DESCONECTADO", 5, desconectado,
                          (int)(sizeof(desconectado) / sizeof(desconectado[0])))) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
