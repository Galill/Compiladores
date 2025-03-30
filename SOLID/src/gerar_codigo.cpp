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
    if (!ast) {
        return "";
    }

    stringstream assembly;

    function<void(shared_ptr<Node>)> generate_assembly =
        [&](shared_ptr<Node> node) {
        if (!node) {
            return;
        }

        if (!node->esquerda && !node->direita) {
            assembly << "    mov $" << node->valor << ", %rax\n";
            return;
        }

        generate_assembly(node->esquerda);
        assembly << "    push %rax\n";
        generate_assembly(node->direita);

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
            assembly << "    cdq\n"; // Extende %eax para %edx:%eax
            assembly << "    idiv %rbx\n";
        }
    };

    generate_assembly(ast);

    string modeloAssembly = R"(.section .text
.globl _start
_start:
    # Código gerado
    call imprime_num
    call sair

.include "runtime.s"
)";

    size_t posInsercao = modeloAssembly.find("# Código gerado");
    if (posInsercao != std::string::npos) {
        modeloAssembly.replace(posInsercao, 15, assembly.str());
    }

    return modeloAssembly;
}