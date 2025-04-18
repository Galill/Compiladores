#include "../include/interpreter.h"
#include <stdexcept>
#include <map>

using std::shared_ptr;
using std::stoi;
using std::runtime_error;
using std::string;
using std::map;

int interpretar(shared_ptr<Node> raiz, const map<string, int>& ambiente) {
    if (!raiz->esquerda && !raiz->direita) {
        if (raiz->tipo == "Numero") {
            return stoi(raiz->valor);
        } else if (raiz->tipo == "Variavel") {
            auto it = ambiente.find(raiz->valor);
            if (it != ambiente.end()) {
                return it->second;
            } else {
                throw runtime_error("Erro: variável '" + raiz->valor + "' não tem valor atribuído.");
            }
        } else {
            throw runtime_error("Erro: tipo de nó folha inválido: " + raiz->tipo);
        }
    }

    int esq = interpretar(raiz->esquerda, ambiente);
    int dir = interpretar(raiz->direita, ambiente);

    if (raiz->valor == "+") return esq + dir;
    if (raiz->valor == "-") return esq - dir;
    if (raiz->valor == "*") return esq * dir;
    if (raiz->valor == "/") return esq / dir;

    throw runtime_error("Erro: operador inválido!");
}