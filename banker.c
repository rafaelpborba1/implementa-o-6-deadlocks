#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int total_clientes = 0;
int total_recursos = 0;
int *disponivel = NULL;
int **maximo = NULL;
int **alocado = NULL;
int **necessidade = NULL;

void limpar_tudo() {
    for (int c = 0; c < total_clientes; c++) {
        free(maximo[c]);
        free(alocado[c]);
        free(necessidade[c]);
    }
    free(maximo);
    free(alocado);
    free(necessidade);
    free(disponivel);
}

int carregar_clientes() {
    FILE *arq = fopen("customer.txt", "r");
    if (!arq) {
        fprintf(stderr, "Erro ao abrir customer.txt\n");
        return 0;
    }

    char buf[512];
    while (fgets(buf, sizeof(buf), arq)) {
        int tem_dado = 0;
        for (int k = 0; buf[k] != '\0'; k++) {
            if (buf[k] != ' ' && buf[k] != '\t' && buf[k] != '\n' && buf[k] != '\r') {
                tem_dado = 1;
                break;
            }
        }
        if (tem_dado) total_clientes++;
    }

    if (total_clientes == 0) {
        fprintf(stderr, "Arquivo customer.txt vazio\n");
        fclose(arq);
        return 0;
    }

    disponivel = calloc(total_recursos, sizeof(int));
    maximo = malloc(total_clientes * sizeof(int *));
    alocado = malloc(total_clientes * sizeof(int *));
    necessidade = malloc(total_clientes * sizeof(int *));

    for (int c = 0; c < total_clientes; c++) {
        maximo[c] = calloc(total_recursos, sizeof(int));
        alocado[c] = calloc(total_recursos, sizeof(int));
        necessidade[c] = calloc(total_recursos, sizeof(int));
    }

    rewind(arq);
    int c = 0;
    while (fgets(buf, sizeof(buf), arq)) {
        int r = 0;
        char *parte = strtok(buf, ",");
        int leu_algo = 0;

        while (parte != NULL) {
            int vazio = 1;
            for (int k = 0; parte[k] != '\0'; k++) {
                if (parte[k] != ' ' && parte[k] != '\t' && parte[k] != '\n' && parte[k] != '\r') {
                    vazio = 0;
                    break;
                }
            }
            if (vazio && r == 0) break;

            leu_algo = 1;
            int valor;
            char sobra;
            if (r >= total_recursos || sscanf(parte, " %d %c", &valor, &sobra) != 1 || valor < 0) {
                fprintf(stderr, "Erro de formatacao em customer.txt\n");
                fclose(arq);
                limpar_tudo();
                return 0;
            }
            maximo[c][r] = valor;
            necessidade[c][r] = valor;
            r++;
            parte = strtok(NULL, ",");
        }

        if (!leu_algo) continue;

        if (r != total_recursos) {
            fprintf(stderr, "Quantidade de recursos invalida em customer.txt\n");
            fclose(arq);
            limpar_tudo();
            return 0;
        }
        c++;
    }

    fclose(arq);
    return 1;
}

int verificar_seguranca() {
    int *trabalho = malloc(total_recursos * sizeof(int));
    int *fim = calloc(total_clientes, sizeof(int));

    for (int r = 0; r < total_recursos; r++) {
        trabalho[r] = disponivel[r];
    }

    for (int c = 0; c < total_clientes; c++) {
        if (fim[c]) continue;

        int atende = 1;
        for (int r = 0; r < total_recursos; r++) {
            if (necessidade[c][r] > trabalho[r]) {
                atende = 0;
                break;
            }
        }

        if (atende) {
            for (int r = 0; r < total_recursos; r++) {
                trabalho[r] += alocado[c][r];
            }
            fim[c] = 1;
            c = -1;
        }
    }

    int seguro = 1;
    for (int c = 0; c < total_clientes; c++) {
        if (!fim[c]) {
            seguro = 0;
            break;
        }
    }

    free(trabalho);
    free(fim);
    return seguro;
}

void formatar_vetor(int *vet, char *destino) {
    destino[0] = '\0';
    char temp[32];
    for (int r = 0; r < total_recursos; r++) {
        sprintf(temp, "%d", vet[r]);
        strcat(destino, temp);
        if (r < total_recursos - 1) {
            strcat(destino, " ");
        }
    }
}

void imprimir_estado(FILE *saida) {
    fprintf(saida, "MAXIMUM   |   ALLOCATION  |   NEED\n");
    printf("MAXIMUM   |   ALLOCATION  |   NEED\n");

    char str_max[256], str_aloc[256], str_nec[256], str_disp[256];

    for (int c = 0; c < total_clientes; c++) {
        formatar_vetor(maximo[c], str_max);
        formatar_vetor(alocado[c], str_aloc);
        formatar_vetor(necessidade[c], str_nec);

        fprintf(saida, "%-10s|   %-12s|   %s \n", str_max, str_aloc, str_nec);
        printf("%-10s|   %-12s|   %s \n", str_max, str_aloc, str_nec);
    }

    formatar_vetor(disponivel, str_disp);
    fprintf(saida, "AVAILABLE %s \n", str_disp);
    printf("AVAILABLE %s \n", str_disp);
}

void tratar_request(int cliente, int *pedido, FILE *saida) {
    char str_pedido[256], str_disp[256];
    formatar_vetor(pedido, str_pedido);

    for (int r = 0; r < total_recursos; r++) {
        if (pedido[r] > necessidade[cliente][r]) {
            fprintf(saida, "The customer %d request %s was denied because exceed its maximum allocation\n", cliente, str_pedido);
            printf("The customer %d request %s was denied because exceed its maximum allocation\n", cliente, str_pedido);
            return;
        }
    }

    for (int r = 0; r < total_recursos; r++) {
        if (pedido[r] > disponivel[r]) {
            formatar_vetor(disponivel, str_disp);
            fprintf(saida, "The resources %s are not enough to customer %d request %s \n", str_disp, cliente, str_pedido);
            printf("The resources %s are not enough to customer %d request %s \n", str_disp, cliente, str_pedido);
            return;
        }
    }

    for (int r = 0; r < total_recursos; r++) {
        disponivel[r] -= pedido[r];
        alocado[cliente][r] += pedido[r];
        necessidade[cliente][r] -= pedido[r];
    }

    if (verificar_seguranca()) {
        fprintf(saida, "Allocate to customer %d the resources %s\n", cliente, str_pedido);
        printf("Allocate to customer %d the resources %s\n", cliente, str_pedido);
    } else {
        for (int r = 0; r < total_recursos; r++) {
            disponivel[r] += pedido[r];
            alocado[cliente][r] -= pedido[r];
            necessidade[cliente][r] += pedido[r];
        }
        fprintf(saida, "The customer %d request %s was denied because result in an unsafe state\n", cliente, str_pedido);
        printf("The customer %d request %s was denied because result in an unsafe state\n", cliente, str_pedido);
    }
}

void tratar_release(int cliente, int *libera, FILE *saida) {
    char str_libera[256];
    formatar_vetor(libera, str_libera);

    for (int r = 0; r < total_recursos; r++) {
        if (libera[r] > alocado[cliente][r]) {
            fprintf(saida, "The customer %d cannot release %s because exceed its allocation\n", cliente, str_libera);
            printf("The customer %d cannot release %s because exceed its allocation\n", cliente, str_libera);
            return;
        }
    }

    for (int r = 0; r < total_recursos; r++) {
        alocado[cliente][r] -= libera[r];
        disponivel[r] += libera[r];
        necessidade[cliente][r] += libera[r];
    }

    fprintf(saida, "Release from customer %d the resources %s\n", cliente, str_libera);
    printf("Release from customer %d the resources %s\n", cliente, str_libera);
}

int processar_comandos() {
    FILE *arq = fopen("commands.txt", "r");
    if (!arq) {
        fprintf(stderr, "Erro ao abrir commands.txt\n");
        return 0;
    }

    FILE *saida = fopen("result.txt", "w");
    if (!saida) {
        fprintf(stderr, "Erro ao criar result.txt\n");
        fclose(arq);
        return 0;
    }

    int *valores = malloc(total_recursos * sizeof(int));
    char linha[512];

    while (fgets(linha, sizeof(linha), arq)) {
        char *op = strtok(linha, " \t\r\n");
        if (!op) continue;

        if (strcmp(op, "*") == 0) {
            imprimir_estado(saida);
        } else if (strcmp(op, "RQ") == 0 || strcmp(op, "RL") == 0) {
            int cliente = atoi(strtok(NULL, " \t\r\n"));
            for (int r = 0; r < total_recursos; r++) {
                valores[r] = atoi(strtok(NULL, " \t\r\n"));
            }

            if (strcmp(op, "RQ") == 0) {
                tratar_request(cliente, valores, saida);
            } else {
                tratar_release(cliente, valores, saida);
            }
        }
    }

    free(valores);
    fclose(arq);
    fclose(saida);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <r0> <r1> ...\n", argv[0]);
        return 1;
    }

    total_recursos = argc - 1;

    if (!carregar_clientes()) {
        return 1;
    }

    for (int r = 0; r < total_recursos; r++) {
        char sobra;
        if (sscanf(argv[r + 1], " %d %c", &disponivel[r], &sobra) != 1 || disponivel[r] < 0) {
            fprintf(stderr, "Argumento invalido: %s\n", argv[r + 1]);
            limpar_tudo();
            return 1;
        }
    }

    if (!processar_comandos()) {
        limpar_tudo();
        return 1;
    }

    limpar_tudo();
    return 0;
}