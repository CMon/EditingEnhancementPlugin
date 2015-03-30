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
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
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

static QString getTextOfCurrentLine (BaseTextEditor & editor, int globalPositionInLine = -1)
{
    int start = editor.position(TextPositionOperation::StartOfLinePosition, globalPositionInLine);
    int end = editor.position(TextPositionOperation::EndOfLinePosition, globalPositionInLine);

    return editor.textAt(start, end - start);
}

void EditingEnhancementPlugin::onSortParagraphAction()
{
    BaseTextEditor *editor = BaseTextEditor::currentTextEditor();
    if (!editor) return;

    const int startingLine = editor->currentLine();
    if (getTextOfCurrentLine(*editor).isEmpty()) return;

    // find start of block (trimmed & empty)
    int lineNr = startingLine;
    QString line;
    QStringList lines;
    while ((!(line = getTextOfCurrentLine(*editor)).trimmed().isEmpty()) && lineNr > 0) {
        lines.prepend(line);
        editor->gotoLine(--lineNr);
    }
    const int topLine = lineNr+1;
    const int topPosition = editor->position(TextPositionOperation::StartOfLinePosition);

    // find end of block (trimmed & empty)
    lineNr = startingLine + 1;
    editor->gotoLine(lineNr);
    while ((!(line = getTextOfCurrentLine(*editor)).trimmed().isEmpty())) {
        lines.append(line);
        editor->gotoLine(++lineNr);
    }
    editor->gotoLine(lineNr - 1);
    const int bottomPosition = editor->position(TextPositionOperation::EndOfLinePosition);

    if (lines.isEmpty()) {
        editor->gotoLine(startingLine);
    } else {
        qSort(lines);
        editor->gotoLine(topLine);
        editor->replace(bottomPosition - topPosition, lines.join(QLatin1Char('\n')));
    }
}
