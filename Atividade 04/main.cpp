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
    std::stack<int> pilha;
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
        std::cout << "Nao foi possivel abrir o arquivo" << std::endl; 
        exit(1);
    }

    std::getline(file,line);
    for(int i = 0; i < line.size(); i++) {
        try {
            pos = i - fe.size();
            std::string s;
            s.push_back(line[i]);
            std::string tipo = dicionario.at(s);

            if(tipo == "ParenEsq") {
                pilha.push(1);
            }

            if(tipo == "ParenDir") {
                if(pilha.empty()){
                    std::cout << "Erro: numero incorreto de parenteses no codigo " << std::endl;
                    exit(1); 
                }
                pilha.pop();
            }

            if(fe != "") {
                tokens.push_back({"Numero", fe, pos});  
                fe = "";
            }

            if(tipo != "Vazio") {
                tokens.push_back({tipo, s, i});
            }

        } catch (const std::out_of_range& e) {
            fe.push_back(line[i]);
        }
    }

    if(!pilha.empty()){
        std::cout << "Erro: numero incorreto de parenteses no codigo " << std::endl;
        exit(1);
    }

    for (int i = 0; i < tokens.size(); i++) {
        std::cout << "<" << tokens[i].tipo << ", " << tokens[i].lexema << ", " << tokens[i].posicao << ">" << std::endl;
    }

    return 0;
}
