#include "cppquickfixadditions.h"

#include <QApplication>

#include <cplusplus/ASTPath.h>
#include <cpptools/baseeditordocumentprocessor.h>
#include <cpptools/cppmodelmanager.h>
#include <cpptools/cpprefactoringchanges.h>
#include <texteditor/texteditor.h>

using namespace CppEditor::Internal;
using namespace CppEditor;
using namespace TextEditor;

namespace {
class AddRemoveNotInfrontOfStatementOp: public CppQuickFixOperation
{
public:
    enum Type {
        AddNot,
        RemoveNot
    };

public:
    AddRemoveNotInfrontOfStatementOp(const CppQuickFixInterface &interface, int priority, Type type, int position)
        : CppQuickFixOperation(interface, priority),
          m_type(type),
          m_position(position)
    {
        if (m_type == AddNot) {
            setDescription(QApplication::translate("EditingEnhancement::QuickFix", "Add ! to statement"));
        } else {
            setDescription(QApplication::translate("EditingEnhancement::QuickFix", "Remove ! from statement"));
        }
    }

    void perform()
    {
        CppTools::CppRefactoringChanges refactoring(CppTools::CppModelManager::instance()->snapshot());
        CppTools::CppRefactoringFilePtr currentFile = refactoring.file(fileName());

        Utils::ChangeSet changes;

        if (m_type == AddNot) {
            changes.insert(m_position, QLatin1String("!"));
        } else {
            changes.remove(m_position-1, m_position);
        }

        currentFile->setChangeSet(changes);
        currentFile->apply();
    }

private:
    Type m_type;
    int m_position;
};
}

void AddNotInfrontOfStatement::match(const CppQuickFixInterface &interface, TextEditor::QuickFixOperations &result)
{
    BaseTextEditor *editor = BaseTextEditor::currentTextEditor();
    const QString fileName = editor->document()->filePath().toString();
    CppTools::SemanticInfo semanticInfo = CppTools::CppModelManager::instance()->editorDocumentProcessor(editor->textDocument())->recalculateSemanticInfo();
    CPlusPlus::ASTPath astPath(semanticInfo.doc);

    const QList<CPlusPlus::AST *> &path = astPath(editor->textCursor());

    if (path.isEmpty() || path.last()->asBinaryExpression()) {
        return;
    }

    int indexOfExpression = -1;
    for (int index = path.size() - 1; index != -1; --index) {
        CPlusPlus::AST *node = path.at(index);
        if (node->asExpression()) {
            indexOfExpression = index;
            break;
        }
    }

    if (indexOfExpression == -1) {
        return;
    }

    int parentIndex = indexOfExpression - 1;
    CPlusPlus::AST *parentNode = path.at(parentIndex);
    CPlusPlus::CallAST * callAst = 0;
    while (parentNode->asMemberAccess() || ((callAst = parentNode->asCall()) && callAst->base_expression == path.at(parentIndex + 1))) {
        parentNode = path.at(--parentIndex);
    }

    const CppTools::CppRefactoringChanges refactoring(CppTools::CppModelManager::instance()->snapshot());
    CppTools::CppRefactoringFilePtr currentFile = refactoring.file(fileName);
    const CPlusPlus::UnaryExpressionAST * unaryType = parentNode->asUnaryExpression();
    const bool isNegation = unaryType && currentFile->tokenAt(unaryType->unary_op_token).is(CPlusPlus::T_EXCLAIM);

    const CPlusPlus::AST *node = path.at(indexOfExpression);
    const int startPos = currentFile->startOf(node);

    const AddRemoveNotInfrontOfStatementOp::Type typeOfChange = isNegation ? AddRemoveNotInfrontOfStatementOp::RemoveNot
                                                                           : AddRemoveNotInfrontOfStatementOp::AddNot;

    result.append(new AddRemoveNotInfrontOfStatementOp(interface, 1, typeOfChange, startPos));
}
