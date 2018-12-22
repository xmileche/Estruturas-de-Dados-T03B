/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com encadeamento
 *
 * RA: 726573
 * Aluno: Michele Argolo Carvalho
 * ========================================================================== */
 
/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
 
/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
 
#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)
 
/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA                              "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM                          "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO                         "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO                        "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO  "**********************EXCLUIR*********************\n"
#define SUCESSO                   "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA                      "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"
 
 
 
/* Registro do jogo */
typedef struct {
    char pk[TAM_PRIMARY_KEY];
    char nome[TAM_NOME];
    char marca[TAM_MARCA];
    char data[TAM_DATA];    /* DD/MM/AAAA */
    char ano[TAM_ANO];
    char preco[TAM_PRECO];
    char desconto[TAM_DESCONTO];
    char categoria[TAM_CATEGORIA];
} Produto;
 
/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave {
    char pk[TAM_PRIMARY_KEY];
    int rrn;
    struct chave *prox;
} Chave;
 
/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave **v;
} Hashtable;
 
/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;
 
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();
 
 
/* Exibe o jogo */
int exibir_registro(int rrn);
 
/*Função de Hash*/
short hash(const char* chave, int tam);
 
 
/*Auxiliar para a função de hash*/
short f(char x);
 
/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);
 
/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);
 
/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
void criar_tabela(Hashtable *tabela, int tam);
void ler_entrada(Produto *novo);
void insere_arquivo(Produto novo);
void insere_hash(Hashtable *tabela, char chave[], int rrn);
int busca(Hashtable *tabela, char chave[]);
Produto recuperar_registro(int rrn);
int verificaDesconto(char desconto[]);
int removeHash(Hashtable *tabela, char chave[]);
void imprimir_tabela(Hashtable tabela);
 
 
/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
 
int main()
{
 
    /* Arquivo */
    int carregarArquivo = 0;
    scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
    if (carregarArquivo)
        carregar_arquivo();
 
    /* Tabela Hash */
    int tam;
    scanf("%d%*c", &tam);
    tam = prox_primo(tam);
 
    Hashtable tabela;
    criar_tabela(&tabela, tam);
    if (carregarArquivo)
        carregar_tabela(&tabela);
 
    /* Execução do programa */
    int opcao = 0;
    while(opcao != 6) {
        scanf("%d%*c", &opcao);
        switch(opcao) {
 
            case 1:
                cadastrar(&tabela);
                break;
            case 2:
                printf(INICIO_ALTERACAO);
                if(alterar(tabela))
                    printf(SUCESSO);
                else
                    printf(FALHA);
                break;
            case 3:
                printf(INICIO_BUSCA);
                buscar(tabela);
                break;
            case 4:
                printf(INICIO_EXCLUSAO);
                printf("%s", (remover(&tabela)) ? SUCESSO : FALHA );
                break;
            case 5:
                printf(INICIO_LISTAGEM);
                imprimir_tabela(tabela);
                break;
            case 6:
                liberar_tabela(&tabela);
                break;
 
            case 10:
                printf(INICIO_ARQUIVO);
                printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
                break;
 
            default:
                printf(OPCAO_INVALIDA);
                break;
        }
    }
    return 0;
}
 
 
/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */
 
/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
    scanf("%[^\n]\n", ARQUIVO);
}
 
/*Auxiliar para a função de hash*/
short f(char x)
{
    return (x < 59) ? x - 48 : x - 54;
}
 
/* Exibe o jogo */
int exibir_registro(int rrn)
{
    if(rrn<0)
        return 0;
    float preco;
    int desconto;
    Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];
    printf("%s\n", j.pk);
    printf("%s\n", j.nome);
    printf("%s\n", j.marca);
    printf("%s\n", j.data);
    printf("%s\n", j.ano);
    sscanf(j.desconto,"%d",&desconto);
    sscanf(j.preco,"%f",&preco);
    preco = preco *  (100-desconto);
    preco = ((int) preco)/ (float) 100 ;
    printf("%07.2f\n",  preco);
    strncpy(categorias, j.categoria, strlen(j.categoria));
    categorias[strlen(j.categoria)] = '\0';
 
    for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
        printf("%s ", cat);
    printf("\n");
    return 1;
}
 
int prox_primo(int a){
    int achou=0, naoehprimo=0, i;
 
    while(!achou){
        for(i = 2; i <= a/2; i++){
            if(a % i == 0){
                naoehprimo = 1;
                break;
            }
        }
 
        if(naoehprimo){
            naoehprimo = 0;
            a++;
        } else
            achou=1;
    }
 
    return a;
}
 
void criar_tabela(Hashtable *tabela, int tam){
    nregistros = 0;
    tabela->tam = tam;
    tabela->v = (Chave **)malloc(sizeof(Chave*)*tam);
    int i = 0;
    while (i < tabela->tam){
        tabela->v[i] = NULL;
        i++;
    }
}
 
void carregar_tabela(Hashtable *tabela){
    char *percorreArquivo = ARQUIVO, chave[TAM_PRIMARY_KEY] = "\0";
    int i=0;
 
    while(*percorreArquivo != '\0'){
        sscanf(percorreArquivo, "%[^@]@", chave);
        if(chave[0] == '*' && chave[1] == '|')   // Não insere elementos marcados como removidos
            percorreArquivo = percorreArquivo + TAM_REGISTRO;
        else {   // Só insere se registro não estiver como removido
            insere_hash(tabela, chave, i);
            percorreArquivo = percorreArquivo + TAM_REGISTRO;
 
        }
        i++;
    }
    nregistros = i;
}
 
void cadastrar(Hashtable* tabela){
    Produto novo;
    int buscaHash = -1;
 
    ler_entrada(&novo);  // Lê as entradas específicas e coloca as informações na strct novo
 
    if(nregistros) { // Se tem registros faz uma busca na Hash
        buscaHash = busca(tabela, novo.pk);
    }
 
    if(buscaHash != -1){
        printf(ERRO_PK_REPETIDA, novo.pk);
        return;
    } else {
        // Então pode inserir
        insere_arquivo(novo);     // Insere no arquivo de dados
        insere_hash(tabela, novo.pk, nregistros);
        nregistros = nregistros + 1;
        printf(REGISTRO_INSERIDO, novo.pk);
 
    }
}
 
// Função de hash
short hash(const char* chave, int tam){
    return ((f(chave[0]) + 2 * f(chave[1]) + 3 * f(chave[2]) + 4 * f(chave[3]) + 5 * f(chave[4]) + 6 * f(chave[5]) + 7 * f(chave[6]) + 8 * f(chave[7])) % tam);
}
 
void insere_hash(Hashtable *tabela, char chave[], int rrn){
    int i = hash(chave, tabela->tam);    // i é posição
 
    Chave *novo = (Chave*)malloc(sizeof(Chave));
    strcpy(novo->pk, chave);
    novo->rrn = rrn;
 
    if(tabela->v[i] == NULL){ // primeiro elemento a ser inserido
        novo->prox = NULL;
        tabela->v[i] = novo;
    } else {  // não é o primeiro elemento a ser inserido
        Chave *atual = tabela->v[i];
        Chave *anterior = NULL;
        while(atual != NULL && strcmp(atual->pk, chave) < 0) {
            anterior = atual;
            atual = atual->prox;
        }
 
        if(atual)
            if(strcmp(atual->pk, chave) == 0)
                return;
 
        if(atual == NULL && anterior != NULL){  // Então quer dizer que vou inserir no fim
            anterior->prox = novo;
            novo->prox = NULL;
        }
 
        else{  // Então quer dizer que vou inserir no meio ou começo
            if(anterior == NULL){ // Vou inserir no começo
                novo->prox = atual;
                tabela->v[i] = novo;
            } else { // Vou inserir no meio
                novo->prox = atual;
                anterior->prox = novo;
            }
        }
    }
}
 
void ler_entrada(Produto *novo){
    // Lê do usuários as informações de um registro
    char teste[TAM_NOME] = "\0";
 
    scanf("%51[^\n]%*c", novo->nome);
    scanf("%51[^\n]%*c", novo->marca);
    scanf("%11[^\n]%*c", novo->data);
    scanf("%3[^\n]%*c", novo->ano);
    scanf("%8[^\n]%*c", novo->preco);
    scanf("%11[^\n]%*c", novo->desconto);
    scanf("%51[^\n]%*c", novo->categoria);
 
    //Gera chave
    novo->pk[0] = toupper(novo->nome[0]);
    novo->pk[1] = toupper(novo->nome[1]);
    novo->pk[2] = toupper(novo->marca[0]);
    novo->pk[3] = toupper(novo->marca[1]);
    novo->pk[4] = novo->data[0];
    novo->pk[5] = novo->data[1];
    novo->pk[6] = novo->data[3];
    novo->pk[7] = novo->data[4];
    novo->pk[8] = novo->ano[0];
    novo->pk[9] = novo->ano[1];
    novo->pk[10] = '\0';
 
}
 
 
void insere_arquivo(Produto novo){
    char *registro = ARQUIVO + nregistros * TAM_REGISTRO;
    char str[193] = "\0";
    int i = 0;
    // Insere em str todas as informações digitadas separadas pelo delimitador "@"
 
    sprintf(str, "%s", novo.pk);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.nome);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.marca);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.data);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.ano);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.preco);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.desconto);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.categoria);
    sprintf(str + strlen(str), "%s", "@");
 
    for(i =  strlen(str); i < TAM_REGISTRO; i++)
        str[i] = '#';
 
    str[i] = '\0';
 
    sprintf(registro, "%s" , str);
}
 
void buscar(Hashtable tabela){
    char chave[TAM_PRIMARY_KEY];
    scanf("%11[^\n]%*c", chave);
 
    int buscar = busca(&tabela, chave);
 
    if(buscar == -1){
        printf(REGISTRO_N_ENCONTRADO);
    } else {
        exibir_registro(buscar);
    }
 
}
 
int busca(Hashtable *tabela, char chave[]){
    int i = hash(chave, tabela->tam), compara = 0;
    Chave *atual = tabela->v[i];
 
    while(atual){
        compara = strcmp(atual->pk, chave);
        if(!compara) // achou
            return atual->rrn;
        else if (compara > 0)
            return -1;
        else
            atual = atual->prox;
    }
    return -1;
}
 
void imprimir_tabela(Hashtable tabela){
    Chave *atual = NULL;
    for (int i = 0; i < tabela.tam; i++) {
        atual = tabela.v[i];
        printf("[%d]", i);
        while(atual){
            printf(" %s", atual->pk);
            atual = atual->prox;
        }
        printf("\n");
    }
 
}
 
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;
    p = strtok(temp,"@");
    strcpy(j.pk,p);
    p = strtok(NULL,"@");
    strcpy(j.nome,p);
    p = strtok(NULL,"@");
    strcpy(j.marca,p);
    p = strtok(NULL,"@");
    strcpy(j.data,p);
    p = strtok(NULL,"@");
    strcpy(j.ano,p);
    p = strtok(NULL,"@");
    strcpy(j.preco,p);
    p = strtok(NULL,"@");
    strcpy(j.desconto,p);
    p = strtok(NULL,"@");
    strcpy(j.categoria,p);
    //gerarChave(&j);
    return j;
}
 
 
int alterar(Hashtable tabela){
    Produto recupera;
    char teste[TAM_NOME] = "\0";
    char *pArquivo;
    char desconto[TAM_DESCONTO] = "\0";
    char chave[TAM_PRIMARY_KEY] = "\0";
    scanf("%11[^\n]%*c", chave);
    // Busca a chave primária digitada na tabela hash
    int buscaChave = busca(&tabela, chave);
 
    if(buscaChave == -1){
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
 
    do {
        scanf("%[^\n]%*c", teste);
    } while (verificaDesconto(teste) == 0);
    strcpy(desconto, teste);
    desconto[3] = '\0';
    pArquivo = ARQUIVO + (TAM_REGISTRO * buscaChave);
    recupera = recuperar_registro(buscaChave);
 
    pArquivo = pArquivo + strlen(recupera.pk) + strlen(recupera.nome) + strlen(recupera.marca) + strlen(recupera.data) + strlen(recupera.ano) + strlen(recupera.preco) + 6;
    *pArquivo = desconto[0];
    pArquivo++;
    *pArquivo = desconto[1];
    pArquivo++;
    *pArquivo = desconto[2];
 
    return 1;
}
 
int verificaDesconto(char desconto[]){
    if(strlen(desconto) != 3){
        printf(CAMPO_INVALIDO);
        return 0;
    }
 
    else{ // senão o tamanho eh igual a 3
        for(int i = 0; i<3; i++){
            if(desconto[i] < 48 || desconto[i] > 57) {
                printf(CAMPO_INVALIDO);
                return 0;
            }
        }
 
        if(((atof(desconto) > 100) || atof(desconto) < 0)){
            printf(CAMPO_INVALIDO);
            return 0;
        }
    }
    return 1;
}
 
int remover(Hashtable* tabela){
    char chave[TAM_PRIMARY_KEY];
    scanf("%11[^\n]%*c", chave);
 
    int remove = removeHash(tabela, chave);
 
    if(remove == -1){
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
 
    char *p = ARQUIVO + TAM_REGISTRO * remove;
    *p = '*';
    p = p + 1;
    *p = '|';
 
    return 1;
 
}
 
int removeHash(Hashtable *tabela, char chave[]){
    int i = hash(chave, tabela->tam), rrn;
 
    if(tabela->v[i] == NULL){ // primeiro elemento a ser inserido
        return -1;
    } else {  // não é o primeiro elemento a ser inserido
        Chave *atual = tabela->v[i];
        Chave *anterior = NULL;
        while(atual != NULL && strcmp(atual->pk, chave) < 0) {
            anterior = atual;
            atual = atual->prox;
        }
 
        if(!atual)   // Não há nada naquela posição, logo o registro não está lá
            return -1;
 
        else if(strcmp(atual->pk, chave) == 0) {
            if (atual == tabela->v[i]) {  // Então quer dizer que vou remover do começo
                tabela->v[i] = atual->prox;
                rrn = atual->rrn;
                free(atual);
            } else {  // Então quer dizer que vou remover no meio ou fim
                anterior->prox = atual->prox;
                rrn = atual->rrn;
                free(atual);
            }
 
            return rrn;
        } else   // Entra nesse else quando strcmp > 0
            return -1;
    }
}
 
void liberar_tabela(Hashtable* tabela){
    for(int i=0; i<tabela->tam; i++){
        if(tabela->v[i]){
            Chave *proxNo, *atual;
            atual = tabela->v[i];
            while(atual != NULL){
                proxNo = atual->prox;
                free(atual);
                atual = proxNo;
            }
        }
    }
}
