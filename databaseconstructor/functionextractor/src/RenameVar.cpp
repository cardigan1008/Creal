#pragma once
#include <algorithm>
#include "RenameVar.hpp"

namespace process
{

/* Rename local variable declaration */
clang::transformer::RewriteRule processRenameVarRule() {
    auto varDeclMatcher = varDecl(
        isExpansionInMainFile(),
        hasLocalStorage(),
        hasAncestor(translationUnitDecl(
            hasDescendant(
                functionDecl(
                    isExpansionInMainFile(),
                    isDefinition()
                ).bind("function")
            )
        ))
    ).bind("varDecl");

    return makeRule(varDeclMatcher, {
        insertAfter(name("varDecl"), cat("_", name("function"))),
    });
}

/* Rename local variable reference */
clang::transformer::RewriteRule processRenameVarRefRule() {
    auto varDefRefMatcher = declRefExpr(
        isExpansionInMainFile(),
        to(varDecl(
            hasLocalStorage()
        )),
        hasAncestor(functionDecl(
                    isExpansionInMainFile(),
                    isDefinition()
                ).bind("function")
            )
    ).bind("varDeclRef");

    return makeRule(varDefRefMatcher, {
        insertAfter(node("varDeclRef"), cat("_", name("function"))),
    });
}

/* Rename local struct name */
clang::transformer::RewriteRule processRenameStructRule() {
    auto structDeclMatcher = recordDecl(
        isExpansionInMainFile(),
        hasAncestor(translationUnitDecl(
            hasDescendant(
                functionDecl(
                    isExpansionInMainFile(),
                    isDefinition()
                ).bind("function")
            )
        ))).bind("structDecl");
    
    return makeRule(structDeclMatcher, {
        insertAfter(name("structDecl"), cat("_", name("function"))),
    });
}

} // namespace process

process::RenameVar::RenameVar(
    std::map<std::string, clang::tooling::Replacements> &FileToReplacements)
    : FileToReplacements{FileToReplacements} {
        ruleCallbacks.emplace_back(ruleactioncallback::RuleActionCallback{
            processRenameVarRule(), FileToReplacements, FileToNumberValueTrackers});
        ruleCallbacks.emplace_back(ruleactioncallback::RuleActionCallback{
            processRenameVarRefRule(), FileToReplacements, FileToNumberValueTrackers});
        ruleCallbacks.emplace_back(ruleactioncallback::RuleActionCallback{
            processRenameStructRule(), FileToReplacements, FileToNumberValueTrackers});
    }

void process::RenameVar::registerMatchers(clang::ast_matchers::MatchFinder &Finder) {
    for (auto &Callback : ruleCallbacks){
        Callback.registerMatchers(Finder);
    }
}
