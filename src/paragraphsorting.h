#ifndef PARAGRAPHSORTING_H
#define PARAGRAPHSORTING_H

#include <QObject>

class ParagraphSorting : public QObject
{
    Q_OBJECT
public:
    explicit ParagraphSorting(QObject *parent = 0);

public slots:
    void onSortParagraphAction();
    void onSortIndentedParagraphAction();

private:
    void sortParagraphPrivate(bool indentedBlock);
};

#endif // PARAGRAPHSORTING_H
