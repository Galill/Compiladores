#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stack>

#define PATH "teste.ec1"

struct token {
    std::string tipo;
    std::string lexema;
    int posicao;
};

int main(int argc, char const *argv[]) {   
    int pos;
    std::string line, fe;
    std::vector<token> tokens;
    std::stack<int> pilhaParen, pilhaNum, pilhaOp;
    std::map <std::string, std::string> dicionario;
    std::ifstream file;

    dicionario["+"] = "Soma";
    dicionario["-"] = "Subtracao";
    dicionario["*"] = "Multiplicacao";
    dicionario["/"] = "Divisao";
    dicionario["("] = "ParenEsq";
    dicionario[")"] = "ParenDir";
    dicionario[" "] = "Vazio";

    file.open(PATH);
    if(!file.is_open()) {
        std::cout << "Erro! Nao foi possivel abrir o arquivo!" << std::endl; 
        exit(1);
    }

    std::getline(file,line);
    for(int i = 0; i < line.size(); i++) {
        try {
            pos = i - fe.size();
            std::string s;
            s.push_back(line[i]);
            std::string tipo = dicionario.at(s);

            if(fe != "") {
                tokens.push_back({"Numero", fe, pos});  
                fe = "";
            }

            if(tipo != "Vazio") {
                tokens.push_back({tipo, s, i});
            }

        } catch (const std::out_of_range& e) {

            //VERIFICA A GRAMÁTICA
            if (isdigit(line[i])) {
                fe.push_back(line[i]);
            } else {
                std::cerr << "Erro! Caractere invalido! " << std::endl;
                exit(1);
            }
        }
    }


    ///////////////////REMENDO////////////////////

    //PRA QUANDO A ENTRADA É SÓ UM NÚMERO EX: 22
    if (!fe.empty()) {
        tokens.push_back({"Numero", fe, 0});
    }
    //////////////////////////////////////////////


    ///////////REMENDO? (possivelmente)///////////
    //REMENDO DMS QUANTO MAIS EU MEXO MAIS DESGOSTO DÁ PQPPPP
    //ATUALIZAR AS MENSAGENS DE ERRO
    //ESSES MILHÕES DE PILHAS VÃO REFRESCAR A CABESA DE VCS DE TEORIA E A GENTE VAI CONSEGUIR DEIXAR MENOS BUCHA

    //PERCORRE OS TOKENS BUSCANDO ERROS
    for (int i = 0; i < tokens.size(); i++) {

        //PRA NÃO PERMITIR OPERAÇÕES SOZINHAS COMO ENTRADA
        // if(tokens.size() == 1 && tokens[i].tipo != "Numero"){
        //     std::cerr << "Erro! Numero incorreto de operacoes no codigo!" << std::endl;
        //         exit(1);
        // }


        /////////TRATAMENTO DE PARENTESES/////////

        if(tokens[i].tipo == "ParenEsq") {
            pilhaParen.push(1);

            //PRA NÃO PERMITIR UM OPERADOR APÓS ABRIR UM PARENTESE EX: ( - 1 * 4)  QUEM SABE QUE FEZ
            if(i + 1 < tokens.size() && (tokens[i+1].tipo == "Soma" || tokens[i+1].tipo == "Subtracao" || tokens[i+1].tipo == "Multiplicacao" || tokens[i+1].tipo == "Divisao")) {
                std::cout << "Erro! Numero incorreto de operacoes no codigo!" << std::endl;
                exit(1);
            }
        }

        if(tokens[i].tipo == "ParenDir") {
            if(pilhaParen.empty()){
                std::cout << "Erro! Numero incorreto de parenteses no codigo!" << std::endl;
                exit(1); 
            }
            pilhaParen.pop();
        }
        //////////////////////////////////////////


        ////TRATAMENTO DE OPERAÇÕES E NÚMEROS/////

        //QUANDO ENCONTRA UM NÚMERO, PÕE UM VALOR NA PILHA DO NÚMERO, ESSE VALOR SÓ É TIRADO QUANDO ELE ENCONTRAR UMA OPERAÇÃO 
        if (tokens[i].tipo == "Numero") {
            pilhaNum.push(1);
            if (!pilhaOp.empty()) {
                pilhaOp.pop();
            }
        }

        // QUANDO ENCONTRA UMA OPERAÇÃO, PÕE UM VALOR NA PILHA DAS OPERAÇÕES, ESSE VALOR SÓ É TIRADO QUANDO ELE ENCONTRAR UM NÚMERO
        if (tokens[i].tipo == "Soma" || tokens[i].tipo == "Subtracao" ||  tokens[i].tipo == "Multiplicacao" || tokens[i].tipo == "Divisao") {
            if(pilhaParen.empty()){
                std::cout << "Erro! Operacao sem parenteses" << std::endl;
                exit(1);
            }
            pilhaOp.push(1);
            if (!pilhaNum.empty()) {
                pilhaNum.pop();
            }     
        }

        // OU SEJA, SE ELE ACHAR UM NÚMERO, TEM QUE VIR UMA OPERAÇÃO DPS E QUANDO ELE ACHAR UMA OPERAÇÃO TEM QUE VIR UM NÚMERO
        //////////////////////////////////////////

    }
    //////////////////////////////////////////////


    // SE A PILHA DE PARENTES NÃO ESTIVER VAZIA TEM UM PARENTESE EXTRA NA ENTRADA 
    if(!pilhaParen.empty()) {
        std::cout << "Erro! Parenteses errados! " << std::endl;
        exit(1);
    }

    // A PILHA DE NÚMEROS SEMPRE TEM QUE TER UM NÚMERO POIS A RELAÇÃO NÚMEROS OPERADORES É DE 2 PRA 1
    if (pilhaNum.size() != 1) {
        std::cout << "Erro! A pilha de numeros sempre tem que ter um numero pos a relacao numero operadores e de 2 para 1!" << std::endl;
        exit(1);
    }

    if (!pilhaOp.empty()) {
        std::cout << "Erro! Numero incorreto de operacoes no codigo!" << std::endl;
        exit(1);
    }

    for (int i = 0; i < tokens.size(); i++) {
        std::cout << "<" << tokens[i].tipo << ", " << tokens[i].lexema << ", " << tokens[i].posicao << ">" << std::endl;
    }

    return 0;
}
