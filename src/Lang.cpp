#include "Lang.h"
#include "Refal.h"

#include <stdio.h>


bool Lang::Run(Refal :: Base *b1, Refal :: Base *b2)
{
    table.clear();

    Refal :: LeftParen *bb = new Refal :: LeftParen();
    Refal :: RightParen *be = new Refal :: RightParen();
    bb -> right_paren = be;
    be -> left_paren = bb;
    bb -> next = b1;
    b1 -> prev = bb;
    be -> prev = b2;
    b2 -> next = be;
    return go(bb, be);    
}

Lang::State::State()
  : nel(0), op(0), b1(0), b2(0)
{
}

Lang::State::State(int n, BasicOp *o, Refal :: Base *a, Refal :: Base *b)
  : nel(n), op(o), b1(a), b2(b)
{
}
  

bool Lang::go(Refal :: Base *b1, Refal :: Base *b2)
{
#define SAVESTATE do\
                  {\
                      stack[stack_pointer] = State(nel, op -> next, b1, b2);\
                  }\
                  while(0)
#define FAIL do\
             {\
                 if( stack_pointer == 0 )\
                 {\
                     return false;\
                 }\
                 --stack_pointer;\
                 nel = stack[stack_pointer].nel;\
                 op  = stack[stack_pointer].op;\
                 b1  = stack[stack_pointer].b1;\
                 b2  = stack[stack_pointer].b2;\
                 goto BEGIN;\
             }\
             while(0)
#define SB1 do\
            {\
                b1 = b1 -> next;\
                if(b1 == b2)\
                {\
                    FAIL;\
                }\
            }\
            while(0)
#define SB2 do\
            {\
                b2 = b2 -> prev;\
                if(b1 == b2)\
                {\
                    FAIL;\
                }\
            }\
            while(0)

    int nel = 0, stack_pointer = 0;
    for(BasicOp *op = begin; op != 0; op = op -> next)
    {
    BEGIN:
        switch(op -> type)
        {
        case NIL :
            if( b1 -> next != b2 )
            {
                FAIL;
            }
            break;
        case LSC :
            SB1;
            if( b1 == b2 || !Refal :: equS(b1, static_cast<OpS*>(op) -> elem) )
            {
                FAIL;
            }
            else
            {
                table[nel] = b1;
                ++nel;
            }
            break;
       case RSC :
            SB2;
            if( b1 == b2 || !Refal :: equS(b2, static_cast<OpS*>(op) -> elem) )
            {
                FAIL;
            }
            else
            {
                table[nel] = b2;
                ++nel;
            }
            break;
        case LB :
            SB1;
            if( b1 -> type != Refal :: Base :: LEFT_PAREN )
            {
                FAIL;
            }
            b2 = static_cast<Refal :: LeftParen*>(b1) -> right_paren;
            table[nel] = b1;
            table[nel + 1] = b2;
            nel += 2;
            break;
        case RB :
            SB2;
            if( b2 -> type != Refal :: Base :: RIGHT_PAREN )
            {
                FAIL;
            }
            b1 = static_cast<Refal :: RightParen*>(b2) -> left_paren;
            table[nel] = b1;
            table[nel + 1] = b2;
            nel += 2;
            break;
        case SB :
            b1 = table[ static_cast<OpNM*>(op) -> n ];
            b2 = table[ static_cast<OpNM*>(op) -> m ];
            break;
        case LS :
            SB1;
            if( b1 -> type >= Refal :: Base :: VARIABLE )
            {
                FAIL;
            }
            table[nel] = b1;
            ++nel;
            break;
        case RS :
            SB2;
            if( b2 -> type >= Refal :: Base :: VARIABLE )
            {
                FAIL;
            }
            table[nel] = b2;
            ++nel;
            break;
        case LSD :
            SB1;
            if( !Refal :: equS(b1, table[ static_cast<OpN*>(op) -> n ]) )
            {
                FAIL;
            }
            table[nel] = b1;
            ++nel;
            break;
        case RSD :
            SB2;
            if( !Refal :: equS(b2, table[ static_cast<OpN*>(op) -> n ]) )
            {
                FAIL;
            }
            table[nel] = b2;
            ++nel;
            break;
        case CE :
            table[nel] = b1 -> next;
            table[nel + 1] = b2 -> prev;
            nel += 2;
            break;
        case RED :
            table[nel] = b1 -> next;
            for(Refal :: Base *bf = table[ static_cast<OpN*>(op) -> n ],
                  *b0 = table[ static_cast<OpN*>(op) -> n - 1 ] -> prev;
                    b0 != bf; )
            {
                b0 = b0 -> next;
                SB1;
                if( !Refal :: equS(b0, b1) &&
                    ( b1 -> type < Refal :: Base :: LEFT_PAREN
                      || b0 -> type != b1 -> type ) )
                {
                    FAIL;
                }
            }
            table[nel + 1] = b1;
            nel += 2;
            break;
        case LED :
            table[nel + 1] = b2 -> prev;
            for(Refal :: Base *bf = table[ static_cast<OpN*>(op) -> n - 1 ],
                  *b0 = table[ static_cast<OpN*>(op) -> n ] -> next;
                    b0 != bf; )
            {
                b0 = b0 -> prev;
                SB2;
                if( !Refal :: equS(b0, b2) &&
                    ( b2 -> type < Refal :: Base :: LEFT_PAREN
                      || b0 -> type != b2 -> type ) )
                {
                    FAIL;
                }
            }
            table[nel] = b2;
            nel += 2;
            break;
        case LW :
            SB1;
            table[nel] = b1;
            if( b1 -> type == Refal :: Base :: LEFT_PAREN )
            {
                b1 = static_cast<Refal :: LeftParen*>(b1) -> right_paren;
            }
            table[nel + 1] = b1;
            nel += 2;
            break;
        case RW :
            SB2;
            table[nel + 1] = b2;
            if( b2 -> type == Refal :: Base :: RIGHT_PAREN )
            {
                b2 = static_cast<Refal :: RightParen*>(b1) -> left_paren;
            }
            table[nel] = b2;
            nel += 2;
            break;
        case NNIL :
            if( table[ nel - 1 ] -> next == table[ nel - 2 ] )
            {
                FAIL;
            }
            break;
        case PLE :
            SAVESTATE;
            ++stack_pointer;
            table[ nel ] = b1 -> next;
            table[ nel + 1 ] = b1;
            nel += 2;
            op = op -> next;
            break;
        case PLV :
            SAVESTATE;
            table[ nel ] = b1 -> next;
            table[ nel + 1 ] = b1;
            break;
        case LE :
        case LESPC :
            b1 = table[ nel + 1 ];
            SB1;
            if( b1 -> type == Refal :: Base :: LEFT_PAREN )
            {
                b1 = static_cast<Refal :: LeftParen*>(b1) -> right_paren;
            }
            if( op -> type == LESPC && !Refal :: check_qualifier(
                   static_cast<OpQ*>(op) -> qualifier, b1 ) )
            {
                FAIL;
            }
            ++stack_pointer;
            table[ nel + 1 ] = b1;
            nel += 2;
            break;
        case PRE :
            SAVESTATE;
            ++stack_pointer;
            table[ nel ] = b2;
            table[ nel + 1 ] = b2 -> prev;
            nel += 2;
            op = op -> next;
            break;
        case PRV :
            SAVESTATE;
            table[ nel ] = b2;
            table[ nel + 1 ] = b2 -> prev;
            break;
        case RE :
        case RESPC :
            b2 = table[ nel ];
            SB2;
            if( b2 -> type == Refal :: Base :: RIGHT_PAREN )
            {
                b2 = static_cast<Refal :: RightParen*>(b2) -> left_paren;
            }
            if( op -> type == RESPC && !Refal :: check_qualifier(
                   static_cast<OpQ*>(op) -> qualifier, b2 ) )
            {
                FAIL;
            }
            ++stack_pointer;
            table[ nel ] = b2;
            nel += 2;
            break;
        case EOE :
            stack_pointer -= static_cast<OpN*>(op) -> n;
        case WSPC :
            if( !Refal :: check_qualifier(
                   static_cast<OpQ*>(op) -> qualifier, table[ nel - 1 ] ) )
            {
                FAIL;
            }
            break;
        case ESPC :
            for(Refal :: Base *b0 = table[ nel - 2 ] -> prev;
                    b0 != table[ nel - 1 ]; )
            {
                b0 = b0 -> next;
                if( b0 -> type == Refal :: Base :: LEFT_PAREN )
                {
                    b0 = static_cast<Refal :: LeftParen*>(b0) -> right_paren;
                }
                if( !Refal :: check_qualifier(
                       static_cast<OpQ*>(op) -> qualifier, b0 ) )
                {
                    FAIL;
                }
            }
            break;
        case LMAX :
            b1 = b1 -> next;
            table[ nel ] = b1;
            for(; b1 != b2; b1 = b1 -> next)
            {
                if( !Refal :: check_qualifier(
                       static_cast<OpQ*>(op) -> qualifier, b1 ) )
                {
                    break;
                }
                if( b1 -> type == Refal :: Base :: LEFT_PAREN )
                {
                    b1 = static_cast<Refal :: LeftParen*>(b1) -> right_paren;
                }
            }
            b1 = b1 -> prev;
            table[ nel + 1] = b1;
            nel += 2;
            break;
        case RMAX :
            b2 = b2 -> prev;
            table[ nel  + 1 ] = b2;
            for(; b1 != b2; b2 = b2 -> prev)
            {
                if( !Refal :: check_qualifier(
                       static_cast<OpQ*>(op) -> qualifier, b2 ) )
                {
                    break;
                }
                if( b2 -> type == Refal :: Base :: RIGHT_PAREN )
                {
                    b2 = static_cast<Refal :: RightParen*>(b2) -> left_paren;
                }
            }
            b2 = b2 -> next;
            table[ nel ] = b2;
            nel += 2;
            break;
        default:
            break;
        }
    }
    print_table(nel);
    return true;
#undef SAVESTATE
#undef FAIL
#undef SB1
#undef SB2
}

void Lang::print_table(int nel)
{
    for(int i = 0; i < nel; i++)
    {
        Refal :: print_e(table[i], table[i] -> next);
        printf("\r\n");
    }
}

void Lang::addop(BasicOp *op)
{
    if(begin == 0)
    {
        begin = op;
        end = op;
    }
    else
    {
        end -> next = op;
        end = end -> next;
    }
}

void Lang::AddOp(Type type)
{
    addop(new BasicOp(type));
}

void Lang::AddOp(Type type, int x)
{
    addop(new OpN(type, x));
}

void Lang::AddOp(Type type, int x, int y)
{
    addop(new OpNM(type, x, y));
}

void Lang::AddOp(Type type, Refal :: Base *elem)
{
    addop(new OpS(type, elem));
}

void Lang::AddOp(Type type, Refal :: Qualifier *qualifier)
{
    addop(new OpQ(type, qualifier));
}

Lang::BasicOp::BasicOp(Type t)
  : type(t), next(0)
{
}

Lang::OpN::OpN(Type t, int x)
  : BasicOp(t), n(x)
{
}

Lang::OpNM::OpNM(Type t, int x, int y)
  : BasicOp(t), n(x), m(y)
{
}

Lang::OpS::OpS(Type t, Refal :: Base *e)
  : BasicOp(t), elem(e)
{
}

Lang::OpQ::OpQ(Type t, Refal :: Qualifier *q)
  : BasicOp(t), qualifier(q)
{
}

Lang::Lang()
  : begin(0), end(0), table(4096), stack(16)
{
}

Lang::~Lang()
{
}

