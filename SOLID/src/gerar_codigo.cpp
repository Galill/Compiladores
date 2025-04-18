#include "../include/gerar_codigo.h"
#include "../include/token.h"
#include <string>
#include <functional>
#include <sstream>

using std::string;
using std::shared_ptr;
using std::function;   
using std::stringstream;

string gerar_codigo(shared_ptr<Node> ast) {
    if (!ast || ast->tipo != "Programa") return "";

    stringstream assembly;
    stringstream bssSection;
    bssSection << ".section .bss\n";

    // Declarar variáveis
    for (const auto& decl : ast->filhos) {
        bssSection << ".lcomm " << decl->valor << ", 8\n";
    }

    assembly << ".section .text\n"
             << ".globl _start\n"
             << "_start:\n";

    // Função para gerar assembly de expressões
    function<void(shared_ptr<Node>)> gerarExp;
    gerarExp = [&](shared_ptr<Node> node) {
        if (!node) return;

        if (node->tipo == "Numero") {
            assembly << "    mov $" << node->valor << ", %rax\n";
        } else if (node->tipo == "Variavel") {
            assembly << "    mov " << node->valor << ", %rax\n";
        } else if (node->tipo == "Operador") {
            gerarExp(node->esquerda);
            assembly << "    push %rax\n";
            gerarExp(node->direita);
            if (node->valor == "+") {
                assembly << "    pop %rbx\n";
                assembly << "    add %rbx, %rax\n";
            } else if (node->valor == "-") {
                assembly << "    mov %rax, %rbx\n";
                assembly << "    pop %rax\n";
                assembly << "    sub %rbx, %rax\n";
            } else if (node->valor == "*") {
                assembly << "    pop %rbx\n";
                assembly << "    imul %rbx, %rax\n";
            } else if (node->valor == "/") {
                assembly << "    mov %rax, %rbx\n";
                assembly << "    pop %rax\n";
                assembly << "    xor %rdx, %rdx\n";
                assembly << "    idiv %rbx\n";
            }
        }
    };

    // Gerar código das declarações (ex: x = ...)
    for (const auto& decl : ast->filhos) {
        gerarExp(decl->esquerda);
        assembly << "    mov %rax, " << decl->valor << "\n\n";
    }

    // Gerar código da expressão final
    gerarExp(ast->esquerda);
    assembly << "\n    call imprime_num\n";
    assembly << "    call sair\n";

    // Montar assembly completo
    stringstream finalAssembly;
    finalAssembly << bssSection.str() << "\n"
                  << assembly.str()
                  << "\n.include \"runtime.s\"\n";

    return finalAssembly.str();
}