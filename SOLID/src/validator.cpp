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

void validar(vector<shared_ptr<Token>> &tokens){
    stack<shared_ptr<Token>> pilha;

    for (const auto&token : tokens){
        if(token->tipo == "ParenEsq") {
            pilha.push(token);
        } else if (token->tipo == "ParenDir"){
            if(pilha.empty()) {
                std::cout << "Erro: Parenteses de fechamento extra." << std::endl;
                exit(1);
            }
            pilha.pop();
        }
    }

    if (!pilha.empty()) {
        std::cout << "Erro: Parentese de abertura extra." << std::endl;
        exit(1);
    }

};