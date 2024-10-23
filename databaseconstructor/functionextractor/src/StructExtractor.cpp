#pragma once

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Tooling/Core/Replacement.h>
#include <clang/Tooling/Transformer/RewriteRule.h>

#include "StructExtractor.hpp"

namespace extractor
{

auto structMatcher = typedefDecl(
    isExpansionInMainFile(),
    hasType(elaboratedType())
).bind("typedef");

} // namespace extractor

extractor::StructExtractor::StructExtractor(
    std::map<std::string, clang::tooling::Replacements> &FileToReplacements)
    : FileToReplacements{FileToReplacements} {}

void extractor::StructExtractor::registerMatchers(clang::ast_matchers::MatchFinder &Finder) {
    Finder.addMatcher(structMatcher, &Printer);
}
