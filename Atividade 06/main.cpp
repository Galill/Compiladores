#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

// Estrutura para representar tokens
struct Token {
    std::string tipo;
    std::string lexema;
    int posicao;
};

// Estrutura para representar nós da AST
struct Node {
    std::string valor;
    std::shared_ptr<Node> esquerda;
    std::shared_ptr<Node> direita;

    Node(std::string val) : valor(val), esquerda(nullptr), direita(nullptr) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    int pos;

public:
    Parser(std::vector<Token> t) : tokens(t), pos(0) {}

    std::shared_ptr<Node> parseExpressao() {
        if (pos >= tokens.size()) {
            throw std::runtime_error("Erro sintatico: expressão inesperada.");
        }
        
        if (tokens[pos].tipo == "Numero") {
            std::shared_ptr<Node> node = std::make_shared<Node>(tokens[pos].lexema);
            pos++;
            return node;
        } else if (tokens[pos].tipo == "ParenEsq") {
            pos++; // Consome "("
            
            std::shared_ptr<Node> esquerda = parseExpressao();
            if (pos >= tokens.size() || (tokens[pos].tipo != "Soma" && tokens[pos].tipo != "Subtracao" && tokens[pos].tipo != "Multiplicacao" && tokens[pos].tipo != "Divisao")) {
                throw std::runtime_error("Erro sintatico: operador esperado.");
            }
            
            std::string operador = tokens[pos].lexema;
            pos++;
            
            std::shared_ptr<Node> direita = parseExpressao();
            
            if (pos >= tokens.size() || tokens[pos].tipo != "ParenDir") {
                throw std::runtime_error("Erro sintatico: fechamento de parentese esperado.");
            }
            pos++; // Consome ")"
            
            std::shared_ptr<Node> node = std::make_shared<Node>(operador);
            node->esquerda = esquerda;
            node->direita = direita;
            return node;
        } else {
            throw std::runtime_error("Erro sintatico: expressao invalida.");
        }
    }
};

// Interpretador para avaliar a AST
int interpretar(std::shared_ptr<Node> raiz) {
    if (!raiz->esquerda && !raiz->direita) {
        return std::stoi(raiz->valor);
    }
    int esq = interpretar(raiz->esquerda);
    int dir = interpretar(raiz->direita);
    
    if (raiz->valor == "+") return esq + dir;
    if (raiz->valor == "-") return esq - dir;
    if (raiz->valor == "*") return esq * dir;
    if (raiz->valor == "/") return esq / dir;
    
    throw std::runtime_error("Erro: operador inválido.");
}

// Função para imprimir a árvore de forma infixa
void imprimirInfixo(std::shared_ptr<Node> raiz) {
    if (!raiz) return;
    if (raiz->esquerda || raiz->direita) std::cout << "(";
    imprimirInfixo(raiz->esquerda);
    std::cout << raiz->valor;
    imprimirInfixo(raiz->direita);
    if (raiz->esquerda || raiz->direita) std::cout << ")";
}
void tolkenizer(std::vector<Token> &tokens, std::string line, std::map<std::string, std::string> dicionario){
    std::string numeroBuffer;
    for (int i = 0; i < line.size(); i++) {
        if (isspace(line[i])) continue;
        
        std::string s(1, line[i]);
        if (isdigit(line[i])) {
            numeroBuffer += line[i];
        } else {
            if (!numeroBuffer.empty()) {
                tokens.push_back(Token{"Numero", numeroBuffer, static_cast<int>(i - numeroBuffer.size())});
                numeroBuffer = "";
            }
            if (dicionario.count(s)) {
                tokens.push_back(Token{dicionario[s], s, static_cast<int>(i)});
            } else {
                std::cerr << "Erro! Caractere inválido: " << s << std::endl;
            }
        }
    }
    if (!numeroBuffer.empty()) {
        tokens.push_back(Token{"Numero", numeroBuffer, static_cast<int>(line.size() - numeroBuffer.size())});
    }
}
void analisa(std::vector<Token> tokens){    
    try {
        Parser parser(tokens);
        std::shared_ptr<Node> ast = parser.parseExpressao();
        std::cout << "Arvore infixa: ";
        imprimirInfixo(ast);
        std::cout << std::endl;
        std::cout << "Resultado da expressao: " << float (interpretar(ast)) << std::endl;
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char **argv) {
    std::vector<Token> tokens;
    std::map<std::string, std::string> dicionario = {
        {"+", "Soma"},
        {"-", "Subtracao"},
        {"*", "Multiplicacao"},
        {"/", "Divisao"},
        {"(", "ParenEsq"},
        {")", "ParenDir"}
    };

    std::ifstream file(argv[1]);
    std::string line;
    if (!file.is_open()) {
        std::cerr << "Erro! Nao foi possivel abrir o arquivo!" << std::endl;
        return 1;
    }
    //ANALISA OS CASOS CORRETOS
    while (std::getline(file, line)){  
        tolkenizer(tokens, line, dicionario);
        analisa(tokens);
        tokens.clear();
    }
    file.close();

    std::ifstream file2(argv[2]);
    if (!file2) {
        std::cerr << "Erro! Nao foi possivel abrir o arquivo!" << std::endl;
        return 1;
    }

    //ANALISA OS CASOS ERRADOS
    while (std::getline(file2, line)){
        std::getline(file2, line);
        
        tolkenizer(tokens, line, dicionario);
        analisa(tokens);
        tokens.clear();
    }
        
    file.close(); 
    return 0;
}
