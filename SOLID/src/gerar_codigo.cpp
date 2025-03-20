#include "../include/gerar_codigo.h"
#include "../include/token.h"
#include <string>
#include <vector>
#include <functional>
#include <sstream>

using std::vector;
using std::string;
using std::shared_ptr;
using std::function;   
using std::stringstream;

string modeloAssembly = R"(.section .text
.globl _start
_start:
aqui
call imprime_num
call sair
.include "runtime.s"
)";

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
                assembly << "    push %rax\n";                      
                return;                                              
            }

            generate_assembly(node->esquerda);
            generate_assembly(node->direita);

            assembly << "    pop %rax\n";
            assembly << "    pop %rax\n";

            // TODO revisar assembly
            if (node->valor == "+") {
                assembly << "    add %rbx, %rax\n";
                assembly << "    push %rax\n";
            } else if (node->valor == "-") {
                assembly << "    sub %rbx, %rax\n";
                assembly << "    push %rax\n";
            } else if (node->valor == "*") {
                assembly << "    imull %rbx, %rax\n";
                assembly << "    pushl %rax\n";
            } else if (node->valor == "/") {
                assembly << "    cdq\n";
                assembly << "    idivl %rbx\n";
                assembly << "    pushl %rax\n";
            }
        };

    generate_assembly(ast);

    assembly << "    pop %rax\n";
    assembly << "    # Resultado final em %rax\n";

    return assembly.str();
}