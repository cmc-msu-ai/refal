#ifndef _REFAL_H_
#define _REFAL_H_

#include <map>
#include <string>

using std::map;
using std::string;

class Compiler;
class Lang;

class Refal
{
    friend class Compiler;
    friend class Lang;

public:
    Refal();
    ~Refal();

    bool SetPattern(const char *text);
    bool AddQualifier(const char *name, const char *text);
    bool Equ(const char *text, int *result);

private:
    /* types */
    typedef unsigned int RefalLabel;
    typedef unsigned int RefalNumber;
    typedef char RefalChar;

    /* Qualifier struct */
    struct Qualifier
    {
        enum Type
        {
            S, F, N, O, L, D, W, B, NG, SL, SN, SC, Q
        };
        
        Type type;
        Qualifier *next;
        
        Qualifier(Type type);
    };

    struct QualifierLabel : Qualifier
    {
        RefalLabel label;
        
        QualifierLabel(RefalLabel label);
    };

    struct QualifierNumber : Qualifier
    {
        RefalNumber number;
        
        QualifierNumber(RefalNumber number);
    };

    struct QualifierChar : Qualifier
    {
        RefalChar c;
        
        QualifierChar(RefalChar c);
    };

    struct QualifierQualifier : Qualifier
    {
        Qualifier *qualifier;
        
        QualifierQualifier(Qualifier *qualifier);
    };
    
    /* Expression struct */
    struct Base
    {
        enum Type
        {
            NUMBER, LABEL, CHAR, VARIABLE, LEFT_PAREN, RIGHT_PAREN
        };
        
        Type type;
        Base *next, *prev;

    protected:
        Base(Type t);
    };

    struct LeftParen : Base
    {
        Base *right_paren;
        
        LeftParen();
    };

    struct RightParen : Base
    {
        Base *left_paren;
        
        RightParen();
    };

    struct Number : Base
    {
        RefalNumber number;
        
        Number(RefalNumber n);
    };

    struct Label : Base
    {
        RefalLabel label;
        
        Label(RefalLabel l);
    };

    struct Char : Base
    {
        RefalChar c;
        
        Char(RefalChar a);
    };

    struct Variable : Base
    {
        typedef char VariableName;
        
        VariableName name;
        
        Variable(VariableName name);
    };

    /* other types */
    struct Var
    {
        Var();

        enum Type
        {
            NONE, S = 's', W = 'w', V = 'v', E = 'e'
        };

        Type type;
        Qualifier *qualifier;
        Base *begin, *end;
    };

    typedef map<string, int> LabelsTable;
    typedef map<string, Qualifier*> QualifiersTable;
    typedef Base *PBase;

    enum
    {
        VARIABLES_TABLE_SIZE = 128
    };

private:
    static bool equS(Base *a, Base *b);

    void resetVariablesTable();
    bool is_ident(const char *s) const;

    Qualifier *read_qualifier(const char *text, int &i);
    void free_qualifier(Qualifier *q);
    static bool check_qualifier(Qualifier *q, Base *e);

    bool read_base(const char *text, PBase &begin, PBase &end,
                                                    bool with_vars = false);
    bool describe_variable(char name, Var :: Type type, Qualifier *q);
    void free_base(Base *base);
    void compile(Base *begin, Base *end);

    static void print_q(Qualifier *q);
    static void print_e(Base *e, Base *f = 0);

private:
    /* fields */
    Lang *currentLang;

    unsigned int labelsCounter;
    LabelsTable labelsTable;
    QualifiersTable qualifiersTable;

    static Var variablesTable[VARIABLES_TABLE_SIZE];
};

#endif

