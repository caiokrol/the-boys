#include <stdio.h>
#include <stdlib.h>
#include "lista.h"


// Cria uma lista vazia.
struct lista_t *lista_cria() {
    struct lista_t *lst = (struct lista_t *)malloc(sizeof(struct lista_t));
    if (lst == NULL) {
        return NULL;  // Erro ao alocar memória
    }
    lst->prim = lst->ult = NULL;
    lst->tamanho = 0;
    return lst;
}

// Remove todos os itens da lista e libera a memória.
struct lista_t *lista_destroi(struct lista_t *lst) {
    if (lst == NULL) {
        return NULL;
    }
    struct item_t *atual = lst->prim;
    while (atual != NULL) {
        struct item_t *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(lst);
    return NULL;
}

// Insere o item na lista na posição indicada.
int lista_insere(struct lista_t *lst, int item, int pos) {
    if (lst == NULL) {
        return -1;
    }

    // Criação do novo item
    struct item_t *novo_item = (struct item_t *)malloc(sizeof(struct item_t));
    if (novo_item == NULL) {
        return -1;  // Erro ao alocar memória
    }
    novo_item->valor = item;
    novo_item->ant = novo_item->prox = NULL;

    // Caso especial: lista vazia
    if (lst->tamanho == 0) {
        lst->prim = lst->ult = novo_item;
    } else if (pos <= 0 || pos >= lst->tamanho) { // Inserir no final
        novo_item->ant = lst->ult;
        lst->ult->prox = novo_item;
        lst->ult = novo_item;
    } else { // Inserção no meio da lista
        struct item_t *atual = lst->prim;
        for (int i = 0; i < pos; i++) {
            atual = atual->prox;
        }
        novo_item->prox = atual;
        novo_item->ant = atual->ant;
        if (atual->ant != NULL) {
            atual->ant->prox = novo_item;
        } else {
            lst->prim = novo_item;
        }
        atual->ant = novo_item;
    }
    lst->tamanho++;
    return lst->tamanho;
}

// Retira o item da lista da posição indicada.
int lista_retira(struct lista_t *lst, int *item, int pos) {
    if (lst == NULL || lst->tamanho == 0 || item == NULL) {
        return -1;
    }

    struct item_t *remover;
    if (pos <= 0) {  // Remover do início
        remover = lst->prim;
        lst->prim = remover->prox;
        if (lst->prim != NULL) {
            lst->prim->ant = NULL;
        } else {
            lst->ult = NULL;  // A lista ficou vazia
        }
    } else if (pos >= lst->tamanho - 1 || pos == -1) {  // Remover do fim
        remover = lst->ult;
        lst->ult = remover->ant;
        if (lst->ult != NULL) {
            lst->ult->prox = NULL;
        } else {
            lst->prim = NULL;  // A lista ficou vazia
        }
    } else {  // Remover do meio
        remover = lst->prim;
        for (int i = 0; i < pos; i++) {
            remover = remover->prox;
        }
        remover->ant->prox = remover->prox;
        remover->prox->ant = remover->ant;
    }

    *item = remover->valor;
    free(remover);
    lst->tamanho--;
    return lst->tamanho;
}

// Informa o valor do item na posição indicada, sem retirá-lo.
int lista_consulta(struct lista_t *lst, int *item, int pos) {
    if (lst == NULL || item == NULL || lst->tamanho == 0) {
        return -1;  // Erro: lista nula, item nulo ou lista vazia
    }

    struct item_t *atual;

    // Consulta do fim
    if (pos == -1 || pos >= lst->tamanho - 1) {
        atual = lst->ult;
    } else {  // Consulta em uma posição específica
        atual = lst->prim;
        for (int i = 0; i < pos; i++) {
            atual = atual->prox;
        }
    }

    *item = atual->valor;
    return lst->tamanho;
}

// Informa a posição da 1ª ocorrência do valor indicado na lista.
int lista_procura(struct lista_t *lst, int valor) {
    if (lst == NULL || lst->tamanho == 0) {
        return -1;  // Erro: lista nula ou vazia
    }

    struct item_t *atual = lst->prim;
    int pos = 0;

    while (atual != NULL) {
        if (atual->valor == valor) {
            return pos;  // Encontrou a primeira ocorrência
        }
        atual = atual->prox;
        pos++;
    }

    return -1;  // Valor não encontrado
}

// Informa o tamanho da lista (número de itens presentes nela).
int lista_tamanho(struct lista_t *lst) {
    if (lst == NULL) {
        return -1;  // Erro: lista nula
    }
    return lst->tamanho;
}

// Imprime o conteúdo da lista do início ao fim.
void lista_imprime(struct lista_t *lst) {    
    if (lst == NULL || lst->tamanho == 0) {
        return;  // Lista nula ou vazia, não imprime nada
    }

    struct item_t *atual = lst->prim;
    while (atual != NULL) {
        printf("%d", atual->valor);
        if (atual->prox != NULL) {
            printf(" ");
        }
        atual = atual->prox;
    }
}

int main() {
    // Criação da lista
    struct lista_t *lst = lista_cria();
    if (lst == NULL) {
        printf("Erro ao criar a lista.\n");
        return 1;
    }

    // Inserção de elementos
    printf("Inserindo elementos na lista...\n");
    lista_insere(lst, 10, 0);  // Inserir no início
    lista_insere(lst, 20, -1); // Inserir no final
    lista_insere(lst, 30, 1);  // Inserir na posição 1
    lista_insere(lst, 40, 3);  // Inserir no final
    lista_imprime(lst);
    printf("\nTamanho da lista: %d\n", lista_tamanho(lst));

    // Consulta de elementos
    int valor;
    lista_consulta(lst, &valor, 0);  // Consulta o primeiro elemento
    printf("Elemento na posição 0: %d\n", valor);
    lista_consulta(lst, &valor, -1); // Consulta o último elemento
    printf("Elemento na última posição: %d\n", valor);

    // Procura por um elemento
    int pos = lista_procura(lst, 20);
    if (pos != -1) {
        printf("O valor 20 está na posição: %d\n", pos);
    } else {
        printf("O valor 20 não foi encontrado.\n");
    }

    // Remoção de elementos
    printf("Removendo elemento na posição 1...\n");
    lista_retira(lst, &valor, 1);
    printf("Elemento removido: %d\n", valor);
    lista_imprime(lst);
    printf("\nTamanho da lista após remoção: %d\n", lista_tamanho(lst));

    // Remoção do fim
    printf("Removendo o último elemento...\n");
    lista_retira(lst, &valor, -1);
    printf("Elemento removido: %d\n", valor);
    lista_imprime(lst);
    printf("\nTamanho da lista após remoção: %d\n", lista_tamanho(lst));

    // Destruir a lista
    lst = lista_destroi(lst);
    if (lst == NULL) {
        printf("Lista destruída com sucesso.\n");
    }

    return 0;
}