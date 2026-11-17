#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

/* ========================= config ========================= */

#define DATA_FILE "dados.bin"
#define TMP_FILE "dados.tmp"
#define SWAP_FILE "dados.swp"

// Arquivo em uso no momento (comeca no arquivo de dados)
const char *nowfile = DATA_FILE;

/* ========================= produtos ========================= */

#define MAX_NM 50
#define MAX_CTG 30
#define MAX_FNDR 50

typedef struct
{
    int id;
    char nm[MAX_NM];
    char ctg[MAX_CTG];
    float vlr;
    int qnt;
    char fndr[MAX_FNDR];
} produto;

/* ========================= interface ========================= */

// Linha de cima da tabela e titulos das colunas
void print_header()
{
    printf("+-----+--------------------------------+----------------------+------------+------------+--------------------------------+\n");
    printf("| %-3s | %-30s | %-20s | %-10s | %-10s | %-30s |\n", "ID", "Produto", "Categoria", "Valor", "Quantidade", "Fornecedor");
    printf("+-----+--------------------------------+----------------------+------------+------------+--------------------------------+\n");
}

// Uma linha da tabela com os dados do produto
void print_item(produto *item)
{
    printf("| %-3d | %-30s | %-20s | %10.2f | %-10d | %-30s |\n", item->id, item->nm, item->ctg, item->vlr, item->qnt, item->fndr);
}

// Linha final que fecha a tabela
void print_footer()
{
    printf("+-----+--------------------------------+----------------------+------------+------------+--------------------------------+\n");
}

/* ========================= filemanager ========================= */

// Retorna 1 se o arquivo existe e 0 se nao existe
int check_file(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        return 0; // o arquivo não encontrado
    }
    fclose(fp);
    return 1; // o arquivo encontrado
}

// Acrescenta um item no final do arquivo binario
void write_bin_line(produto item, const char *file)
{
    FILE *fp = fopen(file, "ab");
    if (fp == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    // fwrite devolve quantos blocos gravou (esperado: 1); so avisa se falhar
    size_t save = fwrite(&item, sizeof(produto), 1, fp);
    if (save != 1)
    {
        printf("Erro ao salva os dados\n");
    }
    fclose(fp);
    return;
}

// Converte o arquivo binario para CSV
void export_csv(const char *csv, const char *file)
{
    produto item;
    FILE *fp = fopen(csv, "w");
    if (fp == NULL)
    {
        printf("Erro de acesso ao %s\n", csv);
        return;
    }
    FILE *fb = fopen(file, "rb");
    if (fb == NULL)
    {
        fclose(fp);
        printf("Erro de acesso ao %s\n", file);
        return;
    }
    // Primeira linha do CSV: nomes das colunas
    fprintf(fp, "id, produto, categoria, valor, quantidade, fornecedor\n");
    // Uma linha do CSV para cada item do arquivo
    while (fread(&item, sizeof(produto), 1, fb) == 1)
    {
        fprintf(fp, "%d, %s, %s, %.2f, %d, %s\n", item.id, item.nm, item.ctg, item.vlr, item.qnt, item.fndr);
    }
    fclose(fp);
    fclose(fb);
    return;
}

// Troca o arquivo original pelo temporario (confirma as alteracoes)
void save_data(const char *file, const char *origin)
{
    // Sem temporario nao ha o que salvar (evita apagar o original)
    if (!check_file(file))
    {
        printf("Nada para salvar!\n");
        return;
    }

    // Apaga o original antes, pois rename falha se o destino existir
    if (remove(origin) != 0)
    {
        perror("Erro ao deletar o arquivo");
        return;
    }

    if (rename(file, origin) != 0)
    {
        perror("Erro ao renomear o arquivo");
        return;
    }
    return;
}

/* ========================= sort ========================= */

// Funcao de comparacao: retorna > 0 se "a" deve vir depois de "b"
typedef int (*comparador)(const produto *a, const produto *b);

static int cmp_az(const produto *a, const produto *b) { return strcmp(a->nm, b->nm); }
static int cmp_za(const produto *a, const produto *b) { return strcmp(b->nm, a->nm); }
static int cmp_menor(const produto *a, const produto *b) { return a->vlr > b->vlr; }
static int cmp_maior(const produto *a, const produto *b) { return a->vlr < b->vlr; }

// Bubble sort generico (estavel): a ordem vem do comparador
static void bubble_sort(produto *list, int size, comparador cmp)
{
    produto aux;

    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (cmp(&list[j], &list[j + 1]) > 0)
            {
                aux = list[j];
                list[j] = list[j + 1];
                list[j + 1] = aux;
            }
        }
    }
}

// Imprime a tabela completa: cabecalho, itens e rodape
void print_body(produto *list, int size)
{
    print_header();

    for (int i = 0; i < size; i++)
    {
        print_item(&list[i]);
    }

    print_footer();
}

// Imprime os itens na ordem pedida
// 0 = sem ordem, 1 = A-Z, 2 = Z-A, 3 = menor valor, 4 = maior valor
void show_sorted(produto *items, int size, int ord)
{
    static const comparador por_ordem[] = {NULL, cmp_az, cmp_za, cmp_menor, cmp_maior};

    if (ord >= 1 && ord <= 4)
        bubble_sort(items, size, por_ordem[ord]);

    print_body(items, size);
}

/* ========================= query ========================= */

// Adiciona um item ao vetor dinamico. Retorna 0 se faltar memoria
static int push_item(produto **items, int *size, const produto *item)
{
    produto *temp = realloc(*items, (*size + 1) * sizeof(produto));
    if (temp == NULL)
        return 0;

    *items = temp;
    (*items)[(*size)++] = *item;
    return 1;
}

// Retorna 1 se o item bate com o texto digitado no campo escolhido
static int match_item(const produto *item, const char *ent, int type)
{
    switch (type)
    {
    case 0:
        return item->id == atoi(ent);
    case 1:
        return strcasecmp(item->nm, ent) == 0;
    case 2:
        return strcasecmp(item->ctg, ent) == 0;
    case 4:
        return item->vlr == strtof(ent, NULL);
    case 5:
        return item->qnt == atoi(ent);
    case 6:
        return strcasecmp(item->fndr, ent) == 0;
    }
    return 0;
}

// Le todos os itens do arquivo e imprime na ordem escolhida (*ord)
void list_products(char *file, int *ord)
{
    produto *items = NULL;
    produto item;
    int size = 0;

    FILE *fp = fopen(file, "rb");
    if (fp == NULL)
    {
        printf("Erro de acesso ao arquivo!\n");
        return;
    }

    while (fread(&item, sizeof(produto), 1, fp) == 1)
    {
        if (!push_item(&items, &size, &item))
        {
            free(items);
            fclose(fp);
            return;
        }
    }
    fclose(fp);

    show_sorted(items, size, *ord);
    free(items);
}

// Busca itens pelo campo escolhido em "type" e imprime os encontrados
// type: 0 = id, 1 = nome, 2 = categoria, 4 = valor, 5 = quantidade, 6 = fornecedor
void search_product(char *ent, int type, char *file, int ord)
{
    produto *items = NULL;
    produto item;
    int size = 0;

    FILE *fp = fopen(file, "rb");
    if (fp == NULL)
    {
        printf("Erro de acesso ao arquivo!\n");
        return;
    }

    while (fread(&item, sizeof(produto), 1, fp) == 1)
    {
        if (item.id == 0)
            break;

        if (match_item(&item, ent, type) && !push_item(&items, &size, &item))
        {
            free(items);
            fclose(fp);
            return;
        }
    }
    fclose(fp);

    if (size == 0)
        printf("Nenhum item encontrado!\n");
    else
        show_sorted(items, size, ord);

    free(items);
}

/* ========================= datamanager ========================= */

// Cria o arquivo de dados com o estoque inicial
void dados_iniciais()
{
    produto estoque_inicial[10] = {
        {1, "Resistor 100R 1/4W", "Resistores", 0.15, 500, "Multilaser"},
        {2, "Resistor 10K 1/4W", "Resistores", 0.15, 450, "Multilaser"},
        {3, "Capacitor Eletrolitico 10uF", "Capacitores", 0.50, 200, "Epcos"},
        {4, "Capacitor Ceramico 100nF", "Capacitores", 0.30, 300, "Vishay"},
        {5, "Diodo Retificador 1N4007", "Diodos", 0.25, 400, "Fairchild"},
        {6, "Diodo Zener 5.1V", "Diodos", 0.40, 150, "NXP"},
        {7, "LED 5mm Vermelho", "Semicondutores", 0.20, 350, "Kingbright"},
        {8, "Transistor BC548", "Transistores", 0.45, 250, "ON Semi"},
        {9, "CI Regulat. Tensao 7805", "CI", 1.80, 100, "ST Micro"},
        {10, "Trimpot 10K", "Resistores", 1.20, 80, "Bourns"}};

    for (int i = 0; i < 10; i++)
    {
        write_bin_line(estoque_inicial[i], DATA_FILE);
    }
}

// Retorna o menor ID livre (reaproveita IDs de itens removidos)
int gen_id(const char *file)
{
    int i = 1, id = 1, atv = 1;
    produto item;
    FILE *fp = fopen(file, "rb");
    if (fp == NULL)
    {
        printf("Erro de acesso ao arquivos!\n");
        return 0;
    }
    while (atv)
    {
        // Assume que "i" esta livre; arquivo vazio nao trava mais o loop
        id = i;
        atv = 0;
        rewind(fp);
        // Procura "i" no arquivo; se achar, tenta o proximo numero
        while (fread(&item, sizeof(produto), 1, fp) == 1)
        {
            if (i == item.id)
            {
                atv = 1;
                break;
            }
        }
        i++;
    }
    fclose(fp);
    return id;
}

// Adiciona um item no arquivo temporario (cria a copia dele se nao existir)
void add_product(produto *p, const char *file, const char *origin)
{
    if (!(check_file(file)))
    {
        produto item;
        FILE *fb = fopen(origin, "rb");
        FILE *fp = fopen(file, "w");
        if (fp == NULL || fb == NULL)
        {
            // Fecha o que chegou a abrir, para nao deixar arquivo preso
            if (fp != NULL)
                fclose(fp);
            if (fb != NULL)
                fclose(fb);
            printf("Erro de acesso aos arquivos!\n");
            return;
        }
        // Copia os itens do arquivo original para o temporario
        while (fread(&item, sizeof(produto), 1, fb) == 1)
        {
            write_bin_line(item, file);
        }
        fclose(fp);
        fclose(fb);
    }
    write_bin_line(*p, file);
    printf("Item cadastrado com ID %d!\n", p->id);
}

// Troca o temporario pelo arquivo auxiliar
static void swap_to_tmp(const char *tmp)
{
    remove(tmp);
    if (rename(SWAP_FILE, tmp) != 0)
        perror("Erro ao atualizar o temporario");
}

// Reescreve "file" no arquivo auxiliar, depois troca pelo temporario.
// Se "novo" for NULL o item de ID "id" e removido; senao e substituido.
// Retorna 1 se o ID foi encontrado
static int rewrite_without(int id, const produto *novo, const char *file, const char *tmp)
{
    produto item;
    int found = 0;
    FILE *fb = fopen(file, "rb");
    if (fb == NULL)
    {
        printf("Erro de acesso ao %s\n", file);
        return 0;
    }
    FILE *fp = fopen(SWAP_FILE, "w");
    if (fp == NULL)
    {
        fclose(fb);
        printf("Erro de acesso ao %s\n", SWAP_FILE);
        return 0;
    }
    fclose(fp);

    while (fread(&item, sizeof(produto), 1, fb) == 1)
    {
        if (item.id != id)
            write_bin_line(item, SWAP_FILE);
        else
        {
            found = 1;
            if (novo != NULL)
                write_bin_line(*novo, SWAP_FILE);
        }
    }
    fclose(fb);

    if (found)
        swap_to_tmp(tmp);
    else
    {
        remove(SWAP_FILE);
        printf("Erro: ID %d nao encontrado!\n", id);
    }
    return found;
}

// Remove o item do ID informado. Le "file" (original ou temporario) e
// grava o resultado no temporario
void delete_product(int id, const char *file, const char *tmp)
{
    if (rewrite_without(id, NULL, file, tmp))
        printf("Item %d removido!\n", id);
}

// Troca o item de mesmo ID por *p
void update_product(produto *p, const char *file, const char *tmp)
{
    if (rewrite_without(p->id, p, file, tmp))
        printf("Item %d atualizado!\n", p->id);
}

/* ========================= cli_parse ========================= */

// Posicao da proxima virgula (ou do fim do texto) a partir de "i"
static int field_end(const char *s, int i)
{
    while (s[i] != ',' && s[i] != '\0')
        i++;
    return i;
}

// Copia um campo de texto de tamanho "len" para dst (capacidade "cap").
// Retorna 0 e mostra "erro" se estiver vazio ou nao couber
static int copy_text(char *dst, size_t cap, const char *src, int len, const char *erro)
{
    if (len == 0 || (size_t)len >= cap)
    {
        printf("%s\n", erro);
        return 0;
    }
    memcpy(dst, src, len);
    dst[len] = '\0';
    return 1;
}

// Valida "Nome,Categoria,Valor,Quantidade,Fornecedor" e preenche *p.
// Retorna 1 se ok e 0 se der erro
int parse_produto(char *info, produto *p)
{
    int start = 0;
    int i;
    char vlr[32] = {0};
    char qnt[32] = {0};
    char *end;

    // Precisa de 4 virgulas, senao as buscas abaixo leriam alem do texto
    int virgulas = 0;
    for (int k = 0; info[k] != '\0'; k++)
    {
        if (info[k] == ',')
            virgulas++;
    }
    if (virgulas < 4)
    {
        printf("Erro: quantidade de campos incorreta!\n");
        return 0;
    }

    // Nome
    i = field_end(info, start);
    if (!copy_text(p->nm, sizeof(p->nm), info + start, i - start,
                   "Erro: nome vazio ou muito longo!"))
        return 0;
    start = ++i;

    // Categoria
    i = field_end(info, start);
    if (!copy_text(p->ctg, sizeof(p->ctg), info + start, i - start,
                   "Erro: categoria vazia ou muito longa!"))
        return 0;
    start = ++i;

    // Valor (usa o buffer local vlr, nao sizeof(p->vlr))
    i = field_end(info, start);
    if (!copy_text(vlr, sizeof(vlr), info + start, i - start,
                   "Erro: valor invalido ou muito longo!"))
        return 0;

    errno = 0;
    p->vlr = strtof(vlr, &end);
    // Rejeita erro de conversao, lixo no fim do texto e valor negativo
    if (errno == ERANGE || *end != '\0' || end == vlr || p->vlr < 0)
    {
        printf("Erro: valor invalido!\n");
        return 0;
    }
    start = ++i;

    // Quantidade (usa o buffer local qnt, aceita 4 digitos ou mais)
    i = field_end(info, start);
    if (!copy_text(qnt, sizeof(qnt), info + start, i - start,
                   "Erro: quantidade invalida ou muito longa!"))
        return 0;

    errno = 0;
    long quantidade = strtol(qnt, &end, 10);
    if (errno == ERANGE || *end != '\0' || end == qnt || quantidade < 0 || quantidade > 2147483647L)
    {
        printf("Erro: quantidade inválida!\n");
        return 0;
    }
    p->qnt = (int)quantidade;
    start = ++i;

    // Fornecedor (o resto do texto; nao pode ter virgula)
    if (!copy_text(p->fndr, sizeof(p->fndr), info + start, (int)strlen(info + start),
                   "Erro: fornecedor vazio ou muito longo!"))
        return 0;

    if (strchr(p->fndr, ',') != NULL)
    {
        printf("Erro: quantidade de campos incorreta!\n");
        return 0;
    }
    return 1;
}

// Converte o texto da ordem em codigo (1 a 4); retorna 0 se invalido
int parse_ord(const char *txt, int *ord)
{
    static const char *nomes[] = {"az", "za", "menor", "maior"};

    for (int k = 0; k < 4; k++)
    {
        if (strcasecmp(txt, nomes[k]) == 0)
        {
            *ord = k + 1;
            return 1;
        }
    }
    return 0;
}

// Converte texto em ID (inteiro positivo). Retorna 1 se ok e 0 se invalido
int parse_id(const char *txt, int *id)
{
    char *end;

    errno = 0;
    long valor = strtol(txt, &end, 10);

    // O ID precisa ser um numero inteiro positivo
    if (errno == ERANGE || *end != '\0' || end == txt || valor <= 0 || valor > 2147483647L)
        return 0;

    *id = (int)valor;
    return 1;
}

/* ========================= cli ========================= */

typedef struct
{
    char name[8];
    void (*func)(char *);
    char descricao[128];
} comandos;

void cmd_insert(char *info);
void cmd_delete(char *info);
void cmd_list(char *info);
void cmd_find(char *info);
void cmd_save(char *info);
void cmd_export(char *info);
void cmd_help(char *info);
void cmd_update(char *info);

// Lista de comandos: nome, funcao e descricao (usada no help)
comandos comandosList[] = {
    {"insert", cmd_insert, "Cadastra um item. Use o formato: Nome,Categoria,Valor,Quantidade,Fornecedor (sem espacos extras)"},
    {"delete", cmd_delete, "Remove um item do sistema informando apenas o ID"},
    {"update", cmd_update, "Atualiza um item. Use o formato: ID,Nome,Categoria,Valor,Quantidade,Fornecedor"},
    {"list", cmd_list, "Lista os itens. Ordem opcional: az, za, menor, maior (ex.: list az)"},
    {"find", cmd_find, "Busca: find -id|-nm|-ct|-vl|-qt|-fn valor [az|za|menor|maior]"},
    {"save", cmd_save, "Salva permanentemente todas as alteracoes realizadas"},
    {"export", cmd_export, "Exporta os dados atuais para um arquivo no formato CSV"},
    {"help", cmd_help, "Exibe a lista de comandos"},
};

const int qnt_funcs = 8;

// Arquivo de trabalho: o temporario (com alteracoes nao salvas) se existir
static const char *arquivo_atual(void)
{
    return check_file(TMP_FILE) ? TMP_FILE : nowfile;
}

// insert Nome,Categoria,Valor,Quantidade,Fornecedor
void cmd_insert(char *info)
{
    produto p = {0};

    if (info == NULL || info[0] == '\0')
    {
        printf("Erro: entrada vazia!\n");
        return;
    }

    // O ID e gerado pelo sistema (olha o temporario tambem,
    // senao dois inserts seguidos recebiam o mesmo ID)
    p.id = gen_id(arquivo_atual());

    if (!parse_produto(info, &p))
        return;

    add_product(&p, TMP_FILE, nowfile);
}

// delete ID
void cmd_delete(char *info)
{
    int id;

    if (info == NULL || info[0] == '\0')
    {
        printf("Erro: ID não informado!\n");
        return;
    }

    if (!parse_id(info, &id))
    {
        printf("Erro: ID inválido!\n");
        return;
    }

    // Parte do temporario (se existir) para nao perder alteracoes nao salvas
    delete_product(id, arquivo_atual(), TMP_FILE);
}

// list [az|za|menor|maior]
void cmd_list(char *info)
{
    int ord = 0;

    if (info != NULL && info[0] != '\0' && !parse_ord(info, &ord))
    {
        printf("Erro: ordem invalida! Use az, za, menor ou maior\n");
        return;
    }

    // Mostra o temporario (com alteracoes nao salvas) se ele existir
    list_products((char *)(check_file(TMP_FILE) ? TMP_FILE : DATA_FILE), &ord);
}

// save (sem argumentos)
void cmd_save(char *info)
{
    if (info != NULL && info[0] != '\0')
    {
        printf("Erro: Esse comando nao possui argumentos\n");
        return;
    }
    save_data(TMP_FILE, DATA_FILE);
}

// export (sem argumentos)
void cmd_export(char *info)
{
    if (info != NULL && info[0] != '\0')
    {
        printf("Erro: Esse comando nao possui argumentos\n");
        return;
    }
    export_csv("dados.csv", DATA_FILE);
}

// Mostra o nome e a descricao de cada comando
void cmd_help(char *info)
{
    (void)info;
    printf("LISTA DE COMANDOS\n");
    printf("=======================\n");
    for (int i = 0; i < qnt_funcs; i++)
    {
        printf("  %-10s", comandosList[i].name);
        if (strlen(comandosList[i].descricao) > 0)
            printf("- %s", comandosList[i].descricao);
        printf("\n");
    }
}

// update ID,Nome,Categoria,Valor,Quantidade,Fornecedor
void cmd_update(char *info)
{
    produto p = {0};
    char id[32] = {0};

    if (info == NULL || info[0] == '\0')
    {
        printf("Erro: entrada vazia!\n");
        return;
    }

    // Separa o ID (ate a primeira virgula) do resto dos campos
    char *virgula = strchr(info, ',');
    if (virgula == NULL || (size_t)(virgula - info) >= sizeof(id))
    {
        printf("Erro: use o formato ID,Nome,Categoria,Valor,Quantidade,Fornecedor\n");
        return;
    }
    strncpy(id, info, virgula - info);

    if (!parse_id(id, &p.id))
    {
        printf("Erro: ID inválido!\n");
        return;
    }

    // Mantem o ID e valida os demais campos como no insert
    if (!parse_produto(virgula + 1, &p))
        return;

    update_product(&p, arquivo_atual(), TMP_FILE);
}

/* ========================= cli_find ========================= */

// Flag do find -> codigo do campo usado no search_product
static int flag_type(const char *flag)
{
    static const struct
    {
        const char *flag;
        int type;
    } flags[] = {
        {"-id", 0}, {"-nm", 1}, {"-ct", 2}, {"-vl", 4}, {"-qt", 5}, {"-fn", 6}};

    for (int k = 0; k < 6; k++)
    {
        if (strcasecmp(flag, flags[k].flag) == 0)
            return flags[k].type;
    }
    return -1;
}

// Campos numericos precisam de numero valido. Retorna 1 se ok
static int valor_valido(const char *valor, int type)
{
    char *end;

    if (type != 0 && type != 4 && type != 5)
        return 1;

    errno = 0;
    if (type == 4)
        strtof(valor, &end);
    else
        strtol(valor, &end, 10);

    if (errno == ERANGE || *end != '\0' || end == valor)
    {
        printf(type == 4 ? "Erro: valor numerico invalido!\n"
                         : "Erro: o valor deve ser um numero inteiro!\n");
        return 0;
    }
    return 1;
}

// find -id|-nm|-ct|-vl|-qt|-fn valor [az|za|menor|maior]
void cmd_find(char *info)
{
    int ord = 0;
    char flag[8] = {0};
    char valor[64] = {0};

    if (info == NULL || info[0] != '-')
    {
        printf("Erro: use find -id|-nm|-ct|-vl|-qt|-fn valor [az|za|menor|maior]\n");
        return;
    }

    // Separa a flag (ate o primeiro espaco) do valor
    char *espaco = strchr(info, ' ');
    if (espaco == NULL || (size_t)(espaco - info) >= sizeof(flag))
    {
        printf("Erro: informe a flag e o valor. Ex.: find -fn empresatech az\n");
        return;
    }
    strncpy(flag, info, espaco - info);

    int type = flag_type(flag);
    if (type < 0)
    {
        printf("Erro: flag invalida! Use -id, -nm, -ct, -vl, -qt ou -fn\n");
        return;
    }

    // Ignora espacos extras antes do valor
    char *resto = espaco + 1;
    while (*resto == ' ')
        resto++;

    if (resto[0] == '\0' || strlen(resto) >= sizeof(valor))
    {
        printf("Erro: valor vazio ou muito longo!\n");
        return;
    }
    strcpy(valor, resto);

    // Se a ultima palavra for uma ordem valida, tira ela do valor
    char *ultimo = strrchr(valor, ' ');
    if (ultimo != NULL && parse_ord(ultimo + 1, &ord))
    {
        while (ultimo > valor && *(ultimo - 1) == ' ')
            ultimo--;
        *ultimo = '\0';
    }

    if (valor[0] == '\0')
    {
        printf("Erro: valor vazio!\n");
        return;
    }

    if (!valor_valido(valor, type))
        return;

    // Busca no temporario (se existir) para incluir alteracoes nao salvas
    search_product(valor, type, check_file(TMP_FILE) ? TMP_FILE : DATA_FILE, ord);
}

/* ========================= main ========================= */

int main()
{
    // Primeira execucao: cria o arquivo de dados com o estoque inicial
    if (!(check_file(DATA_FILE)))
    {
        dados_iniciais();
    }
    // Descarta temporario esquecido de uma execucao anterior
    if (check_file(TMP_FILE))
    {
        remove(TMP_FILE);
    }
    char input[256];
    char comando[8];
    char *parametro;

    printf("Bem vindo\n");

    while (1)
    {
        comando[0] = '\0';
        input[0] = '\0';
        printf("\n> ");
        // Fim da entrada (Ctrl+D / Ctrl+Z): sai do loop em vez de repetir
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        int i = 0;
        // Separa a primeira palavra (o comando), com no maximo 7 letras
        while (input[i] != '\0' && input[i] != ' ' && i < 7)
        {
            comando[i] = input[i];
            i++;
        }
        comando[i] = '\0';
        // O restante da linha vira o parametro do comando
        parametro = &input[i];
        if (*parametro == ' ')
        {
            parametro++;
        }
        // Sair: apaga o temporario (alteracoes nao salvas) e encerra
        if (strcasecmp(comando, "exit") == 0)
        {
            if (check_file(TMP_FILE))
            {
                if (remove(TMP_FILE) != 0)
                {
                    perror("Erro ao deletar o .tmp");
                    system("pause");
                    return 0;
                }
            }
            system("pause");
            return 0;
        }
        int encontrado = 0;
        // Procura o comando na lista e executa a funcao dele
        for (int i = 0; i < qnt_funcs; i++)
        {
            if (strcasecmp(comando, comandosList[i].name) == 0)
            {
                comandosList[i].func(parametro);
                encontrado = 1;
                break;
            }
        }
        if (!encontrado)
        {
            printf("ERRO: comando invalido\n");
        }
    }

    return 0;
}
