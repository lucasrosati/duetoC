#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define NUM_PALAVRAS 101
#define TAM_PALAVRA 10  // Inclui espaço para o caractere nulo '\0'
#define MAX_TENTATIVAS 6

char palavras[NUM_PALAVRAS][TAM_PALAVRA] = {
    "sagaz", "âmago", "negro", "termo", "êxito", "mexer", "nobre", "senso", "afeto", "algoz",
    "ética", "plena", "fazer", "tênue", "mútua", "assim", "vigor", "sutil", "aquém", "porém",
    "seção", "fosse", "sobre", "poder", "sanar", "ideia", "audaz", "cerne", "inato", "moral",
    "desde", "muito", "justo", "quiçá", "honra", "torpe", "sonho", "razão", "fútil", "etnia",
    "ícone", "amigo", "anexo", "égide", "tange", "dengo", "lapso", "haver", "expor", "tempo",
    "então", "mútuo", "seara", "casal", "boçal", "hábil", "saber", "ávido", "ardil", "pesar",
    "graça", "dizer", "causa", "óbice", "dever", "genro", "coser", "pária", "posse", "tenaz",
    "sendo", "brado", "crivo", "prole", "comum", "ainda", "estar", "xibiu", "temor", "corja",
    "ápice", "detém", "ânimo", "ceder", "assaz", "pauta", "ânsia", "culto", "digno", "fugaz",
    "censo", "mundo", "atroz", "forte", "gleba", "vício", "vulgo", "cozer", "valha", "criar",
    "denso"
};

typedef struct Jogador {
    char nome[50];
    int tentativas;
    struct Jogador* prox;
} Jogador;

Jogador* ranking = NULL;

void jogar();
void adicionarAoRanking(char* nome, int tentativas);
void exibirRanking();
void liberarRanking();
void menu();
char* escolherPalavraSecreta(const char* exclude);
void verificarEImprimirPalavras(char* input, char* secreta1, char* secreta2, bool acertou1, bool acertou2);
void quickSort(Jogador** headRef);
Jogador* quickSortRec(Jogador* head, Jogador* end);
Jogador* particao(Jogador* inicio, Jogador* fim, Jogador** novoInicio, Jogador** novoFim);
Jogador* getUltimo(Jogador* cur);

int main() {
    srand(time(NULL));
    menu();
    liberarRanking();
    return 0;
}

void menu() {
    int opcao;
    do {
        printf("\nMenu:\n");
        printf("1. Jogar\n");
        printf("2. Ver Ranking\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                jogar();
                break;
            case 2:
                exibirRanking();
                break;
            case 3:
                printf("Obrigado por jogar!\n");
                break;
            default:
                printf("Opção inválida, tente novamente.\n");
        }
    } while (opcao != 3);
}

void jogar() {
    char nomeJogador[50];
    printf("Digite seu nome: ");
    fgets(nomeJogador, 50, stdin);
    nomeJogador[strcspn(nomeJogador, "\n")] = 0;

    char palavraSecreta1[TAM_PALAVRA], palavraSecreta2[TAM_PALAVRA];
    strcpy(palavraSecreta1, escolherPalavraSecreta(NULL));
    strcpy(palavraSecreta2, escolherPalavraSecreta(palavraSecreta1));

    char tentativas[MAX_TENTATIVAS][TAM_PALAVRA];
    int numTentativas = 0;
    bool acertou1 = false, acertou2 = false;

    while (numTentativas < MAX_TENTATIVAS && (!acertou1 || !acertou2)) {
        printf("\nDigite uma palavra de 5 letras: ");
        scanf("%s", tentativas[numTentativas]);
        getchar(); // Limpa o buffer após a entrada

        if (strcmp(tentativas[numTentativas], palavraSecreta1) == 0) acertou1 = true;
        if (strcmp(tentativas[numTentativas], palavraSecreta2) == 0) acertou2 = true;

        verificarEImprimirPalavras(tentativas[numTentativas], palavraSecreta1, palavraSecreta2, acertou1, acertou2);
        numTentativas++;
    }

    if (acertou1 && acertou2) {
        printf("\nParabéns, você acertou ambas as palavras: %s e %s\n", palavraSecreta1, palavraSecreta2);
    } else {
        printf("\nVocê não acertou todas as palavras. As palavras eram: %s e %s\n", palavraSecreta1, palavraSecreta2);
    }

    adicionarAoRanking(nomeJogador, numTentativas);
}

char* escolherPalavraSecreta(const char* exclude) {
    int idx;
    do {
        idx = rand() % NUM_PALAVRAS;
    } while (exclude != NULL && strcmp(palavras[idx], exclude) == 0);
    return palavras[idx];
}

void verificarEImprimirPalavras(char* input, char* secreta1, char* secreta2, bool acertou1, bool acertou2) {
    // First, handle the output for the first secret word
    if (!acertou1) { // Only print the feedback if the word hasn't been fully guessed yet
        for (size_t i = 0; i < strlen(input); i++) {
            if (input[i] == secreta1[i]) {
                printf("\x1b[32m%c\x1b[0m", input[i]); // Green for correct position
            } else if (strchr(secreta1, input[i])) {
                printf("\x1b[33m%c\x1b[0m", input[i]); // Yellow for correct letter wrong place
            } else {
                printf("\x1b[31m%c\x1b[0m", input[i]); // Red for incorrect letter
            }
        }
    } else { // If already guessed, print the word correctly in green
        for (size_t i = 0; i < strlen(secreta1); i++) {
            printf("\x1b[32m%c\x1b[0m", secreta1[i]);
        }
    }
    printf(" ");

    // Then, handle the output for the second secret word
    if (!acertou2) {
        for (size_t i = 0; i < strlen(input); i++) {
            if (input[i] == secreta2[i]) {
                printf("\x1b[32m%c\x1b[0m", input[i]);
            } else if (strchr(secreta2, input[i])) {
                printf("\x1b[33m%c\x1b[0m", input[i]);
            } else {
                printf("\x1b[31m%c\x1b[0m", input[i]);
            }
        }
    } else { // If already guessed, print the word correctly in green
        for (size_t i = 0; i < strlen(secreta2); i++) {
            printf("\x1b[32m%c\x1b[0m", secreta2[i]);
        }
    }
    printf("\n");
}



void adicionarAoRanking(char* nome, int tentativas) {
    Jogador* novo = (Jogador*) malloc(sizeof(Jogador));
    strcpy(novo->nome, nome);
    novo->tentativas = tentativas;
    novo->prox = NULL;

    if (ranking == NULL || tentativas < ranking->tentativas) {
        novo->prox = ranking;
        ranking = novo;
    } else {
        Jogador* atual = ranking;
        while (atual->prox != NULL && atual->prox->tentativas <= tentativas) {
            atual = atual->prox;
        }
        novo->prox = atual->prox;
        atual->prox = novo;
    }

    quickSort(&ranking);  // Ordena após adicionar novo jogador
}

Jogador* quickSortRec(Jogador* head, Jogador* end) {
    if (!head || head == end)
        return head;

    Jogador* novoInicio = NULL, *novoFim = NULL;
    Jogador* pivot = particao(head, end, &novoInicio, &novoFim);

    if (novoInicio != pivot) {
        Jogador* tmp = novoInicio;
        while (tmp->prox != pivot) tmp = tmp->prox;
        tmp->prox = NULL;

        novoInicio = quickSortRec(novoInicio, tmp);
        tmp = getUltimo(novoInicio);
        tmp->prox = pivot;
    }

    pivot->prox = quickSortRec(pivot->prox, novoFim);
    return novoInicio;
}

void quickSort(Jogador** headRef) {
    *headRef = quickSortRec(*headRef, getUltimo(*headRef));
}

Jogador* particao(Jogador* inicio, Jogador* fim, Jogador** novoInicio, Jogador** novoFim) {
    Jogador* pivot = fim;
    Jogador* anterior = NULL, *atual = inicio, *cauda = pivot;

    while (atual != pivot) {
        if (atual->tentativas < pivot->tentativas) {
            if ((*novoInicio) == NULL) (*novoInicio) = atual;
            anterior = atual;
            atual = atual->prox;
        } else {
            if (anterior) anterior->prox = atual->prox;
            Jogador* tmp = atual->prox;
            atual->prox = NULL;
            cauda->prox = atual;
            cauda = atual;
            atual = tmp;
        }
    }

    if ((*novoInicio) == NULL) (*novoInicio) = pivot;
    (*novoFim) = cauda;
    return pivot;
}

Jogador* getUltimo(Jogador* cur) {
    while (cur != NULL && cur->prox != NULL) cur = cur->prox;
    return cur;
}

void exibirRanking() {
    if (ranking == NULL) {
        printf("Ainda não há jogadores no ranking.\n");
    } else {
        printf("Ranking:\n");
        Jogador* atual = ranking;
        while (atual != NULL) {
            printf("%s: %d tentativas\n", atual->nome, atual->tentativas);
            atual = atual->prox;
        }
    }
}

void liberarRanking() {
    Jogador* atual = ranking;
    while (atual != NULL) {
        Jogador* temp = atual;
        atual = atual->prox;
        free(temp);
    }
}

