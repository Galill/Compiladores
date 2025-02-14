//compilador ECI (Expressões )
#define PATH "teste.ec1"
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <map>
#include <vector>

struct token
{
    std::string tipo;
    std::string lexema;
    int posicao;
};

int main(int argc, char const *argv[])
{
    std::string line, fe;
    std::vector<token> Tokens;
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
    if(!file.is_open()){
        std::cout << "Não foi possivel abrir o arquivo" << std::endl; 
        exit(1);
    }
    
    std::getline(file,line);
    int pos;
    for(int i = 0; i < line.size(); i++){
        try {
            pos = i - fe.size();
            std::string s;
            s.push_back(line[i]);
            std::string tipo = dicionario.at(s);
            if(fe != ""){
                Tokens.push_back({"Numero", fe, pos});  
                fe = "";
            }

            if(tipo != "Vazio"){
                Tokens.push_back({tipo, s, i});
            }

        } catch (const std::out_of_range& e) {
            fe.push_back(line[i]);
        }
    }

    for (int i = 0; i < Tokens.size(); i++) {
        std::cout << "<" << Tokens[i].tipo << ", " << Tokens[i].lexema << ", " << Tokens[i].posicao << ">" << std::endl;
    }

    return 0;
}
