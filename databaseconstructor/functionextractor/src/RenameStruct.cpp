#pragma once
#include <algorithm>
#include "RenameStruct.hpp"

namespace process {

clang::transformer::RewriteRule processRenameStructRule(const std::string& randomSuffix) {
    auto structDeclMatcher = typedefDecl(
        isExpansionInMainFile(),
        hasAncestor(translationUnitDecl(
            hasDescendant(
                fieldDecl(
                    isExpansionInMainFile()
                ).bind("structDecl")
            )
        ))
    ).bind("structDecl");

    return makeRule(structDeclMatcher, {
        insertAfter(name("structDecl"), cat("_", randomSuffix)),
    });
}

struct clang::transformer::RewriteRule processRenameStructRefRule(const std::string& randomSuffix) {
    auto structDeclRefMatcher = typeLoc(
        loc(typedefType(
            hasDeclaration(
                typedefDecl(
                    isExpansionInMainFile()
                ).bind("structDecl")
            )
        ))
    ).bind("structDeclRef");

    return makeRule(structDeclRefMatcher, {
        insertAfter(node("structDeclRef"), cat("_", randomSuffix)),
    });
}

}

process::RenameStruct::RenameStruct(
    std::map<std::string, clang::tooling::Replacements> &FileToReplacements)
    : FileToReplacements{FileToReplacements} {
        std::string randomSuffix = extractor_utils::generate_random_string(6);
        ruleCallbacks.emplace_back(ruleactioncallback::RuleActionCallback{
            processRenameStructRule(randomSuffix), FileToReplacements, FileToNumberValueTrackers});
        ruleCallbacks.emplace_back(ruleactioncallback::RuleActionCallback{
            processRenameStructRefRule(randomSuffix), FileToReplacements, FileToNumberValueTrackers});
    }

void process::RenameStruct::registerMatchers(clang::ast_matchers::MatchFinder &Finder) {
    for (auto &Callback : ruleCallbacks){
        Callback.registerMatchers(Finder);
    }
}


