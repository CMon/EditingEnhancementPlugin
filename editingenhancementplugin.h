#ifndef EDITINGENHANCEMENT_H
#define EDITINGENHANCEMENT_H

#include "editingenhancement_global.h"

#include <extensionsystem/iplugin.h>

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

private slots:
    void onSortParagraphAction();
};

} // namespace Internal
} // namespace EditingEnhancement

#endif // EDITINGENHANCEMENT_H

