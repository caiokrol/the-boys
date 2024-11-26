#include <stdio.h>
#include <stdlib.h>
#include "fprio.h"
// A COMPLETAR

struct  fprio_t *fprio_cria()
{
 // Aloca memória para a estrutura da fila de prioridades
    struct fprio_t *fila = (struct fprio_t *)malloc(sizeof(struct fprio_t));
    if (fila == NULL) {
        // Retorna NULL se a alocação de memória falhar
        return NULL;
    }

    // Inicializa os campos da estrutura
    fila->prim = NULL;  // A fila começa vazia, sem nós
    fila->num = 0;      // Número de itens é zero

    return fila;        // Retorna o ponteiro para a fila criada
}

struct fprio_t *fprio_destroi(struct fprio_t *f) {
    if (f == NULL) {
        return NULL;  // Retorna NULL se a fila já for NULL
    }

    // Ponteiro auxiliar para iterar pelos nós
    struct fpnodo_t *atual = f->prim;
    while (atual != NULL) {
        struct fpnodo_t *prox = atual->prox;  // Armazena o próximo nodo
        free(atual->item);  // Libera o item associado, se necessário
        free(atual);        // Libera o nodo
        atual = prox;       // Move para o próximo nodo
    }

    // Libera a estrutura da fila
    free(f);
    
    return NULL;  // Retorna NULL conforme especificado
}


int fprio_insere(struct fprio_t *f, void *item, int tipo, int prio) {
    if (f == NULL || item == NULL) {
        return -1;  // Erro se a fila ou o item for NULL
    }

    // Verifica se o item já existe na fila
    struct fpnodo_t *atual = f->prim;
    while (atual != NULL) {
        if (atual->item == item) {
            // Erro: o mesmo item (ponteiro) já existe na fila
            return -1;
        }
        atual = atual->prox;
    }

    // Cria um novo nodo
    struct fpnodo_t *novo = (struct fpnodo_t *)malloc(sizeof(struct fpnodo_t));
    if (novo == NULL) {
        return -1;  // Erro se não conseguir alocar memória para o novo nodo
    }

    // Inicializa o novo nodo
    novo->item = item;
    novo->tipo = tipo;
    novo->prio = prio;
    novo->prox = NULL;

    // Caso especial: a fila está vazia ou o novo nodo tem a menor prioridade
    if (f->prim == NULL || prio < f->prim->prio) {
        novo->prox = f->prim;  // O novo nodo se torna o primeiro
        f->prim = novo;
    } else {
        // Percorre a fila para encontrar a posição correta
        atual = f->prim;
        while (atual->prox != NULL && atual->prox->prio <= prio) {
            atual = atual->prox;
        }

        // Insere o novo nodo na posição correta
        novo->prox = atual->prox;
        atual->prox = novo;
    }

    // Incrementa o número de itens na fila
    f->num++;

    return f->num;  // Retorna o número de itens na fila
}

void *fprio_retira(struct fprio_t *f, int *tipo, int *prio) {
    if (f == NULL || f->prim == NULL || tipo == NULL || prio == NULL) {
        return NULL;  // Retorna NULL se a fila for NULL, vazia ou se tipo for NULL
    }

    // Ponteiro para o primeiro nodo da fila
    struct fpnodo_t *primeiro = f->prim;

    // Atualiza o tipo e a prioridade
    *tipo = primeiro->tipo;
    if (prio != NULL) {
        *prio = primeiro->prio;
    }

    // Obtém o item e remove o nodo da fila
    void *item = primeiro->item;
    f->prim = primeiro->prox;
    free(primeiro);
    f->num--;

    return item;
}
int fprio_tamanho(struct fprio_t *f) {
    if (f == NULL) {
        return -1;  // Retorna -1 se a fila for NULL (erro)
    }
    
    return f->num;  // Retorna o número de itens na fila
}

void fprio_imprime(struct fprio_t *f) {
    if (f == NULL || f->prim == NULL) {
        return;  // Não imprime nada se a fila for NULL ou estiver vazia
    }

    struct fpnodo_t *atual = f->prim;
    int primeiro = 1;  // Flag para controlar o espaço entre os itens

    while (atual != NULL) {
        if (!primeiro) {
            printf(" ");  // Imprime espaço antes dos itens seguintes
        }
        printf("(%d %d)", atual->tipo, atual->prio);
        primeiro = 0;  // Depois do primeiro item, o flag é desativado
        atual = atual->prox;  // Avança para o próximo nodo
    }
}