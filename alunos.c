#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // para ftruncate() e fileno()

// ---------------- PROTÓTIPOS ----------------
void limpaBuffer();
void lerString(char* dest, int tamanho);
long tamanho(FILE* arq);
void cadastrar(FILE* arq);
void consultar(FILE* arq);
void Excluir(FILE* arq);
void MostrarTodos(FILE* arq);
void GerarRelatorioTXT(FILE* arq);

// --------------- STRUCT PRINCIPAL ---------------
typedef struct {
    char nome[50];
    int matricula;
    float nota;
} Aluno;

// ================================================================
// Função principal
// ================================================================
int main() {
    FILE* arq = fopen("alunos.dat", "r+b");

    if (arq == NULL) {
        arq = fopen("alunos.dat", "w+b");
        if (arq == NULL) {
            printf("Erro ao criar o arquivo!\n");
            return 1;
        }
    }

    int opcao;

    do {
        printf("\n==== SISTEMA DE NOTAS ====\n");
        printf("1 - Cadastrar aluno\n");
        printf("2 - Consultar aluno por índice\n");
        printf("3 - Mostrar quantidade de registros\n");
        printf("4 - Excluir aluno por índice\n");
        printf("5 - Mostrar todos os registros\n");
        printf("6 - Gerar relatório em arquivo texto\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        limpaBuffer();

        switch (opcao) {
        case 1:
            cadastrar(arq);
            break;
        case 2:
            consultar(arq);
            break;
        case 3:
            printf("Total de registros: %ld\n", tamanho(arq));
            break;
        case 4:
            Excluir(arq);
            break;
        case 5:
            MostrarTodos(arq);
            break;
        case 6:
            GerarRelatorioTXT(arq);
            break;
        case 0:
            printf("Encerrando...\n");
            break;
        default:
            printf("Opção inválida!\n");
        }

    } while (opcao != 0);

    fclose(arq);
    return 0;
}

// ================================================================
// Funções Auxiliares
// ================================================================
void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerString(char* dest, int tamanho) {
    fgets(dest, tamanho, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}

// ================================================================
// Funções do Sistema
// ================================================================

// Retorna número total de registros
long tamanho(FILE* arq) {
    fseek(arq, 0, SEEK_END);
    long bytes = ftell(arq);
    return bytes / sizeof(Aluno);
}

// Cadastrar novo aluno
void cadastrar(FILE* arq) {
    Aluno a;

    printf("\n--- Cadastro de Aluno ---\n");
    printf("Nome: ");
    lerString(a.nome, 50);

    printf("Matrícula: ");
    scanf("%d", &a.matricula);
    limpaBuffer();

    printf("Nota: ");
    scanf("%f", &a.nota);
    limpaBuffer();

    fseek(arq, 0, SEEK_END);
    fwrite(&a, sizeof(Aluno), 1, arq);

    printf("Aluno cadastrado com sucesso!\n");
}

// Consultar aluno pelo índice
void consultar(FILE* arq) {
    long total = tamanho(arq);
    long indice;

    if (total == 0) {
        printf("Nenhum registro encontrado!\n");
        return;
    }

    printf("Digite o índice (0 a %ld): ", total - 1);
    scanf("%ld", &indice);
    limpaBuffer();

    if (indice < 0 || indice >= total) {
        printf("Índice inválido!\n");
        return;
    }

    Aluno a;
    fseek(arq, indice * sizeof(Aluno), SEEK_SET);
    fread(&a, sizeof(Aluno), 1, arq);

    printf("\n--- Dados do Aluno ---\n");
    printf("Nome: %s\n", a.nome);
    printf("Matrícula: %d\n", a.matricula);
    printf("Nota: %.2f\n", a.nota);
}

// Excluir registro
void Excluir(FILE* arq) {
    long total = tamanho(arq);
    long indice;

    if (total == 0) {
        printf("Nenhum registro encontrado!\n");
        return;
    }

    printf("Digite o índice para excluir (0 a %ld): ", total - 1);
    scanf("%ld", &indice);
    limpaBuffer();

    if (indice < 0 || indice >= total) {
        printf("Índice inválido!\n");
        return;
    }

    // deslocar registros posteriores para trás
    for (long i = indice; i < total - 1; i++) {
        Aluno prox;

        fseek(arq, (i + 1) * sizeof(Aluno), SEEK_SET);
        fread(&prox, sizeof(Aluno), 1, arq);

        fseek(arq, i * sizeof(Aluno), SEEK_SET);
        fwrite(&prox, sizeof(Aluno), 1, arq);
    }

    // reduzir tamanho do arquivo
    long novoTamanho = (total - 1) * sizeof(Aluno);
    if (ftruncate(fileno(arq), novoTamanho) != 0) {
        printf("Erro ao reduzir o arquivo!\n");
        return;
    }

    printf("Registro excluído com sucesso!\n");
}

// Mostrar todos os registros
void MostrarTodos(FILE* arq) {
    long total = tamanho(arq);

    if (total == 0) {
        printf("Nenhum registro encontrado!\n");
        return;
    }

    Aluno a;

    printf("\n===== LISTA COMPLETA DE ALUNOS =====\n");

    for (long i = 0; i < total; i++) {
        fseek(arq, i * sizeof(Aluno), SEEK_SET);
        fread(&a, sizeof(Aluno), 1, arq);

        printf("\n[Índice %ld]\n", i);
        printf("Nome: %s\n", a.nome);
        printf("Matrícula: %d\n", a.matricula);
        printf("Nota: %.2f\n", a.nota);
    }

    printf("\n========= Fim da Lista =========\n");
}

// Gerar relatório em TXT
void GerarRelatorioTXT(FILE* arq) {
    long total = tamanho(arq);

    if (total == 0) {
        printf("Nenhum registro encontrado! Nada para exportar.\n");
        return;
    }

    FILE* txt = fopen("alunos.txt", "w");
    if (txt == NULL) {
        printf("Erro ao criar arquivo TXT!\n");
        return;
    }

    Aluno a;

    fprintf(txt, "RELATORIO DE ALUNOS\n");
    fprintf(txt, "====================\n\n");

    for (long i = 0; i < total; i++) {
        fseek(arq, i * sizeof(Aluno), SEEK_SET);
        fread(&a, sizeof(Aluno), 1, arq);

        fprintf(txt, "Índice: %ld\n", i);
        fprintf(txt, "Nome: %s\n", a.nome);
        fprintf(txt, "Matrícula: %d\n", a.matricula);
        fprintf(txt, "Nota: %.2f\n", a.nota);
        fprintf(txt, "---------------------------\n");
    }

    fclose(txt);

    printf("Relatório gerado com sucesso em 'alunos.txt'!\n");
}
