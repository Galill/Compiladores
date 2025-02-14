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
    std::string line;
    std::ifstream file;
    std::map <std::string, std::string> dicionario;

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
    token Tokens[100];
    std::string fe;
    int ContadorToken = 0;
    for(int i = 0; i < line.size(); ++i){
      
        try{
            std::string s;
            s.push_back(line[i]);
            std::string tipo = dicionario.at(s);
            //coloca como token qualquer outro tipo que não seja numero
            // da problema pela natureza do código, já que isso é excutado antes
            // acaba botando algum parantese ou espaço vazio na frente de um número
            Tokens[ContadorToken].tipo = tipo;
            Tokens[ContadorToken].lexema = line[i];
            Tokens[ContadorToken].posicao = i;
            ContadorToken++;
            if(fe != ""){
                //só chega aqui se fe não estiver vazia, logo é um número
                Tokens[ContadorToken].tipo = "Numero";
                Tokens[ContadorToken].lexema = fe;
                Tokens[ContadorToken].posicao = i-fe.size()+1;
                ContadorToken++;
            }
            fe = "";
        }
        catch(const std::out_of_range& e){
            //só chega aqui se for número 
            //adiciona o numero para a fe
            fe.push_back(line[i]);
        }
    }
    for(int i = 0;i<ContadorToken;i++){
    std::cout << "<"<< Tokens[i].tipo << ", "<<  Tokens[i].lexema << ", " << Tokens[i].posicao << ">" << std::endl;
    }
    return 0;
}
