//
// Created by bbard on 7/31/2024.
//

#include "LanguageManager.h"

const std::unordered_map<LanguageMode, Language*> LanguageManager::m_languages = {
        {
            LanguageMode::Cpp,
            new Language {
                    std::set<std::string> {
                            "alignas", "alignof", "and", "and_eq", "asm",
                            "atomic_cancel", "atomic_commit", "atomic_noexcept",
                            "auto", "bitand", "bitor", "bool", "break", "case",
                            "catch", "char", "char8_t", "char16_t", "char32_t",
                            "class", "compl", "concept", "const", "consteval",
                            "constexpr", "constinit", "const_cast", "continue",
                            "co_await", "co_return", "co_yield", "decltype",
                            "default", "delete", "do", "double", "dynamic_cast",
                            "else", "enum", "explicit", "export", "extern",
                            "false", "float", "for", "friend", "goto", "if",
                            "inline", "int", "long", "mutable", "namespace",
                            "new", "noexcept", "not", "not_eq", "nullptr",
                            "operator", "or", "or_eq", "private", "protected",
                            "public", "reflexpr", "register", "reinterpret_cast",
                            "requires", "return", "short", "signed", "sizeof",
                            "static", "static_assert", "static_cast", "struct",
                            "switch", "synchronized", "template", "this",
                            "thread_local", "throw", "true", "try", "typedef",
                            "typeid", "typename", "union", "unsigned", "using",
                            "virtual", "void", "volatile", "wchar_t", "while",
                            "xor", "xor_eq"
                    },

                    "//",
                    "/*",
                    "*/",
                    "C++",
                    true
            }
        },
        {
            LanguageMode::Java,
            new Language {
                std::set<std::string> {
                    "abstract", "assert", "boolean",
                    "break", "byte", "case", "catch", "char", "class",
                    "continue", "const", "default", "do", "double", "else",
                    "enum", "exports", "extends", "false", "final", "finally", "float",
                    "for", "goto", "if", "implements", "import", "instanceof",
                    "int", "interface", "long", "module", "new", "null", "package", "permits",
                    "private", "protected", "public", "requires", "return", "sealed"
                    "short", "static", "strictfp", "super", "switch", "synchronized",
                    "this", "throw", "transient", "true", "try", "var", "volatile", "while"
                },

                "//",
                "/*",
                "*/",
                "Java",
                false
            }
        }
};

const Language* LanguageManager::getLanguage(const LanguageMode mode) {
    return m_languages.at(mode);
}
