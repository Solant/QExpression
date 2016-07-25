#ifndef QEXPRESSION_H
#define QEXPRESSION_H

#include <QList>
#include <QString>

class BaseFunction
{
public:
    enum FunctionType { Function, BinaryOperator, UnaryOperator };

protected:
    int m_priority;
    int m_numberOfArguments;
    FunctionType m_type;
    QString m_name;

public:
    BaseFunction(const int &priority, const int &numberOfArguments,
                 const FunctionType &type, const QString &name);
    virtual ~BaseFunction() {}
    int priority() const;
    int numberOfArguments() const;
    FunctionType type() const;
    QString name() const;
};

typedef double (*CustomFunction)(const QList<double> &);
class Function : public BaseFunction
{
    CustomFunction m_function;

public:
    Function(const QString &name, const int numberOfArguments,
             const CustomFunction function);
    CustomFunction function() const;
};

typedef double (*BinaryOperatorFunction)(double a, double b);
class BinaryOperator : public BaseFunction
{
private:
    BinaryOperatorFunction m_function;

public:
    BinaryOperator(const QString &name, const int priority,
                   const BinaryOperatorFunction function);
    BinaryOperatorFunction function() const;
};

class FunctionList
{
    QList<BaseFunction *> m_functionList;

public:
    FunctionList();
    ~FunctionList();
    const BaseFunction *at(int i) const;
    const BaseFunction *op(const QString &signature) const;
    int size() const;
    int priority(const QString &signature) const;
    bool hasOperator(const QString &signature) const;
};

class QExpression
{
public:
    enum EvaluationError {
        NoError = 0,
        NoExpression,
        UnknownIdentifier,
        UnexpectedEnd,
        WrongNumberOfArguments
    };
    QExpression();
    QExpression(const QString &expression);
    bool eval();
    double result();
    EvaluationError error();
    void setExpression(const QString &expression);

private:
    static FunctionList m_functions;
    QString m_expression;
    QString m_reversePolishNotation;
    double m_result = 0.0;
    EvaluationError m_error = NoError;
    bool toReversePolishNotation();
    void normalize();
};

#endif // QEXPRESSION_H
