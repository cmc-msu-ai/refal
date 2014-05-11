#ifndef _REFAL_LANG_H_
#define _REFAL_LANG_H_

#include "Refal.h"

#include <vector>

using std::vector;

class Compiler;

class Lang
{
    friend class Compiler;

public:
    Lang();
    ~Lang();

    bool Run(Refal :: Base *begin, Refal :: Base *end);

private:
    enum Type
    {
        NIL,
        LSC, RSC,
        LB, RB,
        SB,
        LS, RS,
        LSD, RSD,
        CE,
        LED, RED,
        LW, RW,
        NNIL,
        PLE, PLV, LE,
        PRE, PRV, RE,
        EOE,
        WSPC, ESPC,
        /*PLESPC,*/ LESPC,
        /*PRESPC,*/ RESPC,
        LMAX, RMAX
    };

private:
    void AddOp(Type type);
    void AddOp(Type type, int x);
    void AddOp(Type type, int x, int y);
    void AddOp(Type type, Refal :: Base *elem);
    void AddOp(Type type, Refal :: Qualifier *qualifier);

private:
    struct BasicOp
    {
        Type type;
        BasicOp *next;

        BasicOp(Type type);
    };

    struct OpN : BasicOp
    {
        int n;

        OpN(Type type, int x);
    };

    struct OpNM : BasicOp
    {
        int n, m;

        OpNM(Type type, int x, int y);
    };

    struct OpS : BasicOp
    {
        Refal :: Base *elem;

        OpS(Type type, Refal :: Base *elem);
    };

    struct OpQ : BasicOp
    {
        Refal :: Qualifier *qualifier;

        OpQ(Type type, Refal :: Qualifier *qualifier);
    };

    struct State
    {
        int nel;
        BasicOp *op;
        Refal :: Base *b1, *b2;

        State();
        State(int nel, BasicOp *op, Refal :: Base *b1, Refal :: Base *b2);
    };
private:
    void addop(BasicOp *op);
    bool go(Refal :: Base *b1, Refal :: Base *b2);

    void print_table(int nel);
private:
    BasicOp *begin, *end;
    vector< Refal :: Base* > table;
    vector< State > stack;
};

#endif

