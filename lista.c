#include <stdio.h>
#include <stdlib.h>
#include "lista.h"


// Cria uma lista vazia.
#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

struct lista_t *lista_cria (){
    struct lista_t *lista;
    if(!(lista = malloc(sizeof(struct lista_t)))) {
        return NULL;
    };
    lista->prim = NULL;
    lista->ult = NULL;
    lista->tamanho = 0;
    return lista;
};

struct lista_t *lista_destroi(struct lista_t *lst) {
    // Verifica a lista é nula
    if (lst == NULL) {
        return NULL;
    }

    struct item_t *aux;

    while (lst->prim != NULL) {
        aux = lst->prim;
        
        // Atualiza o ponteiro prim para o próximo
        lst->prim = lst->prim->prox;

        // Libera o atual
        free(aux);
    }

    //libera a struct lista
    free(lst);
    
    //indica que a lista foi destruída
    return NULL;
}

int lista_insere(struct lista_t *lst, int item, int pos) {
    struct item_t *novo;

    if(lst == NULL) {
        return -1;
    }

    // Tenta alocar memoria para um novo item
    if (!(novo = malloc(sizeof(struct item_t)))) {
        return -1;
    }

    novo->valor = item;
    novo->ant = NULL;
    novo->prox = NULL;

    // Se a lista está vazia
    if (lst->prim == NULL) {
        lst->prim = novo;
        lst->ult = novo;
    }
    else if (pos == 0) { 
        // Insere no início se pos é 0
        novo->prox = lst->prim;
        lst->prim->ant = novo;
        lst->prim = novo;
    }
    else if (pos >= lst->tamanho || pos == -1) {
        // Insere no final se pos é -1 ou maior que o último índice
        novo->ant = lst->ult;
        lst->ult->prox = novo;
        lst->ult = novo;
    }
    else {
        // Insere em uma posição especifica
        struct item_t *atual = lst->prim;
        for (int i = 0; i < pos - 1; i++) {
            atual = atual->prox;
        }
        novo->prox = atual->prox;
        novo->ant = atual;
        if (atual->prox) {
            atual->prox->ant = novo;
        }
        atual->prox = novo;
    }

    lst->tamanho++;
    return lst->tamanho;
};

int lista_retira(struct lista_t *lst, int *item, int pos) {
    // Verifica se a lista está vazia
    if (lst== NULL || item == NULL|| pos >= lst->tamanho) {
        return -1;
    }

    struct item_t *atual;

    lst->tamanho--;
    
    // Remover do início da lista
    if (pos == 0) {
        atual = lst->prim;
        *item = atual->valor;
        lst->prim = atual->prox;
        if (lst->prim) {
            lst->prim->ant = NULL;
        } else {
            lst->ult = NULL;  // Lista fica vazia
        }
        free(atual);
        return lst->tamanho;
    }

    // Remover do final da lista
    if (pos == -1) {
        atual = lst->ult;
        *item = atual->valor;
        lst->ult = atual->ant;
        if (lst->ult) {
            lst->ult->prox = NULL;
        } else {
            lst->prim = NULL;  // Lista fica vazia
        }
        free(atual);
        return lst->tamanho;
    }

    // Remover de uma posição específica
    atual = lst->prim;
    for (int i = 0; i < pos; i++) {
        atual = atual->prox;
    }
    *item = atual->valor;
    atual->ant->prox = atual->prox;
    if (atual->prox) {
        atual->prox->ant = atual->ant;
    }
    free(atual);
    return lst->tamanho;
}

int lista_consulta(struct lista_t *lst, int *item, int pos) {
    if (lst == NULL || item == NULL) {
        return -1;
    };
    struct item_t *a;
    if (pos == 0) {
        a = lst->prim;
    }
    else if (pos == -1) {
        a = lst->ult;
    }
    else if (pos >= lst->tamanho) { //Fiz diferente do que pede nos comentarios da funcao porque os testes davam errado quando executavam 
        return -1;
    }
    else {
        a = lst->prim;
        for (int i = 0; i < pos; i++) {
            a = a->prox;
        }
    }
    *item = a->valor;
    return lst->tamanho;
}

int lista_procura(struct lista_t *lst, int valor) {
    if (lst == NULL) {
        return -1;
    }

    struct item_t *atual = lst->prim;
    int pos = 0;

    // Percorre a lista em busca do valor
    while (atual != NULL) {
        if (atual->valor == valor) {
            return pos; 
        }
        atual = atual->prox;
        pos++;
    }

    // Retorna -1 se o valor não for encontrado
    return -1;
}

int lista_tamanho(struct lista_t *lst) {
    if (lst == NULL) {
        return -1;
    }
    
    return lst->tamanho;
}

void lista_imprime(struct lista_t *lst) {
    // Verifica se a lista está vazia
    if (lst == NULL || lst->prim == NULL) {
        return;
    }

    struct item_t *atual = lst->prim;

    // Imprime o primeiro item
    printf("%d", atual->valor);
    atual = atual->prox;

    // Imprime o resto
    while (atual != NULL) {
        printf(" %d", atual->valor);
        atual = atual->prox;
    }
}