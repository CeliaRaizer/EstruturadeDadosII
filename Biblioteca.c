#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Livro {
    char titulo[100];
    char autor[100];
    int cod;  // Código do livro (ISSN/ISBN)
    struct Livro *esq, *dir; //esq(valores menores) dir(valores maiores)
} Livro;

typedef struct Biblioteca {
    Livro *raiz;
} Biblioteca;

Biblioteca* criarBiblioteca() {
    Biblioteca *b = (Biblioteca*)malloc(sizeof(Biblioteca));
    if (b) {
        b->raiz = NULL;
    }
    return b;
}

Livro* criarLivro(int cod, char* titulo, char* autor) {
    Livro *novo = (Livro*)malloc(sizeof(Livro));
    if (novo) {
        strcpy(novo->titulo, titulo);
        strcpy(novo->autor, autor);
        novo->cod = cod;
        novo->esq = novo->dir = NULL; //inicializa os filhos como null
    }
    return novo;
}

Livro* inserirLivro(Livro *raiz, Livro *novo) {
    if (raiz == NULL) {
        return novo;
    }
    if (novo->cod < raiz->cod) {
        raiz->esq = inserirLivro(raiz->esq, novo); // Insere à esquerda se for menor
    } else {
        raiz->dir = inserirLivro(raiz->dir, novo); // Insere à direita se for maior.
    }
    return raiz;
}

Livro* buscarLivro(Livro *raiz, int cod) {
    if (raiz == NULL || raiz->cod == cod) {
        return raiz;
    }
    if (cod < raiz->cod) {
        return buscarLivro(raiz->esq, cod); // Busca na esquerda
    }
    return buscarLivro(raiz->dir, cod); // Busca na direita
}

Livro* encontrarMinimo(Livro *raiz) {
    while (raiz->esq != NULL) {
        raiz = raiz->esq; // Percorre esquerda até encontrar o menor valor.
    }
    return raiz;
}

Livro* removerLivro(Livro *raiz, int cod) {
    if (raiz == NULL) {
        return NULL;
    }

    if (cod < raiz->cod) {
        raiz->esq = removerLivro(raiz->esq, cod); //busca esq
    } else if (cod > raiz->cod) {
        raiz->dir = removerLivro(raiz->dir, cod); //busca dir
    } else {
        // Nó sem filhos
        if (raiz->esq == NULL && raiz->dir == NULL) {
            free(raiz);
            return NULL;
        }
        // Nó com um único filho
        if (raiz->esq == NULL) {
            Livro *temp = raiz->dir;
            free(raiz);
            return temp;
        } else if (raiz->dir == NULL) {
            Livro *temp = raiz->esq;
            free(raiz);
            return temp;
        }
        // Nó com dois filhos
        Livro *sucessor = encontrarMinimo(raiz->dir);
        raiz->cod = sucessor->cod;
        strcpy(raiz->titulo, sucessor->titulo);
        strcpy(raiz->autor, sucessor->autor);
        raiz->dir = removerLivro(raiz->dir, sucessor->cod);
    }
    return raiz;
}

void exibirArvore(Livro *raiz, int nivel) {
    if (raiz != NULL) {
        exibirArvore(raiz->dir, nivel + 1);
        for (int i = 0; i < nivel; i++) printf("   ");
        printf("%d: %s\n", raiz->cod, raiz->titulo);
        exibirArvore(raiz->esq, nivel + 1);
    }
}

void liberarArvore(Livro *raiz) {
    if (raiz != NULL) {
        liberarArvore(raiz->esq);
        liberarArvore(raiz->dir);
        free(raiz);
    }
}

int main() {
    Biblioteca *biblioteca = criarBiblioteca();
    if (!biblioteca) {
        printf("Erro ao criar a biblioteca!\n");
        return 1;
    }

    int opc, cod;
    char titulo[100], autor[100];

    do {
        printf("\n===== MENU BIBLIOTECA =====\n");
        printf("1 - Inserir livro\n");
        printf("2 - Buscar livro\n");
        printf("3 - Exibir arvore de livros\n");
        printf("4 - Remover livro\n");
        printf("5 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opc);
        getchar();

        if (opc == 1) {
            printf("Digite o codigo do livro (ISSN/ISBN): ");
            scanf("%d", &cod);
            getchar(); 

            printf("Digite o titulo do livro: ");
            scanf(" %[^\n]", titulo);
            getchar(); 

            printf("Digite o autor do livro: ");
            scanf(" %[^\n]", autor);
            getchar();

            Livro *novo = criarLivro(cod, titulo, autor);
            if (novo) {
                biblioteca->raiz = inserirLivro(biblioteca->raiz, novo);
                printf("Livro inserido com sucesso!\n");
            } else {
                printf("Erro ao alocar memoria para o livro!\n");
            }
        } else if (opc == 2) {
            printf("Digite o codigo do livro a buscar: ");
            scanf("%d", &cod);
            Livro *encontrado = buscarLivro(biblioteca->raiz, cod);
            if (encontrado) {
                printf("Livro encontrado- Titulo:%s Autor:%s\n", encontrado->titulo, encontrado->autor);
            } else {
                printf("Livro nao encontrado!\n");
            }
        } else if (opc == 3) {
            printf("\nExibindo arvore de livros:\n");
            exibirArvore(biblioteca->raiz, 0);
        } else if (opc == 4) {
            printf("Digite o codigo do livro a remover: ");
            scanf("%d", &cod);
            biblioteca->raiz = removerLivro(biblioteca->raiz, cod);
            printf("Livro removido (se existia)!\n");
        } else if (opc == 5) {
            printf("Saindo...\n");
            liberarArvore(biblioteca->raiz);
            free(biblioteca);
        } else {
            printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opc != 5);

    return 0;
}
