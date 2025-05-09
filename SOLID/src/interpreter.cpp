#include "../include/interpreter.h"
#include <stdexcept>
#include <map>

using std::shared_ptr;
using std::stoi;
using std::runtime_error;
using std::string;
using std::map;
using std::make_shared;

map<string, shared_ptr<Node>> funcoesGlobais;

int interpretarChamada(shared_ptr<Node> chamada, map<string, int>& ambiente) {
    string nome = chamada->valor;

    if (!funcoesGlobais.count(nome)) {
        throw runtime_error("Erro: função '" + nome + "' não foi definida.");
    }

    auto func = funcoesGlobais[nome];

    // Verifica número de argumentos
    if (func->filhos.size() != chamada->filhos.size()) {
        throw runtime_error("Erro: número de argumentos incompatível na chamada de '" + nome + "'.");
    }

    map<string, int> escopoLocal;

    // Passa parâmetros
    for (size_t i = 0; i < chamada->filhos.size(); ++i) {
        int val = interpretar(chamada->filhos[i], ambiente);
        string nomeParam = func->filhos[i]->valor;
        escopoLocal[nomeParam] = val;
    }

    // Inicializa variáveis locais, se houver
    for (const auto& local : func->locais) {
        escopoLocal[local] = 0;
    }

    // Executa comandos do corpo até encontrar Retorno
    for (const auto& cmd : func->comandos) {
        if (cmd->tipo == "Retorno") {
            // Quando chegar no retorno, avalia e devolve
            return interpretar(cmd->esquerda, escopoLocal);
        }
        interpretar(cmd, escopoLocal);
    }

    // Se não tiver Retorno explícito, avalia expressão esquerda geral
    if (func->esquerda) {
        return interpretar(func->esquerda, escopoLocal);
    }

    return 0;
}

int interpretar(shared_ptr<Node> raiz, map<string, int>& ambiente) {
    if (!raiz) return 0;

    // Nó de retorno (dentro de função)
    if (raiz->tipo == "Retorno") {
        return interpretar(raiz->esquerda, ambiente);
    }

    // Declaração de variável (inicialização)
    if (raiz->tipo == "Declaracao") {
        int valor = interpretar(raiz->esquerda, ambiente);
        ambiente[raiz->valor] = valor;
        return 0;
    }

    // Atribuição (reatribui valor a variável existente)
    if (raiz->tipo == "Atribuicao") {
        int valor = interpretar(raiz->esquerda, ambiente);
        ambiente[raiz->valor] = valor;
        return 0;
    }
    
    // Programa: registra funções e chama main
    if (raiz->tipo == "Programa") {
        // Armazena todas as funções globais
        for (const auto& func : raiz->filhos) {
            if (func->tipo == "Funcao") {
                funcoesGlobais[func->valor] = func;
            }
        }

        if (!funcoesGlobais.count("main")) {
            throw runtime_error("Erro: função 'main' não encontrada.");
        }

        // Inicia execução pela função main
        return interpretarChamada(make_shared<Node>("Chamada", "main"), ambiente);
    }

    if (raiz->tipo == "Numero") return stoi(raiz->valor);

    if (raiz->tipo == "Variavel") {
        if (!ambiente.count(raiz->valor)) {
            throw runtime_error("Erro: variável '" + raiz->valor + "' não inicializada.");
        }
        return ambiente[raiz->valor];
    }

    if (raiz->tipo == "Operador") {
        int esq = interpretar(raiz->esquerda, ambiente);
        int dir = interpretar(raiz->direita, ambiente);
        if (raiz->valor == "+") return esq + dir;
        if (raiz->valor == "-") return esq - dir;
        if (raiz->valor == "*") return esq * dir;
        if (raiz->valor == "/") return esq / dir;
        throw runtime_error("Erro: operador aritmético inválido.");
    }

    if (raiz->tipo == "Comparacao") {
        int esq = interpretar(raiz->esquerda, ambiente);
        int dir = interpretar(raiz->direita, ambiente);
        if (raiz->valor == "==") return esq == dir;
        if (raiz->valor == "<")  return esq < dir;
        if (raiz->valor == ">")  return esq > dir;
        throw runtime_error("Erro: operador de comparação inválido.");
    }

    if (raiz->tipo == "If") {
        int cond = interpretar(raiz->esquerda, ambiente);
        if (cond) {
            for (auto& cmd : raiz->filhos[0]->comandos)
                interpretar(cmd, ambiente);
        } else if (raiz->filhos.size() > 1) {
            for (auto& cmd : raiz->filhos[1]->comandos)
                interpretar(cmd, ambiente);
        }
        return 0;
    }

    if (raiz->tipo == "While") {
        while (interpretar(raiz->esquerda, ambiente)) {
            for (auto& cmd : raiz->filhos[0]->comandos)
                interpretar(cmd, ambiente);
        }
        return 0;
    }

    if (raiz->tipo == "Chamada") {
        return interpretarChamada(raiz, ambiente);
    }

    throw runtime_error("Erro: tipo de nó inválido no interpretador: " + raiz->tipo);
}
