#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 50

typedef struct Cidade {
    char nome[MAX_NOME];
    struct Cidade *prox;
} Cidade;

typedef struct No {
    int id;
    struct No *prox;
} No;

typedef struct Vertice {
    char nome[MAX_NOME];
    No *adj;
} Vertice;

typedef struct Grafo {
    Vertice *vertices;
    int tamanho;
} Grafo;

// Funções auxiliares
int encontrarIndiceCidade(Grafo *grafo, const char *nome) {
	int i;
    for (i = 0; i < grafo->tamanho; i++) {
        if (strcmp(grafo->vertices[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

void adicionarCidade(Grafo *grafo, const char *nome) {
    grafo->vertices = realloc(grafo->vertices, (grafo->tamanho + 1) * sizeof(Vertice));
    strcpy(grafo->vertices[grafo->tamanho].nome, nome);
    grafo->vertices[grafo->tamanho].adj = NULL;
    grafo->tamanho++;
}

void adicionarAresta(Grafo *grafo, const char *origem, const char *destino) {
    int o = encontrarIndiceCidade(grafo, origem);
    int d = encontrarIndiceCidade(grafo, destino);
    if (o == -1 || d == -1) {
        printf("Erro: cidade nao encontrada.\n");
        return;
    }
    No *novo1 = malloc(sizeof(No));
    novo1->id = d;
    novo1->prox = grafo->vertices[o].adj;
    grafo->vertices[o].adj = novo1;

    No *novo2 = malloc(sizeof(No));
    novo2->id = o;
    novo2->prox = grafo->vertices[d].adj;
    grafo->vertices[d].adj = novo2;
}

void removerAresta(Grafo *grafo, const char *origem, const char *destino) {
    int o = encontrarIndiceCidade(grafo, origem);
    int d = encontrarIndiceCidade(grafo, destino);
    if (o == -1 || d == -1) return;

    No **ptr = &grafo->vertices[o].adj;
    while (*ptr) {
        if ((*ptr)->id == d) {
            No *remover = *ptr;
            *ptr = (*ptr)->prox;
            free(remover);
            break;
        }
        ptr = &(*ptr)->prox;
    }

    // Remove também o inverso
    ptr = &grafo->vertices[d].adj;
    while (*ptr) {
        if ((*ptr)->id == o) {
            No *remover = *ptr;
            *ptr = (*ptr)->prox;
            free(remover);
            break;
        }
        ptr = &(*ptr)->prox;
    }
}


// DFS recursivo
void DFS(Grafo *grafo, int v, int *visitado) {
    visitado[v] = 1;
    printf("%s ", grafo->vertices[v].nome);
    No *adj = grafo->vertices[v].adj;
    while (adj) {
        if (!visitado[adj->id]) {
            DFS(grafo, adj->id, visitado);
        }
        adj = adj->prox;
    }
}

void buscarDFS(Grafo *grafo, const char *cidade) {
    int v = encontrarIndiceCidade(grafo, cidade);
    if (v == -1) {
        printf("Cidade não encontrada.\n");
        return;
    }
    int *visitado = calloc(grafo->tamanho, sizeof(int));
    printf("DFS a partir de %s: ", cidade);
    DFS(grafo, v, visitado);
    printf("\n");
    free(visitado);
}

// BFS com fila
void buscarBFS(Grafo *grafo, const char *cidade) {
    int v = encontrarIndiceCidade(grafo, cidade);
    if (v == -1) {
        printf("Cidade não encontrada.\n");
        return;
    }

    int *visitado = calloc(grafo->tamanho, sizeof(int));
    int *fila = malloc(grafo->tamanho * sizeof(int));
    int inicio = 0, fim = 0;

    fila[fim++] = v;
    visitado[v] = 1;

    printf("BFS a partir de %s: ", cidade);
    while (inicio < fim) {
        int atual = fila[inicio++];
        printf("%s ", grafo->vertices[atual].nome);
        No *adj = grafo->vertices[atual].adj;
        while (adj) {
            if (!visitado[adj->id]) {
                fila[fim++] = adj->id;
                visitado[adj->id] = 1;
            }
            adj = adj->prox;
        }
    }
    printf("\n");

    free(visitado);
    free(fila);
}

void imprimirGrafo(Grafo *grafo) {
    printf("\nGrafo:\n");
    int i;
    for ( i = 0; i < grafo->tamanho; i++) {
        printf("%s -> ", grafo->vertices[i].nome);
        No *adj = grafo->vertices[i].adj;
        while (adj) {
            printf("%s ", grafo->vertices[adj->id].nome);
            adj = adj->prox;
        }
        printf("\n");
    }
}

void removerCidade(Grafo *grafo, const char *nome) {
    int indice = encontrarIndiceCidade(grafo, nome);
    if (indice == -1) {
        printf("Erro: cidade não encontrada.\n");
        return;
    }

    // Remover arestas associadas à cidade
    int i;
    for (i = 0; i < grafo->tamanho; i++) {
        if (i != indice) {
            removerAresta(grafo, grafo->vertices[i].nome, nome);
        }
    }

    // Remover o vértice
    int j;
    for ( j = indice; j < grafo->tamanho - 1; j++) {
        grafo->vertices[i] = grafo->vertices[i + 1];
    }

    grafo->tamanho--;
    grafo->vertices = realloc(grafo->vertices, grafo->tamanho * sizeof(Vertice));

    printf("Cidade %s removida.\n", nome);
}

void liberarGrafo(Grafo *grafo) {
	int i;
    for ( i = 0;  i < grafo->tamanho; i++) {
        No *adj = grafo->vertices[i].adj;
        while (adj) {
            No *temp = adj;
            adj = adj->prox;
            free(temp);
        }
    }
    free(grafo->vertices);
}

// Função principal
int main() {
    Grafo grafo = {NULL, 0};

    int opcao;
    char cidade1[MAX_NOME], cidade2[MAX_NOME];

    do {
        printf("\nEscolha uma opcao:\n");
        printf("1- Adicionar cidade\n");
        printf("2- Remover cidade\n");
        printf("3- Inserir conexao\n");
        printf("4- Remover conexao\n");
        printf("5- DFS\n");
        printf("6- BFS\n");
        printf("7- Imprimir Grafo\n");
        printf("8- Sair\n");
        scanf("%d", &opcao);
        getchar(); // limpar buffer

        switch (opcao) {
            case 1:
                printf("Nome da cidade: ");
                fgets(cidade1, MAX_NOME, stdin);
                cidade1[strcspn(cidade1, "\n")] = '\0';
                adicionarCidade(&grafo, cidade1);
                break;
            case 2:
			    printf("Nome da cidade a remover: ");
			    fgets(cidade1, MAX_NOME, stdin);
			    cidade1[strcspn(cidade1, "\n")] = '\0';
			    removerCidade(&grafo, cidade1);
			    break;
            case 3:
                printf("Cidade origem: ");
                fgets(cidade1, MAX_NOME, stdin);
                cidade1[strcspn(cidade1, "\n")] = '\0';
                printf("Cidade destino: ");
                fgets(cidade2, MAX_NOME, stdin);
                cidade2[strcspn(cidade2, "\n")] = '\0';
                adicionarAresta(&grafo, cidade1, cidade2);
                break;
            case 4:
                printf("Cidade origem: ");
                fgets(cidade1, MAX_NOME, stdin);
                cidade1[strcspn(cidade1, "\n")] = '\0';
                printf("Cidade destino: ");
                fgets(cidade2, MAX_NOME, stdin);
                cidade2[strcspn(cidade2, "\n")] = '\0';
                removerAresta(&grafo, cidade1, cidade2);
                break;
            case 5:
                printf("Cidade de partida para DFS: ");
                fgets(cidade1, MAX_NOME, stdin);
                cidade1[strcspn(cidade1, "\n")] = '\0';
                buscarDFS(&grafo, cidade1);
                break;
            case 6:
                printf("Cidade de partida para BFS: ");
                fgets(cidade1, MAX_NOME, stdin);
                cidade1[strcspn(cidade1, "\n")] = '\0';
                buscarBFS(&grafo, cidade1);
                break;
            case 7:
                imprimirGrafo(&grafo);
                break;

            case 8:
    			liberarGrafo(&grafo);
    			printf("Saindo...\n");
    			break;

            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}

