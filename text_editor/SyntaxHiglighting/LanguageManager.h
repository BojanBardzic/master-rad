//
// Created by bbard on 7/31/2024.
//

#ifndef TEXT_EDITOR_LANGUAGEMANAGER_H
#define TEXT_EDITOR_LANGUAGEMANAGER_H

#include "Language.h"
#include "LanguageMode.h"

#include <unordered_map>

class LanguageManager {
public:
    static const Language* getLanguage(const LanguageMode mode);
private:
    static const std::unordered_map<LanguageMode, Language*> m_languages;
};


#endif //TEXT_EDITOR_LANGUAGEMANAGER_H
