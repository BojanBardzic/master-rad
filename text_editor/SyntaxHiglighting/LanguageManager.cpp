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
            LanguageMode::C,
            new Language {
                std::set<std::string> {
                    "alignas", "alignof", "auto", "bool", "break", "case", "char", "const", "constexpr",
                    "continue", "default", "do", "double", "else", "enum", "extern", "false", "float",
                    "for", "goto", "if", "inline", "int", "long", "null", "nullptr", "register", "register",
                    "restrict", "return", "short", "signed", "sizeof", "static", "static_assert", "struct",
                    "switch", "thread_local", "true", "typedef", "typeof", "union", "unsigned", "void",
                    "volatile", "while"
                },
                "//",
                "/*",
                "*/",
                "C",
                true
            }
        },

        {
            LanguageMode::CSharp,
            new Language {
                std::set<std::string> {
                    "abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char",
                    "checked", "class", "const", "continue", "decimal", "default", "delegate",
                    "do", "double", "dynamic", "else", "enum", "event", "explicit", "extern", "false", "finally",
                    "fixed", "float", "for", "foreach", "from", "goto", "if", "implicit", "in", "int",
                    "interface", "internal", "is", "lock", "long", "namespace", "new", "null",
                    "object", "operator", "out", "override", "params", "private", "protected",
                    "public", "readonly", "ref", "return", "sbyte", "sealed", "short", "sizeof",
                    "stackalloc", "static", "string", "struct", "switch", "this", "throw", "true",
                    "try", "typeof", "uint", "ulong", "unchecked", "unsafe", "ushort", "using",
                    "virtual", "void", "volatile", "while"
                },
                "//",
                "/*",
                "*/",
                "C#",
                false
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
                    "private", "protected", "public", "requires", "return", "sealed",
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
