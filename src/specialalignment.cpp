#include "specialalignment.h"

#include <texteditor/icodestylepreferences.h>
#include <texteditor/tabsettings.h>
#include <texteditor/textdocument.h>
#include <texteditor/texteditor.h>
#include <texteditor/texteditorsettings.h>

#include <QTextBlock>

using namespace TextEditor;

static bool getAlignToLastOccournce(const QString & endPartFirstLine, const QString & startPartLastLine, const QString & alignPattern)
{
    bool firstLineHasLaterOccourences    = endPartFirstLine.lastIndexOf(alignPattern) > 0;
    bool firstLineHasPreviousOccourences = startPartLastLine.indexOf(alignPattern) < (startPartLastLine.length() - alignPattern.length());

    if (firstLineHasLaterOccourences && !firstLineHasPreviousOccourences) return false;
    return true;
}

static QString getAlignPattern(const QString & firstLine, int startOfSelection, const QString & lastLine, int endOfSelection, bool & alignToLastOccournce)
{
    alignToLastOccournce = true;

    const QString endPartFirstLine  = firstLine.mid(startOfSelection);
    const QString startPartLastLine = lastLine.left(lastLine.length() - endOfSelection);

    QString retval;
    for (int pos = qMin(endPartFirstLine.length(), startPartLastLine.length()); pos > 0 ; --pos) {
        if (QStringView{endPartFirstLine}.left(pos).compare(QStringView{startPartLastLine}.right(pos)) == 0) {
            retval = endPartFirstLine.left(pos);
            alignToLastOccournce = getAlignToLastOccournce(endPartFirstLine, startPartLastLine, retval);
            return retval;
        }
    }

    return retval;
}

static int getIndentionDepth(const QStringList & linesToAlign, const TabSettings & tabSettings, const QString & alignBy, const bool alignToLastOccournce)
{
    // now find the max indentation depth
    int depthToIndent = 0;
    for (int i = 0 ; i < linesToAlign.size(); ++i) {
        QString line = linesToAlign[i];
        int pos = (alignToLastOccournce ? line.lastIndexOf(alignBy) : line.indexOf(alignBy));

        // remove spaces before aligner
        int len = 0;
        for ( ; pos > 0 && line.at(pos-1).isSpace() ; --pos, ++len ) ;
        line.remove(pos, len);

        // re-add a single space
        line.insert(pos++, QLatin1Char(' '));

        depthToIndent = qMax(depthToIndent, tabSettings.columnAt(line, pos));
    }
    return depthToIndent;
}

static void alignLines(QStringList & linesToAlign, int indentionDepth, const TabSettings & tabSettings, const QString & alignBy, const bool alignToLastOccournce)
{
    // now indent all lines
    for (int i = 0 ; i < linesToAlign.size(); ++i) {
        QString & line = linesToAlign[i];
        const int pos = (alignToLastOccournce ? line.lastIndexOf(alignBy) : line.indexOf(alignBy));
        if (pos >= 0) {
            const int diff = indentionDepth - tabSettings.columnAt(line, pos);
            if (diff > 0)
                line.insert(pos, QString(diff, QLatin1Char(' ')));
        }
    }
}

SpecialAlignment::SpecialAlignment(QObject *parent)
    :
      QObject(parent)
{
}

void SpecialAlignment::align()
{
    BaseTextEditor *editor = BaseTextEditor::currentTextEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    if (!cursor.hasSelection()) return;

    const int startOfSelection = cursor.selectionStart();
    const int endOfSelection   = cursor.selectionEnd();

    // selecte the complete block (from position 0 of line 1 to end of line X)
    cursor.setPosition(startOfSelection);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.setPosition(endOfSelection, QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    const QString selectedTxt = cursor.selectedText();
    QStringList linesToAlign = selectedTxt.split(QChar::ParagraphSeparator, Qt::KeepEmptyParts);
    if (linesToAlign.size() < 2) return;

    // we just want the absolute start and end positions within the lines so remove the rest from the positions in the textblock
    bool alignToLastOccourenceInLine = true;
    const QString alignBy = getAlignPattern(linesToAlign.first(), startOfSelection - cursor.selectionStart(),
                                            linesToAlign.last(), cursor.selectionEnd() - endOfSelection,
                                            alignToLastOccourenceInLine);
    if (alignBy.isEmpty()) return; // nothing found to align by

    const int indentionDepth = getIndentionDepth(linesToAlign, TextEditorSettings::codeStyle()->tabSettings(), alignBy, alignToLastOccourenceInLine);

    alignLines(linesToAlign, indentionDepth, TextEditorSettings::codeStyle()->tabSettings(), alignBy, alignToLastOccourenceInLine);

    cursor.beginEditBlock();
    cursor.insertText(linesToAlign.join(QString(QChar::ParagraphSeparator)));
    cursor.endEditBlock();
}
