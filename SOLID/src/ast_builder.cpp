#include "../include/ast_builder.h"
#include <stdexcept>
#include <iostream>

using std::shared_ptr;
using std::vector;
using std::runtime_error;
using std::make_shared;
using std::string;

shared_ptr<Node> ASTBuilder::build(const vector<shared_ptr<Token>> &tokens) {
    int pos = 0;
    symbolTable.clear();
    return parseProgramaFun(tokens, pos);
}
   
    shared_ptr<Node> ASTBuilder::parseProgramaFun(const vector<shared_ptr<Token>> &tokens, int &pos) {
     map<string, shared_ptr<Node>> funcoes;
    
     while (pos < tokens.size() && tokens[pos]->lexema == "fun") {
      auto func = parseFuncDecl(tokens, pos);
      string nome = func->valor;
      if (funcoes.count(nome)) {
       throw runtime_error("Erro: função '" + nome + "' já foi definida.");
      }
      funcoes[nome] = func;
     }
    
    
     if (pos >= tokens.size() || tokens[pos]->lexema != "main") {
      throw runtime_error("Erro: esperado 'main' após declarações de função.");
     }
     pos++; // consome 'main'
     if (tokens[pos]->lexema != "{") {
      throw runtime_error("Erro: esperado '{' após 'main'.");
     }
     pos++; // consome '{'
    
     // Parseia corpo de main
     vector<shared_ptr<Node>> comandos;
     // Modifique esta condição do loop while
     while (pos < tokens.size() && tokens[pos]->tipo != "ChaveDir") {
        comandos.push_back(parseCmd(tokens, pos));
        if (pos < tokens.size() && tokens[pos]->tipo == "PontoVirgula") {
            pos++;
        }
    }

    if (pos >= tokens.size() || tokens[pos]->tipo != "ChaveDir") {
        throw runtime_error("Erro: esperado '}' no fim da função.");
    }
    pos++;
     
     // Se o loop terminou porque encontrou 'return', parseie o retorno
     if (tokens[pos-1]->lexema == "return") {
      auto retorno = parseExpCmp(tokens, pos);
      if (tokens[pos]->lexema != ";") {
       throw runtime_error("Erro: esperado ';' após return em main.");
      }
      pos++; // consome ';'
    
      // Monta e adiciona nó de retorno em main
      auto retNode = make_shared<Node>("Retorno", "return");
      retNode->esquerda = retorno;
      comandos.push_back(retNode);
     }
    
     if (tokens[pos]->lexema != "}") {
      throw runtime_error("Erro: esperado '}' ao final de main.");
     }
     pos++;
    
     // Monta o nó main como se fosse uma Funcao
     auto mainNode = make_shared<Node>("Funcao", "main");
     mainNode->comandos = comandos;
     // armazenamos também a expressão de retorno para geração de código
     mainNode->esquerda = nullptr; // deixamos nullptr pois usamos comandos
     funcoes["main"] = mainNode;
    
     // Construção do programa
     auto programa = make_shared<Node>("Programa", "Raiz");
     for (auto &p : funcoes) {
      programa->filhos.push_back(p.second);
     }
     funcoesDeclaradas = funcoes;
     return programa;
    }


    shared_ptr<Node> ASTBuilder::parseFuncDecl(const std::vector<std::shared_ptr<Token>>& tokens, int& pos) {
        std::vector<std::shared_ptr<Node>> comandos;

        // consome 'fun'
        pos++;
        if (pos >= tokens.size() || tokens[pos]->tipo != "Identificador") {
            throw std::runtime_error("Erro: esperado nome da função após 'fun'.");
        }
        // nome da função
        std::string nomeFunc = tokens[pos]->lexema;
        pos++;

        // abre parênteses de parâmetros
        if (pos >= tokens.size() || tokens[pos]->lexema != "(") {
            throw std::runtime_error("Erro: esperado '(' após o nome da função.");
        }
        pos++; // consome '('

        // ======== 1) PARÂMETROS =========
        std::vector<std::shared_ptr<Node>> parametros;
        while (pos < tokens.size() && tokens[pos]->lexema != ")") {
            if (tokens[pos]->tipo != "Identificador") {
                throw std::runtime_error("Erro: esperado identificador como parâmetro.");
            }
            std::string nomeParam = tokens[pos]->lexema;
            auto paramNode = std::make_shared<Node>("Parametro", nomeParam);
            parametros.push_back(paramNode);
            // <<< aqui inserimos o parâmetro na tabela !!!
            symbolTable.insert(nomeParam);
            pos++; // consome o identificador

            if (pos < tokens.size() && tokens[pos]->lexema == ",") {
                pos++; // consome vírgula
                if (pos < tokens.size() && tokens[pos]->lexema == ")") {
                    throw std::runtime_error("Erro: vírgula extra antes de ')'.");
                }
            } else if (pos < tokens.size() && tokens[pos]->lexema != ")") {
                throw std::runtime_error("Erro: esperado ',' ou ')' após parâmetro.");
            }
        }
        if (pos >= tokens.size() || tokens[pos]->lexema != ")") {
            throw std::runtime_error("Erro: esperado ')' após a lista de parâmetros.");
        }
        pos++; // consome ')'

        // abre chaves do corpo
        if (pos >= tokens.size() || tokens[pos]->lexema != "{") {
            throw std::runtime_error("Erro: esperado '{' após a lista de parâmetros.");
        }
        pos++; // consome '{'

        // ======== 2) VARIÁVEIS LOCAIS =========
        std::vector<std::string> locals;
        while (pos < tokens.size() && tokens[pos]->lexema == "var") {
            pos++; // consome 'var'
            if (pos >= tokens.size() || tokens[pos]->tipo != "Identificador") {
                throw std::runtime_error("Erro: esperado nome da variável local após 'var'.");
            }
            std::string varName = tokens[pos]->lexema;
            pos++;

            // insere local na tabela antes de parsear o inicializador
            symbolTable.insert(varName);
            locals.push_back(varName);

            if (pos >= tokens.size() || tokens[pos]->lexema != "=") {
                throw std::runtime_error("Erro: esperado '=' após o nome da variável local.");
            }
            pos++; // consome '='
            auto init = parseExpCmp(tokens, pos);

            if (pos >= tokens.size() || tokens[pos]->lexema != ";") {
                throw std::runtime_error("Erro: esperado ';' após inicialização da variável local.");
            }
            pos++; // consome ';'

            // cria nó de declaração e guarda em comandos
            auto declNode = std::make_shared<Node>("Declaracao", varName);
            declNode->esquerda = init;
            comandos.push_back(declNode);
        }

        // ======== 3) COMANDOS DO CORPO =========
        while (pos < tokens.size() && tokens[pos]->lexema != "return" && tokens[pos]->lexema != "}") {
            comandos.push_back(parseCmd(tokens, pos));
            // O parseCmd já deve consumir o ponto e vírgula se necessário para comandos individuais
        }

        std::shared_ptr<Node> retorno = nullptr;
        if (pos < tokens.size() && tokens[pos]->lexema == "return") {
            pos++; // consome 'return'
            retorno = parseExpCmp(tokens, pos);
            if (pos >= tokens.size() || tokens[pos]->lexema != ";") {
                throw std::runtime_error("Erro: esperado ';' após return.");
            }
            pos++; // consome ';'
        }

        // fecha chaves da função
        if (pos >= tokens.size() || tokens[pos]->lexema != "}") {
            throw std::runtime_error("Erro: esperado '}' no fim da função.");
        }
        pos++;

        // ======== 4) MONTA O NÓ FUNÇÃO =========
        auto func = std::make_shared<Node>("Funcao", nomeFunc);
        func->filhos = parametros;
        func->locais = locals;
        func->comandos = comandos;
        func->esquerda = retorno;

        // ======== 5) "Desfaz" o escopo da função =========
        for (const auto& p : parametros) {
            symbolTable.erase(p->valor);
        }
        for (const auto& v : locals) {
            symbolTable.erase(v);
        }

        return func;
}

shared_ptr<Node> ASTBuilder::parseVarDecl(const vector<shared_ptr<Token>> &tokens, int &pos) {
    //std::cout << "Entrou em parseVarDecl na posição: " << pos << ", token: " << tokens[pos]->lexema << std::endl;
    string varName = tokens[pos]->lexema;
    pos++;
    //std::cout << "Leu nome da variável: " << varName << ", nova posição: " << pos
              //<< ", próximo token: " << (pos < tokens.size() ? tokens[pos]->lexema : "Fim dos tokens") << std::endl;

    if (tokens[pos]->lexema != "=") {
        throw runtime_error("Erro sintático: '=' esperado após identificador.");
    }
    pos++;

    auto expr = parseExpA(tokens, pos);

    if (pos >= tokens.size() || tokens[pos]->lexema != ";") {
        throw runtime_error("Erro sintático: ';' esperado após expressão.");
    }
    pos++;

    if (symbolTable.count(varName)) {
        throw runtime_error("Erro semântico: variável '" + varName + "' já declarada.");
    }
    symbolTable.insert(varName);

    auto declNode = make_shared<Node>("Declaracao", varName);
    declNode->esquerda = expr;
    return declNode;
}

shared_ptr<Node> ASTBuilder::parseCmd(const vector<shared_ptr<Token>> &tokens, int &pos) {
    if (tokens[pos]->lexema == "if") return parseIf(tokens, pos);
    if (tokens[pos]->lexema == "while") return parseWhile(tokens, pos);
    if (tokens[pos]->lexema == "return") return parseRetorno(tokens, pos);
    if (tokens[pos]->tipo == "Identificador") return parseAtrib(tokens, pos);
    throw runtime_error("Erro: comando inválido na posição: " + std::to_string(tokens[pos]->posicao) + ", token: " + tokens[pos]->lexema);
}
shared_ptr<Node> ASTBuilder::parseRetorno(const vector<shared_ptr<Token>> &tokens, int &pos) {
    pos++; // Consome o token "return"

    shared_ptr<Node> expr = nullptr;
    // Verifica se há uma expressão a ser retornada
    if (pos < tokens.size() && tokens[pos]->lexema != ";") {
        expr = parseExpCmp(tokens, pos); // Analisa a expressão de retorno usando sua hierarquia
    }

    shared_ptr<Node> returnNode = make_shared<Node>("Retorno", "return");
    returnNode->esquerda = expr; // A expressão de retorno é o filho esquerdo
    return returnNode;
}
std::shared_ptr<Node> ASTBuilder::parseIf(const std::vector<std::shared_ptr<Token>> &tokens, int &pos) {
    pos++; // Consome "if"
    auto condicao = parseExpCmp(tokens, pos);

    if (pos >= tokens.size() || tokens[pos]->tipo != "ChaveEsq") {
        throw std::runtime_error("Erro: esperado '{' após a condição do if.");
    }
    std::vector<std::shared_ptr<Node>> blocoIf = parseBloco(tokens, pos);

    std::vector<std::shared_ptr<Node>> blocoElse;
    if (pos < tokens.size() && tokens[pos]->lexema == "else") {
        pos++;
        if (pos >= tokens.size() || tokens[pos]->tipo != "ChaveEsq") {
            throw std::runtime_error("Erro: esperado '{' após o else.");
        }
        blocoElse = parseBloco(tokens, pos);
    }

    auto noIf = std::make_shared<Node>("If", "if");
    noIf->esquerda = condicao;
    auto blocoIfNode = std::make_shared<Node>("Bloco", "");
    blocoIfNode->comandos = blocoIf;
    auto blocoElseNode = std::make_shared<Node>("Bloco", "");
    blocoElseNode->comandos = blocoElse;
    noIf->filhos.push_back(blocoIfNode);
    noIf->filhos.push_back(blocoElseNode);

    return noIf;
}

std::vector<std::shared_ptr<Node>> ASTBuilder::parseBloco(const std::vector<std::shared_ptr<Token>> &tokens, int &pos) {
    std::vector<std::shared_ptr<Node>> comandos;
    pos++; // Consome a chave de abertura '{'

    while (pos < tokens.size() && tokens[pos]->tipo != "ChaveDir") {
        comandos.push_back(parseCmd(tokens, pos));
        if (pos < tokens.size() && tokens[pos]->tipo == "PontoVirgula") {
            pos++;
        }
    }

    if (pos >= tokens.size() || tokens[pos]->tipo != "ChaveDir") {
        throw std::runtime_error("Erro: esperado '}' no fim do bloco.");
    }
    pos++; // Consome a chave de fechamento '}'
    return comandos;
}
std::shared_ptr<Node> ASTBuilder::parseWhile(const std::vector<std::shared_ptr<Token>> &tokens, int &pos) {
    pos++; // Consome "while"
    auto condicao = parseExpCmp(tokens, pos);

    if (pos >= tokens.size() || tokens[pos]->tipo != "ChaveEsq") {
        throw std::runtime_error("Erro: esperado '{' após a condição do while.");
    }
    std::vector<std::shared_ptr<Node>> corpoWhile = parseBloco(tokens, pos);

    auto noWhile = std::make_shared<Node>("While", "while");
    noWhile->esquerda = condicao;
    auto corpoNode = std::make_shared<Node>("Bloco", "");
    corpoNode->comandos = corpoWhile;
    noWhile->filhos.push_back(corpoNode);

    return noWhile;
}

shared_ptr<Node> ASTBuilder::parseAtrib(const vector<shared_ptr<Token>> &tokens, int &pos) {
    string var = tokens[pos]->lexema;
    if (!symbolTable.count(var)) {
        throw runtime_error("Erro: variável '" + var + "' não declarada");
    }
    pos++;
    if (tokens[pos]->tipo != "Atribuicao") throw runtime_error("Erro: esperado '=' na atribuição");
    pos++;
    auto expr = parseExpCmp(tokens, pos);
    if (tokens[pos]->tipo != "PontoVirgula") throw runtime_error("Erro: esperado ';' após atribuição");
    pos++;
    auto no = make_shared<Node>("Atribuicao", var);
    no->esquerda = expr;
    return no;
}

shared_ptr<Node> ASTBuilder::parseExpCmp(const vector<shared_ptr<Token>> &tokens, int &pos) {
    shared_ptr<Node> esquerda = parseExpA(tokens, pos);
    while (pos < tokens.size() &&
        (tokens[pos]->tipo == "Igualdade" || tokens[pos]->tipo == "Menor" || tokens[pos]->tipo == "Maior")) {
        string op = tokens[pos]->lexema;
        pos++;
        auto direita = parseExpA(tokens, pos);
        auto cmp = make_shared<Node>("Comparacao", op);
        cmp->esquerda = esquerda;
        cmp->direita = direita;
        esquerda = cmp;
    }
    return esquerda;
}

shared_ptr<Node> ASTBuilder::parseExpA(const vector<shared_ptr<Token>> &tokens, int &pos) {
    shared_ptr<Node> esquerda = parseExpM(tokens, pos);

    while (pos < tokens.size() && (tokens[pos]->tipo == "Soma" || tokens[pos]->tipo == "Subtracao")) {
        string operador = tokens[pos]->lexema;
        pos++;
        shared_ptr<Node> direita = parseExpM(tokens, pos);
        shared_ptr<Node> node = make_shared<Node>("Operador", operador);
        node->esquerda = esquerda;
        node->direita = direita;
        esquerda = node;
    }

    return esquerda;
}

shared_ptr<Node> ASTBuilder::parseExpM(const vector<shared_ptr<Token>> &tokens, int &pos) {
    shared_ptr<Node> esquerda = parsePrim(tokens, pos);

    while (pos < tokens.size() && (tokens[pos]->tipo == "Multiplicacao" || tokens[pos]->tipo == "Divisao")) {
        string operador = tokens[pos]->lexema;
        pos++;
        shared_ptr<Node> direita = parsePrim(tokens, pos);
        shared_ptr<Node> node = make_shared<Node>("Operador", operador);
        node->esquerda = esquerda;
        node->direita = direita;
        esquerda = node;
    }

    return esquerda;
}

shared_ptr<Node> ASTBuilder::parsePrim(const vector<shared_ptr<Token>> &tokens, int &pos) {
    if (pos >= tokens.size()) {
        throw runtime_error("Erro sintático: expressão inesperada (fim da entrada).");
    }

    if (tokens[pos]->tipo == "Identificador") {
        string nome = tokens[pos]->lexema;
        pos++;
    
        // Verifica se é chamada de função
        if (pos < tokens.size() && tokens[pos]->lexema == "(") {
            pos++; // consome '('
            vector<shared_ptr<Node>> args;
    
            if (tokens[pos]->lexema != ")") {
                while (true) {
                    auto arg = parseExpCmp(tokens, pos); // os argumentos podem ser expressões completas
                    args.push_back(arg);
    
                    if (tokens[pos]->lexema == ")") break;
                    if (tokens[pos]->lexema != ",") {
                        throw runtime_error("Erro: esperado ',' entre argumentos.");
                    }
                    pos++; // consome ','
                }
            }
            pos++; // consome ')'
    
            auto chamada = make_shared<Node>("Chamada", nome);
            chamada->filhos = args;
            return chamada;
        }
    
        // Caso contrário, é uma variável
        if (!symbolTable.count(nome)) {
            throw runtime_error("Erro semântico: variável '" + nome + "' não declarada.");
        }
        return make_shared<Node>("Variavel", nome);
    }

    if (tokens[pos]->tipo == "Numero") {
        shared_ptr<Node> node = make_shared<Node>("Numero", tokens[pos]->lexema);
        pos++;
        return node;
    } else if (tokens[pos]->tipo == "ParenEsq") {
        pos++;
        shared_ptr<Node> node = parseExpA(tokens, pos);
        if (pos >= tokens.size() || tokens[pos]->tipo != "ParenDir") {
            throw runtime_error("Erro sintático: fechamento do parênteses esperado.");
        }
        pos++;
        return node;
    } else {
        throw runtime_error("Erro sintático: expressão inválida.");
    }
}
