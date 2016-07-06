#ifndef QEXPRESSION_H
#define QEXPRESSION_H

#include <QList>
#include <QString>

typedef double (*FunctionCall)(double a, double b);

struct Operator {
    QString signature;
    int priority;
    FunctionCall function;
};

class OperatorList
{
    QList<Operator> m_operatorList;

public:
    OperatorList();
    int priority(const QString &signature);
    bool hasOperator(const QString &signature);
    FunctionCall function(const QString &signature);
};

class QExpression
{
public:
    enum EvaluationError {
        NoError = 0,
        NoExpression,
        UnknownIdentifier,
        UnexpectedEnd
    };
    QExpression();
    QExpression(const QString &expression);
    bool eval();
    double result();
    EvaluationError error();
    void setExpression(const QString &expression);

private:
    static OperatorList m_operators;
    QString m_expression;
    QString m_reversePolishNotation;
    double m_result = 0.0;
    EvaluationError m_error = NoError;
    bool toReversePolishNotation();
};

#endif // QEXPRESSION_H
