#ifndef EDITINGENHANCEMENT_H
#define EDITINGENHANCEMENT_H

#include <extensionsystem/iplugin.h>

#include "editingenhancement_global.h"

#include "paragraphsorting.h"
#include "specialalignment.h"

namespace EditingEnhancement {
namespace Internal {

class EditingEnhancementPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "EditingEnhancement.json")

public:
    EditingEnhancementPlugin();
    ~EditingEnhancementPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private:
    ParagraphSorting m_paragraphSorting;
    SpecialAlignment m_specialAlignment;
};

} // namespace Internal
} // namespace EditingEnhancement

#endif // EDITINGENHANCEMENT_H
