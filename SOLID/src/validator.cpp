#include "../include/validator.h"
#include <vector>
#include <memory>
#include <stack>
#include <stdexcept>
#include <iostream>

using std::vector;
using std::shared_ptr;
using std::stack;
using std::runtime_error;


void validar(vector<shared_ptr<Token>> &tokens) {
    stack<shared_ptr<Token>> pilha;

    for (const auto& token : tokens) {
        if (token->tipo == "ParenEsq") {
            pilha.push(token);
        } else if (token->tipo == "ParenDir") {
            if (pilha.empty()) {
                throw runtime_error("Erro: Parenteses de fechamento extra.");
            }
            pilha.pop();
        }
    }

    if (!pilha.empty()) {
        throw runtime_error("Erro: Parenteses de abertura extra.");
    }
}




