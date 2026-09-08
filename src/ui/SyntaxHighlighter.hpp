#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <vector>

#include "core/languages/Language.hpp"

namespace aznora::ui {

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument* parent, aznora::core::Language language);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void setupCppRules();
    void setupPythonRules();
    void setupJavaScriptRules();

    std::vector<HighlightingRule> m_highlightingRules;
    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;
    QTextCharFormat m_multiLineCommentFormat;
    bool m_hasMultiLineComments{false};
};

} // namespace aznora::ui
