#include "Compiler.h"
#include "Refal.h"

#include <stdio.h>

#include <list>
#include <stack>
#include <vector>

using std::list;
using std::stack;
using std::vector;

#define REFAL_DEBUG_FLAG 0

void Compiler::Compile(Lang *lang, Refal :: Base *begin, Refal :: Base *end,
                                                               Refal :: Var *t)
{
    Compiler compiler(lang, begin, end, t);
}

Compiler::Compiler(Lang *l, Refal :: Base *begin, Refal :: Base *end,
                                                            Refal :: Var *t)
  : lang(l), b1(-2), b2(-1), nel(0), jsp(0)
{
#if REFAL_DEBUG_FLAG
    printf("COMPILE\r\n");
#endif
    for(int i = 0; i < 128; i++)
    {
        if( t[i].type != Refal :: Var :: NONE )
        {
            table[i].type = t[i].type;
            table[i].qualifier = t[i].qualifier;
        }
    }

    HolesTuple holes;
    holes.push_back(Hole(-2, -1, begin, end));
    processHoles(holes);
#if REFAL_DEBUG_FLAG
    printf("END OF COMPILE\r\n\r\n");
#endif
}

Compiler::~Compiler()
{
}

Compiler::Hole::Hole()
  : left(0), right(0), begin(0), end(0)
{
}

Compiler::Hole::Hole(int l, int r, Refal::Base *b, Refal::Base *e)
  : left(l), right(r), begin(b), end(e)
{
}

Compiler::Var::Var()
  : have_value(false), mv_jsp(-1), nel_index(-1), qualifier(0)
  , type(Refal :: Var :: NONE)
{
}

bool Compiler::isVE(const Refal :: Base *elem, int &k) const
{
    if( isVar(elem, k) &&
        ( table[k].type == Refal :: Var :: E
          || table[k].type == Refal :: Var :: V ) )
        return true;
    return false;
}

bool Compiler::isVar(const Refal :: Base *elem, int &k) const
{
    if( !elem || elem -> type != Refal :: Base :: VARIABLE )
    {
        return false;
    }
    k = static_cast<const Refal :: Variable*>(elem) -> name;
    return true;
}

void Compiler::processHoles(HolesTuple &holes)
{
#if REFAL_DEBUG_FLAG
    printf("\t\tPH>>>\r\n");
    for(HolesTuple::const_iterator j = holes.begin(); j != holes.end(); ++j)
    {
        printf("\t%d %d %d %d\r\n", j -> left, j -> right,
                                      int(j -> begin), int(j -> end));
    }
    printf("\t\t<<<PH\r\n");
#endif
    i = holes.begin();
    processHoles1(holes);
}

void Compiler::processHoles1(HolesTuple &holes)
{
#if REFAL_DEBUG_FLAG
    printf("\t\tPH1>>>\r\n");
    for(HolesTuple::const_iterator j = holes.begin(); j != holes.end(); ++j)
    {
        printf("\t%d %d %d %d\r\n", j -> left, j -> right,
                                      int(j -> begin), int(j -> end));
    }
    printf("\t\t<<<PH1\r\n");
#endif
    if(i == holes.end())
    {
        processHoles4(holes);
    }
    else
    {
        int x;
        if( i -> begin != 0 && isVE(i -> begin, x) && table[x].mv_jsp != -1)
        {
            processHoles2(holes, x);
        }
        else
        {
            processHoles3(holes);
        }
    }
}

void Compiler::processHoles2(HolesTuple &holes, int var)
{
#if REFAL_DEBUG_FLAG
    printf("\t\tPH2>>>\r\n");
    for(HolesTuple::const_iterator j = holes.begin(); j != holes.end(); ++j)
    {
        printf("\t%d %d %d %d\r\n", j -> left, j -> right,
                                      int(j -> begin), int(j -> end));
    }
    printf("\t\t<<<PH2\r\n");
#endif
    HolesTuple holes1;
    holes1.splice(holes.begin(), holes, i, holes.end());

    int tmp = table[ var ].mv_jsp;
    table[ var ].mv_jsp = -1;
    processHoles4(holes);
    if(jsp > tmp)
    {
        eoe(jsp - tmp);
        jsp = tmp;
    }
    processHoles(holes1);
}

void Compiler::processHoles3(HolesTuple &holes)
{
#if REFAL_DEBUG_FLAG
    printf("\t\tPH3>>>\r\n");
    for(HolesTuple::const_iterator j = holes.begin(); j != holes.end(); ++j)
    {
        printf("\t%d %d %d %d\r\n", j -> left, j -> right,
                                      int(j -> begin), int(j -> end));
    }
    printf("\t\t<<<PH3\r\n");
#endif
    int x, y;
    if( i -> begin != i -> end
        && isVE(i -> begin, x) && !table[x].have_value
        && isVE(i -> begin, y) && !table[y].have_value )
    {
        ++i;
        processHoles1(holes);
    }
    else
    {
        elementMatch(holes);
    }
}

void Compiler::processHoles4(HolesTuple &holes)
{
#if REFAL_DEBUG_FLAG
    printf("\t\tPH4>>>\r\n");
    for(HolesTuple::const_iterator j = holes.begin(); j != holes.end(); ++j)
    {
        printf("\t%d %d %d %d\r\n", j -> left, j -> right,
                                      int(j -> begin), int(j -> end));
    }
    printf("\t\t<<<PH4\r\n");
#endif
    if( !holes.empty() )
    {
        holesLDEcompile(holes);
    }
}

void Compiler::eoe(int n)
{
    lang -> AddOp(Lang :: EOE, n);
#if REFAL_DEBUG_FLAG
    printf("EOE(%d);\r\n", n);
#endif
}

void Compiler::holesLDEcompile(HolesTuple &holes)
{
#if REFAL_DEBUG_FLAG
    printf("\t\tholesLDEcompile\r\n");
#endif
    if( b1 != holes.front().left || b2 != holes.front().right )
    {
        lang -> AddOp(Lang :: SB, holes.front().left, holes.front().right);
#if REFAL_DEBUG_FLAG
        printf("SB(%d, %d)\r\n", holes.front().left, holes.front().right);
#endif
    }
    glve(holes);
}

void Compiler::glve(HolesTuple &holes)
{
    int var = static_cast<Refal :: Variable*>(holes.front().begin) -> name;

    if( table[ var ].type == Refal :: Var :: E )
    {
        lang -> AddOp(Lang :: PLE);
#if REFAL_DEBUG_FLAG
        printf("PLE;\r\n");
#endif
    }
    else
    {
        lang -> AddOp(Lang :: PLV);
#if REFAL_DEBUG_FLAG
        printf("PLV;\r\n");
#endif
    }
    if( table[ var ].qualifier != 0 )
    {
        lang -> AddOp(Lang :: LESPC, table[ var ].qualifier);
#if REFAL_DEBUG_FLAG
        printf("LESPC();\r\n");
#endif
    }
    else
    {
        lang -> AddOp(Lang :: LE);
#if REFAL_DEBUG_FLAG
        printf("LE;\r\n");
#endif
    }
    ++jsp;
    table[ var ].have_value = true;
    table[ var ].nel_index = nel + 1;
    b1 = nel + 1;
    holes.front().begin = holes.front().begin -> next;
    holes.front().left = b1;
    nel += 2;
    processHoles(holes);
}

void Compiler::glmax(HolesTuple &holes)
{
    int var = static_cast<Refal :: Variable*>(holes.front().begin) -> name;

    lang -> AddOp(Lang :: LMAX);
#if REFAL_DEBUG_FLAG
    printf("LMAX()\r\n");
#endif
    if( table[ var ].type == Refal :: Var :: V )
    {
        lang -> AddOp(Lang :: NNIL);
#if REFAL_DEBUG_FLAG
        printf("NNIL;\r\n");
#endif
    }
    table[ var ].have_value = true;
    table[ var ].nel_index = nel + 1;
    b1 = nel + 1;
    holes.front().begin = holes.front().begin -> next;
    holes.front().left = b1;
    nel += 2;
    processHoles(holes);
}

void Compiler::grmax(HolesTuple &holes)
{
    int var = static_cast<Refal :: Variable*>(holes.front().end) -> name;

    lang -> AddOp(Lang :: RMAX);
#if REFAL_DEBUG_FLAG
    printf("RMAX()\r\n");
#endif
    if( table[ var ].type == Refal :: Var :: V )
    {
        lang -> AddOp(Lang :: NNIL);
#if REFAL_DEBUG_FLAG
        printf("NNIL;\r\n");
#endif
    }
    table[ var ].have_value = true;
    table[ var ].nel_index = nel + 1;
    b2 = nel;
    holes.front().end = holes.front().end -> prev;
    holes.front().right = b2;
    nel += 2;
    processHoles(holes);
}

void Compiler::elementMatch(HolesTuple &holes)
{
#if REFAL_DEBUG_FLAG
    printf("\t\telementMatch\r\n");
#endif
    int x;
    if( b1 != i -> left || b2 != i -> right )
    {
        lang -> AddOp(Lang :: SB, i -> left, i -> right);
#if REFAL_DEBUG_FLAG
        printf("SB(%d, %d)\r\n", i -> left, i -> right);
#endif
    }
    if( i -> begin == 0 || i -> end == 0)
    {
        gnil(holes);
        return;
    }
    if( i -> begin == i -> end && isVE(i -> begin, x)
             && !table[x].have_value )
    {
        gcve(holes, x);
        return;
    }
    if( i -> begin -> type == Refal :: Base :: LEFT_PAREN )
    {
        glb(holes);
        return;
    }
    if( i -> begin -> type < Refal :: Base :: VARIABLE )
    {
        glsc(holes);
        return;
    }
    if( isVar(i -> begin, x) ) 
    {
        switch(table[x].type)
        {
        case Refal :: Var :: S :
            gls(holes, x);
            return;
            break;
        case Refal :: Var :: W :
            glw(holes, x);
            return;
            break;
        default:
            if( table[x].have_value )
            {
                glved(holes, x);
                return;
            }
            break;
        }
    }
    if( i -> end -> type == Refal :: Base :: RIGHT_PAREN )
    {
        grb(holes);
        return;
    }
    if( i -> end -> type < Refal :: Base :: VARIABLE )
    {
        grsc(holes);
        return;
    }
    if( isVar(i -> end, x) ) 
    {
        switch(table[x].type)
        {
        case Refal :: Var :: S :
            grs(holes, x);
            return;
            break;
        case Refal :: Var :: W :
            grw(holes, x);
            return;
            break;
        default:
            if( table[x].have_value )
            {
                grved(holes, x);
                return;
            }
            break;
        }
    }
}

void Compiler::gnil(HolesTuple &holes)
{
    lang -> AddOp(Lang :: NIL);
#if REFAL_DEBUG_FLAG
    printf("NIL;\r\n");
#endif
    i = holes.erase(i);
    processHoles(holes);
}

void Compiler::gcve(HolesTuple &holes, int k)
{
    lang -> AddOp(Lang :: CE);
#if REFAL_DEBUG_FLAG
    printf("CE;\r\n");
#endif
    if( table[k].qualifier != 0)
    {
        glmax(holes);
        return;
    }
    if( table[k].type == Refal :: Var :: V )
    {
        lang -> AddOp(Lang :: NNIL);
#if REFAL_DEBUG_FLAG
        printf("NNIL;\r\n");
#endif
    }
    i = holes.erase(i);
    table[k].have_value = true;
    table[k].nel_index = nel + 1;
    nel += 2;
    processHoles(holes);
}

void Compiler::glb(HolesTuple &holes)
{
    lang -> AddOp(Lang :: LB);
#if REFAL_DEBUG_FLAG
    printf("LB;\r\n");
#endif
    b1 = nel;
    b2 = nel + 1;
    nel += 2;
    Refal :: LeftParen *lp = static_cast<Refal :: LeftParen*>(i -> begin);
    holes.insert(i, Hole(b1, b2, lp -> next, lp -> right_paren -> prev));
    i -> begin = lp -> right_paren -> next;
    i -> left = b2;
    processHoles(holes);
}

void Compiler::glsc(HolesTuple &holes)
{
    lang -> AddOp(Lang :: LSC, i -> begin);
#if REFAL_DEBUG_FLAG
    printf("LSC(%d)", i -> begin -> type);
#endif
    i -> begin = i -> begin -> next;
    b1 = nel;
    i -> left = b1;
    b2 = i -> right;
    ++nel;
    processHoles(holes);
}

void Compiler::gls(HolesTuple &holes, int var)
{
    if( !table[ var ].have_value )
    {
        lang -> AddOp(Lang :: LS);
#if REFAL_DEBUG_FLAG
        printf("LS-%c-;\r\n", char(var));
#endif
        table[ var ].have_value = true;
        table[ var ].nel_index = nel;
    }
    else
    {
        lang -> AddOp(Lang :: LSD, table[ var ].nel_index);
#if REFAL_DEBUG_FLAG
        printf("LSD(%d);\r\n", table[ var ].nel_index);
#endif
    }
    if( table[ var ].qualifier != 0)
    {
        lang -> AddOp(Lang :: WSPC, table[ var ].qualifier);
#if REFAL_DEBUG_FLAG
        printf("WSPC();\r\n");
#endif
    }
    i -> begin = i -> begin -> next;
    b1 = nel;
    i -> left = b1;
    b2 = i -> right;
    ++nel;
    processHoles(holes);
}

void Compiler::glw(HolesTuple &holes, int var)
{
    if( !table[ var ].have_value )
    {
        lang -> AddOp(Lang :: LW);
#if REFAL_DEBUG_FLAG
        printf("LW-%c-;\r\n", char(var));
#endif
        table[ var ].have_value = true;
        table[ var ].nel_index = nel + 1;
    }
    else
    {
        lang -> AddOp(Lang :: LED, table[ var ].nel_index);
#if REFAL_DEBUG_FLAG
        printf("LED(%d);\r\n", table[ var ].nel_index);
#endif
    }
    if( table[ var ].qualifier != 0)
    {
        lang -> AddOp(Lang :: WSPC, table[ var ].qualifier);
#if REFAL_DEBUG_FLAG
        printf("WSPC;\r\n");
#endif
    }
    i -> begin = i -> begin -> next;
    b1 = nel + 1;
    i -> left = b1;
    b2 = i -> right;
    nel += 2;
    processHoles(holes);
}

void Compiler::glved(HolesTuple &holes, int var)
{
    lang -> AddOp(Lang :: LED, table[ var ].nel_index);
#if REFAL_DEBUG_FLAG
    printf("LED(%d);\r\n", table[ var ].nel_index);
#endif
    if( table[ var ].qualifier != 0)
    {
        lang -> AddOp(Lang :: ESPC, table[ var ].qualifier);
#if REFAL_DEBUG_FLAG
        printf("ESPC;\r\n");
#endif
    }
    i -> begin = i -> begin -> next;
    b1 = nel + 1;
    i -> left = b1;
    b2 = i -> right;
    nel += 2;
    processHoles(holes);
}

void Compiler::grb(HolesTuple &holes)
{
    lang -> AddOp(Lang :: RB);
#if REFAL_DEBUG_FLAG
    printf("RB;\r\n");
#endif
    Refal :: RightParen *rp = static_cast<Refal :: RightParen*>(i -> end);
    holes.insert(++i, Hole(nel, nel + 1, rp -> left_paren -> next, rp -> prev));
    --i;
    i -> end = rp -> left_paren -> prev;
    i -> right = nel;
    b1 = i -> left;
    b2 = nel;
    nel += 2;
    processHoles(holes);
}

void Compiler::grsc(HolesTuple &holes)
{
    lang -> AddOp(Lang :: RSC, i -> end);
#if REFAL_DEBUG_FLAG
    printf("RSC(%d)", i -> end -> type);
#endif
    i -> end = i -> end -> prev;
    b1 = i -> left;
    b2 = nel;
    i -> right = b2;
    ++nel;
    processHoles(holes);
}

void Compiler::grs(HolesTuple &holes, int var)
{
    if( !table[ var ].have_value )
    {
        lang -> AddOp(Lang :: RS);
#if REFAL_DEBUG_FLAG
        printf("RS-%c-;\r\n", char(var));
#endif
        table[ var ].have_value = true;
        table[ var ].nel_index = nel;
    }
    else
    {
        lang -> AddOp(Lang :: RSD, table[ var ].nel_index);
#if REFAL_DEBUG_FLAG
        printf("RSD(%d);\r\n", table[ var ].nel_index);
#endif
    }
    if( table[ var ].qualifier != 0)
    {
        lang -> AddOp(Lang :: WSPC, table[ var ].qualifier);
#if REFAL_DEBUG_FLAG
        printf("WSPC;\r\n");
#endif
    }
    i -> end = i -> end -> prev;
    b1 = i -> left;
    b2 = nel;
    i -> right = b2;
    ++nel;
    processHoles(holes);
}

void Compiler::grw(HolesTuple &holes, int var)
{
    if( !table[ var ].have_value )
    {
        lang -> AddOp(Lang :: RW);
#if REFAL_DEBUG_FLAG
        printf("RW-%c-;\r\n", char(var));
#endif
        table[ var ].have_value = true;
        table[ var ].nel_index = nel + 1;
    }
    else
    {
        lang -> AddOp(Lang :: RED, table[ var ].nel_index);
#if REFAL_DEBUG_FLAG
        printf("RED(%d);\r\n", table[ var ].nel_index);
#endif
    }
    if( table[ var ].qualifier != 0)
    {
        lang -> AddOp(Lang :: WSPC, table[ var ].qualifier);
#if REFAL_DEBUG_FLAG
        printf("WSPC;\r\n");
#endif
    }
    i -> end = i -> end -> prev;
    b1 = i -> left;
    b2 = nel;
    i -> right = b2;
    nel += 2;
    processHoles(holes);
}

void Compiler::grved(HolesTuple &holes, int var)
{
    lang -> AddOp(Lang :: RED, table[ var ].nel_index);
#if REFAL_DEBUG_FLAG
    printf("RED(%d);\r\n", table[ var ].nel_index);
#endif
    if( table[ var ].qualifier != 0)
    {
        lang -> AddOp(Lang :: ESPC, table[ var ].qualifier);
#if REFAL_DEBUG_FLAG
        printf("ESPC;\r\n");
#endif
    }
    i -> end = i -> end -> prev;
    b1 = i -> left;
    b2 = nel;
    i -> right = b2;
    nel += 2;
    processHoles(holes);
}

