#include "ui/SyntaxHighlighter.hpp"

namespace aznora::ui {

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent, aznora::core::Language language)
    : QSyntaxHighlighter(parent)
{
    switch (language)
    {
    case aznora::core::Language::Cpp:
        setupCppRules();
        break;
    case aznora::core::Language::Python:
        setupPythonRules();
        break;
    case aznora::core::Language::JavaScript:
        setupJavaScriptRules();
        break;
    default:
        break;
    }
}

void SyntaxHighlighter::setupCppRules()
{
    HighlightingRule rule;

    // Keywords
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(86, 156, 214)); // #569cd6
    keywordFormat.setFontWeight(QFont::Bold);
    const QStringList keywordPatterns = {
        QStringLiteral("\\bauto\\b"), QStringLiteral("\\bbool\\b"), QStringLiteral("\\bbreak\\b"),
        QStringLiteral("\\bcase\\b"), QStringLiteral("\\bcatch\\b"), QStringLiteral("\\bchar\\b"),
        QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"), QStringLiteral("\\bconstexpr\\b"),
        QStringLiteral("\\bcontinue\\b"), QStringLiteral("\\bdefault\\b"), QStringLiteral("\\bdelete\\b"),
        QStringLiteral("\\bdo\\b"), QStringLiteral("\\bdouble\\b"), QStringLiteral("\\belse\\b"),
        QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"), QStringLiteral("\\bexport\\b"),
        QStringLiteral("\\bextern\\b"), QStringLiteral("\\bfalse\\b"), QStringLiteral("\\bfloat\\b"),
        QStringLiteral("\\bfor\\b"), QStringLiteral("\\bfriend\\b"), QStringLiteral("\\bgoto\\b"),
        QStringLiteral("\\bif\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"), QStringLiteral("\\bmutable\\b"), QStringLiteral("\\bnamespace\\b"),
        QStringLiteral("\\bnew\\b"), QStringLiteral("\\bnoexcept\\b"), QStringLiteral("\\bnullptr\\b"),
        QStringLiteral("\\boperator\\b"), QStringLiteral("\\boverride\\b"), QStringLiteral("\\bprivate\\b"),
        QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"), QStringLiteral("\\breturn\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsigned\\b"), QStringLiteral("\\bsizeof\\b"),
        QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"), QStringLiteral("\\bswitch\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\bthis\\b"), QStringLiteral("\\bthrow\\b"),
        QStringLiteral("\\btrue\\b"), QStringLiteral("\\btry\\b"), QStringLiteral("\\btypedef\\b"),
        QStringLiteral("\\btypename\\b"), QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"),
        QStringLiteral("\\busing\\b"), QStringLiteral("\\bvirtual\\b"), QStringLiteral("\\bvoid\\b"),
        QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bwhile\\b")
    };
    for (const QString& pattern : keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.push_back(rule);
    }

    // Types / Classes
    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor(78, 201, 176)); // #4ec9b0
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Z][A-Za-z0-9_]*\\b"));
    rule.format = typeFormat;
    m_highlightingRules.push_back(rule);

    // Preprocessor
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(197, 134, 192)); // #c586c0
    rule.pattern = QRegularExpression(QStringLiteral("#[a-zA-Z_]+"));
    rule.format = preprocessorFormat;
    m_highlightingRules.push_back(rule);

    // Strings
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(206, 145, 120)); // #ce9178
    rule.pattern = QRegularExpression(QStringLiteral("\".*?\""));
    rule.format = stringFormat;
    m_highlightingRules.push_back(rule);
    rule.pattern = QRegularExpression(QStringLiteral("'.*?'"));
    m_highlightingRules.push_back(rule);

    // Numbers
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168)); // #b5cea8
    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9]+(\\.[0-9]+)?\\b"));
    rule.format = numberFormat;
    m_highlightingRules.push_back(rule);

    // Single-line comments
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor(106, 153, 85)); // #6a9955
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    m_highlightingRules.push_back(rule);

    // Multi-line comments
    m_multiLineCommentFormat.setForeground(QColor(106, 153, 85));
    m_multiLineCommentFormat.setFontItalic(true);
    m_commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    m_commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
    m_hasMultiLineComments = true;
}

void SyntaxHighlighter::setupPythonRules()
{
    HighlightingRule rule;

    // Keywords
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(86, 156, 214));
    keywordFormat.setFontWeight(QFont::Bold);
    const QStringList keywordPatterns = {
        QStringLiteral("\\band\\b"), QStringLiteral("\\bas\\b"), QStringLiteral("\\bassert\\b"),
        QStringLiteral("\\basync\\b"), QStringLiteral("\\bawait\\b"), QStringLiteral("\\bbreak\\b"),
        QStringLiteral("\\bclass\\b"), QStringLiteral("\\bcontinue\\b"), QStringLiteral("\\bdef\\b"),
        QStringLiteral("\\bdel\\b"), QStringLiteral("\\belif\\b"), QStringLiteral("\\belse\\b"),
        QStringLiteral("\\bexcept\\b"), QStringLiteral("\\bfinally\\b"), QStringLiteral("\\bfor\\b"),
        QStringLiteral("\\bfrom\\b"), QStringLiteral("\\bglobal\\b"), QStringLiteral("\\bif\\b"),
        QStringLiteral("\\bimport\\b"), QStringLiteral("\\bin\\b"), QStringLiteral("\\bis\\b"),
        QStringLiteral("\\blambda\\b"), QStringLiteral("\\bnonlocal\\b"), QStringLiteral("\\bnot\\b"),
        QStringLiteral("\\bor\\b"), QStringLiteral("\\bpass\\b"), QStringLiteral("\\braise\\b"),
        QStringLiteral("\\breturn\\b"), QStringLiteral("\\btry\\b"), QStringLiteral("\\bwhile\\b"),
        QStringLiteral("\\bwith\\b"), QStringLiteral("\\byield\\b"), QStringLiteral("\\bTrue\\b"),
        QStringLiteral("\\bFalse\\b"), QStringLiteral("\\bNone\\b")
    };
    for (const QString& pattern : keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.push_back(rule);
    }

    // Function definitions
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(220, 220, 170));
    rule.pattern = QRegularExpression(QStringLiteral("\\bdef\\s+([A-Za-z0-9_]+)"));
    rule.format = functionFormat;
    m_highlightingRules.push_back(rule);

    // Strings
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(206, 145, 120));
    rule.pattern = QRegularExpression(QStringLiteral("\".*?\""));
    rule.format = stringFormat;
    m_highlightingRules.push_back(rule);
    rule.pattern = QRegularExpression(QStringLiteral("'.*?'"));
    m_highlightingRules.push_back(rule);

    // Numbers
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9]+(\\.[0-9]+)?\\b"));
    rule.format = numberFormat;
    m_highlightingRules.push_back(rule);

    // Comments
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(106, 153, 85));
    commentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = commentFormat;
    m_highlightingRules.push_back(rule);
}

void SyntaxHighlighter::setupJavaScriptRules()
{
    HighlightingRule rule;

    // Keywords
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(86, 156, 214));
    keywordFormat.setFontWeight(QFont::Bold);
    const QStringList keywordPatterns = {
        QStringLiteral("\\bawait\\b"), QStringLiteral("\\bbreak\\b"), QStringLiteral("\\bcase\\b"),
        QStringLiteral("\\bcatch\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bcontinue\\b"), QStringLiteral("\\bdebugger\\b"), QStringLiteral("\\bdefault\\b"),
        QStringLiteral("\\bdelete\\b"), QStringLiteral("\\bdo\\b"), QStringLiteral("\\belse\\b"),
        QStringLiteral("\\bexport\\b"), QStringLiteral("\\bextends\\b"), QStringLiteral("\\bfinally\\b"),
        QStringLiteral("\\bfor\\b"), QStringLiteral("\\bfunction\\b"), QStringLiteral("\\bif\\b"),
        QStringLiteral("\\bimport\\b"), QStringLiteral("\\bin\\b"), QStringLiteral("\\binstanceof\\b"),
        QStringLiteral("\\bnew\\b"), QStringLiteral("\\breturn\\b"), QStringLiteral("\\bsuper\\b"),
        QStringLiteral("\\bswitch\\b"), QStringLiteral("\\bthis\\b"), QStringLiteral("\\bthrow\\b"),
        QStringLiteral("\\btry\\b"), QStringLiteral("\\btypeof\\b"), QStringLiteral("\\bvar\\b"),
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bwhile\\b"), QStringLiteral("\\bwith\\b"),
        QStringLiteral("\\byield\\b"), QStringLiteral("\\blet\\b"), QStringLiteral("\\bstatic\\b")
    };
    for (const QString& pattern : keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.push_back(rule);
    }

    // Strings
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(206, 145, 120));
    rule.pattern = QRegularExpression(QStringLiteral("\".*?\""));
    rule.format = stringFormat;
    m_highlightingRules.push_back(rule);
    rule.pattern = QRegularExpression(QStringLiteral("'.*?'"));
    m_highlightingRules.push_back(rule);
    rule.pattern = QRegularExpression(QStringLiteral("`.*?`"));
    m_highlightingRules.push_back(rule);

    // Numbers
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9]+(\\.[0-9]+)?\\b"));
    rule.format = numberFormat;
    m_highlightingRules.push_back(rule);

    // Comments
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(106, 153, 85));
    commentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = commentFormat;
    m_highlightingRules.push_back(rule);

    m_multiLineCommentFormat.setForeground(QColor(106, 153, 85));
    m_multiLineCommentFormat.setFontItalic(true);
    m_commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    m_commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
    m_hasMultiLineComments = true;
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightingRule& rule : m_highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    if (!m_hasMultiLineComments)
        return;

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(m_commentStartExpression);

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = m_commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        startIndex = text.indexOf(m_commentStartExpression, startIndex + commentLength);
    }
}

} // namespace aznora::ui
