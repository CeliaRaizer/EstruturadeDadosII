#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> 
#include <stdbool.h>
#define MAX_ARESTAS (MAX_VERTICES * (MAX_VERTICES - 1) / 2)
#define MAX_VERTICES 1000

typedef struct Aresta {
    int destino;
    int peso;
    struct Aresta* prox;
} Aresta;

typedef struct Vertice {
    char nome[50];
    Aresta* lista_arestas;
} Vertice;

typedef struct Grafo {
    int num_vertices;
    Vertice vertices[MAX_VERTICES];
} Grafo;

typedef struct {
    int origem;
    int destino;
    int peso;
} ArestaK;

void inicializar_grafo(Grafo* g) {
    g->num_vertices = 0;
}

int encontrar_indice(Grafo* g, const char* nome) {
    for (int i = 0; i < g->num_vertices; i++) {
        if (strcmp(g->vertices[i].nome, nome) == 0)
            return i;
    }
    return -1;
}

int adicionar_vertice(Grafo* g, const char* nome) {
    if (g->num_vertices >= MAX_VERTICES) {
        printf("Numero maximo de vertices atingido.\n");
        return -1;
    }
    if (encontrar_indice(g, nome) != -1) {
        printf("Vertice com esse nome je existe.\n");
        return -1;
    }
    strcpy(g->vertices[g->num_vertices].nome, nome);
    g->vertices[g->num_vertices].lista_arestas = NULL;
    return g->num_vertices++;
}

void adicionar_aresta(Grafo* g, int origem, int destino, int peso) {
    Aresta* nova = (Aresta*)malloc(sizeof(Aresta));
    nova->destino = destino;
    nova->peso = peso;
    nova->prox = g->vertices[origem].lista_arestas;
    g->vertices[origem].lista_arestas = nova;

    // Bidirecional
    Aresta* nova2 = (Aresta*)malloc(sizeof(Aresta));
    nova2->destino = origem;
    nova2->peso = peso;
    nova2->prox = g->vertices[destino].lista_arestas;
    g->vertices[destino].lista_arestas = nova2;
}

void mostrar_grafo(Grafo* g) {
    printf("\n--- Grafo Atual ---\n");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("%s -> ", g->vertices[i].nome);
        Aresta* atual = g->vertices[i].lista_arestas;
        while (atual != NULL) {
            printf("%s (%d) ", g->vertices[atual->destino].nome, atual->peso);
            atual = atual->prox;
        }
        printf("\n");
    }
    printf("-------------------\n");
}

void remover_aresta(Grafo* g, int origem, int destino) {
    Aresta **atual = &g->vertices[origem].lista_arestas;
    while (*atual != NULL) {
        if ((*atual)->destino == destino) {
            Aresta* temp = *atual;
            *atual = (*atual)->prox;
            free(temp);
            break;
        }
        atual = &(*atual)->prox;
    }

    // Remover também do destino (grafo bidirecional)
    atual = &g->vertices[destino].lista_arestas;
    while (*atual != NULL) {
        if ((*atual)->destino == origem) {
            Aresta* temp = *atual;
            *atual = (*atual)->prox;
            free(temp);
            break;
        }
        atual = &(*atual)->prox;
    }
}

void remover_vertice(Grafo* g, const char* nome) {
    int indice = encontrar_indice(g, nome);
    if (indice == -1) {
        printf("Vertice nao encontrado.\n");
        return;
    }

    // 1. Remover arestas que apontam para esse v�rtice
    for (int i = 0; i < g->num_vertices; i++) {
        if (i == indice) continue;
        Aresta **atual = &g->vertices[i].lista_arestas;
        while (*atual != NULL) {
            if ((*atual)->destino == indice) {
                Aresta* temp = *atual;
                *atual = (*atual)->prox;
                free(temp);
            } else {
                atual = &(*atual)->prox;
            }
        }
    }

    // 2. Liberar a lista de arestas do v�rtice a ser removido
    Aresta* atual = g->vertices[indice].lista_arestas;
    while (atual != NULL) {
        Aresta* temp = atual;
        atual = atual->prox;
        free(temp);
    }

    // 3. Deslocar os v�rtices
    for (int i = indice; i < g->num_vertices - 1; i++) {
        g->vertices[i] = g->vertices[i + 1];
    }
    g->num_vertices--;

    // 4. Atualizar os �ndices nas arestas restantes
    for (int i = 0; i < g->num_vertices; i++) {
        Aresta* a = g->vertices[i].lista_arestas;
        while (a != NULL) {
            if (a->destino > indice) {
                a->destino--; // �ndice mudou por causa do deslocamento
            }
            a = a->prox;
        }
    }

    printf("Vertice e conexoes removidos com sucesso.\n");
}


//Exibe o caminho reconstruído a partir do vetor anterior.
void mostrar_caminho(int anterior[], int destino, Grafo* g) {
    if (anterior[destino] == -1) {
        printf("%s", g->vertices[destino].nome);
        return;
    }
    mostrar_caminho(anterior, anterior[destino], g);
    printf(" -> %s", g->vertices[destino].nome);
}

//Calcula o menor caminho entre dois vértices (pesos positivos).
void dijkstra(Grafo* g, int origem, int destino) {
    int dist[g->num_vertices];
    bool visitado[g->num_vertices];
    int anterior[g->num_vertices];

    for (int i = 0; i < g->num_vertices; i++) {
        dist[i] = INT_MAX;
        visitado[i] = false;
        anterior[i] = -1;
    }

    dist[origem] = 0;

    for (int i = 0; i < g->num_vertices - 1; i++) {
        // Encontrar o vértice com menor distância não visitado
        int min = INT_MAX, u = -1;
        for (int j = 0; j < g->num_vertices; j++) {
            if (!visitado[j] && dist[j] < min) {
                min = dist[j];
                u = j;
            }
        }

        if (u == -1) break; // Nenhum vértice acessível

        visitado[u] = true;

        Aresta* a = g->vertices[u].lista_arestas;
        while (a != NULL) {
            int v = a->destino;
            if (!visitado[v] && dist[u] + a->peso < dist[v]) {
                dist[v] = dist[u] + a->peso;
                anterior[v] = u;
            }
            a = a->prox;
        }
    }

    if (dist[destino] == INT_MAX) {
        printf("Nao existe caminho entre os pontos.\n");
        return;
    }

    printf("Caminho mais curto de %s para %s:\n", g->vertices[origem].nome, g->vertices[destino].nome);
    mostrar_caminho(anterior, destino, g);
    printf("\nDistancia total: %d\n", dist[destino]);
}

//	Conta quantas arestas um vértice tem.
int grau_vertice(Grafo* g, int v) {
    int grau = 0;
    Aresta* a = g->vertices[v].lista_arestas;
    while (a) {
        grau++;
        a = a->prox;
    }
    return grau;
}

//	Verifica se o grafo possui caminho/circuito euleriano.
bool tem_caminho_euleriano(Grafo* g) {
    int impares = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (grau_vertice(g, i) % 2 != 0)
            impares++;
    }
    return (impares == 0 || impares == 2);
}

//Percorre o grafo usando DFS, visitando cada aresta uma vez.
void dfs_euler(int u, Grafo* g, int visitado[MAX_VERTICES][MAX_VERTICES]) {
    Aresta* a = g->vertices[u].lista_arestas;
    while (a != NULL) {
        int v = a->destino;
        if (!visitado[u][v]) {
            visitado[u][v] = 1;
            visitado[v][u] = 1;
            printf(" -> %s", g->vertices[v].nome);
            dfs_euler(v, g, visitado);
        }
        a = a->prox;
    }
}

//verifica possibilidade e imprime caminho.
void caminho_euleriano(Grafo* g) {
    if (!tem_caminho_euleriano(g)) {
        printf("O grafo NAO possui caminho ou circuito euleriano.\n");
        return;
    }

    int visitado[MAX_VERTICES][MAX_VERTICES] = {0};

    int inicio = -1;
    for (int i = 0; i < g->num_vertices; i++) {
        if (grau_vertice(g, i) % 2 != 0) {
            inicio = i;
            break;
        }
    }
    if (inicio == -1) inicio = 0;

    printf("Caminho Euleriano:\n%s", g->vertices[inicio].nome);
    dfs_euler(inicio, g, visitado);
    printf("\n");
}

int pai[MAX_VERTICES];

////Busca com compressão de caminho (Union-Find)
int encontrar(int x) {
    if (pai[x] != x)
        pai[x] = encontrar(pai[x]);
    return pai[x];
}

//Junta dois conjuntos distintos
void unir(int x, int y) {
    int raiz_x = encontrar(x);
    int raiz_y = encontrar(y);
    if (raiz_x != raiz_y)
        pai[raiz_x] = raiz_y;
}

//Ordena arestas pelo peso
int comparar_arestas(const void* a, const void* b) {
    return ((ArestaK*)a)->peso - ((ArestaK*)b)->peso;
}

//Aplica Kruskal para formar a AGM do grafo
void agm_kruskal(Grafo* g) {
    ArestaK arestas[MAX_ARESTAS];
    int total_arestas = 0;

    for (int i = 0; i < g->num_vertices; i++) {
        Aresta* a = g->vertices[i].lista_arestas;
        while (a != NULL) {
            if (i < a->destino) {
                arestas[total_arestas].origem = i;
                arestas[total_arestas].destino = a->destino;
                arestas[total_arestas].peso = a->peso;
                total_arestas++;
            }
            a = a->prox;
        }
    }

    qsort(arestas, total_arestas, sizeof(ArestaK), comparar_arestas);

    for (int i = 0; i < g->num_vertices; i++)
        pai[i] = i;

    printf("\nÁrvore Geradora Mínima (Kruskal):\n");
    int peso_total = 0;
    int arestas_incluidas = 0;

    for (int i = 0; i < total_arestas && arestas_incluidas < g->num_vertices - 1; i++) {
        int u = arestas[i].origem;
        int v = arestas[i].destino;
        if (encontrar(u) != encontrar(v)) {
            unir(u, v);
            printf("%s -- %s  (peso: %d)\n", g->vertices[u].nome, g->vertices[v].nome, arestas[i].peso);
            peso_total += arestas[i].peso;
            arestas_incluidas++;
        }
    }

    if (arestas_incluidas != g->num_vertices - 1) {
        printf("Aviso: o grafo não é conexo. A AGM não cobre todos os vértices.\n");
    }

    printf("Peso total da AGM: %d\n", peso_total);
}

void liberar_memoria(Grafo* g) {
    for (int i = 0; i < g->num_vertices; i++) {
        Aresta* atual = g->vertices[i].lista_arestas;
        while (atual != NULL) {
            Aresta* temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
}

 int main() {
    Grafo g;
    inicializar_grafo(&g);

    int opcao;
    char nome1[50], nome2[50];
    int peso;
    int i1, i2;

    adicionar_vertice(&g, "Deposito 2");
    adicionar_vertice(&g, "Padaria Bom dia");
    adicionar_vertice(&g, "Mercado descontao");
    adicionar_vertice(&g, "Posto Avenida");
    adicionar_vertice(&g, "Deposito 3");
    adicionar_vertice(&g, "Padaria Pao da Casa");
    adicionar_vertice(&g, "Supermercado Essencial");
    adicionar_vertice(&g, "Padaria do campo");
    adicionar_vertice(&g, "Deposito Central");
    adicionar_vertice(&g, "EcoPosto");
    adicionar_vertice(&g, "Padaria Amanhecer");
    adicionar_vertice(&g, "Supermercado supermais");
    adicionar_vertice(&g, "Posto de gasolina");
    adicionar_vertice(&g, "Mercado Sol");
    adicionar_vertice(&g, "Mercado Sitio novo");

    adicionar_aresta(&g, encontrar_indice(&g, "Deposito 2"), encontrar_indice(&g, "Mercado Sol"), 2);
    adicionar_aresta(&g, encontrar_indice(&g, "Deposito 2"), encontrar_indice(&g, "Padaria Bom dia"), 3);
    adicionar_aresta(&g, encontrar_indice(&g, "Padaria Bom dia"), encontrar_indice(&g, "Mercado descontao"), 1);
    adicionar_aresta(&g, encontrar_indice(&g, "Mercado descontao"), encontrar_indice(&g, "Posto Avenida"), 3);
    adicionar_aresta(&g, encontrar_indice(&g, "Posto Avenida"), encontrar_indice(&g, "Deposito 3"), 2);
    adicionar_aresta(&g, encontrar_indice(&g, "Deposito 3"), encontrar_indice(&g, "Padaria Pao da Casa"), 3);
    adicionar_aresta(&g, encontrar_indice(&g, "Posto Avenida"), encontrar_indice(&g, "Supermercado Essencial"), 3);
    adicionar_aresta(&g, encontrar_indice(&g, "Supermercado Essencial"), encontrar_indice(&g, "Padaria do campo"), 12);
    adicionar_aresta(&g, encontrar_indice(&g, "Deposito Central"), encontrar_indice(&g, "Mercado descontao"), 2);
    adicionar_aresta(&g, encontrar_indice(&g, "Deposito Central"), encontrar_indice(&g, "Posto de gasolina"), 2);
    adicionar_aresta(&g, encontrar_indice(&g, "Deposito Central"), encontrar_indice(&g, "EcoPosto"), 5);
    adicionar_aresta(&g, encontrar_indice(&g, "EcoPosto"), encontrar_indice(&g, "Padaria Amanhecer"), 5);
    adicionar_aresta(&g, encontrar_indice(&g, "EcoPosto"), encontrar_indice(&g, "Supermercado supermais"), 2);
    adicionar_aresta(&g, encontrar_indice(&g, "Supermercado supermais"), encontrar_indice(&g, "Posto de gasolina"), 3);
    adicionar_aresta(&g, encontrar_indice(&g, "Supermercado supermais"), encontrar_indice(&g, "Padaria Amanhecer"), 4);
    adicionar_aresta(&g, encontrar_indice(&g, "Posto de gasolina"), encontrar_indice(&g, "Mercado Sol"), 1);
    adicionar_aresta(&g, encontrar_indice(&g, "Posto de gasolina"), encontrar_indice(&g, "Mercado Sitio novo"), 8); 

    do {
        printf("\n=====Sistema Logistico======\n");
        printf("1 - Adicionar ponto\n");
        printf("2 - Adicionar conexao\n");
        printf("3 - Mostrar grafo\n");
        printf("4 - Remover conexao\n");
        printf("5 - Remover ponto \n");
        printf("6 - Calcular menor caminho (Dijkstra)\n");
        printf("7 - Verificar e mostrar caminho Euleriano\n");
        printf("8 - Gerar Arvore Geradora Minima (Kruskal)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // consumir \n

        switch (opcao) {
            case 1:
                printf("Digite o nome do ponto (ex: Cliente A, Deposito 1): ");
                fgets(nome1, 50, stdin);
                nome1[strcspn(nome1, "\n")] = '\0'; // remover \n
                adicionar_vertice(&g, nome1);
                break;

            case 2:
                printf("Digite o nome do ponto de origem: ");
                fgets(nome1, 50, stdin);
                nome1[strcspn(nome1, "\n")] = '\0';

                printf("Digite o nome do ponto de destino: ");
                fgets(nome2, 50, stdin);
                nome2[strcspn(nome2, "\n")] = '\0';

                printf("Digite o peso da conexao (distancia ou tempo): ");
                scanf("%d", &peso);
                getchar();

                int i1 = encontrar_indice(&g, nome1);
                int i2 = encontrar_indice(&g, nome2);

                if (i1 == -1 || i2 == -1) {
                    printf("Um ou ambos os pontos nao existem.\n");
                } else {
                    adicionar_aresta(&g, i1, i2, peso);
                }
                break;

            case 3:
                mostrar_grafo(&g);
                break;

            case 4:
                printf("Digite o nome do ponto de origem da conexao a remover: ");
                fgets(nome1, 50, stdin);
                nome1[strcspn(nome1, "\n")] = '\0';

                printf("Digite o nome do ponto de destino da conexao a remover: ");
                fgets(nome2, 50, stdin);
                nome2[strcspn(nome2, "\n")] = '\0';

                i1 = encontrar_indice(&g, nome1);
                i2 = encontrar_indice(&g, nome2);

                if (i1 == -1 || i2 == -1) {
                    printf("Um ou ambos os pontos nao existem.\n");
                } else {
                    remover_aresta(&g, i1, i2);
                    printf("Conexao removida com sucesso.\n");
                }
                break;

            case 5:
                printf("Digite o nome do ponto (vertice) que deseja remover: ");
                fgets(nome1, 50, stdin);
                nome1[strcspn(nome1, "\n")] = '\0';

                remover_vertice(&g, nome1);
                break;

            case 6:
                printf("Digite o ponto de origem: ");
                fgets(nome1, 50, stdin);
                nome1[strcspn(nome1, "\n")] = '\0';

                printf("Digite o ponto de destino: ");
                fgets(nome2, 50, stdin);
                nome2[strcspn(nome2, "\n")] = '\0';

                i1 = encontrar_indice(&g, nome1);
                i2 = encontrar_indice(&g, nome2);

                if (i1 == -1 || i2 == -1) {
                    printf("Um ou ambos os pontos nao existem.\n");
                } else {
                    dijkstra(&g, i1, i2);
                }
                break;
            case 7:
                caminho_euleriano(&g);
                break;

            case 8:
                agm_kruskal(&g);
                break;

            case 0:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    liberar_memoria(&g);
    return 0;
}
