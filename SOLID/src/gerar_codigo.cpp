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
                assembly << "    movl $" << node->valor << ", %eax\n";
                assembly << "    pushl %eax\n";                      
                return;                                              
            }

            generate_assembly(node->esquerda);
            generate_assembly(node->direita);

            assembly << "    popl %ebx\n";
            assembly << "    popl %eax\n";

            if (node->valor == "+") {
                assembly << "    addl %ebx, %eax\n";
                assembly << "    pushl %eax\n";
            } else if (node->valor == "-") {
                assembly << "    subl %ebx, %eax\n";
                assembly << "    pushl %eax\n";
            } else if (node->valor == "*") {
                assembly << "    imull %ebx, %eax\n";
                assembly << "    pushl %eax\n";
            } else if (node->valor == "/") {
                assembly << "    cdq\n";
                assembly << "    idivl %ebx\n";
                assembly << "    pushl %eax\n";
            }
        };

    generate_assembly(ast);

    assembly << "    popl %eax\n";
    assembly << "    # Resultado final em %eax\n";

    return assembly.str();
}