#include "../include/interpreter.h"
#include <stdexcept>

using std::shared_ptr;
using std::stoi;
using std::runtime_error;

int interpretar(shared_ptr<Node> raiz){
    if(!raiz->esquerda && !raiz->direita){
        return stoi(raiz->valor);
    }

    int esq = interpretar(raiz->esquerda);
    int dir = interpretar(raiz->direita);

    if(raiz->valor == "+") return esq + dir;
    if(raiz->valor == "-") return esq - dir;
    if(raiz->valor == "*") return esq * dir;
    if(raiz->valor == "/") return esq / dir;

    throw runtime_error("Erro: operador inválido!");

};