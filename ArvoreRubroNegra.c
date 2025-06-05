#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct No {
    int id;
    char nome[100];
    int cor;
    struct No *direita, *esquerda, *pai;
} No;

typedef struct {
    No *raiz;
    No *nulo;
} ArvoreRN;

void inicializarArvore(ArvoreRN *arvore) {
    arvore->nulo = (No *)malloc(sizeof(No));
    arvore->nulo->cor = 0; //preto
    arvore->nulo->esquerda = arvore->nulo->direita = arvore->nulo->pai = NULL;
    arvore->raiz = arvore->nulo;
}

No *criarNo(ArvoreRN *arvore, int id, char *nome) {
    No *novo = (No *)malloc(sizeof(No));
    novo->id = id;
    strcpy(novo->nome, nome);
    novo->cor = 1; //vermelho
    novo->esquerda = novo->direita = novo->pai = arvore->nulo;
    return novo;
}

void rotacaoEsquerda(ArvoreRN *arvore, No *x) {
    No *y = x->direita;
    x->direita = y->esquerda;
    if (y->esquerda != arvore->nulo)
        y->esquerda->pai = x;

    y->pai = x->pai;
    if (x->pai == arvore->nulo)
        arvore->raiz = y;
    else if (x == x->pai->esquerda)
        x->pai->esquerda = y;
    else
        x->pai->direita = y;

    y->esquerda = x;
    x->pai = y;
}

void rotacaoDireita(ArvoreRN *arvore, No *x) {
    No *y = x->esquerda;
    x->esquerda = y->direita;
    if (y->direita != arvore->nulo)
        y->direita->pai = x;

    y->pai = x->pai;
    if (x->pai == arvore->nulo)
        arvore->raiz = y;
    else if (x == x->pai->direita)
        x->pai->direita = y;
    else
        x->pai->esquerda = y;

    y->direita = x;
    x->pai = y;
}
// Busca nó na arvore atraves do ID
No *buscarNo(ArvoreRN *arvore, int id) {
    No *atual = arvore->raiz;
    while (atual != arvore->nulo && id != atual->id) {
        if (id < atual->id)
            atual = atual->esquerda;
        else
            atual = atual->direita;
    }
    return atual;
}

void buscar(ArvoreRN *arvore, int id) {
    No *resultado = buscarNo(arvore, id);
    if (resultado != arvore->nulo) {
        printf("\nFuncionario encontrado: \nID: %d\nNome: %s\nCor: %s\n",
               resultado->id, resultado->nome, (resultado->cor == 1) ? "Vermelho" : "Preto");
    } else {
        printf("Funcionario nao encontrado.\n");
    }
}
//substituir um nó da árvore por outro
void transplantar(ArvoreRN *arvore, No *u, No *v) {
    if (u->pai == arvore->nulo) {
        arvore->raiz = v;
    } else if (u == u->pai->esquerda) {
        u->pai->esquerda = v;
    } else {
        u->pai->direita = v;
    }
    v->pai = u->pai;
}

No *minimo(No *x, No *nulo) {
    while (x->esquerda != nulo) {
        x = x->esquerda;
    }
    return x;
}

void ajustarInsercao(ArvoreRN *arvore, No *novo) {
    while (novo->pai->cor == 1) {  // Enquanto o pai for vermelho
        if (novo->pai == novo->pai->pai->esquerda) {  // Caso o pai seja o filho esquerdo
            No *tio = novo->pai->pai->direita;

            if (tio->cor == 1) {  // Caso o tio seja vermelho
                novo->pai->cor = 0;  // O pai fica preto
                tio->cor = 0;  // O tio fica preto
                novo->pai->pai->cor = 1;  // O avô fica vermelho
                novo = novo->pai->pai;  // Subir para o avô
            } else {  // Caso o tio seja preto
                if (novo == novo->pai->direita) {  // Caso o novo nó seja o filho direito
                    novo = novo->pai;
                    rotacaoEsquerda(arvore, novo);
                }
                novo->pai->cor = 0;  // O pai fica preto
                novo->pai->pai->cor = 1;  // O avô fica vermelho
                rotacaoDireita(arvore, novo->pai->pai);  // Rotação direita
            }
        } else {  // Caso o pai seja o filho direito (simetria)
            No *tio = novo->pai->pai->esquerda;

            if (tio->cor == 1) {
                novo->pai->cor = 0;
                tio->cor = 0;
                novo->pai->pai->cor = 1;
                novo = novo->pai->pai;
            } else {
                if (novo == novo->pai->esquerda) {
                    novo = novo->pai;
                    rotacaoDireita(arvore, novo);
                }
                novo->pai->cor = 0;
                novo->pai->pai->cor = 1;
                rotacaoEsquerda(arvore, novo->pai->pai);
            }
        }
    }
    arvore->raiz->cor = 0;  // A raiz deve ser sempre preta
}


void inserir(ArvoreRN *arvore, int id, char *nome) {
    No *novo = criarNo(arvore, id, nome);
    No *y = arvore->nulo;
    No *x = arvore->raiz;

    while (x != arvore->nulo) {
        y = x;
        if (novo->id < x->id)
            x = x->esquerda;
        else
            x = x->direita;
    }

    novo->pai = y;
    if (y == arvore->nulo)
        arvore->raiz = novo;
    else if (novo->id < y->id)
        y->esquerda = novo;
    else
        y->direita = novo;

    novo->esquerda = novo->direita = arvore->nulo;

    // Balancear após inserção
    ajustarInsercao(arvore, novo);
}

void ajustarRemocao(ArvoreRN *arvore, No *x) {
    while (x != arvore->raiz && x->cor == 0) {
        if (x == x->pai->esquerda) {
            No *w = x->pai->direita;
            if (w->cor == 1) {
                w->cor = 0;
                x->pai->cor = 1;
                rotacaoEsquerda(arvore, x->pai);
                w = x->pai->direita;
            }
            if (w->esquerda->cor == 0 && w->direita->cor == 0) {
                w->cor = 1;
                x = x->pai;
            } else {
                if (w->direita->cor == 0) {
                    w->esquerda->cor = 0;
                    w->cor = 1;
                    rotacaoDireita(arvore, w);
                    w = x->pai->direita;
                }
                w->cor = x->pai->cor;
                x->pai->cor = 0;
                w->direita->cor = 0;
                rotacaoEsquerda(arvore, x->pai);
                x = arvore->raiz;
            }
        } else {
            No *w = x->pai->esquerda;
            if (w->cor == 1) {
                w->cor = 0;
                x->pai->cor = 1;
                rotacaoDireita(arvore, x->pai);
                w = x->pai->esquerda;
            }
            if (w->direita->cor == 0 && w->esquerda->cor == 0) {
                w->cor = 1;
                x = x->pai;
            } else {
                if (w->esquerda->cor == 0) {
                    w->direita->cor = 0;
                    w->cor = 1;
                    rotacaoEsquerda(arvore, w);
                    w = x->pai->esquerda;
                }
                w->cor = x->pai->cor;
                x->pai->cor = 0;
                w->esquerda->cor = 0;
                rotacaoDireita(arvore, x->pai);
                x = arvore->raiz;
            }
        }
    }
    x->cor = 0;
}

void remover(ArvoreRN *arvore, int id) {
    No *z = buscarNo(arvore, id);
    if (z == arvore->nulo) {
        printf("Funcionario nao encontrado.\n");
        return;
    }

    No *y = z, *x;
    int corOriginal = y->cor;
    if (z->esquerda == arvore->nulo) {
        x = z->direita;
        transplantar(arvore, z, z->direita);
    } else if (z->direita == arvore->nulo) {
        x = z->esquerda;
        transplantar(arvore, z, z->esquerda);
    } else {
        y = minimo(z->direita, arvore->nulo);
        corOriginal = y->cor;
        x = y->direita;
        if (y->pai == z)
            x->pai = y;
        else {
            transplantar(arvore, y, y->direita);
            y->direita = z->direita;
            y->direita->pai = y;
        }
        transplantar(arvore, z, y);
        y->esquerda = z->esquerda;
        y->esquerda->pai = y;
        y->cor = z->cor;
    }
    free(z);
    if (corOriginal == 0)
        ajustarRemocao(arvore, x);
}

void imprimirArvoreVisual(No *raiz, No *nulo, int espaco) {
    if (raiz == nulo) 
        return;

    espaco += 10; 

    // Primeiro imprime a subárvore da direita
    imprimirArvoreVisual(raiz->direita, nulo, espaco);

    // Imprime o nó atual com a devida formatação
    printf("\n");
    for (int i = 10; i < espaco; i++)
        printf(" ");
    
    printf("(%d - %s) [%s]\n", raiz->id, raiz->nome, (raiz->cor == 1) ? "Vermelho" : "Preto");

    // Depois imprime a subárvore da esquerda
    imprimirArvoreVisual(raiz->esquerda, nulo, espaco);
}

void liberarArvore(ArvoreRN *arvore, No *no) {
    if (no != arvore->nulo) {
        liberarArvore(arvore, no->esquerda);  // Libera o subárvore esquerda
        liberarArvore(arvore, no->direita);  // Libera o subárvore direita
        free(no);  // Libera o nó atual
    }
}

void destruirArvore(ArvoreRN *arvore) {
    liberarArvore(arvore, arvore->raiz);
    free(arvore->nulo);  // Libera o nó nulo
    free(arvore);  // Libera a estrutura da árvore
}

int main() {
    ArvoreRN arvore;
    inicializarArvore(&arvore);
    int opcao, id;
    char nome[100];

    while (1) {
        printf("\nEscolha uma opcao:\n");
        printf("1- Inserir Funcionario\n2- Remover funcionario\n3- Buscar funcionario\n4- Imprimir\n5- Sair\n");
        scanf("%d", &opcao);
        switch (opcao) {
            case 1:
                printf("ID: ");
                scanf("%d", &id);
                printf("Nome: ");
                scanf("%s", nome);
                inserir(&arvore, id, nome);  // Inserção do funcionário
                break;
            case 2:
                printf("ID para remover: ");
                scanf("%d", &id);
                remover(&arvore, id);  // Remover funcionário
                break;
            case 3:
                printf("ID para buscar: ");
                scanf("%d", &id);
                buscar(&arvore, id);  // Buscar funcionário
                break;
            case 4:
                imprimirArvoreVisual(arvore.raiz, arvore.nulo, 0);  // Imprimir a árvore
                break;
            case 5:
                destruirArvore(&arvore);  // Liberar arvore
                return 0;
            default:
            	printf("Opcao invalida\n");
        }
    }
}

