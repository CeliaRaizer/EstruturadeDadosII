#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T 3 // Ordem da Árvore B

typedef struct Produto {
    int codigo;
    char nome[50];
} Produto;

typedef struct NoB {
    int numChaves;
    Produto chaves[2*T - 1];
    struct NoB *filhos[2*T];
    int folha;
} NoB;

NoB* criarNo(int folha) {
    NoB* novoNo = (NoB*) malloc(sizeof(NoB));
    if (!novoNo) {
        printf("Erro de alocacao de memoria.\n");
        exit(1);
    }
    novoNo->numChaves = 0;
    novoNo->folha = folha;
    for (int i = 0; i < 2*T; i++)
        novoNo->filhos[i] = NULL;
    return novoNo;
}

Produto* buscarProduto(NoB* raiz, int codigo) {
    if (raiz == NULL) return NULL;

    int i = 0;
    while (i < raiz->numChaves && codigo > raiz->chaves[i].codigo)
        i++;

    if (i < raiz->numChaves && codigo == raiz->chaves[i].codigo)
        return &raiz->chaves[i];

    if (raiz->folha)
        return NULL;

    return buscarProduto(raiz->filhos[i], codigo);
}


void dividirFilho(NoB* pai, int i, NoB* filho);
void inserirNaoCheio(NoB* no, Produto produto);
void removerProduto(NoB** raiz, int codigo);
void removerDeNo(NoB* no, int codigo);
void removerDeNoInterno(NoB* no, int idx);
void ajustarNo(NoB* pai, int idx);
void fundirNos(NoB* pai, int idx);

void inserirProduto(NoB** raiz, Produto produto) {
    NoB* r = *raiz;
    if (r->numChaves == 2*T - 1) {
        NoB* s = criarNo(0);
        s->filhos[0] = r;
        *raiz = s;
        dividirFilho(s, 0, r);
        inserirNaoCheio(s, produto);
    } else {
        inserirNaoCheio(r, produto);
    }
}

//Insere um produto em um nó que ainda não está cheio.
void inserirNaoCheio(NoB* no, Produto produto) {
    int i = no->numChaves - 1;

    if (no->folha) {
        while (i >= 0 && produto.codigo < no->chaves[i].codigo) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = produto;
        no->numChaves++;
    } else {
        while (i >= 0 && produto.codigo < no->chaves[i].codigo)
            i--;
        i++;
        if (no->filhos[i]->numChaves == 2*T - 1) {
            dividirFilho(no, i, no->filhos[i]);
            if (produto.codigo > no->chaves[i].codigo)
                i++;
        }
        inserirNaoCheio(no->filhos[i], produto);
    }
}

//Quando um nó está cheio, ele é dividido, e o valor do meio sobe para o pai.
void dividirFilho(NoB* pai, int i, NoB* filho) {
    NoB* novoNo = criarNo(filho->folha);
    novoNo->numChaves = T - 1;
    
    for (int j = 0; j < T - 1; j++)
        novoNo->chaves[j] = filho->chaves[j + T];

    if (!filho->folha) {
        for (int j = 0; j < T; j++)
            novoNo->filhos[j] = filho->filhos[j + T];
    }

    filho->numChaves = T - 1;

    for (int j = pai->numChaves; j >= i + 1; j--)
        pai->filhos[j + 1] = pai->filhos[j];

    pai->filhos[i + 1] = novoNo;

    for (int j = pai->numChaves - 1; j >= i; j--)
        pai->chaves[j + 1] = pai->chaves[j];

    pai->chaves[i] = filho->chaves[T - 1];
    pai->numChaves++;
}

void removerProduto(NoB** raiz, int codigo) {
    if (*raiz == NULL) return;

    removerDeNo(*raiz, codigo);

    // Se a raiz ficar vazia, ajustamos a árvore
    if ((*raiz)->numChaves == 0) {
        NoB* temp = *raiz;
        if ((*raiz)->folha) {
            *raiz = NULL;
        } else {
            *raiz = (*raiz)->filhos[0]; // Promove o primeiro filho como nova raiz
        }
        free(temp);
    }
    
}

//Remove a chave com o código fornecido de um determinado nó da árvore
void removerDeNo(NoB* no, int codigo) {
    int i = 0;
    while (i < no->numChaves && codigo > no->chaves[i].codigo)
        i++;

    // Caso 1: A chave está no nó atual
    if (i < no->numChaves && no->chaves[i].codigo == codigo) {
        if (no->folha) {
            // Simplesmente remover a chave em um nó folha
            for (; i < no->numChaves - 1; i++)
                no->chaves[i] = no->chaves[i + 1];
            no->numChaves--;
        } else {
            // Caso 2: A chave está em um nó interno
            removerDeNoInterno(no, i);
        }
    } else {
        // Caso 3: A chave não está neste nó (recursivamente remover do filho)
        if (no->folha) {
            printf("Chave %d nao encontrada!\n", codigo);
            return;
        }

        int filhoIndex = i;
        if (no->filhos[filhoIndex]->numChaves < T) {
            ajustarNo(no, filhoIndex);
        }

        removerDeNo(no->filhos[filhoIndex], codigo);
    }
}

//Remover uma chave de nó interno (não folha), mantendo as propriedades da Árvore B.
void removerDeNoInterno(NoB* no, int idx) {
    int codigo = no->chaves[idx].codigo;

    if (no->filhos[idx]->numChaves >= T) {
        // Substituir pelo predecessor
        NoB* pred = no->filhos[idx];
        while (!pred->folha)
            pred = pred->filhos[pred->numChaves];

        no->chaves[idx] = pred->chaves[pred->numChaves - 1];
        removerDeNo(no->filhos[idx], pred->chaves[pred->numChaves - 1].codigo);
    } else if (no->filhos[idx + 1]->numChaves >= T) {
        // Substituir pelo sucessor
        NoB* succ = no->filhos[idx + 1];
        while (!succ->folha)
            succ = succ->filhos[0];

        no->chaves[idx] = succ->chaves[0];
        removerDeNo(no->filhos[idx + 1], succ->chaves[0].codigo);
    } else {
        // Fundir os filhos e remover a chave do pai
        fundirNos(no, idx);
        removerDeNo(no->filhos[idx], codigo);
    }
}

//Ajustar o nó antes da remoção
void ajustarNo(NoB* pai, int idx) {
    NoB* filho = pai->filhos[idx];

    // Se houver um irmão esquerdo com pelo menos T chaves
    if (idx > 0 && pai->filhos[idx - 1]->numChaves >= T) {
        NoB* irmao = pai->filhos[idx - 1];

        // Puxa uma chave do pai para o filho
        for (int i = filho->numChaves; i > 0; i--)
            filho->chaves[i] = filho->chaves[i - 1];

        filho->chaves[0] = pai->chaves[idx - 1];
        if (!filho->folha) {
            for (int i = filho->numChaves + 1; i > 0; i--)
                filho->filhos[i] = filho->filhos[i - 1];

            filho->filhos[0] = irmao->filhos[irmao->numChaves];
        }

        pai->chaves[idx - 1] = irmao->chaves[irmao->numChaves - 1];
        irmao->numChaves--;
        filho->numChaves++;
    }
    // Se houver um irmão direito com pelo menos T chaves
    else if (idx < pai->numChaves && pai->filhos[idx + 1]->numChaves >= T) {
        NoB* irmao = pai->filhos[idx + 1];

        filho->chaves[filho->numChaves] = pai->chaves[idx];
        if (!filho->folha)
            filho->filhos[filho->numChaves + 1] = irmao->filhos[0];

        pai->chaves[idx] = irmao->chaves[0];

        for (int i = 0; i < irmao->numChaves - 1; i++)
            irmao->chaves[i] = irmao->chaves[i + 1];

        if (!irmao->folha) {
            for (int i = 0; i < irmao->numChaves; i++)
                irmao->filhos[i] = irmao->filhos[i + 1];
        }

        irmao->numChaves--;
        filho->numChaves++;
    }
    // Caso contrário, funde o nó com um irmão
    else {
        if (idx < pai->numChaves)
            fundirNos(pai, idx);
        else
            fundirNos(pai, idx - 1);
    }
}

//Fusão de dois nós filhos
void fundirNos(NoB* pai, int idx) {
    NoB* filho = pai->filhos[idx];
    NoB* irmao = pai->filhos[idx + 1];

    filho->chaves[T - 1] = pai->chaves[idx];

    for (int i = 0; i < irmao->numChaves; i++)
        filho->chaves[i + T] = irmao->chaves[i];

    if (!filho->folha) {
        for (int i = 0; i <= irmao->numChaves; i++)
            filho->filhos[i + T] = irmao->filhos[i];
    }

    for (int i = idx; i < pai->numChaves - 1; i++)
        pai->chaves[i] = pai->chaves[i + 1];

    for (int i = idx + 1; i < pai->numChaves; i++)
        pai->filhos[i] = pai->filhos[i + 1];

    filho->numChaves += irmao->numChaves + 1;
    pai->numChaves--;

    free(irmao);
}

void imprimirArvore(NoB* raiz) {

    if (raiz == NULL) {
        printf("Árvore vazia.\n");
        return;
    }
    
    NoB* fila[100];
    int frente = 0, fim = 0;
    
    fila[fim++] = raiz;
    
    while (frente < fim) {
        int nivelSize = fim - frente;
        while (nivelSize--) {
            NoB* no = fila[frente++];
            printf("[");
            for (int i = 0; i < no->numChaves; i++) {
                printf("%d ", no->chaves[i].codigo);
            }
            printf("] ");
            for (int i = 0; i <= no->numChaves; i++) {
                if (no->filhos[i] != NULL)
                    fila[fim++] = no->filhos[i];
            }
        }
        printf("\n");
    }
}

int main() {
    NoB* raiz = criarNo(1);

    // Inserção de 30 produtos
     Produto produtos[30] = {
        {10, "Arroz"}, {15, "Feijao"}, {20, "Acucar"}, {25, "Sal"}, {30, "Oleo"},
        {35, "Cafe"}, {40, "Leite"}, {45, "Manteiga"}, {50, "Farinha"}, {55, "Macarrao"},
        {60, "Biscoito"}, {65, "Refrigerante"}, {70, "Suco"}, {75, "Chocolate"}, {80, "Carne"},
        {85, "Frango"}, {90, "Peixe"}, {95, "Batata"}, {100, "Cebola"}, {105, "Alho"},
        {110, "Tomate"}, {115, "Cenoura"}, {120, "Laranja"}, {125, "Banana"}, {130, "Maca"},
        {135, "Pera"}, {140, "Uva"}, {145, "Melancia"}, {150, "Morango"}, {155, "Abacaxi"}
    };

    for (int i = 0; i < 30; i++) {
        inserirProduto(&raiz, produtos[i]);
    }

    int opcao;
    Produto p;
    int codigo;

    do {
        printf("\nMenu:\n1. Inserir Produto\n2. Remover Produto\n3. Buscar Produto\n4. Imprimir Arvore\n5. Sair\nEscolha: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                printf("Codigo: ");
                scanf("%d", &p.codigo);
                printf("Nome: ");
                scanf("%49s", p.nome);
                inserirProduto(&raiz, p);
                break;
            case 2:
                printf("Codigo a remover: ");
                scanf("%d", &codigo);
                removerProduto(&raiz, codigo);
                break;
           case 3: {
			    printf("Codigo a buscar: ");
			    scanf("%d", &codigo);
			    Produto* encontrado = buscarProduto(raiz, codigo);
			    if (encontrado)
			        printf("Produto encontrado: %d - %s\n", encontrado->codigo, encontrado->nome);
			    else
			        printf("Produto nao encontrado.\n");
			    break;
			}
            case 4:
                imprimirArvore(raiz);
                break;
        }
    } while (opcao != 5);

    return 0;
}

