#include "paragraphsorting.h"

#include <texteditor/texteditor.h>
#include <texteditor/textdocument.h>

#include <QTextBlock>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

using namespace TextEditor;

ParagraphSorting::ParagraphSorting(QObject *parent)
    :
      QObject(parent)
{
}

void ParagraphSorting::onSortParagraphAction()
{
    sortParagraphPrivate(false);
}

void ParagraphSorting::onSortIndentedParagraphAction()
{
    sortParagraphPrivate(true);
}

void ParagraphSorting::sortParagraphPrivate(bool indentedBlock)
{
    BaseTextEditor *editor = BaseTextEditor::currentTextEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) return;

    QTextCursor sortingSelection = cursor;

    QTextBlock block = cursor.block();

    // used if indentedBlock is enabled
    QRegularExpression leadingWhitespaceRegExp("(^\\s*).*");
    QRegularExpressionMatch match = leadingWhitespaceRegExp.match(block.text());
    const QString indention = match.captured(1);

    // find start of block (trimmed && empty || not same indention)
    while (block.previous().isValid() && !block.previous().text().trimmed().isEmpty()) {
        block = block.previous();

        if (indentedBlock) {
            match = leadingWhitespaceRegExp.match(block.text());
            const QString currentIndention = match.captured(1);
            if (currentIndention != indention) {
                block = block.next();
                break;
            }
        }
    }
    sortingSelection.setPosition(block.position());

    // find end of block (trimmed && empty)
    block = cursor.block();
    while (block.next().isValid() && !block.next().text().trimmed().isEmpty()) {
        block = block.next();

        if (indentedBlock) {
            match = leadingWhitespaceRegExp.match(block.text());
            const QString currentIndention = match.captured(1);
            if (currentIndention != indention) {
                block = block.previous();
                break;
            }
        }
    }

    // select all lines that should be sorted
    sortingSelection.setPosition(block.position() + block.length() - 1, QTextCursor::KeepAnchor);

    // order lines
    QStringList lines = sortingSelection.selectedText().split(QChar::ParagraphSeparator, Qt::SkipEmptyParts);
    lines.sort(Qt::CaseInsensitive);
    const QString sortedLines = lines.join(QChar::ParagraphSeparator);

    // replace text
    sortingSelection.beginEditBlock();
    sortingSelection.insertText(sortedLines);
    sortingSelection.endEditBlock();

    sortingSelection.setPosition(sortingSelection.position() + 1);
    editor->setTextCursor(sortingSelection);
}
