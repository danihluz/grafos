#include <stdio.h>
#include "grafo_matriz.h"
#include "grafo_lista.h"

int main(void) {

    printf("=== GRAFO COM MATRIZ DE ADJACENCIA ===\n");

    GrafoMatriz *matriz = criar_grafo_matriz(5);

    if (matriz == NULL) {
        printf("Erro ao criar grafo com matriz.\n");
        return 1;
    }

    inserir_aresta_matriz(matriz, 0, 1);
    inserir_aresta_matriz(matriz, 0, 2);
    inserir_aresta_matriz(matriz, 1, 3);
    inserir_aresta_matriz(matriz, 2, 4);

    printf("Grau do vertice 0: %d\n", grau_matriz(matriz, 0));

    printf("0 e 1 sao adjacentes? %s\n",
           sao_adjacentes_matriz(matriz, 0, 1) ? "Sim" : "Nao");

    printf("0 e 4 sao adjacentes? %s\n",
           sao_adjacentes_matriz(matriz, 0, 4) ? "Sim" : "Nao");

    remover_aresta_matriz(matriz, 0, 1);

    printf("Depois de remover a aresta 0-1:\n");

    printf("0 e 1 sao adjacentes? %s\n",
           sao_adjacentes_matriz(matriz, 0, 1) ? "Sim" : "Nao");

    liberar_grafo_matriz(matriz);


    printf("\n=== GRAFO COM LISTA DE ADJACENCIA ===\n");

    GrafoLista *lista = criar_grafo_lista(5);

    if (lista == NULL) {
        printf("Erro ao criar grafo com lista.\n");
        return 1;
    }

    inserir_aresta_lista(lista, 0, 1);
    inserir_aresta_lista(lista, 0, 2);
    inserir_aresta_lista(lista, 1, 3);
    inserir_aresta_lista(lista, 2, 4);

    printf("Grau do vertice 0: %d\n", grau_lista(lista, 0));

    printf("0 e 1 sao adjacentes? %s\n",
           sao_adjacentes_lista(lista, 0, 1) ? "Sim" : "Nao");

    printf("0 e 4 sao adjacentes? %s\n",
           sao_adjacentes_lista(lista, 0, 4) ? "Sim" : "Nao");

    remover_aresta_lista(lista, 0, 1);

    printf("Depois de remover a aresta 0-1:\n");

    printf("0 e 1 sao adjacentes? %s\n",
           sao_adjacentes_lista(lista, 0, 1) ? "Sim" : "Nao");

    liberar_grafo_lista(lista);

    return 0;
}