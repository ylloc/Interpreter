#pragma once

#include <node.h>
#include <tokenizer.h>
#include <vector>
#include <error.h>
#include <memory>

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);
std::shared_ptr<Object> Read(Tokenizer* tokenizer);