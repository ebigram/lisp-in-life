#ifdef ELVM
#include "elvm.h"
#else
#include "lisp.h"
#endif

#define debug(x)

//================================================================================
// Parser
//================================================================================
typedef enum {
    ATOM, LIST, INT, LAMBDA
} Valuetype;

typedef struct List {
    struct Value* value;
    struct List* next;
} List;

typedef struct Value {
    Valuetype type;
    union {
        char* str;
        struct List* list;
        int n;
        struct Lambda* lambda;
    };
} Value;


char* _str;
Value* _value;
#define str_in _str
Value* newAtomNode() {
#define ret _value
    ret = (Value*)malloc(sizeof(Value));
    debug("malloc at newAtomNode\n");
    ret->type = ATOM;
    ret->str = str_in;
    return ret;
#undef ret
}
#undef str_in

List* _list;
#define list_in _list
Value* newListNode() {
#define ret _value
    ret = (Value*)malloc(sizeof(Value));
    debug("malloc at newListNode\n");
    ret->type = LIST;
    ret->list = list_in;
    return ret;
#undef ret
}
#undef list_in

List* newList(Value* node, List* next) {
#define ret _list
    ret = (List*)malloc(sizeof(List));
    debug("malloc at newList\n");
    ret->value = node;
    ret->next = next;
    return ret;
#undef ret
}

void parseAtom();
void parseList();
void parseExpr();

char c;
char buf[32];
int i;
int j;
void parseAtom() {
    c = curchar();
    if (c == ')' || c == '(' || !c || c == EOF) {
        // return NULL;
        _value = NULL;
        return;
    }

    // char buf[32];
    i = 0;
    while (c != ' ' && c != '\n' && c != ')' && c != '(' && c != ';' && c != EOF) {
        buf[i] = c;
        i++;
        popchar();
        c = curchar();
    }
    // buf[i] = '\0';

    _str = malloc(i+1);
    debug("malloc at parseAtom\n");
    for (j=0; j<i; j++) {
        _str[j] = buf[j];
    }

    _value = newAtomNode();
    // return newAtomNode();
}

List* parseListLoop() {
    parseExpr();
    Value* parsednode = _value;
    return parsednode ? newList(parsednode, parseListLoop()) : NULL;
}

void parseList() {
    if (curchar() != '('){
        _value = NULL;
        return;
    }
    popchar(); // '('

    _list = parseListLoop();

    popchar(); // ')'
    _value = newListNode();
    // return newListNode();
}

void parseExpr() {
space:;
    c = curchar();
    if(c == ' ' || c == '\n') {
        popchar();
        goto space;
    }
    if (c == ';') {
        do {
            c = popchar();
        } while(c != '\n' && c != EOF);
        goto space;
    }
    // _value = parseAtom();
    parseAtom();
    if (!_value) {
        parseList();
    }
    // return _value;
    // return _value ? _value : parseList();
}

//================================================================================
// Evaluator
//================================================================================
typedef struct Env {
    char* varname;
    struct Value* value;
    struct Env* next;
} Env;

typedef enum {
    L_LAMBDA, L_MACRO
} Lambdatype;

typedef struct Lambda {
    struct List* argnames;
    struct Value* body;
    struct Env* env;
    Lambdatype type;
} Lambda;


// Value* newIntValue(int n){
void newIntValue(){
#define ret _value
    ret = malloc(sizeof(Value));
    debug("malloc at newIntValue\n");
    ret->type = INT;
    ret->n = i;
    // return ret;
#undef ret
}

Lambda* _lambda;
// Value* newLambdaValue(){
//     _value = malloc(sizeof(Value));
//     debug("malloc at newLambdaValue\n");
//     _value->type = LAMBDA;
//     _value->lambda = _lambda;
//     return _value;
// }

// Value* getVariableValue(char* varname, Env* env) {
//     do {
//         if (eqstr(varname, env->varname)) {
//             return env->value;
//         }
//     } while ((env = env->next));
//     return NULL;
// }

Env* _env;
Env* _env2;
#define varname_in _str
#define value_in _value
#define env_in _env
#define env _env2
Env* newEnv() {
    env = malloc(sizeof(Env));
    debug("malloc at newEnv\n");
    env->varname = varname_in;
    env->value = value_in;
    env->next = env_in;
    return env;
}
#undef varname_in
#undef value_in
#undef env_in
#undef env

// Value* eqAtom(Value* n1, Value* n2) {
//     // Integer equality
//     if (n1->type == INT && n2->type == INT && n1->n == n2->n) {
//         return n1;
//     }
//     // Atom equality
//     if (n1->type == ATOM && n2->type == ATOM &&
//         eqstr(n1->str, n2->str)) {
//         return n1;
//     }
//     return NULL;
// }

#define v _value
void printValue() {
    if (!v) {
        printStr("()");
    } else {
        i = v->type;
        if (i == INT) {
            printInt(v->n);
        } else if (i == LAMBDA) {
            printStr(v->lambda->type == L_LAMBDA ? "#<Closure>" : "#<Macro>");
        } else if (i == ATOM) {
            printStr(v->str);
        } else if (i == LIST){
            putchar('(');
            List* list = v->list;
            while(list) {
                _value = list->value;
                printValue();
                list = list->next;
                if (list) {
                    putchar(' ');
                }
            }
            putchar(')');
        }
    }
}
#undef v


#define str _str
#define sign j
int k;
int parseInt() {
    sign = 1;
    i = 0;
    if (str[0] == '-') {
        sign = 0;
        str++;
    }
    while (*str) {
        // i *= 10
        i += i;
        k = i;
        i += i;
        i += i + k + (*str - '0');
        // i = 10*i + (*str - '0');
        str++;
    }
    return sign ? i : -i;
}
#undef str
#undef sign

void eval(Value* node, Env* env);
void evalAsInt(Value* node, Env* env) {
    c = node->str[0];
    if (c == '-' || ('0' <= c && c <= '9')) {
        _str = node->str;
        i = parseInt();
    } else {
        eval(node, env);
        i = _value->n;
    }
    // return _value->n;
}

void eval(Value* node, Env* env) {
    List* _list_eval;
    // Is an atom
    if (node->type == ATOM) {
        _str = node->str;
        c = node->str[0];
        if (('0' <= c && c <= '9') || c == '-') {
            i = parseInt();
            #define ret _value
            newIntValue();
            // return ret;
            return;
            #undef ret
        }
        // Get variable value from the environment
        do {
            if (eqstr(_str, env->varname)) {
                // return env->value;
                _value = env->value;
                return;
            }
        } while ((env = env->next));
        // return NULL;
        _value = NULL;
        return;
    }

    // Is an int or a lambda
    if (node->type == INT || node->type == LAMBDA) {
        // return node;
        _value = node;
        return;
    }

    // Is a list

    // Is ()
    if (!(node->list)) {
        // return NULL;
        _value = NULL;
        return;
    }

    // The head of the list is an atom
    if (node->list->value->type == ATOM) {
        char* headstr = node->list->value->str;
        if (!(node->list->next)) {
            goto eval_lambda;
        }
        Value* arg1 = node->list->next->value;
        List* arg2list = node->list->next->next;

        if (eqstr(headstr, "define")) {
            #define ret _value
            // ret = eval(arg2list->value, env);
            eval(arg2list->value, env);
            do {
                if (eqstr(env->varname, arg1->str)){
                    env->value = ret;
                    // _value = ret;
                    return;
                    // return ret;
                }
            } while(env->next && (env = env->next));
            // Append to the global environment
            _str = arg1->str;
            _value = ret;
            _env = NULL;
            env->next = newEnv();
            // return ret;
            return;
            #undef ret
        }
        if (eqstr(headstr, "if")) {
            #define condition _value
            // condition = eval(arg1, env);
            eval(arg1, env);
            eval(condition ? arg2list->value : arg2list->next->value, env);
            return;
            // return (condition ?
            // eval(arg2list->value, env) :
            // eval(arg2list->next->value, env));
            #undef condition
        }
        if (eqstr(headstr, "quote")) {
            _value = arg1;
            return;
            // return arg1;
        }
        if (eqstr(headstr, "car") || eqstr(headstr, "cdr")) {
            // _value = eval(arg1, env);
            eval(arg1, env);
            // if (!_value) {
            //     return NULL;
            // }
            if (_value) {
                if (headstr[1] == 'a') {
                    // return _value->list->value;
                    _value = _value->list->value;
                } else {
                    _list = _value->list->next;
                    // return !_list ? NULL : newListNode();
                    _value = !_list ? NULL : newListNode();
                }
            }
            return;
        }
        if (eqstr(headstr, "cons")) {
            #define cdr node
            // cdr = eval(arg2list->value, env);
            eval(arg2list->value, env);
            cdr = _value;
            eval(arg1, env);
            _list = newList(_value, cdr ? cdr->list : NULL);
            // return newListNode();
            _value = newListNode();
            return;
            #undef cdr
        }
        if (eqstr(headstr, "atom")) {
            // _value = eval(arg1, env);
            eval(arg1, env);
            _str = "t";
            // return (
            //     !_value ? newAtomNode() :
            //     (_value->type == ATOM) || (_value->type == INT) ? _value : NULL
            // );
            _value = (
                !_value ? newAtomNode() :
                (_value->type == ATOM) || (_value->type == INT) ? _value : NULL
            );
            return;
        }
        if (eqstr(headstr, "print")) {
            // _value = eval(arg1, env);
            eval(arg1, env);
            node = _value;
            printValue();
            if(arg2list) {
                putchar('\n');
            }
            _value = node;
            // return _value;
            return;
        }
        if (eqstr(headstr, "progn")) {
            #define curlist _list_eval
            curlist = node->list->next;
            _value = NULL;
            while (curlist) {
                // _value = eval(curlist->value, env);
                eval(curlist->value, env);
                curlist = curlist->next;
            }
            // return _value;
            return;
            #undef curlist
        }
        if (eqstr(headstr, "while")) {
            _value = NULL;
            while (eval(arg1, env), _value) {
                // _value = eval(arg2list->value, env);
                eval(arg2list->value, env);
            }
            // return _value;
            return;
        }
        if (eqstr(headstr, "lambda") || eqstr(headstr, "macro")) {
            _lambda = malloc(sizeof(Lambda));
            debug("malloc at lambda\n");
            _lambda->argnames = arg1->list;
            _lambda->body = arg2list->value;
            _lambda->env = env;
            _lambda->type = headstr[0] == 'l' ? L_LAMBDA : L_MACRO;

            _value = malloc(sizeof(Value));
            debug("malloc at newLambdaValue\n");
            _value->type = LAMBDA;
            _value->lambda = _lambda;
            // return _value;
            return;
            // return newLambdaValue();
        }
        if (eqstr(headstr, "eval")) {
            eval(arg1, env);
            eval(_value, env);
            return;
            // return eval(_value, env);
        }
        if (eqstr(headstr, "eq")) {
            // node = eval(arg1, env);
            eval(arg1, env);
            node = _value;
            eval(arg2list->value, env);
            #define n1 node
            #define n2 _value
            // Integer equality
            if (n1->type == INT && n2->type == INT && n1->n == n2->n) {
                // return n1;
                _value = n1;
            }
            // Atom equality
            else if (n1->type == ATOM && n2->type == ATOM &&
                eqstr(n1->str, n2->str)) {
                // return n1;
                _value = n1;
            } else {
                _value = NULL;
            }
            // return NULL;
            return;
            #undef n1
            #undef n2
            // return eqAtom(eval(arg1, env), eval(arg2list->value, env));
        }
        if (eqstr(headstr, "+") || eqstr(headstr, "-") || eqstr(headstr, "*")
            || eqstr(headstr, "/") || eqstr(headstr, "mod")) {
        // c = headstr[0];
        // if (((c == '+' || c == '-' || c == '*' || c == '/') && headstr[1] == '\0') || eqstr(headstr, "mod")) {
            #define nextlist _list_eval
            nextlist = node->list->next;
            evalAsInt(nextlist->value, env);
            int ret = i;
            for (nextlist = nextlist->next; nextlist; nextlist = nextlist->next) {
                evalAsInt(nextlist->value, env);
                // int nextint = i;
                ret = (
                    headstr[0] == '+' ? (ret + i) :
                    headstr[0] == '-' ? (ret - i) :
                    headstr[0] == '*' ? (ret * i) :
                    headstr[0] == '/' ? (ret / i) :
                    (ret%i)
                );
            }
            #define ret_ _value
            i = ret;
            newIntValue();
            // return ret_;
            return;
            #undef ret_
            #undef nextlist
        }
        if (eqstr(headstr, "<") || eqstr(headstr, ">")) {
            #define ret _value
            evalAsInt(arg2list->value, env);
            int m = i;
            // ret = eval(arg1, env);
            eval(arg1, env);
            int n = ret->n;
            if (headstr[0] == '<') {
                _value = n < m ? ret : NULL;
                // return n < m ? ret : NULL;
            } else {
                _value = n > m ? ret : NULL;
            }
            // return n > m ? ret : NULL;
            return;
            #undef ret
        }
    }

eval_lambda:;
    #define curargname _list_eval
    #define isMacro node
    // If the head of the list is a list or an atom not any of the above,
    // it is expected for it to evaluate to a lambda.
    // Lambda* lambda = eval(node->list->value, env)->lambda;
    eval(node->list->value, env);
    Lambda* lambda = _value->lambda;
    // _lambda = eval(node->list->value, env)->lambda;
    curargname = lambda->argnames;
    List* curarg = node->list->next;
    // Macros should be evaluated in the environment they are called in
    isMacro = (Value*) (lambda->type == L_MACRO);
    Env* curenv = ((int)isMacro) ? env : lambda->env;

    while (curargname) {
        // char* argname = curargname->value->str;
        // For macros, simply pass the arguments without evaluating them, as nodes
        // _value = ((int)isMacro) ? eval(curarg->value, env) : curarg->value;
        if ((int)isMacro) {
            _value = curarg->value;
        } else {
            eval(curarg->value, env);
        }
        _str = curargname->value->str;
        _env = curenv;
        curenv = newEnv();
        curargname = curargname->next;
        curarg = curarg->next;
    }
    // For macros, evaluate the result before returning it
    // _value = eval(lambda->body, curenv);
    eval(lambda->body, curenv);
    // return ((int)isMacro) ? _value : eval(_value, curenv);
    // _value = ((int)isMacro) ? _value : eval(_value, curenv);
    if ((int)isMacro) {
        eval(_value, curenv);
    }
    #undef curargname
    #undef isMacro
}

#ifdef ELVM
char* init_stdin = QFTASM_RAMSTDIN_BUF_STARTPOSITION;
#endif
Env* globalEnv;
List* initlist;
List* curlist;
Value* parsed;
int main (void) {
    _str = "";
    _value = NULL;
    _env = NULL;
    globalEnv = newEnv();
    _str = "progn";
    initlist = newList(newAtomNode(), NULL);
    curlist = initlist;
    // Value* parsed;
    while((parseExpr(), _value)) {
        curlist->next = newList(_value, NULL);
        curlist = curlist->next;
    }
    // printValue(newListNode(initlist));
#ifdef ELVM
    *init_stdin = 0;
    // for (;*init_stdin;init_stdin++) {
    //     *init_stdin = 0;
    // }
#endif
    _list = initlist;
    eval(newListNode(), globalEnv);
}
