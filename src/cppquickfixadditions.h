#ifndef CPPQUICKFIXADDITIONS_H
#define CPPQUICKFIXADDITIONS_H

#include <cppeditor/cppquickfix.h>

namespace CppEditor {
namespace Internal { class CppQuickFixInterface; }

class AddNotInfrontOfStatement : public CppQuickFixFactory
{
    Q_OBJECT
public:
    void match(const Internal::CppQuickFixInterface &interface, TextEditor::QuickFixOperations &result);
};

} // namespace CppEditor

#endif // CPPQUICKFIXADDITIONS_H
