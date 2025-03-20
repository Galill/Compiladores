#include "ast_builder.h"
#include "token.h"
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::shared_ptr;

string gerar_codigo(shared_ptr<Node> ast);