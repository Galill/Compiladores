#include "../include/gerar_codigo.h"
#include "../include/token.h"
#include <string>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <iostream> // Para depuração
#include <set> // Para armazenar variáveis declaradas

using std::string;
using std::shared_ptr;
using std::function;
using std::stringstream;
using std::runtime_error;
using std::cerr;
using std::endl;
using std::set;

int labelCounter = 0;

string gerar_codigo(shared_ptr<Node> ast) {
    if (!ast || ast->tipo != "Bloco") return "";

    stringstream bss;
    stringstream code;
    bss << ".section .bss\n";
    code << ".section .text\n.globl _start\n_start:\n";

    set<string> declaredVars; // Keep track of declared variables

    function<void(shared_ptr<Node>)> gerarExp;
    gerarExp = [&](shared_ptr<Node> node) {
        if (!node) return;

        if (node->tipo == "Numero") {
            code << "    mov $" << node->valor << ", %rax\n";
        } else if (node->tipo == "Variavel") {
            if (declaredVars.find(node->valor) == declaredVars.end()) {
                throw runtime_error("Variável não declarada: " + node->valor);
            }
            code << "    mov " << node->valor << ", %rax\n"; // Acesso direto agora
        } else if (node->tipo == "Operador") {
            gerarExp(node->esquerda);
            code << "    push %rax\n";
            gerarExp(node->direita);
            if (node->valor == "+") {
                code << "    pop %rbx\n    add %rbx, %rax\n";
            } else if (node->valor == "-") {
                code << "    mov %rax, %rbx\n    pop %rax\n    sub %rbx, %rax\n";
            } else if (node->valor == "*") {
                code << "    pop %rbx\n    imul %rbx, %rax\n";
            } else if (node->valor == "/") {
                code << "    mov %rax, %rbx\n    pop %rax\n    xor %rdx, %rdx\n    idiv %rbx\n";
            }
        } else if (node->tipo == "Comparacao") {
            gerarExp(node->esquerda);
            code << "    push %rax\n";
            gerarExp(node->direita);
            code << "    pop %rbx\n    xor %rcx, %rcx\n    cmp %rax, %rbx\n";
            if (node->valor == "==") {
                code << "    sete %cl\n";
            } else if (node->valor == "<") {
                code << "    setl %cl\n";
            } else if (node->valor == ">") {
                code << "    setg %cl\n";
            }
            code << "    movzx %cl, %rax\n";
        }
    };

    function<void(shared_ptr<Node>)> gerarCmd = [&](shared_ptr<Node> node) {
        if (!node) return;

        if (node->tipo == "Declaracao") {
            if (declaredVars.find(node->valor) == declaredVars.end()) {
                bss << "    .lcomm " << node->valor << ", 8\n"; // Use .lcomm as in the target
                declaredVars.insert(node->valor);
                gerarExp(node->esquerda); // Generate code for the initial value
                code << "    mov %rax, " << node->valor << "\n"; // Initialize directly
            } else {
                throw runtime_error("Variável já declarada: " + node->valor);
            }
        } else if (node->tipo == "Atribuicao") {
            if (declaredVars.find(node->valor) == declaredVars.end()) {
                throw runtime_error("Atribuição a variável não declarada: " + node->valor);
            }
            gerarExp(node->esquerda);
            code << "    mov %rax, " << node->valor << "\n"; // Assign directly
        } else if (node->tipo == "If") {
            int id = labelCounter++;
            gerarExp(node->esquerda);
            code << "    cmp $0, %rax\n    jz Lfalso" << id << "\n"; // Use jz for jump if zero
            for (const auto& cmd : node->filhos[0]->comandos) gerarCmd(cmd);
            if (node->filhos.size() > 1) {
                code << "    jmp Lfim" << id << "\nLfalso" << id << ":\n";
                for (const auto& cmd : node->filhos[1]->comandos) gerarCmd(cmd);
                code << "Lfim" << id << ":\n";
            } else {
                code << "Lfalso" << id << ":\n";
                code << "Lfim" << id << ":\n"; // Need an Lfim even without else
            }
        } else if (node->tipo == "While") {
            int id = labelCounter++;
            code << "Linicio" << id << ":\n";
            gerarExp(node->esquerda);
            code << "    cmp $0, %rax\n    jz Lfim" << id << "\n"; // Use jz
            for (const auto& cmd : node->filhos[0]->comandos) gerarCmd(cmd);
            code << "    jmp Linicio" << id << "\nLfim" << id << ":\n";
        }
    };

    // Process declarations first to initialize variables at the start
    for (const auto& decl : ast->filhos) {
        if (decl->tipo == "Declaracao") {
            gerarCmd(decl);
        }
    }

    // Then process the commands in the main block
    for (const auto& cmd : ast->comandos) {
        gerarCmd(cmd);
    }

    gerarExp(ast->esquerda); // expressão final
    code << "    call imprime_num\n    call sair\n";

    stringstream final;
    final << bss.str() << "\n" << code.str() << "\n.include \"runtime.s\"\n";
    return final.str();
}