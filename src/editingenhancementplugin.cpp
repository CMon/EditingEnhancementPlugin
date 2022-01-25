#include "editingenhancementplugin.h"

#include "editingenhancementconstants.h"
//#include "cppquickfixadditions.h"

#include <coreplugin/icore.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QAction>
#include <QMenu>

#include <QtPlugin>

using namespace Core;

using namespace EditingEnhancement::Internal;

EditingEnhancementPlugin::EditingEnhancementPlugin()
    :
      m_paragraphSorting(this)
    , m_specialAlignment(this)
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
    cmd1->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+E,Ctrl+Shift+S")));
    connect(action, SIGNAL(triggered()), &m_paragraphSorting, SLOT(onSortParagraphAction()));

    // sort lines that are identically indented with the current line, usefull when ordering SOURCES or HEADERS in a .pro file
    action = new QAction(tr("Sort indented paragraph"), this);
    Core::Command *cmd2 = Core::ActionManager::registerAction(action, Constants::SortIndentedParagraphActionID, Core::Context(Core::Constants::C_GLOBAL));
    cmd2->setDefaultKeySequence(QKeySequence(tr("Ctrl+E,Ctrl+S")));
    connect(action, SIGNAL(triggered()), &m_paragraphSorting, SLOT(onSortIndentedParagraphAction()));

    /* special alignment to verticaly align elements for example:
     * Input:
     * asdf = 3
     * a = 2
     * bcderf = 4
     * select from first line '=' until last line '= ', than it the matcher searches for all similar chars from the start of the selection and the end of the selection
     * (in this case '= ' ) and than aligns all lines inbetween the selection.
     * output:
     * asdf   = 3
     * a      = 2
     * bcderf = 4
     */
    action = new QAction(tr("Advanced Alignment"), this);
    Core::Command *cmd3 = Core::ActionManager::registerAction(action, Constants::AdvancedAlignmentActionID, Core::Context(Core::Constants::C_GLOBAL));
    cmd3->setDefaultKeySequence(QKeySequence(tr("Ctrl+E,Ctrl+R")));
    connect(action, SIGNAL(triggered()), &m_specialAlignment, SLOT(align()));

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("Editing enhancement"));
    menu->addAction(cmd1);
    menu->addAction(cmd2);
    menu->addAction(cmd3);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    // add custom quickfixes for cppeditor
//    new CppEditor::AddNotInfrontOfStatement;

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
