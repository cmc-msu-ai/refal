#include "Refal.h"

#include <stdio.h>




int main(int argc, const char *argv[])
{
    const char *pattern = "s(/plus//minus/)1w2 e:vaSya:3v4s((:petya:))5s1v4";
    const char *expr1 = "/plus/('go' 'm' /44/ 'prit') 'hE95Dt*fj' "
                        "'5t774' 'q' /plus/ '*fj5t' '774'";
    const char *expr2 = "/minus/('go' 'm' /44/ 'prit') 'hE95Dt*fj' "
                        "'5t774' 'q' /plus/ '*fj5t' '774'";
    const char *expr3 = "/minus/('go' 'm' /44/ 'prit') 'hE95Dt*fj' "
                        "'5t774' 'q' /minus/ '*fj5t' '774'";
    try
    {
        Refal refal;

        if( !refal.AddQualifier("Vasya", "LD") )
        {
            throw 1;
        }
        if( !refal.AddQualifier("Petya", "'abbfde'") )
        {
            throw 1;
        }
        
        if( !refal.SetPattern(pattern) )
        {
            throw 1;
        }
        
        int result = 0;
        if( !refal.Equ(expr1, &result) )
        {
                throw 1;
        }
        printf("%d\r\n", result);
        if( !refal.Equ(expr2, &result) )
        {
                throw 1;
        }
        printf("%d\r\n", result);
        if( !refal.Equ(expr3, &result) )
        {
                throw 1;
        }
        printf("%d\r\n", result);
    }
    catch(...)
    {
        return 1;
    }
    return 0;
}
