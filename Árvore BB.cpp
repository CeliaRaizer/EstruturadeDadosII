#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Livro {
    char titulo[100];
    char autor[100];
    int cod;
    struct Livro *esq, *dir;
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
        novo->esq = novo->dir = NULL;
    }
    return novo;
}

Livro* inserirLivro(Livro *raiz, Livro *novo) {
    if (raiz == NULL) {
        return novo;
    }
    if (novo->cod < raiz->cod) {
        raiz->esq = inserirLivro(raiz->esq, novo); //insere esquerda
    } else if (novo->cod > raiz->cod) { 
        raiz->dir = inserirLivro(raiz->dir, novo); //insere direita
    }
    return raiz;
}

Livro* buscarLivro(Livro *raiz, int cod) {
    if (raiz == NULL || raiz->cod == cod) {
        return raiz;
    }
    if (cod < raiz->cod) {
        return buscarLivro(raiz->esq, cod);  //cód buscado menor que o do nó pesquisa esq
    } else {
        return buscarLivro(raiz->dir, cod);  //cód buscado maior pesquisa dir
    }
}

Livro* removerLivro(Livro *raiz, int cod) {
    if (raiz == NULL) {
        return NULL; 
    }

    if (cod < raiz->cod) {
        raiz->esq = removerLivro(raiz->esq, cod); 
    } else if (cod > raiz->cod) {
        raiz->dir = removerLivro(raiz->dir, cod);  
    } else {
        if (raiz->esq == NULL) { //nó folha
            Livro *temp = raiz->dir; 
            free(raiz);  
            return temp;  
        }else if (raiz->dir == NULL) { //O no tem apenas o filho esquerdo
            Livro *temp = raiz->esq;  
            free(raiz); 
            return temp;  
        }else { //O nó tem dois filhos
            Livro *temp = raiz->dir;
            while (temp->esq != NULL) {  
                temp = temp->esq;
            }
            raiz->cod = temp->cod;  
            strcpy(raiz->titulo, temp->titulo);  
            strcpy(raiz->autor, temp->autor);  
            raiz->dir = removerLivro(raiz->dir, temp->cod); 
        }
    }
    return raiz;  
}

    
void exibirArvore(Livro *raiz, int nivel) {
    if (raiz != NULL) {
        exibirArvore(raiz->dir, nivel + 1);
        
        for (int i = 0; i < nivel; i++) {
            printf("   ");  
        }
        printf("|-- %d (%s)\n", raiz->cod, raiz->titulo);

        exibirArvore(raiz->esq, nivel + 1);
    }
}
//Raiz - esquerda - direita
void preOrdem(Livro *raiz, int nivel) {
    if (raiz != NULL) {
       
        printf("|-- %d (%s)\n", raiz->cod, raiz->titulo);
        
        preOrdem(raiz->esq, nivel + 1);
        preOrdem(raiz->dir, nivel + 1);
    }
}
//esquerda - raiz - direita
void emOrdem(Livro *raiz, int nivel) {
    if (raiz != NULL) {
        emOrdem(raiz->esq, nivel + 1);  

        printf("|-- %d (%s)\n", raiz->cod, raiz->titulo);

        emOrdem(raiz->dir, nivel + 1);  
    }
}
//esquera - direita - raiz
void posOrdem(Livro *raiz, int nivel) {
    if (raiz != NULL) {
        posOrdem(raiz->esq, nivel + 1);  
        posOrdem(raiz->dir, nivel + 1); 

        printf("|-- %d (%s)\n", raiz->cod, raiz->titulo);
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

    for (;;) {
        printf("\n===== MENU BIBLIOTECA =====\n");
        printf("1 - Inserir livro\n");
        printf("2 - Buscar livro por Codigo\n");
        printf("3 - Remover livro por Codigo\n");
        printf("4 - Exibir percurso em pre-ordem \n");
        printf("5 - Exibir percurso em ordem \n");
        printf("6 - Exibir percurso em pos-ordem \n");
        printf("7 - Exibir Arvore estruturada\n");
        printf("8 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opc);

        if (opc == 1) {
            printf("Digite o codigo do livro (ISSN/ISBN): ");
            scanf("%d", &cod);

            printf("Digite o titulo do livro: ");
            scanf(" %[^\n]", titulo);

            printf("Digite o autor do livro: ");
            scanf(" %[^\n]", autor);

            if (buscarLivro(biblioteca->raiz, cod)) {
                printf("Livro com esse código já existe!\n");
            } else {
                Livro *novo = criarLivro(cod, titulo, autor);
                biblioteca->raiz = inserirLivro(biblioteca->raiz, novo);
                printf("Livro inserido com sucesso!\n");
            }
        } else if (opc == 2) {
            printf("Digite o codigo do livro para buscar: ");
            scanf("%d", &cod);

            Livro *encontrado = buscarLivro(biblioteca->raiz, cod);
            if (encontrado) {
                printf("Livro encontrado!\nCodigo: %d\nTitulo: %s\nAutor: %s\n", encontrado->cod, encontrado->titulo, encontrado->autor);
            } else {
                printf("Livro nao encontrado!\n");
            }
        } else if (opc == 3) {
            printf("Digite o codigo do livro para remover: ");
            scanf("%d", &cod);
            biblioteca->raiz = removerLivro(biblioteca->raiz, cod);
            printf("Livro removido!\n");
        } else if (opc == 4) {
            preOrdem(biblioteca->raiz,0);
        } else if (opc == 5) {
            emOrdem(biblioteca->raiz,0);
        } else if (opc == 6) {
            posOrdem(biblioteca->raiz,0);
        } else if (opc == 7) {
            exibirArvore(biblioteca->raiz, 0);
        } else if (opc == 8) {
            liberarArvore(biblioteca->raiz);
            free(biblioteca);
            break;
        } else {
            printf("Opcao invalida!\n");
        }
    }
    return 0;
}

