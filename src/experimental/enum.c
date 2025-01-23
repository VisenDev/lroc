/*#define stringify_tokens(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) \
  #a,#b,#c,#d,#e,#f,#g,#h,#i,#j,#k,#l,#m,#n,#o,#p,#q,#r,#s,#t,#u,#v,#w,#x,#y,#z*/


#define STR_VAARGS_11(arg, ...) #arg
#define STR_VAARGS_10(arg, ...) #arg, STR_VAARGS_11(__VA_ARGS__)
#define STR_VAARGS_9(arg, ...)  #arg, STR_VAARGS_10(__VA_ARGS__)
#define STR_VAARGS_8(arg, ...)  #arg, STR_VAARGS_9(__VA_ARGS__)
#define STR_VAARGS_7(arg, ...)  #arg, STR_VAARGS_8(__VA_ARGS__)
#define STR_VAARGS_6(arg, ...)  #arg, STR_VAARGS_7(__VA_ARGS__)
#define STR_VAARGS_5(arg, ...)  #arg, STR_VAARGS_6(__VA_ARGS__)
#define STR_VAARGS_4(arg, ...)  #arg, STR_VAARGS_5(__VA_ARGS__)
#define STR_VAARGS_3(arg, ...)  #arg, STR_VAARGS_4(__VA_ARGS__)
#define STR_VAARGS_2(arg, ...)  #arg, STR_VAARGS_3(__VA_ARGS__)
#define STR_VAARGS_1(arg, ...)  #arg, STR_VAARGS_2(__VA_ARGS__)
#define STR_VAARGS(arg, ...)    #arg, STR_VAARGS_1(__VA_ARGS__)

#define strenum(name, ...) \
    typedef enum {__VA_ARGS__, name##_count} name; \
    const char * name##_name[] = {STR_VAARGS(__VA_ARGS__)}

strenum(State, MIN, MAX, START, END, TEST, HELLO, WORLD, SOMETHING, ELSE, HAHA);

#include <stdio.h>
 
int main() {
    State s = MIN;
    unsigned int i = 0;

    for(i = 0; i < State_count; ++i) {
        printf("State: %s\n", State_name[i]);
    }
    return 0;
}

