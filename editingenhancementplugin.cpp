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

    QAction *action = new QAction(tr("Sort paragraph"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+E,Ctrl+S")));
    connect(action, SIGNAL(triggered()), this, SLOT(onSortParagraphAction()));

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("Editing enhancement"));
    menu->addAction(cmd);
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

void EditingEnhancementPlugin::onSortParagraphAction()
{
    BaseTextEditor *editor = BaseTextEditor::currentTextEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) return;

    QTextCursor sortingSelection = cursor;

    // find start of block (trimmed && empty)
    QTextBlock block = cursor.block();
    while (block.previous().isValid() && !block.previous().text().trimmed().isEmpty()) {
        block = block.previous();
    }
    sortingSelection.setPosition(block.position());

    // find end of block (trimmed && empty)
    block = cursor.block();
    while (block.next().isValid() && !block.next().text().trimmed().isEmpty()) {
        block = block.next();
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
