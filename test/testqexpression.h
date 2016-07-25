#ifndef TESTQEXPRESSION_H
#define TESTQEXPRESSION_H

#include <QtTest>

class TestQExpression : public QObject
{
    Q_OBJECT
public:
    explicit TestQExpression(QObject *parent = 0);

signals:

private slots:
    void emptyString();
    void evalAfterEmpty();
    void unknownIdentifier();
    void unexpectedEnd();
    void simpleSum();
    void simpleDiv();
    void complexExpression();
    void sampleFunction();
    void functionWithThreeParameters();
    void functionWithWrongNumberOfArguments();
    void unaryOperator();
};

#endif // TESTQEXPRESSION_H
