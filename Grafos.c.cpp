#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct Aresta {
    int destino;
    struct Aresta *prox;
} Aresta;

typedef struct {
    char nome[50];
    Aresta *lista_adj;
} Vertice;

typedef struct {
    Vertice *vertices;
    int num_vertices;
} Grafo;

Grafo *criar_grafo() {
    Grafo *g = (Grafo *) malloc(sizeof(Grafo));
    g->vertices = NULL;
    g->num_vertices = 0;
    return g;
}

int encontrar_indice(Grafo *g, const char *nome) {
    for (int i = 0; i < g->num_vertices; i++) {
        if (strcmp(g->vertices[i].nome, nome) == 0)
            return i;
    }
    return -1;
}

void inserir_cidade(Grafo *g, const char *nome) {
    Vertice *temp = (Vertice *) realloc(g->vertices, (g->num_vertices + 1) * sizeof(Vertice));
    if (temp == NULL) {
        printf("Erro de alocacao de memoria.\n");
        return;
    }
    
    g->vertices = temp;  // ? Atualize o ponteiro do grafo!

    strncpy(g->vertices[g->num_vertices].nome, nome, sizeof(g->vertices[g->num_vertices].nome) - 1);
    g->vertices[g->num_vertices].nome[sizeof(g->vertices[g->num_vertices].nome) - 1] = '\0';
    g->vertices[g->num_vertices].lista_adj = NULL;

    g->num_vertices++;
    printf("Cidade '%s' inserida com sucesso.\n", nome);
}


void inserir_conexao(Grafo *g, const char *origem_nome, const char *destino_nome) {
    int origem = encontrar_indice(g, origem_nome);
    int destino = encontrar_indice(g, destino_nome);

    if (origem == -1 || destino == -1) {
        printf("Erro: cidade invalida.\n");
        return;
    }

    // origem -> destino
    Aresta *nova1 = (Aresta *) malloc(sizeof(Aresta));
    nova1->destino = destino;
    nova1->prox = g->vertices[origem].lista_adj;
    g->vertices[origem].lista_adj = nova1;

    // destino -> origem (bidirecional)
    Aresta *nova2 = (Aresta *) malloc(sizeof(Aresta));
    nova2->destino = origem;
    nova2->prox = g->vertices[destino].lista_adj;
    g->vertices[destino].lista_adj = nova2;

    printf("Conexao inserida entre '%s' e '%s'.\n", origem_nome, destino_nome);
}

void remover_conexao(Grafo *g, const char *origem_nome, const char *destino_nome) {
    int origem = encontrar_indice(g, origem_nome);
    int destino = encontrar_indice(g, destino_nome);

    if (origem == -1 || destino == -1) {
        printf("Erro: cidade invalida.\n");
        return;
    }

    // Remover destino da lista de origem
    Aresta *atual = g->vertices[origem].lista_adj;
    Aresta *anterior = NULL;

    while (atual && atual->destino != destino) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual) {
        if (anterior == NULL)
            g->vertices[origem].lista_adj = atual->prox;
        else
            anterior->prox = atual->prox;
        free(atual);
    }

    // Remover origem da lista de destino
    atual = g->vertices[destino].lista_adj;
    anterior = NULL;

    while (atual && atual->destino != origem) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual) {
        if (anterior == NULL)
            g->vertices[destino].lista_adj = atual->prox;
        else
            anterior->prox = atual->prox;
        free(atual);
    }

    printf("Conexao entre '%s' e '%s' removida com sucesso.\n", origem_nome, destino_nome);
}


void imprimir_grafo(Grafo *g) {
    printf("\n--- Grafo ---\n");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("%s -> ", g->vertices[i].nome);
        Aresta *adj = g->vertices[i].lista_adj;
        while (adj) {
            printf("%s ", g->vertices[adj->destino].nome);
            adj = adj->prox;
        }
        printf("\n");
    }
}

void remover_cidade(Grafo *g, const char *nome) {
    int indice = encontrar_indice(g, nome);
    if (indice == -1) {
        printf("Cidade '%s' nao encontrada.\n", nome);
        return;
    }

    // 1. Remove todas as conexões para a cidade
    for (int i = 0; i < g->num_vertices; i++) {
        if (i == indice) continue;
        Aresta *atual = g->vertices[i].lista_adj;
        Aresta *anterior = NULL;
        while (atual) {
            if (atual->destino == indice) {
                if (anterior == NULL)
                    g->vertices[i].lista_adj = atual->prox;
                else
                    anterior->prox = atual->prox;
                free(atual);
                break; // Só uma aresta para a cidade (não é multigrafo)
            }
            anterior = atual;
            atual = atual->prox;
        }
    }

    // 2. Remove a lista de adjacência da cidade
    Aresta *a = g->vertices[indice].lista_adj;
    while (a) {
        Aresta *prox = a->prox;
        free(a);
        a = prox;
    }

    // 3. Move os vértices posteriores para preencher o espaço
    for (int i = indice; i < g->num_vertices - 1; i++) {
        g->vertices[i] = g->vertices[i + 1];
    }
    g->num_vertices--;

    // 4. Corrige os índices das arestas (já que os índices mudaram)
    for (int i = 0; i < g->num_vertices; i++) {
        Aresta *atual = g->vertices[i].lista_adj;
        while (atual) {
            if (atual->destino > indice)
                atual->destino--;
            atual = atual->prox;
        }
    }

    // 5. Realoca o vetor para economizar memória
    Vertice *temp = (Vertice *) realloc(g->vertices, g->num_vertices * sizeof(Vertice));
    if (temp != NULL || g->num_vertices == 0) {
        g->vertices = temp;
    }

    printf("Cidade '%s' removida com sucesso.\n", nome);
}

// Busca em Profundidade 
int dfs(Grafo *g, int atual, int destino, int *visitado, int *caminho, int *tam) {
    visitado[atual] = 1;
    caminho[(*tam)++] = atual;

    if (atual == destino)
        return 1;

    Aresta *adj = g->vertices[atual].lista_adj;
    while (adj) {
        if (!visitado[adj->destino]) {
            if (dfs(g, adj->destino, destino, visitado, caminho, tam))
                return 1;
        }
        adj = adj->prox;
    }

    (*tam)--;
    return 0;
}

void mostrar_rota(Grafo *g, const char *origem_nome, const char *destino_nome) {
    int origem = encontrar_indice(g, origem_nome);
    int destino = encontrar_indice(g, destino_nome);

    if (origem == -1 || destino == -1) {
        printf("Cidade invalida.\n");
        return;
    }

    int *visitado = (int *) calloc(g->num_vertices, sizeof(int));
    int *caminho = (int *) malloc(g->num_vertices * sizeof(int));
    int tam = 0;

    if (dfs(g, origem, destino, visitado, caminho, &tam)) {
        printf("Rota de %s ate %s:\n", origem_nome, destino_nome);
        for (int i = 0; i < tam; i++) {
            printf("%s", g->vertices[caminho[i]].nome);
            if (i < tam - 1) printf(" -> ");
        }
        printf("\n");
    } else {
        printf("Nao existe rota entre %s e %s.\n", origem_nome, destino_nome);
    }

    free(visitado);
    free(caminho);
}

int main() {
    Grafo *g = criar_grafo();
    int opcao;
    char origem[50], destino[50];

    do {
        printf("\n1. Inserir cidade\n");
		printf("2. Remover cidade\n");
		printf("3. Inserir conexao entre cidades\n");
		printf("4. Remover conexao\n");
		printf("5. Imprimir grafo\n");
		printf("6. Mostrar rota entre duas cidades\n");
		printf("7. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Nome da cidade: ");
                scanf(" %[^\n]", origem);
                inserir_cidade(g, origem);
                break;
            case 2:
            	printf("Nome da cidade a remover: ");
		        scanf(" %[^\n]", origem);
		        remover_cidade(g, origem);
        		break;
		
            case 3:
                printf("Cidade origem: ");
                scanf(" %[^\n]", origem);
                printf("Cidade destino: ");
                scanf(" %[^\n]", destino);
                inserir_conexao(g, origem, destino);
                break;
			case 4:
                printf("Cidade de origem: ");
                scanf(" %[^\n]", origem);
                printf("Cidade de destino: ");
                scanf(" %[^\n]", destino);
                remover_conexao(g, origem, destino);
                break;

            case 5:
                imprimir_grafo(g);
                break;

            case 6:
                printf("Cidade de origem: ");
                scanf(" %[^\n]", origem);
                printf("Cidade de destino: ");
                scanf(" %[^\n]", destino);
                mostrar_rota(g, origem, destino);
                break;
                
            case 7:
                printf("Encerrando...\n");

                // Libera memória
                for (int i = 0; i < g->num_vertices; i++) {
                    Aresta *a = g->vertices[i].lista_adj;
                    while (a) {
                        Aresta *prox = a->prox;
                        free(a);
                        a = prox;
                    }
                }
                free(g->vertices);
                free(g);
                break;

            default:
                printf("Opcao invalida!\n");
        }

    } while (opcao != 7);

    return 0;
}


