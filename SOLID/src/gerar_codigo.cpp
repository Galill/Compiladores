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
                assembly << "   mov $" << node->valor << ", %rax\n";                      
                return;                                              
            }

            generate_assembly(node->esquerda);
            assembly << "   push %rax\n";
            generate_assembly(node->direita);

            // TODO revisar assembly
            if (node->valor == "+") {
                assembly << "   pop %rbx\n";
                assembly << "   add %rbx, %rax\n";
            } else if (node->valor == "-") {
                assembly << "   mov %rax, %rbx\n";
                assembly << "   pop %rax\n";
                assembly << "   sub, %rbx, %rax\n";
            } else if (node->valor == "*") {
                assembly << "    MUL %rbx, %rax\n";
                assembly << "    push %rax\n";
            } else if (node->valor == "/") {
                assembly << "    cdq\n";
                assembly << "    DIV %rbx\n";
                assembly << "    push %rax\n";
            }
        };

    generate_assembly(ast);

    assembly << "    popl %rax\n";
    assembly << "    # Resultado final em %rax\n";

    return assembly.str();
}