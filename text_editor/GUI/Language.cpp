//
// Created by bbard on 7/31/2024.
//

#include "Language.h"

Language::Language(const std::set<std::string> &mKeywords, const std::string &mSingleLineCommentStart,
                   const std::string &mMultiLineCommentStart, const std::string &mMultiLineCommentEnd,
                   const std::string& name, bool mPreprocessor) : m_keywords(mKeywords),
                                         m_singleLineCommentStart(mSingleLineCommentStart),
                                         m_multiLineCommentStart(mMultiLineCommentStart),
                                         m_multiLineCommentEnd(mMultiLineCommentEnd),
                                         m_name(name), m_preprocessor(mPreprocessor) {}

Language::~Language() {}

const std::set<std::string> &Language::getKeywords() const { return m_keywords; }

const std::string &Language::getSingleLineCommentStart() const { return m_singleLineCommentStart; }

const std::string &Language::getMultiLineCommentStart() const { return m_multiLineCommentStart; }

const std::string &Language::getMultiLineCommentEnd() const { return m_multiLineCommentEnd; }

const std::string &Language::getName() const { return m_name; }

bool Language::isPreprocessor() const { return m_preprocessor; }


