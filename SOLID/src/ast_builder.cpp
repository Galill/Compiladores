

// TODO -------- Revisar Código Gerado por IA --------
// #include "ast.h"
// #include <stdexcept>

// std::shared_ptr<Node> ASTBuilder::build(const std::vector<std::shared_ptr<IToken>>& tokens) {
//     int pos = 0;
//     return parseExpressao(tokens, pos);
// }

// std::shared_ptr<Node> ASTBuilder::parseExpressao(const std::vector<std::shared_ptr<IToken>>& tokens, int& pos) {
//     if (pos >= tokens.size()) {
//         throw std::runtime_error("Erro sintatico: expressão inesperada.");
//     }

//     if (tokens[pos]->getTipo() == "Numero") {
//         std::shared_ptr<Node> node = std::make_shared<Node>(tokens[pos]->getLexema());
//         pos++;
//         return node;
//     } else if (tokens[pos]->getTipo() == "ParenEsq") {
//         pos++; // Consome "("

//         std::shared_ptr<Node> esquerda = parseExpressao(tokens, pos);
//         if (pos >= tokens.size() || (tokens[pos]->getTipo() != "Soma" && tokens[pos]->getTipo() != "Subtracao" && tokens[pos]->getTipo() != "Multiplicacao" && tokens[pos]->getTipo() != "Divisao")) {
//             throw std::runtime_error("Erro sintatico: operador esperado.");
//         }

//         std::string operador = tokens[pos]->getLexema();
//         pos++;

//         std::shared_ptr<Node> direita = parseExpressao(tokens, pos);

//         if (pos >= tokens.size() || tokens[pos]->getTipo() != "ParenDir") {
//             throw std::runtime_error("Erro sintatico: fechamento de parentese esperado.");
//         }
//         pos++; // Consome ")"

//         std::shared_ptr<Node> node = std::make_shared<Node>(operador);
//         node->esquerda = esquerda;
//         node->direita = direita;
//         return node;
//     } else {
//         throw std::runtime_error("Erro sintatico: expressao invalida.");
//     }
// }