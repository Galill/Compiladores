#include "../include/gerar_codigo.h"
#include <sstream>
#include <map>
#include <string>
#include <functional>

using std::string;
using std::stringstream;
using std::shared_ptr;
using std::map;
using std::vector;

int labelCounter = 0;

string gerar_codigo(shared_ptr<Node> ast) {
    if (!ast || ast->tipo != "Programa") return "";

    stringstream out;
    // Seção de dados para variáveis globais
    out << ".section .bss\n";
    for (auto &decl : ast->filhos) {
        if (decl->tipo == "Declaracao") {
            out << "    .lcomm " << decl->valor << ", 8\n";
        }
    }

    // Seção de texto
    out << ".section .text\n";
    out << ".globl _start\n\n";

    // Offsets para variáveis locais/parâmetros
    map<string,int> offsets;

    
    auto gerarExp = [&](auto&& self, shared_ptr<Node> node, int &stackOff) -> void {
        if (node->tipo == "Numero") {
            out << "    mov $" << node->valor << ", %rax\n";
        } else if (node->tipo == "Variavel") {
            out << "    mov " << offsets[node->valor] << "(%rbp), %rax\n";
        } else if (node->tipo == "Operador") {
            self(self, node->esquerda, stackOff);
            out << "    push %rax\n";
            stackOff += 8;
            self(self, node->direita, stackOff);
            out << "    pop %rbx\n";
            stackOff -= 8;
            if (node->valor == "+") out << "    add %rbx, %rax\n";
            else if (node->valor == "-") {
                out << "    sub %rax, %rbx\n";
                out << "    mov %rbx, %rax\n";
            } else if (node->valor == "*") out << "    imul %rbx, %rax\n";
            else {
                out << "    mov %rax, %rcx\n    mov %rbx, %rax\n    xor %rdx, %rdx\n    div %rcx\n";
            }
        } else if (node->tipo == "Comparacao") {
            self(self, node->esquerda, stackOff);
            out << "    push %rax\n";
            stackOff += 8;
            self(self, node->direita, stackOff);
            out << "    pop %rbx\n";
            stackOff -= 8;
            out << "    cmp %rax, %rbx\n    mov $0, %rbx\n";
            if (node->valor == "==") out << "    sete %bl\n";
            else if (node->valor == "<") out << "    setl %bl\n";
            else out << "    setg %bl\n";
            out << "    movzbq %bl, %rax\n";
        } else if (node->tipo == "Chamada") {
            for (int i = int(node->filhos.size()) - 1; i >= 0; --i) {
                self(self, node->filhos[i], stackOff);
                out << "    push %rax\n";
                stackOff += 8;
            }
            out << "    call " << node->valor << "\n";
            if (!node->filhos.empty()) {
                out << "    add $" << (node->filhos.size() * 8) << ", %rsp\n";
                stackOff -= int(node->filhos.size() * 8);
            }
        }
    };

    // Helper para gerar comandos
    auto gerarCmd = [&](auto&& self, shared_ptr<Node> node, int &stackOff) -> void {
        if (node->tipo == "Atribuicao") {
            gerarExp(gerarExp, node->esquerda, stackOff);
            out << "    mov %rax, " << offsets[node->valor] << "(%rbp)\n";
        } else if (node->tipo == "If") {
            int lbl = labelCounter++;
            gerarExp(gerarExp, node->esquerda, stackOff);
            out << "    cmp $0, %rax\n    je .LELSE" << lbl << "\n";
            for (auto &c : node->filhos[0]->comandos) self(self, c, stackOff);
            out << "    jmp .LEND" << lbl << "\n.LELSE" << lbl << ":\n";
            if (node->filhos.size() > 1) for (auto &c : node->filhos[1]->comandos) self(self, c, stackOff);
            out << ".LEND" << lbl << ":\n";
        } else if (node->tipo == "While") {
            int lbl = labelCounter++;
            out << ".LLOOP" << lbl << ":\n";
            gerarExp(gerarExp, node->esquerda, stackOff);
            out << "    cmp $0, %rax\n    je .LEXIT" << lbl << "\n";
            for (auto &c : node->filhos[0]->comandos) self(self, c, stackOff);
            out << "    jmp .LLOOP" << lbl << "\n.LEXIT" << lbl << ":\n";
        } else if (node->tipo == "Retorno") {
            gerarExp(gerarExp, node->esquerda, stackOff);
            int totalLocals = int(offsets.size());
            out << "    add $" << (totalLocals * 8) << ", %rsp\n    pop %rbp\n    ret\n";
        } else if (node->tipo == "Chamada") {
            for (int i = int(node->filhos.size()) - 1; i >= 0; --i) {
                gerarExp(gerarExp, node->filhos[i], stackOff);
                out << "    push %rax\n";
                stackOff += 8;
            }
            out << "    call " << node->valor << "\n";
            if (!node->filhos.empty()) {
                out << "    add $" << (node->filhos.size() * 8) << ", %rsp\n";
                stackOff -= int(node->filhos.size() * 8);
            }
        }
    };

    // Geração de funções
    for (auto &decl : ast->filhos) {
        if ((decl->tipo == "Funcao" || decl->tipo == "Fundecl")) {
            auto gerarFunc = [&](auto&& self, shared_ptr<Node> func) -> void {
                out << func->valor << ":\n    push %rbp\n    mov %rsp, %rbp\n";

                offsets.clear();
                for (size_t i = 0; i < func->filhos.size(); ++i)
                    offsets[func->filhos[i]->valor] = 16 + int(i) * 8;
                int negOff = -8;
                for (auto &v : func->locais) offsets[v] = negOff, negOff -= 8;

                int numLoc = int(func->locais.size());
                if (numLoc > 0) out << "    sub $" << (numLoc * 8) << ", %rsp\n";

                int stackOff = 0;
                // Lógica para funções com expressão de retorno direta
                if (func->comandos.empty() && func->esquerda) {
                    gerarExp(gerarExp, func->esquerda, stackOff); // Avalia a expressão de retorno
                } else {
                    // Lógica para funções com comandos no corpo
                    for (auto &cmd : func->comandos) {
                        if (cmd->tipo == "Declaracao") {
                            if (cmd->esquerda) {
                                gerarExp(gerarExp, cmd->esquerda, stackOff);
                                out << "    mov %rax, " << offsets[cmd->valor] << "(%rbp)\n";
                            }
                        } else if (cmd->tipo == "Retorno") {
                            gerarExp(gerarExp, cmd->esquerda, stackOff);
                        } else {
                            gerarCmd(gerarCmd, cmd, stackOff);
                        }
                    }
                }

                if (numLoc > 0) out << "    add $" << (numLoc * 8) << ", %rsp\n";
                out << "    pop %rbp\n    ret\n";
            };
            gerarFunc(gerarFunc, decl);
        }
    }

    // Entry point
    out << "_start:\n    call main\n    mov %rax, %rdi\n    call imprime_num\n    call sair\n\n.include \"runtime.s\"\n";

    return out.str();
}