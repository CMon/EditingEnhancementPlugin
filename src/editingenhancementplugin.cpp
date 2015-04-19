#include "editingenhancementplugin.h"
#include "editingenhancementconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <texteditor/texteditor.h>
#include <texteditor/textdocument.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QTextBlock>

#include <QtPlugin>

using namespace Core;
using namespace TextEditor;

using namespace EditingEnhancement::Internal;

EditingEnhancementPlugin::EditingEnhancementPlugin()
{
    // Create your members
}

EditingEnhancementPlugin::~EditingEnhancementPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool EditingEnhancementPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // sort whole paragraph
    QAction *action = new QAction(tr("Sort paragraph"), this);
    Core::Command *cmd1 = Core::ActionManager::registerAction(action, Constants::SortParagraphActionID, Core::Context(Core::Constants::C_GLOBAL));
    cmd1->setDefaultKeySequence(QKeySequence(tr("Ctrl+E,Ctrl+S")));
    connect(action, SIGNAL(triggered()), this, SLOT(onSortParagraphAction()));

    // sort lines that are identically indented with the current line, usefull when ordering SOURCES or HEADERS in a .pro file
    action = new QAction(tr("Sort indented paragraph"), this);
    Core::Command *cmd2 = Core::ActionManager::registerAction(action, Constants::SortIndentedParagraphActionID, Core::Context(Core::Constants::C_GLOBAL));
    cmd2->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+E,Ctrl+Shift+S")));
    connect(action, SIGNAL(triggered()), this, SLOT(onSortIndentedParagraphAction()));

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("Editing enhancement"));
    menu->addAction(cmd1);
    menu->addAction(cmd2);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    return true;
}

void EditingEnhancementPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag EditingEnhancementPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void EditingEnhancementPlugin::sortParagraphPrivate(bool indentedBlock)
{
    BaseTextEditor *editor = BaseTextEditor::currentTextEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) return;

    QTextCursor sortingSelection = cursor;

    QTextBlock block = cursor.block();

    // used if indentedBlock is enabled
    QRegExp leadingWhitespaceRegExp(QLatin1String("(^\\s*).*"));
    leadingWhitespaceRegExp.indexIn(block.text());
    const QString indention = leadingWhitespaceRegExp.cap(1);

    // find start of block (trimmed && empty || not same indention)
    while (block.previous().isValid() && !block.previous().text().trimmed().isEmpty()) {
        block = block.previous();

        if (indentedBlock) {
            leadingWhitespaceRegExp.indexIn(block.text());
            const QString currentIndention = leadingWhitespaceRegExp.cap(1);
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
            leadingWhitespaceRegExp.indexIn(block.text());
            const QString currentIndention = leadingWhitespaceRegExp.cap(1);
            if (currentIndention != indention) {
                block = block.previous();
                break;
            }
        }
    }

    // select all lines that should be sorted
    sortingSelection.setPosition(block.position() + block.length() - 1, QTextCursor::KeepAnchor);

    // order lines
    QStringList lines = sortingSelection.selectedText().split(QChar::ParagraphSeparator, QString::SkipEmptyParts);
    lines.sort(Qt::CaseInsensitive);
    const QString sortedLines = lines.join(QChar::ParagraphSeparator);

    // replace text
    sortingSelection.beginEditBlock();
    sortingSelection.insertText(sortedLines);
    sortingSelection.endEditBlock();

    sortingSelection.setPosition(sortingSelection.position() + 1);
    editor->setTextCursor(sortingSelection);
}

void EditingEnhancementPlugin::onSortParagraphAction()
{
    sortParagraphPrivate(false);
}

void EditingEnhancementPlugin::onSortIndentedParagraphAction()
{
    sortParagraphPrivate(true);
}
