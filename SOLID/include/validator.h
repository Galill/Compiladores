#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "token.h"
#include <vector>
#include <memory>
#include <stack>
#include <stdexcept>

using std::vector;
using std::shared_ptr;

void validar(vector<shared_ptr<Token>> &tokens);

#endif