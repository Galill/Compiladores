#include "../include/interpreter.h"
#include <stdexcept>
#include <map>

using std::shared_ptr;
using std::stoi;
using std::runtime_error;
using std::string;
using std::map;

int interpretar(shared_ptr<Node> raiz, map<string, int>& ambiente) {
    if (!raiz) return 0;

    if (raiz->tipo == "Numero") {
        return stoi(raiz->valor);
    }

    if (raiz->tipo == "Variavel") {
        if (!ambiente.count(raiz->valor)) {
            throw runtime_error("Erro: variável '" + raiz->valor + "' não tem valor atribuído.");
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
        if (raiz->valor == "<") return esq < dir;
        if (raiz->valor == ">") return esq > dir;
        throw runtime_error("Erro: operador de comparação inválido.");
    }

    if (raiz->tipo == "Atribuicao") {
        int valor = interpretar(raiz->esquerda, ambiente);
        ambiente[raiz->valor] = valor;
        return 0;
    }

    if (raiz->tipo == "If") {
        int cond = interpretar(raiz->esquerda, ambiente);
        if (cond) {
            for (auto& cmd : raiz->filhos[0]->comandos) {
                interpretar(cmd, ambiente);
            }
        } else {
            for (auto& cmd : raiz->filhos[1]->comandos) {
                interpretar(cmd, ambiente);
            }
        }
        return 0;
    }

    if (raiz->tipo == "While") {
        while (interpretar(raiz->esquerda, ambiente)) {
            for (auto& cmd : raiz->filhos[0]->comandos) {
                interpretar(cmd, ambiente);
            }
        }
        return 0;
    }

    if (raiz->tipo == "Bloco") {
        for (auto& decl : raiz->filhos) {
            if (decl->tipo == "Declaracao") {
                int val = interpretar(decl->esquerda, ambiente);
                ambiente[decl->valor] = val;
            }
        }
        for (auto& cmd : raiz->comandos) {
            interpretar(cmd, ambiente);
        }
        return interpretar(raiz->esquerda, ambiente); // expressão final
    }

    throw runtime_error("Erro: tipo de nó inválido no interpretador: " + raiz->tipo);
}