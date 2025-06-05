#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct aluno {
    int matricula;
    char nome[100];
    struct aluno *esq, *dir;
    int altura;
} Aluno;

//calcular a altura de um nó
int altura(Aluno *nodo) {
    if (nodo == NULL) return 0;
    return nodo->altura;
}

//calcular o fator de balanceamento
int fatorBalanceamento(Aluno *nodo) {
    if (nodo == NULL) return 0;
    return altura(nodo->esq) - altura(nodo->dir);  //altEsq-altDir
}

// Função para criar um novo nó
Aluno* novoAluno(int matricula, char *nome) {
    Aluno* novo = (Aluno*)malloc(sizeof(Aluno));
    novo->matricula = matricula;
    strcpy(novo->nome, nome);
    novo->esq = novo->dir = NULL;
    novo->altura = 1; // Inicializa a altura como 1
    return novo;
}

// Função para rotacionar à direita
Aluno* rotacionarDireita(Aluno *y) {
    Aluno *x = y->esq;  // x é o filho esquerdo de y (vai virar a nova raiz)
    Aluno *T2 = x->dir; // T2 é a subárvore direita de x (vai ser realocada)

    // Realizando a rotação
    x->dir = y; // y desce e se torna o filho direito de x
    y->esq = T2; // A subárvore T2 passa a ser o filho esquerdo de y

    // Atualizando as alturas
    y->altura = 1 + (altura(y->esq) > altura(y->dir) ? altura(y->esq) : altura(y->dir));
    x->altura = 1 + (altura(x->esq) > altura(x->dir) ? altura(x->esq) : altura(x->dir));

    // Retorna o novo nó raiz
    return x;
}

// Função para rotacionar à esquerda
Aluno* rotacionarEsquerda(Aluno *x) {
    Aluno *y = x->dir;
    Aluno *T2 = y->esq;

    // Realizando a rotação
    y->esq = x;
    x->dir = T2;

    // Atualizando as alturas
    x->altura = 1 + (altura(x->esq) > altura(x->dir) ? altura(x->esq) : altura(x->dir));
    y->altura = 1 + (altura(y->esq) > altura(y->dir) ? altura(y->esq) : altura(y->dir));

    // Retorna o novo nó raiz
    return y;
}

// Função para balancear a árvore após a inserção
Aluno* balancear(Aluno *nodo) {
    int balance = fatorBalanceamento(nodo);

    // Caso 1: Rotação à direita
    if (balance > 1 && fatorBalanceamento(nodo->esq) >= 0)
        return rotacionarDireita(nodo);

    // Caso 2: Rotação à esquerda-direita
    if (balance > 1 && fatorBalanceamento(nodo->esq) < 0) {
        nodo->esq = rotacionarEsquerda(nodo->esq);
        return rotacionarDireita(nodo);
    }

    // Caso 3: Rotação à esquerda
    if (balance < -1 && fatorBalanceamento(nodo->dir) <= 0)
        return rotacionarEsquerda(nodo);

    // Caso 4: Rotação à direita-esquerda
    if (balance < -1 && fatorBalanceamento(nodo->dir) > 0) {
        nodo->dir = rotacionarDireita(nodo->dir);
        return rotacionarEsquerda(nodo);
    }

    return nodo;
}

// Função para inserir um aluno
Aluno* inserirAluno(Aluno *nodo, int matricula, char *nome) {
    if (nodo == NULL) return novoAluno(matricula, nome);

    if (matricula < nodo->matricula)
        nodo->esq = inserirAluno(nodo->esq, matricula, nome);
    else if (matricula > nodo->matricula)
        nodo->dir = inserirAluno(nodo->dir, matricula, nome);
    else
        return nodo; // Matrícula já existe

    // Atualizando a altura do nó
    nodo->altura = 1 + (altura(nodo->esq) > altura(nodo->dir) ? altura(nodo->esq) : altura(nodo->dir));

    // Balancear
    return balancear(nodo);
}

Aluno* buscarAluno(Aluno *nodo, int matricula) {
    if (nodo == NULL || nodo->matricula == matricula)
        return nodo;

    if (matricula < nodo->matricula)
        return buscarAluno(nodo->esq, matricula);

    return buscarAluno(nodo->dir, matricula);
}

// Função para encontrar o nó com o valor mínimo
Aluno* minValueNode(Aluno *nodo) {
    Aluno* atual = nodo;
    while (atual->esq != NULL)
        atual = atual->esq;
    return atual;
}

// Função para remover um aluno
Aluno* removerAluno(Aluno *nodo, int matricula) {
    if (nodo == NULL) return nodo;

    if (matricula < nodo->matricula)
        nodo->esq = removerAluno(nodo->esq, matricula);
    else if (matricula > nodo->matricula)
        nodo->dir = removerAluno(nodo->dir, matricula);
    else {
        if (nodo->esq == NULL) {
            Aluno *temp = nodo->dir;
            free(nodo);
            return temp;
        } else if (nodo->dir == NULL) {
            Aluno *temp = nodo->esq;
            free(nodo);
            return temp;
        }

        Aluno *temp = minValueNode(nodo->dir);
        nodo->matricula = temp->matricula;
        strcpy(nodo->nome, temp->nome);
        nodo->dir = removerAluno(nodo->dir, temp->matricula);
    }

    if (nodo == NULL) return nodo;

    nodo->altura = 1 + (altura(nodo->esq) > altura(nodo->dir) ? altura(nodo->esq) : altura(nodo->dir));

    return balancear(nodo);
}

void imprimirEmArvore(Aluno *nodo, int espacos) {
    if (nodo == NULL) return;

    espacos += 10;

    imprimirEmArvore(nodo->dir, espacos);

    printf("\n");
    for (int i = 10; i < espacos; i++) printf(" ");
    printf("%d - %s\n", nodo->matricula, nodo->nome);

    imprimirEmArvore(nodo->esq, espacos);
}

int main() {
    Aluno *raiz = NULL;
    int matricula;
    char nome[100];
    int opcao;

    do {
        printf("\nMenu:\n");
        printf("1. Inserir Aluno\n");
        printf("2. Buscar Aluno\n");
        printf("3. Remover Aluno\n");
        printf("4. Imprimir Arvore AVL\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1: {
                printf("\nDigite o numero da matricula: ");
                scanf("%d", &matricula);
                printf("Digite o nome do aluno: ");
                getchar(); 
                fgets(nome, 100, stdin);
                nome[strcspn(nome, "\n")] = '\0'; 

                raiz = inserirAluno(raiz, matricula, nome);
                printf("Aluno inserido com sucesso!\n");
                break;
            }
            case 2: {
                printf("\nDigite o numero da matricula para buscar: ");
                scanf("%d", &matricula);

                Aluno *aluno = buscarAluno(raiz, matricula);
                if (aluno != NULL)
                    printf("\nAluno encontrado: %s\n", aluno->nome);
                else
                    printf("\nAluno nao encontrado.\n");
                break;
            }
            case 3: {
                printf("\nDigite o numero da matricula para remover: ");
                scanf("%d", &matricula);

                raiz = removerAluno(raiz, matricula);
                printf("Aluno removido com sucesso!\n");
                break;
            }
            case 4: {
                printf("\nArvore AVL:\n");
                imprimirEmArvore(raiz, 0);
                break;
            }
            case 5:
                printf("\nSaindo...\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
        }
    } while(opcao != 5);

    return 0;
}

