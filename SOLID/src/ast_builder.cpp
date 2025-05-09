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
     while (pos < tokens.size() && tokens[pos]->lexema != "return" && tokens[pos]->lexema != "}") {
      if (tokens[pos]->lexema == "var") {

       pos++; // Consome o 'var'

       if (pos < tokens.size() && tokens[pos]->tipo == "Identificador") {
        std::cout << "Nome da variável encontrado na posição: " << pos << ", token: " << tokens[pos]->lexema << std::endl;
        comandos.push_back(parseVarDecl(tokens, pos));
       } 
       else {
        throw runtime_error("Erro sintático: nome de variável esperado após 'var'.");
       }
      } 
      else {
       comandos.push_back(parseCmd(tokens, pos));
      }
     }
     pos++; // consome 'return' ou '}'
     
     // Se o loop terminou porque encontrou 'return', parseia o retorno
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


shared_ptr<Node> ASTBuilder::parseFuncDecl(const vector<shared_ptr<Token>> &tokens, int &pos){
    vector<shared_ptr<Node>> comandos;    
    // consome 'fun'
    pos++;
    // nome da função
    string nomeFunc = tokens[pos]->lexema;
    pos++;

    // abre parênteses de parâmetros
    pos++; // consome '('

    // ======== 1) PARÂMETROS =========
    vector<shared_ptr<Node>> parametros;
    while (tokens[pos]->lexema != ")") {
        // cada parâmetro é identificador
        string nomeParam = tokens[pos]->lexema;
        auto paramNode = make_shared<Node>("Parametro", nomeParam);
        parametros.push_back(paramNode);
        
        symbolTable.insert(nomeParam); // aqui inserimos o parâmetro na tabela 

        pos++; // consome o identificador
        if (tokens[pos]->lexema == ",") pos++; // consome vírgula
    }
    pos++; // consome ')'

    // abre chaves do corpo
    pos++; // consome '{'

    // ======== 2) VARIÁVEIS LOCAIS =========
    vector<string> locals;
    while (tokens[pos]->lexema == "var") {
        pos++; // consome 'var'
        string varName = tokens[pos]->lexema;
        pos++;

        // insere local na tabela antes de parsear o inicializador
        symbolTable.insert(varName);
        locals.push_back(varName);

        // inicializador
        pos++; // consome '='
        auto init = parseExpCmp(tokens, pos);

        if (tokens[pos]->lexema != ";")
            throw runtime_error("Erro: esperado ';' após declaração de variável local.");
        pos++; // consome ';'

        // cria nó de declaração e guarda em comandos
        auto declNode = make_shared<Node>("Declaracao", varName);
        declNode->esquerda = init;
        comandos.push_back(declNode);
    }

    // ======== 3) COMANDOS DO CORPO =========
    
    while (tokens[pos]->lexema != "return") {
        comandos.push_back(parseCmd(tokens, pos));
    
        if (pos < tokens.size() && tokens[pos]->tipo == "PontoVirgula") {
            pos++;
        }
    }

    pos++; // consome 'return'
    auto retorno = parseExpCmp(tokens, pos);
    if (tokens[pos]->lexema != ";")
        throw runtime_error("Erro: esperado ';' após return.");
    pos++; // consome ';'

    // fecha chaves da função
    if (tokens[pos]->lexema != "}")
        throw runtime_error("Erro: esperado '}' no fim da função.");
    pos++;

    // ======== 4) MONTA O NÓ FUNÇÃO =========
    auto func = make_shared<Node>("Funcao", nomeFunc);
    func->filhos   = parametros;
    func->locais   = locals;
    func->comandos = comandos;
    func->esquerda = retorno;

    // ======== 5) "Desfaz" o escopo da função =========
    for (auto &p : parametros)
        symbolTable.erase(p->valor);
    for (auto &v : locals)
        symbolTable.erase(v);

    return func;
}

shared_ptr<Node> ASTBuilder::parseVarDecl(const vector<shared_ptr<Token>> &tokens, int &pos) {

    string varName = tokens[pos]->lexema;
    pos++;

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
shared_ptr<Node> ASTBuilder::parseIf(const vector<shared_ptr<Token>> &tokens, int &pos) {
    pos++; // Consome "if"
    auto cond = parseExpCmp(tokens, pos);

    if (tokens[pos]->tipo != "ChaveEsq") throw runtime_error("Erro: esperado '{' após if");
    pos++;
    vector<shared_ptr<Node>> cmdVerdadeiro;
    while (tokens[pos]->tipo != "ChaveDir") {
        cmdVerdadeiro.push_back(parseCmd(tokens, pos));
        // Consome o ponto e vírgula após o comando, se presente
        if (pos < tokens.size() && tokens[pos]->tipo == "PontoVirgula") {
            pos++;
        }
    }
    pos++; // Consome "}"

    if (pos >= tokens.size() || tokens[pos]->lexema != "else") throw runtime_error("Erro: esperado 'else'");
    pos++;
    if (tokens[pos]->tipo != "ChaveEsq") throw runtime_error("Erro: esperado '{' após else");
    pos++;
    vector<shared_ptr<Node>> cmdFalso;
    while (tokens[pos]->tipo != "ChaveDir") {
        cmdFalso.push_back(parseCmd(tokens, pos));
        // Consome o ponto e vírgula após o comando, se presente
        if (pos < tokens.size() && tokens[pos]->tipo == "PontoVirgula") {
            pos++;
        }
    }
    pos++; // Consome "}"

    auto noIf = make_shared<Node>("If", "if");
    noIf->esquerda = cond;
    auto bloco1 = make_shared<Node>("Bloco", "");
    bloco1->comandos = cmdVerdadeiro;
    auto bloco2 = make_shared<Node>("Bloco", "");
    bloco2->comandos = cmdFalso;
    noIf->filhos.push_back(bloco1);
    noIf->filhos.push_back(bloco2);
    return noIf;
}


shared_ptr<Node> ASTBuilder::parseWhile(const vector<shared_ptr<Token>> &tokens, int &pos) {
    pos++;
    auto cond = parseExpCmp(tokens, pos);
    if (tokens[pos]->tipo != "ChaveEsq") throw runtime_error("Erro: esperado '{' no while");
    pos++;
    vector<shared_ptr<Node>> comandos;
    while (tokens[pos]->tipo != "ChaveDir") {
        comandos.push_back(parseCmd(tokens, pos));
    }
    pos++;
    auto no = make_shared<Node>("While", "while");
    no->esquerda = cond;
    auto bloco = make_shared<Node>("Bloco", "");
    bloco->comandos = comandos;
    no->filhos.push_back(bloco);
    return no;
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
                    auto arg = parseExpCmp(tokens, pos); // os argumentos podem ser expressões completasd
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
