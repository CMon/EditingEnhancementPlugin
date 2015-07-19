#ifndef SPECIALALIGNMENT_H
#define SPECIALALIGNMENT_H

#include <QObject>

class SpecialAlignment : public QObject
{
    Q_OBJECT
public:
    explicit SpecialAlignment(QObject *parent = 0);

public slots:
    void align();
};

#endif // SPECIALALIGNMENT_H
