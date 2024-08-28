//
// Created by bbard on 7/31/2024.
//

#ifndef TEXT_EDITOR_LANGUAGE_H
#define TEXT_EDITOR_LANGUAGE_H

#include <set>
#include <string>

class Language {
public:
    Language(const std::set<std::string> &mKeywords, const std::string &mSingleLineCommentStart,
             const std::string &mMultiLineCommentStart, const std::string &mMultiLineCommentEnd, const std::string& name, bool mPreprocessor);

    ~Language();

    const std::set<std::string>& getKeywords() const;
    const std::string& getSingleLineCommentStart() const;
    const std::string& getMultiLineCommentStart() const;
    const std::string& getMultiLineCommentEnd() const;
    const std::string& getName() const;
    bool isPreprocessor() const;
private:
    std::set<std::string> m_keywords;
    std::string m_singleLineCommentStart;
    std::string m_multiLineCommentStart;
    std::string m_multiLineCommentEnd;
    std::string m_name;
    bool m_preprocessor;
};


#endif //TEXT_EDITOR_LANGUAGE_H
