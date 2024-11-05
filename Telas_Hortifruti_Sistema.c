#include <windows.h>
#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <locale.h>

char* senhas[3]; // Array para armazenar 3 senhas
char* logins[3]; // Array para os logins
int count = 0;

void setConsoleColors(int textColor, int bgColor) { // Função para mudar a cor do texto e fundo no console
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)((bgColor << 4) | textColor));
}

void PuxarCredenciais(MYSQL* conn) { // Função para puxar as credenciais do banco de dados
    MYSQL_RES* res;
    MYSQL_ROW row;

    mysql_query(conn, "SELECT LOGIN, SENHA FROM CREDENCIAIS_ACESSO");

    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) && count < 3) {
        logins[count] = _strdup(row[0]);
        senhas[count] = _strdup(row[1]);
        count++;
    }
    mysql_free_result(res);
}

int teste_de_login(char login[31]) { // Função para testar a igualdade dos logins inseridos com os armazenados no banco de dados
    for (int i = 0; i < count; i++) {
        if (strcmp(login, logins[i]) == 0) {
            return 1; // Login aprovado
        }
    }
    return 0; // Login reprovado
}

int teste_de_senha(char senha[31]) { // Função para testar a igualdade das senhas inseridas com as armazenadas no banco de dados
    for (int i = 0; i < count; i++) {
        if (strcmp(senha, senhas[i]) == 0) {
            return 1; // Senha aprovada
        }
    }
    return 0; // Senha reprovada
}

void ParaMinúsculas(char s1[]) { // Transforma uma string em minúscula
    int i = 0;
    while (s1[i] != '\0') {
        s1[i] = tolower(s1[i]);
        i++;
    }
}

void PuxarCódigoeProdutos(MYSQL* conn) { // Puxa a tabela de produtos com seus valores e códigos do banco de dados
    MYSQL_ROW row;
    MYSQL_RES* res;

    if (mysql_query(conn, "SELECT CODE, NOME, VALOR_KG FROM PRODUTOS")) {
        fprintf(stderr, "Erro na consulta: %s\n", mysql_error(conn));
        return;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "Erro ao obter resultado: %s\n", mysql_error(conn));
        return;
    }

    // Imprimir cabeçalhos das colunas
    printf("%-10s %-15s %-10s\n", "CÓDIGO", "NOME", "VALOR_KG");

    // Imprimir linhas de dados
    while ((row = mysql_fetch_row(res))) {
        printf("%-10s %-15s R$%-8s\n", row[0], row[1], row[2]);
    }

    mysql_free_result(res);
}

void MostrarMenu() { // Função para mostrar o menu inicial
    setConsoleColors(14, 0);
    printf("--------------SISTEMA HORTIFRUTI---------------\n");
    printf("Escolha uma opção do sistema \n");
    setConsoleColors(15, 0);
    printf("[COMPRA]: Para passar os produtos\n");
    printf("[PRODUTOS]: Para adicionar um produto no sistema (*Apenas Funcionários Permitidos*)\n");
    printf("[FUNCIONÁRIOS]: Para adicionar um funcionário no sistema (*Apenas funcionários permitidos*)\n");
    printf("[ENCERRAR]: Para encerrar o sistema digite\n");
}

int main() {
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    char* server = "localhost";
    char* user = "root";
    char* password = "senha"; // Sua senha
    char* database = "hortifruti"; // Seu banco de dados

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        fprintf(stderr, "Erro na conexão: %s\n", mysql_error(conn));
        exit(1);
    }

    setlocale(LC_ALL, "Portuguese");

    PuxarCredenciais(conn);
    float troco;
    float dinheiro_entregue = 0;
    int senha_cartao = 0;
    char forma_pagamento[15];
    char tentativaSenha[30];
    char tentativaLogin[30];
    char selecionar_opcao[20];
    float quantidade_produto = 0;
    char selecionar_produto[20];
    float total_final = 0;
    float valor_compra = 0;
    int a = 0;
    int a1 = 0;
    int limite = 0;

    //variaveis pro banco de dados
    int codigo;
    char nome_produto[30];
    float preço_kg = 0;
    char query[256];
    

    do {
        int valor_final = 0;
        int valor_compra = 0;
        system("cls");
        MostrarMenu();
        fflush(stdin);
        scanf_s("%14s", selecionar_opcao, (unsigned)_countof(selecionar_opcao));
        printf("\n");
        ParaMinúsculas(selecionar_opcao);

        if (strcmp(selecionar_opcao, "compra") == 0) {
            system("cls");
            setConsoleColors(14, 0);
            printf("----------ESTAÇÃO DE COMPRA---------     [DIGITE 'RETORNAR' PARA VOLTAR]\n");
            printf("                                         [DIGITE 'PAGAMENTO' PARA IR AO PAGAMENTO]\n");
            PuxarCódigoeProdutos(conn);
           

            do {
                setConsoleColors(15, 0);
                printf("\n");
                printf("Digite o código do produto:\n");
                fflush(stdin);
                scanf_s("%19s", selecionar_produto, (unsigned)_countof(selecionar_produto));
                ParaMinúsculas(selecionar_produto);

                if (strcmp(selecionar_produto, "retornar") == 0) {
                    printf("Retornando ao menu principal...\n");
                    break;
                }
                else if (strcmp(selecionar_produto, "pagamento") == 0) {
                    system("cls");
                    setConsoleColors(15, 0);
                    printf("VALOR TOTAL DA COMPRA: R$% .2F\n\n", total_final);
                    printf("Escolha a forma de pagamento: \n");
                    setConsoleColors(14, 0);
                    printf("[DÉBITO]\n[CRÉDITO]\n[DINHEIRO]\n");
                    fflush(stdin);
                    scanf_s("%14s", forma_pagamento, (unsigned)_countof(forma_pagamento));
                    ParaMinúsculas(forma_pagamento);

                    if (strcmp(forma_pagamento, "débito") == 0 || strcmp(forma_pagamento, "debito") == 0) {
                        printf("\n");
                        setConsoleColors(15, 0);
                        printf("Débito selecionado:\nDigite sua senha:\n");
                        fflush(stdin);
                        scanf_s("%d", &senha_cartao);
                        setConsoleColors(10, 0);
                        system("cls");
                        printf("Pagamento aprovado\n");
                        system("pause");
                        exit(0);
                        
                    }
                    else if (strcmp(forma_pagamento, "crédito") == 0 || strcmp(forma_pagamento, "credito") == 0) {
                        printf("\n");
                        setConsoleColors(15, 0);
                        printf("Crédito selecionado:\nDigite sua senha:\n");
                        fflush(stdin);
                        scanf_s("%d", &senha_cartao);
                        setConsoleColors(10, 0);
                        system("cls");
                        printf("Pagamento aprovado\n");
                        system("pause");
                        exit(0);
                        
                    }
                    else if (strcmp(forma_pagamento, "dinheiro") == 0) {
                        setConsoleColors(15, 0);
                        printf("Digite quanto entregou, para o cálculo do troco:\n");
                        fflush(stdin);
                        scanf_s("%f", &dinheiro_entregue);
                        troco = dinheiro_entregue - total_final;
                        if (troco < 0) {
                            system("cls");
                            setConsoleColors(4, 0);
                            printf("Valor insuficiente:\nCompra cancelada\n");
                            system("pause");
                            exit(0);
                        }
                        else if (troco == 0) {
                            system("cls");
                            setConsoleColors(10, 0);
                            printf("Compra finalizada:\nObrigado pela compra\n");
                            system("pause");
                            exit(0);
                        }
                        else {
                            system("cls");
                            setConsoleColors(15, 0);
                            printf("Troco a receber: R$%.2f\n", troco);
                            setConsoleColors(10, 0);
                            printf("Troco entregue: Compra finalizada\nObrigado pela compra\n");
                            system("pause");
                            exit(0);
                            
                        }
                    }
                    break;
                }
                else if (strcmp(selecionar_produto, "22984") == 0) {
                    printf("Digite a quantidade de cenoura em KG: ");
                    scanf_s("%f", &quantidade_produto);
                    valor_compra = 2.40f * quantidade_produto;
                    total_final += valor_compra;                   
                }
                else if (strcmp(selecionar_produto, "23114") == 0) {
                    printf("Digite a quantidade de Beterraba em KG: ");
                    scanf_s("%f", &quantidade_produto);
                    valor_compra = 2.10f * quantidade_produto;
                    total_final += valor_compra;                   
                }
                else if (strcmp(selecionar_produto, "24321") == 0) {
                    printf("Digite a quantidade de Couve em KG: ");
                    scanf_s("%f", &quantidade_produto);
                    valor_compra = 4.57f * quantidade_produto;
                    total_final += valor_compra;                   
                }
                else if (strcmp(selecionar_produto, "29008") == 0) {
                    printf("Digite a quantidade de Abacate em KG: ");
                    scanf_s("%f", &quantidade_produto);
                    valor_compra = 10.32f * quantidade_produto;
                    total_final += valor_compra;                 
                }
                else if (strcmp(selecionar_produto, "29011") == 0) {
                    printf("Digite a quantidade de Laranja-Pera em KG: ");
                    scanf_s("%f", &quantidade_produto);
                    valor_compra = 4.05f * quantidade_produto;
                    total_final += valor_compra;                  
                }
                else if (strcmp(selecionar_produto, "47562") == 0) {
                    printf("Digite a quantidade de Banana em KG: ");
                    scanf_s("%f", &quantidade_produto);
                    valor_compra = 4.80f * quantidade_produto;
                    total_final += valor_compra;
                }
                else {
                    setConsoleColors(4, 0);
                    printf("Produto não encontrado\n");
                    setConsoleColors(15, 0);
                }

            } while (1);

        }else if (strcmp(selecionar_opcao, "encerrar") == 0) {
            printf("Sistema finalizado.\n");
            break;
        }else if(strcmp(selecionar_opcao,"produtos")==0) {
            do {
                system("cls");
                setConsoleColors(15, 0);
                printf("Digite seu login:");
                fflush(stdin);
                scanf_s("%29s", tentativaLogin, (unsigned)_countof(tentativaLogin));
                printf("\nDigite sua senha:");
                fflush(stdin);
                scanf_s("%29s", tentativaSenha, (unsigned)_countof(tentativaSenha));

                int acesso = teste_de_senha(tentativaSenha);
                int acesso2 = teste_de_login(tentativaLogin);
                a = acesso;
                a1 = acesso2;
                printf("\n");
                if (acesso == 1 && acesso2 == 1) {
                    setConsoleColors(10, 0);
                    printf("Acesso permitido\n");
                    system("pause");
                    system("cls");
                    setConsoleColors(14, 0);
                    printf("-------------PRODUTOS ATUAIS------------\n");
                    PuxarCódigoeProdutos(conn);
                    printf("----------ADICIONAR NOVOS PRODUTOS?------------\nDigite 'S' para adicionar e 'N' para retornar ao menu\n");
                    system("Pause");
                    do{
                        system("cls");
                        printf("----------ADICIONAR NOVOS PRODUTOS?------------\nDigite 'S' para adicionar e 'N' para retornar ao menu\n");
                        scanf_s("%2s", selecionar_opcao, (unsigned)_countof(selecionar_opcao));
                        ParaMinúsculas(selecionar_opcao);
                        setConsoleColors(15, 0);

                        if (strcmp(selecionar_opcao, "s") == 0) {
                            int codigo;
                            char nome_produto[50];
                            float preco_kg;

                            printf("Digite o código do produto que deseja adicionar:\n");
                            scanf_s("%d", &codigo);
                            printf("Digite o nome do produto que deseja adicionar:\n");
                            scanf_s("%49s", nome_produto, (unsigned)_countof(nome_produto));
                            char* locale_atual = setlocale(LC_NUMERIC, NULL);
                            setlocale(LC_NUMERIC, "C");
                            printf("Digite o preço por KG do produto que deseja adicionar(utilize '.' ao invés de ','):\n");
                            scanf_s("%f", &preco_kg);
                            snprintf(query, sizeof(query), "INSERT INTO PRODUTOS (CODE, NOME, VALOR_KG) VALUES (%d, '%s', %.2f)", codigo, nome_produto, preco_kg);
                            setlocale(LC_NUMERIC, locale_atual);
                            printf("Consulta SQL: %s\n", query);
                            if (mysql_query(conn, query)) {
                                fprintf(stderr, "Erro na consulta: %s\n", mysql_error(conn));
                                system("pause");
                                return 1;
                            }
                            setConsoleColors(10, 0);
                            printf("Produto adicionado com sucesso!!\n");
                            system("pause");
                            limite = 3;
                        }
                        else if (strcmp(selecionar_opcao, "n") == 0) {
                            limite = 3;
                        }
                        else {
                            printf("Opção não indentificada: Digite novamente\n");
                            limite = 2;
                        }
                    } while (limite != 3);
                 }
                

                    else if (acesso == 1 && acesso2 == 0) {
                    setConsoleColors(4, 0);
                    printf("Login incorreto: \n");
                    if (limite == 0) {
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("2 TENTATIVAS RESTANTES POR SEGURANÇA\n");
                        limite++;
                        system("pause");
                    }
                    else if (limite == 1) {
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("!! 1 TENTATIVA RESTANTE POR SEGURANÇA!!\n");
                        limite++;
                        system("pause");
                    }
                    else if (limite == 2) {
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("SUAS TENTATIVAS ACABARAM\n TENTE NOVAMENTE MAIS TARDE\n");
                        limite++;
                        system("pause");
                        exit(0);
                    }

                }
                else if (acesso == 0 && acesso2 == 1) {
                    setConsoleColors(4, 0);
                    printf("Senha incorreta: \n");
                    if (limite == 0) {
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("2 TENTATIVAS RESTANTES POR SEGURANÇA\n");
                        limite++;
                        system("pause");
                    }
                    else if (limite == 1) {
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("!! 1 TENTATIVA RESTANTE POR SEGURANÇA!!\n");
                        limite++;
                        system("pause");
                    }
                    else if (limite == 2) {
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("SUAS TENTATIVAS ACABARAM\n TENTE NOVAMENTE MAIS TARDE\n");
                        limite++;
                        system("pause");
                        exit(0);
                    }
                }
                else {
                    setConsoleColors(4, 0);
                    printf("Login e Senha incorretos \n");
                    if (limite == 0) {
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("2 TENTATIVAS RESTANTES POR SEGURANÇA\n");
                        limite++;
                        system("pause");
                    }else if (limite == 1) {
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("!! 1 TENTATIVA RESTANTE POR SEGURANÇA!!\n");
                        limite++;
                        system("pause");
                    }else if (limite == 2){
                        setConsoleColors(14, 0);
                        printf("----------------TENTE NOVAMENTE------------------\n");
                        printf("SUAS TENTATIVAS ACABARAM\n TENTE NOVAMENTE MAIS TARDE\n");
                        limite++;
                        system("pause");
                        exit(0);
                    }
                       
                }
            } while (limite!=3);


        }
    } while (1);

    mysql_close(conn);
    return 0;
}
