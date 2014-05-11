#include "Refal.h"
#include "Compiler.h"
#include "Lang.h"

#include <stdio.h>
#include <ctype.h>
#include <map>
#include <stack>
#include <string>
#include <vector>

using std::map;
using std::stack;
using std::string;
using std::vector;

Refal::Var Refal::variablesTable[VARIABLES_TABLE_SIZE];

Refal::Var::Var()
  : type(NONE), qualifier(0), begin(0), end(0)
{
}

Refal::Refal()
  : currentLang(0), labelsCounter(0)
{
}

Refal::~Refal()
{
    /*
    for(QualifiersTable::const_iterator i = qualifiersTable.begin();
          i != qualifiersTable.end(); ++i)
    {
        printf("%s\r\n", i -> first . c_str());
    }
    */
}

bool Refal::equS(Base *a, Base *b)
{
    switch(a -> type)
    {
    case Base :: LABEL :
        return b -> type == Base :: LABEL &&
                static_cast<Label*>(a) -> label ==
                  static_cast<Label*>(b) -> label;
        break;
    case Base :: NUMBER :
        return b -> type == Base :: NUMBER &&
                static_cast<Number*>(a) -> number ==
                  static_cast<Number*>(b) -> number;
        break;
    case Base :: CHAR :
        return b -> type == Base :: CHAR &&
                static_cast<Char*>(a) -> c ==
                  static_cast<Char*>(b) -> c;
        break;
    default:
        return false;
    }
}

bool Refal::SetPattern(const char *text)
{
    resetVariablesTable();

    Base *begin, *end;
    if( !read_base(text, begin, end, true) )
    {
        return false;
    }

    compile(begin, end);
    /*free_base(begin);*/
    
    return true;
}

void Refal::compile(Base *begin, Base *end)
{
    if( currentLang != 0 )
    {
        delete currentLang;
    }
    currentLang = new Lang();
    Compiler :: Compile(currentLang, begin, end, variablesTable);
}

bool Refal::Equ(const char *text, int *result)
{
    Base *begin, *end;
    if( !read_base(text, begin, end) )
    {
        return false;
    }
    *result = currentLang -> Run(begin, end);
    free_base(begin);
    return true;
}

bool Refal::AddQualifier(const char *name, const char *text)
{
    if( !is_ident(name) )
    {
        fputs("Wrong qualifier identifier\r\n", stderr);
        return false;
    }

    int s = 0;
    Qualifier *q = read_qualifier(text, s);
    if( q == 0 )
    {
        return false;
    }
    else if( text[s] != '\0' )
    {
        fputs("Wrong symbol in qualifier\r\n", stderr);
        return false;
    }
    string sname;
    for(; *name != '\0'; ++name)
    {
        sname += tolower(*name);
    }
    try
    {
        Qualifier *wq = qualifiersTable.at(sname);
        free_qualifier(wq);
        fputs("Warning: Duplicate qualifier\r\n", stderr);
    }
    catch(...)
    {
    }
    qualifiersTable[sname] = q;

    return true;
}

bool Refal::is_ident(const char *s) const
{
    if( !isalpha(*s) && *s != '_' )
    {
        return false;
    }
    while( *s != '\0' && ( isalnum(*s) || *s == '_' ) )
    {
        ++s;
    }
    return *s == '\0';
}

void Refal::resetVariablesTable()
{
    for(int i = 0; i < VARIABLES_TABLE_SIZE; ++i)
    {
        free_qualifier(variablesTable[i].qualifier);
        variablesTable[i].type  = Var :: NONE;
        variablesTable[i].begin = 0;
        variablesTable[i].end   = 0;
    }
}

/* Qualifier struct */
Refal::Qualifier::Qualifier(Type t)
  : type(t)
{
}

Refal::QualifierLabel::QualifierLabel(RefalLabel l)
  : Qualifier(SL), label(l)
{
}

Refal::QualifierNumber::QualifierNumber(RefalNumber n)
  : Qualifier(SN), number(n)
{
}

Refal::QualifierChar::QualifierChar(RefalChar a)
  : Qualifier(SC), c(a)
{
}

Refal::QualifierQualifier::QualifierQualifier(Qualifier *q)
  : Qualifier(Q), qualifier(q)
{
}

/* Qualifier parser */
Refal::Qualifier *Refal::read_qualifier(const char *s, int &i)
{
#define APPEND(q)\
    do\
    {\
        if(begin)\
        {\
            end -> next = q;\
            end = end -> next;\
        }\
        else\
        {\
            begin = q;\
            end = begin;\
        }\
        end -> next = 0;\
    }\
    while(0)

    enum State
    {
        H, A, B, C, D, E, F, G
    };
    
    State state = H;
    Qualifier *begin = 0, *end;
    bool in = false;
    int tmp;
    string name;
    
    for(; s[i] != '\0'; ++i)
    {
        switch(state)
        {
        case H :
            if( s[i] == '\'' )
            {
                state = A;
            }
            else if( s[i] == '/' )
            {
                state = C;
            }
            else if( s[i] == ':' )
            {
                state = F;
            }
            else if( s[i] == '(' )
            {
                in = true;
                APPEND(new Qualifier(Qualifier :: NG));
            }
            else if( s[i] == ')' )
            {
                if( in )
                {
                    in = false;
                    APPEND(new Qualifier(Qualifier :: NG));
                }
                else
                {
                    return begin;
                }
            }
            else if( s[i] == 's' || s[i] == 'S' )
            {
                APPEND(new Qualifier(Qualifier :: S));
            }
            else if( s[i] == 'f' || s[i] == 'F' )
            {
                APPEND(new Qualifier(Qualifier :: F));
            }
            else if( s[i] == 'n' || s[i] == 'N' )
            {
                APPEND(new Qualifier(Qualifier :: N));
            }
            else if( s[i] == 'o' || s[i] == 'O' )
            {
                APPEND(new Qualifier(Qualifier :: O));
            }
            else if( s[i] == 'l' || s[i] == 'L' )
            {
                APPEND(new Qualifier(Qualifier :: L));
            }
            else if( s[i] == 'd' || s[i] == 'D' )
            {
                APPEND(new Qualifier(Qualifier :: D));
            }
            else if( s[i] == 'w' || s[i] == 'W' )
            {
                APPEND(new Qualifier(Qualifier :: W));
            }
            else if( s[i] == 'b' || s[i] == 'B' )
            {
                APPEND(new Qualifier(Qualifier :: B));
            }
            else if( isblank(s[i]) )
            {
            }
            else
            {
                return begin;
            }
            break;
        case A :
            if( s[i] == '\'' )
            {
                state = B;
            }
            else
            {
                APPEND(new QualifierChar(s[i]));
            }
            break;
        case B :
            if( s[i] == '\'' )
            {
                APPEND(new QualifierChar('\''));
                state = A;
            }
            else
            {
                --i;
                state = H;
            }
            break;
        case C :
            if( isdigit(s[i]) )
            {
                tmp = s[i] - '0';
                state = D;
            }
            else if( isalpha(s[i]) || s[i] == '_' )
            {
                name = tolower(s[i]);
                state = E;
            }
            else
            {
                fputs("Incorrect symbol in label or number in qualifier\r\n",
                                                                       stderr);
                return 0;
            }
            break;
        case D :
            if( isdigit(s[i]) )
            {
                tmp = tmp * 10 + s[i] - '0';
            }
            else if( s[i] == '/' )
            {
                APPEND(new QualifierNumber(tmp));
                state = H;
            }
            else
            {
                fputs("Incorrect symbol in label or number in qualifier\r\n",
                                                                       stderr);
                return 0;
            }
            break;
        case E :
            if( isalnum(s[i]) || s[i] == '_' )
            {
                name += tolower(s[i]);
            }
            else if( s[i] == '/' )
            {
                try
                {
                    int n = labelsTable.at(name);
                    APPEND(new QualifierLabel(n));
                }
                catch(...)
                {
                    labelsCounter++;
                    labelsTable[name] = labelsCounter;
                    APPEND(new QualifierLabel(labelsCounter));
                }
                state = H;
            }
            else
            {
                fputs("Incorrect symbol in label or number in qualifier\r\n",
                                                                       stderr);
                return 0;
            }
            break;
        case F :
            if( isalpha(s[i]) || s[i] == '_' )
            {
                name = tolower(s[i]);
                state = G;
            }
            else
            {
                fputs("Incorrect symbol in qualifier name\r\n", stderr);
                return 0;
            }
            break;
        case G :
            if( isalnum(s[i]) || s[i] == '_' )
            {
                name += tolower(s[i]);
            }
            else if( s[i] == ':' )
            {
                try
                {
                    APPEND(new QualifierQualifier(qualifiersTable.at(name)));
                    state = H;
                }
                catch(...)
                {
                    fputs("Qualifier not found\r\n", stderr);
                    return 0;
                }
            }
            else
            {
                fputs("Incorrect symbol in qualifier name\r\n", stderr);
                return 0;
            }
            break;
        default :
            break;
        }
    }
    return begin;
#undef APPEND
}

void Refal::free_qualifier(Qualifier *q)
{
    while(q)
    {
        Qualifier *tmp = q;
        q = q -> next;
        delete tmp;
    }
}

/* Qualifier check */
bool Refal::check_qualifier(Qualifier *q, Base *e)
{
    bool pls = true, last_ng = true;
    Base :: Type type = e -> type;
    
    for(; q != 0; q = q -> next)
    {
        last_ng = false;
        switch( q -> type )
        {
        case Qualifier :: NG :
            pls = ! pls;
            last_ng = true;
            break;
        case Qualifier :: SL :
            if( type == Base :: LABEL &&
                static_cast<QualifierLabel*>(q) -> label ==
                static_cast<Label*>(e) -> label )
            {
                return pls;
            }
            break;
        case Qualifier :: SN :
            if( type == Base :: NUMBER &&
                static_cast<QualifierNumber*>(q) -> number ==
                static_cast<Number*>(e) -> number )
            {
                return pls;
            }
            break;
        case Qualifier :: SC :
            if( type == Base :: CHAR &&
                static_cast<QualifierChar*>(q) -> c ==
                static_cast<Char*>(e) -> c )
            {
                return pls;
            }
            break;
        case Qualifier :: Q :
            if( check_qualifier(
                    static_cast<QualifierQualifier*>(q) -> qualifier, e) )
            {
                return pls;
            }
            break;
        case Qualifier :: S :
            if( type == Base :: LABEL || type == Base :: NUMBER ||
                type == Base ::  CHAR )
            {
                return pls;
            }
            break;
        case Qualifier :: F :
            if( type == Base :: LABEL )
            {
                return pls;
            }
            break;
        case Qualifier :: N :
            if( type == Base :: NUMBER )
            {
                return pls;
            }
            break;
        case Qualifier :: O :
            if( type == Base :: CHAR )
            {
                return pls;
            }
            break;
        case Qualifier :: L :
            if( type == Base :: CHAR && isalpha(static_cast<Char*>(e) -> c) )
            {
                return pls;
            }
            break;
        case Qualifier :: D :
            if( type == Base :: CHAR && isdigit(static_cast<Char*>(e) -> c) )
            {
                return pls;
            }
            break;
        case Qualifier :: W :
            return pls;
            break;
        case Qualifier :: B :
            if( type == Base :: LEFT_PAREN || type == Base :: RIGHT_PAREN )
            {
                return pls;
            }
            break;
        default :
            break;
        }
    }
    return last_ng;
}

/* Expression struct */
Refal::Base::Base(Type t)
  : type(t), next(0), prev(0)
{
}

Refal::LeftParen::LeftParen()
  : Base(LEFT_PAREN), right_paren(0)
{
}
Refal::RightParen::RightParen()
  : Base(RIGHT_PAREN), left_paren(0)
{
}
Refal::Number::Number(RefalNumber n)
  : Base(NUMBER), number(n)
{
}
Refal::Label::Label(RefalLabel l)
  : Base(LABEL), label(l)
{
}
Refal::Char::Char(RefalChar a)
  : Base(CHAR), c(a)
{
}
Refal::Variable::Variable(VariableName n)
  : Base(VARIABLE), name(n)
{
}

/* parser */
bool Refal::read_base(const char *s, PBase &begin, PBase &end, bool with_vars)
{
#define APPEND(e)\
    do\
    {\
        if(begin)\
        {\
            end -> next = e;\
            end -> next -> prev = end;\
            end = end -> next;\
        }\
        else\
        {\
            begin = e;\
            begin -> prev = 0;\
            end = begin;\
        }\
        end -> next = 0;\
    }\
    while(0)
    
    enum State
    {
        H, A, B, C, D, E, F, G, I, J
    };
    
    State state = H;
    stack<Base*> parenthesis;
    int tmp;
    Var :: Type tmpt;
    Qualifier *tmpq;
    string name;
    
    begin = 0;
    for(int i = 0; s[i] != '\0'; ++i)
    {
        switch(state)
        {
        case H :
            if( s[i] == '\'' )
            {
                state = A;
            }
            else if( s[i] == '/' )
            {
                state = C;
            }
            else if( s[i] == '(' )
            {
                APPEND(new LeftParen);
                parenthesis.push(end);
            }
            else if( s[i] == ')' )
            {
                if( parenthesis.empty() )
                {
                    fputs("Unexpected right parenthesis\r\n", stderr);
                    return false;
                }
                APPEND(new RightParen);
                Base *lp = parenthesis.top();
                static_cast<LeftParen*>(lp) -> right_paren = end;
                static_cast<RightParen*>(end) -> left_paren = lp;
                parenthesis.pop();
            }
            else if( with_vars &&
                     (  s[i] == 's' || s[i] == 'S'
                     || s[i] == 'w' || s[i] == 'W'
                     || s[i] == 'v' || s[i] == 'V'
                     || s[i] == 'e' || s[i] == 'E' ) )
            {
                switch(tolower(s[i]))
                {
                    case 's' : tmpt = Var :: S; break;
                    case 'w' : tmpt = Var :: W; break;
                    case 'v' : tmpt = Var :: V; break;
                    case 'e' : tmpt = Var :: E; break;
                }
                tmpq = 0;
                state = F;
            }
            else if( isblank(s[i]) )
            {
            }
            else
            {
                fputs("Unexpected symbol\r\n", stderr);
                return false;
            }
            break;
        case A :
            if( s[i] == '\'' )
            {
                state = B;
            }
            else
            {
                APPEND(new Char(s[i]));
            }
            break;
        case B :
            if( s[i] == '\'' )
            {
                APPEND(new Char('\''));
                state = A;
            }
            else
            {
                --i;
                state = H;
            }
            break;
        case C :
            if( isdigit(s[i]) )
            {
                tmp = s[i] - '0';
                state = D;
            }
            else if( isalpha(s[i]) || s[i] == '_' )
            {
                name = tolower(s[i]);
                state = E;
            }
            else
            {
                fputs("Incorrect symbol in label or number\r\n", stderr);
                return false;
            }
            break;
        case D :
            if( isdigit(s[i]) )
            {
                tmp = tmp * 10 + s[i] - '0';
            }
            else if( s[i] == '/' )
            {
                APPEND(new Number(tmp));
                state = H;
            }
            else
            {
                fputs("Incorrect symbol in label or number\r\n", stderr);
                return false;
            }
            break;
        case E :
            if( isalnum(s[i]) || s[i] == '_' )
            {
                name += tolower(s[i]);
            }
            else if( s[i] == '/' )
            {
                try
                {
                    int n = labelsTable.at(name);
                    APPEND(new Label(n));
                }
                catch(...)
                {
                    labelsCounter++;
                    labelsTable[name] = labelsCounter;
                    APPEND(new Label(labelsCounter));
                }
                state = H;
            }
            else
            {
                fputs("Incorrect symbol in label or number\r\n", stderr);
                return false;
            }
            break;
        case F :
            if( s[i] == '(' )
            {
                ++i;
                tmpq = read_qualifier(s, i);
                if( tmpq == 0 )
                {
                    return false;
                }
                if( s[i] != ')' )
                {
                    fputs("Unexpceted symbol\r\n", stderr);
                    return false;
                }
                state = J;
            }
            else if( s[i] == ':' )
            {
                state = G;
            }
            else if( isalnum(s[i]) )
            {
                if( !describe_variable(s[i], tmpt, tmpq) )
                {
                    return false;
                }
                APPEND(new Variable(s[i]));
                state = H;
            }
            else
            {
                fputs("Incorrect variable name\r\n", stderr);
                return false;
            }
            break;
        case G :
            if( isalpha(s[i]) || s[i] == '_' )
            {
                name = tolower(s[i]);
                state = I;
            }
            else
            {
                fputs("Incorrect symbol in qualifier name\r\n", stderr);
                return false;
            }
            break;
        case I :
            if( isalnum(s[i]) || s[i] == '_' )
            {
                name += tolower(s[i]);
            }
            else if( s[i] == ':' )
            {
                try
                {
                    tmpq = qualifiersTable.at(name);
                    state = J;
                }
                catch(...)
                {
                    fputs("Qualifier not found\r\n", stderr);
                    return false;
                }
            }
            else
            {
                fputs("Incorrect symbol in qualifier name\r\n", stderr);
                return false;
            }
            break;
        case J :
            if( isalnum(s[i]) )
            {
                if( !describe_variable(s[i], tmpt, tmpq) )
                {
                    return false;
                }
                APPEND(new Variable(s[i]));
                state = H;
            }
            else
            {
                fputs("Incorrect variable name\r\n", stderr);
                return false;
            }
            break;
        default :
            break;
        }
    }
    
    if( !parenthesis.empty() )
    {
        fputs("Unclosed parenthesis\r\n", stderr);
        return false;
    }

    return true;
#undef APPEND
}

bool Refal::describe_variable(char name, Var :: Type type, Qualifier *q)
{
    Var &v = variablesTable[ int(name) ];
    if( v.type != Var :: NONE && v.type != type )
    {
        fputs("Type mismatched\r\n", stderr);
        return false;
    }
    v.type = type;
    if( v.qualifier && q )
    {
        fputs("Warning: More than one qualifier\r\n", stderr);
        free_qualifier(q);
    }
    else
    {
        v.qualifier = q;
    }
    return true;
}

void Refal::free_base(Base *base)
{
    while(base)
    {
        Base *tmp = base;
        base = base -> next;
        delete tmp;
    }
}

void Refal::print_q(Qualifier *q)
{
    bool pls = true;
    for(; q != 0; q = q -> next)
    {
        switch( q -> type )
        {
        case Qualifier :: NG :
            printf(pls ? "(" : ")");
            pls = ! pls;
            break;
        case Qualifier :: SL :
            printf("/L%d/", static_cast<QualifierLabel*>(q) -> label);
            break;
        case Qualifier :: SN :
            printf("/%d/", static_cast<QualifierNumber*>(q) -> number);
            break;
        case Qualifier :: SC :
            printf("'%c'", static_cast<QualifierChar*>(q) -> c);
            break;
        case Qualifier :: Q :
            printf(":---:");
            break;
        case Qualifier :: S :
            printf("S");
            break;
        case Qualifier :: F :
            printf("F");
            break;
        case Qualifier :: N :
            printf("N");
            break;
        case Qualifier :: O :
            printf("O");
            break;
        case Qualifier :: L :
            printf("L");
            break;
        case Qualifier :: D :
            printf("D");
            break;
        case Qualifier :: W :
            printf("W");
            break;
        case Qualifier :: B :
            printf("B");
            break;
        default :
            break;
        }
    }
}

void Refal::print_e(Base *e, Base *f)
{
    for(; e != f; e = e -> next)
    {
        Variable *var = static_cast<Variable*>(e);
        switch( e -> type )
        {
        case Base :: LABEL :
            printf("/L%d/", static_cast<Label*>(e) -> label);
            break;
        case Base :: NUMBER :
            printf("/%d/", static_cast<Number*>(e) -> number);
            break;
        case Base :: CHAR :
            printf("'%c'", static_cast<Char*>(e) -> c);
            break;
        case Base :: LEFT_PAREN :
            printf("(");
            break;
        case Base :: RIGHT_PAREN :
            printf(")");
            break;
        case Base :: VARIABLE :
            switch(variablesTable[ int(var -> name) ].type)
            {
            case Var :: S :
                printf("S");
                break;
            case Var :: W :
                printf("W");
                break;
            case Var :: V :
                printf("V");
                break;
            case Var :: E :
                printf("E");
                break;
            default :
                break;
            }
            if(variablesTable[ int(var -> name) ].qualifier)
            {
                printf("(");
                print_q(variablesTable[ int(var -> name) ].qualifier);
                printf(")");
            }
            printf("%c", var -> name);
            break;
        default :
            break;
        }
    }
}

