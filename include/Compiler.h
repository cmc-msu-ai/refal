#ifndef _REFAL_COMPILER_H_
#define _REFAL_COMPILER_H_

#include "Refal.h"
#include "Lang.h"

#include <list>
#include <stack>
#include <vector>

using std::list;
using std::stack;
using std::vector;

class Compiler
{
public:
    static void Compile(Lang *lang, Refal :: Base *begin, Refal :: Base *end,
                                                        Refal :: Var *table);

private:
    Compiler(Lang *lang, Refal :: Base *begin, Refal :: Base *end,
                                                        Refal :: Var *table);
    ~Compiler();

private:
    struct Hole
    {
        int left, right;
        Refal :: Base *begin, *end;

        Hole();
        Hole(int left, int right, Refal::Base *begin, Refal::Base *end);
    };

    struct Var
    {
        bool have_value;
        int mv_jsp;
        int nel_index;
        Refal :: Qualifier *qualifier;
        Refal :: Var :: Type type;

        Var();
    };
    typedef list< Hole > HolesTuple;

private:
    bool isVE(const Refal :: Base *elem, int &k) const;
    bool isVar(const Refal :: Base *elem, int &k) const;

    void processHoles(HolesTuple &holes);
    void processHoles1(HolesTuple &holes);
    void processHoles2(HolesTuple &holes, int var);
    void processHoles3(HolesTuple &holes);
    void processHoles4(HolesTuple &holes);

    void eoe(int n);
    void elementMatch(HolesTuple &holes);
    void holesLDEcompile(HolesTuple &holes);

    void glve(HolesTuple &holes);
    void glmax(HolesTuple &holes);
    void grmax(HolesTuple &holes);

    void gnil(HolesTuple &holes);
    void gcve(HolesTuple &holes, int var);
    void glb(HolesTuple &holes);
    void glsc(HolesTuple &holes);
    void gls(HolesTuple &holes, int var);
    void glw(HolesTuple &holes, int var);
    void glved(HolesTuple &holes, int var);
    void grb(HolesTuple &holes);
    void grsc(HolesTuple &holes);
    void grs(HolesTuple &holes, int var);
    void grw(HolesTuple &holes, int var);
    void grved(HolesTuple &holes, int var);

private:
    Lang *lang;
    HolesTuple::iterator i;
    int b1, b2, nel, jsp;
    Var table[128];
};

#endif

