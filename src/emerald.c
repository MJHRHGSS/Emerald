#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
typedef enum {
    LEFT_BRACE, RIGHT_BRACE, LEFT_PAREN, RIGHT_PAREN,
    COMMA, DOT, PLUS, MINUS, STAR, SLASH,
    BANG, MORE, LESS, EQUALS,
    BANG_EQ, MORE_EQ, LESS_EQ, EQUALS_EQ,
    IDENTIFIER, STRING, DOUBLE, ARRAY,
    YES, NO,
    AND, OR, NOT, OTHERWISE, REPEAT, UNTIL, FOR, NOTHING, SAY, COMMENT, ACTION, GIVE, HAVE, IF, TEXT, NUMBER, YESNO, LIST, VAR, FUNC, IS,
    WHITESPACE, NEWLINE,
    TOK_EOF
} tokentype;
typedef struct {
    union {
        char *str;
        double num;
        int boolean, isnull;
    } value;
    tokentype type;
    int ID, line, whitespace;
} token;
typedef struct {
    token *tokens;
    size_t cap;
    int size;
} tokenslist;
typedef struct {
    char *key;
    tokentype value;
} hashmap;
typedef enum {NUM, STR, BOOL, LI, CHAR} objtype;
typedef struct object object;
typedef struct {
    size_t cap;
    int size;
    object *o;
} list;
struct object {
    union {
        int boolean;
        double num;
        char *str;
        char c;
        list l;
    } val;
    objtype type;
};
typedef struct astnode astnode;
struct astnode {
    tokentype type;
    union {
        struct {
            union {
                double num;
                char *str;
                int boolean;
                list l;
            } val;
            char *name;
            int id;
        } var;
        struct {
            struct astnode **elements;
            size_t size;
        } list;
        struct {
            struct astnode *left;
            char *op;
            struct astnode *right;
        } binary;
        struct {struct astnode *value;} give;
        struct {struct astnode *condition, *then, *otherwise;} ifstmt;
        struct {struct astnode *times, *body;} repeat;
        struct {struct astnode *condition, *body;} until;
        struct {
            char *name;
            struct astnode **args;
            int argc;
            tokentype retval;
        } action;
        struct {
            struct astnode **statements;
            int count;
        } program;
    };
};
int ids = 0, varids = 0;
int *idptr = &ids, *varptr = &varids;
hashmap keywords[21] = {
    {"if", IF},
    {"and", AND},
    {"text", TEXT},
    {"number", NUMBER},
    {"yesno", YESNO},
    {"list", LIST},
    {"have", HAVE},
    {"give", GIVE},
    {"action", ACTION},
    {"comment", COMMENT},
    {"say", SAY},
    {"nothing", NOTHING},
    {"for", FOR},
    {"until", UNTIL},
    {"repeat", REPEAT},
    {"otherwise", OTHERWISE},
    {"or", OR},
    {"not", NOT},
    {"is", IS},
    {"list", LIST},
    {NULL, TOK_EOF}
};
hashmap booleans[3] = {
    {"yes", YES},
    {"no", NO},
    {NULL, TOK_EOF}
};
// Calculator block
// Calculator block
double res = 0.0;
int containsop(const char *op, const char *level[]) {
    for (int i = 0; level[i] != NULL; i++) if (strcmp(level[i], op) == 0) return 1;
    return 0;
}
void removetoken(tokenslist *tokens, size_t *size, size_t index) {
    if (index >= *size) return;
    printf("[REMOVETOKEN] Removing token at index %zu (tokens count before removal: %zu)\n", index, *size);
    for (size_t i = index; i < *size; i++) {
        tokens->tokens[i] = tokens->tokens[i + 1];
        printf("[REMOVETOKEN] Shifted token from index %zu to index %zu\n", i + 1, i);
    }
    (*size)--;
    printf("[REMOVETOKEN] Token removed. New tokens count: %zu\n", *size);
}
void calculate(tokenslist *tokens, size_t size, int *count) {
    const char *precedenselevels[5][3] = {
        {"(", NULL}, 
        {"*", "/", NULL}, 
        {"+", "-", NULL}, 
        {NULL}
    };
    for (int i = 0; precedenselevels[i][0] != NULL; i++) {
        const char **level = precedenselevels[i];
        for (size_t t = 0; t < size; t++) {
            token tok = tokens->tokens[t];
            if (tok.type == PLUS || tok.type == MINUS || tok.type == STAR || tok.type == SLASH || tok.type == LEFT_PAREN) {
                const char *operator = tok.value.str;
                if (containsop(operator, level)) {
                    double a = tokens->tokens[t - 1].value.num, b = tokens->tokens[t + 1].value.num;
                    char op = operator[0];
                    switch (op) {
                        case '*': {
                            res = a * b;
                            break;
                        } case '/': {
                            if (b == 0) {
                                printf("[CALCULATOR] ArithmeticException: Cannot divide by 0\n");
                                exit(1);
                            }
                            res = a / b;
                            break;
                        } case '+': {
                            res = a + b;
                            break;
                        } case '-': {
                            res = a - b;
                            break;
                        } default:
                            printf("[CALCULATOR] Unknown operator: %c\n", op);
                            exit(1);
                    }
                    printf("[CALCULATOR] Calculated result of operation '%c': %f\n", op, res);
                    tokens->tokens[t - 1].value.num = res;
                    removetoken(tokens, &size, t);
                    (*count)--;
                    removetoken(tokens, &size, t);
                    (*count)--;
                }
            }
        }
    }
}
double calc(tokenslist *tokens) {
    printf("[CALC] Parsing...\n");
    while (tokens->size != 1) calculate(tokens, tokens->size, &tokens->size);
    if (tokens->tokens[0].type == DOUBLE) res = tokens->tokens[0].value.num;
    printf("[CALC] Returning result: %f\n", res);
    return res;
}
// Calculator block
// Calculator block
tokentype get(hashmap *map, const char *key) {
    printf("[GET] Scanning hashmap for key: %s\n", key);
    for (int i = 0; map[i].key != NULL; i++) {
        if (strcmp(map[i].key, key) == 0) {
            printf("[GET] Found key with value: %d, returning\n", map[i].value);
            return map[i].value;
        }
    }
    printf("[GET] Key not found\n");
    return TOK_EOF;
}
void add(tokenslist *l, token *item) {
    printf("[ADD] Adding item to array\n");
    if (l->size >= l->cap) {
        l->cap *= 2;
        l->tokens = realloc(l->tokens, l->cap * sizeof(token));
        printf("[ADD] Increased array capacity for safety\n");
    }
    item->ID = (*idptr)++;
    l->tokens[l->size++] = item[0];
    printf("[ADD] Added item to array\n");
}
void listadd(list *l, object val) {
    printf("[LISTADD] Adding item to array\n");
    if (l->size >= l->cap) {
        l->cap *= 2;
        l->o = realloc(l->o, l->cap * sizeof(object));
        printf("[LISTADD] Increased array capacity for safety\n");
    }
    l->o[l->size++] = val;
    printf("[LISTADD] Added item to array\n");
}
int isnull(token item) {
    printf("[ISNULL] Checking if token is null... %s\n", (item.value.isnull && item.type == TOK_EOF) ? "true" : "false");
    if (item.value.isnull && item.type == TOK_EOF) return 1;
    return 0;
}
char *nameof(tokentype t) {
    switch (t) {
        case 0:
            return "LEFT_BRACE";
        case 1:
            return "RIGHT_BRACE";
        case 2:
            return "LEFT_PAREN";
        case 3:
            return "RIGHT_PAREN";
        case 4:
            return "COMMA";
        case 5:
            return "DOT";
        case 6:
            return "PLUS";
        case 7:
            return "MINUS";
        case 8:
            return "STAR";
        case 9:
            return "SLASH";
        case 10:
            return "BANG";
        case 11:
            return "MORE";
        case 12:
            return "LESS";
        case 13:
            return "EQUALS";
        case 14:
            return "BANG_EQ";
        case 15:
            return "MORE_EQ";
        case 16:
            return "LESS_EQ";
        case 17:
            return "EQUALS_EQ";
        case 18:
            return "IDENTIFIER";
        case 19:
            return "STRING";
        case 20:
            return "DOUBLE";
        case 21:
            return "ARRAY";
        case 22:
            return "YES";
        case 23:
            return "NO";
        case 24:
            return "AND";
        case 25:
            return "OR";
        case 26:
            return "NOT";
        case 27:
            return "OTHERWISE";
        case 28:
            return "REPEAT";
        case 29:
            return "UNTIL";
        case 30:
            return "FOR";
        case 31:
            return "NOTHING";
        case 32:
            return "SAY";
        case 33:
            return "COMMENT";
        case 34:
            return "ACTION";
        case 35:
            return "GIVE";
        case 36:
            return "HAVE";
        case 37:
            return "IF";
        case 38:
            return "TEXT";
        case 39:
            return "NUMBER";
        case 40:
            return "YESNO";
        case 41:
            return "LIST";
        case 42:
            return "VAR";
        case 43:
            return "FUNC";
        case 44:
            return "IS";
        case 45:
            return "WHITESPACE";
        case 46:
            return "NEWLINE";
        case 47:
            return "EOF";
        default:
            return "NULL";
    }
}
token tokenize(char c, char *src, token *token, int *line, int *current, int *start, tokenslist *tokens) {
    switch(c) {
        case '(': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = "(";
            token->type = LEFT_PAREN;
            token->line = *line;
            add(tokens, token);
            break;
        } case ')': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = ")";
            token->type = RIGHT_PAREN;
            token->line = *line;
            add(tokens, token);
            break;
        } case '[': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = "[";
            token->type = LEFT_BRACE;
            token->line = *line;
            add(tokens, token);
            break;
        } case ']': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = "]";
            token->type = RIGHT_BRACE;
            token->line = *line;
            add(tokens, token);
            break;
        } case ',': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = ",";
            token->type = COMMA;
            token->line = *line;
            add(tokens, token);
            break;
        } case '.': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = ".";
            token->type = DOT;
            token->line = *line;
            add(tokens, token);
            break;
        } case '+': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = "+";
            token->type = PLUS;
            token->line = *line;
            add(tokens, token);
            break;
        } case '-': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = "-";
            token->type = MINUS;
            token->line = *line;
            add(tokens, token);
            break;
        } case '*': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = "*";
            token->type = STAR;
            token->line = *line;
            add(tokens, token);
            break;
        } case '/': {
            printf("[TOKENIZE] Found character %c, adding to tokens array\n", c);
            token->value.str = "/";
            token->type = SLASH;
            token->line = *line;
            add(tokens, token);
            break;
        } case '!': {
            printf("[TOKENIZE] Found character %c, scanning...\n", c);
            if (src[*current + 1] == '=') {
                printf("[TOKENIZE] Found token !=, adding to tokens array\n");
                token->value.str = "!=";
                token->type = BANG_EQ;
                token->line = *line;
                add(tokens, token);
                current++;
            } else {
                printf("[TOKENIZE] Tokenising as !\n");
                token->value.str = "!";
                token->type = BANG;
                token->line = *line;
                add(tokens, token);
            }
            break;
        } case '<': {
            printf("[TOKENIZE] Found character %c, scanning...\n", c);
            if (src[*current + 1] == '=') {
                printf("[TOKENIZE] Found token <=, adding to tokens array\n");
                token->value.str = "<=";
                token->type = LESS_EQ;
                token->line = *line;
                add(tokens, token);
                current++;
            } else {
                printf("[TOKENIZE] Tokenising as <\n");
                token->value.str = "<";
                token->type = LESS;
                token->line = *line;
                add(tokens, token);
            }
            break;
        } case '>': {
            printf("[TOKENIZE] Found character %c, scanning...\n", c);
            if (src[*current + 1] == '=') {
                printf("[TOKENIZE] Found token >=, adding to tokens array\n");
                token->value.str = ">=";
                token->type = MORE_EQ;
                token->line = *line;
                add(tokens, token);
                current++;
            } else {
                printf("[TOKENIZE] Tokenising as >\n");
                token->value.str = ">";
                token->type = MORE;
                token->line = *line;
                add(tokens, token);
            }
            break;
        } case '=': {
            printf("[TOKENIZE] Found character %c, scanning...\n", c);
            if (src[*current + 1] == '=') {
                printf("[TOKENIZE] Found token ==, adding to tokens array\n");
                token->value.str = "==";
                token->type = EQUALS_EQ;
                token->line = *line;
                add(tokens, token);
                current++;
            } else {
                printf("[TOKENIZE] Tokenising as =\n");
                token->value.str = "=";
                token->type = EQUALS;
                token->line = *line;
                add(tokens, token);
            }
            break;
        }
        case '\r':
        case ' ':
            printf("[TOKENIZE] Skipping whitespace\n");
            token->type = -1;
            token->line = *line;
            token->value.str = " ";
            break;
        case '\n': {
            printf("[TOKENIZE] Skipping whitespace\n");
            token->line = *line;
            token->type = NEWLINE;
            token->value.str = "\n";
            (*line)++;
            add(tokens, token);
            break;
        }
        case '\t': {
            int whitespace = 0;
            while (src[*current] == '\t') {do {(*current)++; whitespace++;} while (src[*current + 1] == '\t');}
            token->type = WHITESPACE;
            token->whitespace = whitespace;
            token->line = *line;
            add(tokens, token);
        } case '"': {
            printf("[TOKENIZE] Found string literal, scanning\n");
            while (*current < strlen(src) && src[*current + 1] != '"') {
                if (src[*current] == '\n') (*line)++;
                (*current)++;
            }
            if (*current >= strlen(src)) {
                printf("[ERROR] I'm sorry bro u tripping? Bro forgot a whole \" ToT\n");
                exit(1);
            }
            (*current) += 2;
            char value[256];
            int len = (*current) - (*start);
            strncpy(value, src + (*start), len);
            value[len] = '\0';
            token->value.str = strdup(value);
            token->type = STRING;
            token->line = *line;
            printf("[TOKENIZE] Finished tokenizing string, start index: %d, end index: %d\n[TOKENIZE] Adding string literal to tokens array\n", *start, *current);
            add(tokens, token);
            break;
        } default:
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
                printf("[TOKENIZE] Found an identifier, scanning\n");
                while ((src[*current] >= 'A' && src[*current] <= 'Z') || (src[*current] >= 'a' && src[*current] <= 'z') || (src[*current] >= '0' && src[*current] <= '9') || src[*current] == '_') (*current)++;
                printf("[TOKENIZE] Finished tokenising identifier, start index: %d, end index: %d\n[TOKENIZE] Adding identifier to tokens array\n", *start, *current);
                char txt[256];
                int len = *current - *start;
                strncpy(txt, src + *start, len);
                txt[len] = '\0';
                tokentype type = get(booleans, txt);
                if (type == TOK_EOF) {
                    type = get(keywords, txt);
                    if (type == TOK_EOF) type = IDENTIFIER;
                }
                if (type == COMMENT) {
                    while (src[*current] != '\n' && *current <= strlen(src)) (*current)++;
                    break;
                }
                if (type == YES || type == NO) token->value.boolean = type == YES;
                else token->value.str = strdup(txt);
                printf("[TOKENIZE] tokentype for identifier: %s\n", nameof(type));
                token->type = type;
                token->line = *line;
                add(tokens, token);
                if ((token->type == NUMBER || token->type == TEXT || token->type == YESNO || token->type == LIST) && (*start = *current) && tokenize(c, src, token, line, current, start, tokens).type != ACTION) {
                    printf("[TOKENIZE] Found var\n");
                    for (int i = 0; i < tokens->size && src[i] != ' '; i++) (*current)++;
                    token->type = VAR;
                    token->line = *line;
                    char val[256];
                    len = *current - *start;
                    strncpy(val, src + *start, len);
                    val[len] = '\0';
                    token->value.str = strdup(val);
                    add(tokens, token);
                } else if (token->type == ACTION) {
                    while (src[*current] == ' ') (*current)++;
                    *start = *current;
                    while (((src[*current] >= 'A' && src[*current] <= 'Z') || (src[*current] >= 'a' && src[*current] <= 'z') || (src[*current] >= '0' && src[*current] <= '9') || src[*current] == '_') && src[*current] != '\n') (*current)++;
                    printf("[TOKENIZE] Found variable name, start index: %d, end index: %d\n", *start, *current);
                    char val[256];
                    len = *current - *start;
                    strncpy(val, src + *start, len);
                    txt[len] = '\0';
                    token->value.str = strdup(val);
                    token->type = FUNC;
                    token->line = *line;
                    add(tokens, token);
                }
            } else if (c >= '0' && c <= '9') {
                printf("[TOKENIZE] Found a number, scanning\n");
                while (src[*current] >= '0' && src[*current] <= '9') (*current)++;
                if (src[*current] == '.' && (src[*current + 1] >= '0' && src[*current + 1] <= '9')) do (*current)++; while (src[*current] >= '0' && src[*current] <= '9');
                printf("[TOKENIZE] Finished tokenising number, start index: %d, end index: %d\n[TOKENIZE] Adding number to tokens array\n", *start, *current);
                char value[256];
                strncpy(value, src + *start, *current - *start);
                value[*current - *start] = '\0';
                token->value.num = atof(value);
                token->type = DOUBLE;
                token->line = *line;
                add(tokens, token);
            } else {
                printf("[TOKENIZE] Unknown token: %c, line: %d\n", c, *line);
                exit(1);
            }
            break;
    }
    printf("[TOKENIZE] Done tokenizing, returning token.\n");
    return *token;
}
void lex(char *src, tokenslist *tokens) {
    printf("[LEX] Initialising important debugging variables, start = 0, current = 0, line = 1\n");
    int start = 0, current = 0, line = 1;
    while (src[current] != '\0') {
        start = current;
        char c = src[current++];
        if (c != '\n' && c != '\t' && c != '\r' && c != ' ') printf("[TOKENIZE] Lexing character: %c\n", c);
        token token;
        token.whitespace = 0;
        tokenize(c, src, &token, &line, &current, &start, tokens);
        printf("[LEX] Created token of type: %s, line: %d\n", nameof(token.type), token.line);
    }
}
int isemr(char *filename) {
    if (!filename) return 0;
    size_t len = strlen(filename);
    if (len < 4) return 0;
    printf("[ISEMR] Checking if file %s is emerald... %s\n", filename, (strcmp(filename + (len - 4), ".emr") == 0) ? "true" : "false");
    return strcmp(filename + (len - 4), ".emr") == 0;
}

void print_token(token t) {
    printf("[PRINT_TOKEN] Token type: %s, value: ", nameof(t.type));
    switch (t.type) {
        case IS:
        case STRING:
        case IDENTIFIER:
        case TEXT:
        case COMMENT:
        case NUMBER:
        case FUNC:
        case VAR:
        case YESNO:
        case AND:
        case OR:
        case OTHERWISE:
        case REPEAT:
        case UNTIL:
        case FOR:
        case NOTHING:
        case SAY:
        case ACTION:
        case GIVE:
        case HAVE:
        case IF:
            printf("'%s'\n", t.value.str);
            break;
        case DOUBLE:
            printf("%f\n", t.value.num);
            break;
        case YES:
        case NO:
            printf("%d\n", t.value.boolean);
            break;
        default:
            if (t.value.str) printf("'%s'\n", t.value.str);
            else printf("NULL\n");
        break;
    }
}
astnode *make_list(list val, char *name) {
    astnode *node = malloc(sizeof(astnode));
    node->type = ARRAY;
    node->var.val.l = val;
    node->var.id = (*varptr)++;
    node->var.name = name;
    return node;
}
astnode *make_num(double val, char *name) {
    astnode *node = malloc(sizeof(astnode));
    node->type = DOUBLE;
    node->var.val.num = val;
    node->var.id = (*varptr)++;
    node->var.name = name;
    return node;
}
astnode *make_give(struct astnode *val) {
    astnode *node = malloc(sizeof(astnode));
    node->type = GIVE;
    node->give.value = val;
    return node;
}
astnode *make_txt(char *val, char *name) {
    astnode *node = malloc(sizeof(astnode));
    node->type = STRING;
    node->var.val.str = val;
    node->var.id = (*varptr)++;
    node->var.name = name;
    return node;
}
astnode *make_bool(int val, char *name) {
    astnode *node = malloc(sizeof(astnode));
    node->type = (val) ? YES : NO;
    node->var.val.boolean = val;
    node->var.id = (*varptr)++;
    node->var.name = name;
    return node;
}
astnode *make_action(int argc, astnode **args, char *name, tokentype retval) {
    astnode *node = malloc(sizeof(astnode));
    node->type = ACTION;
    node->action.argc = argc;
    node->action.args = args;
    node->action.name = name;
    node->action.retval = retval;
    return node;
}
void debug(const char *funcname) {printf("[%s] Do you see this?\n", funcname);}
char *strntok(tokenslist *tokens, int first, int eof) {
    char buffer[64];
    size_t total_len = 0;
    for (int i = first; i < eof; i++) {
        if (tokens->tokens[i].type == STRING) total_len += strlen(tokens->tokens[i].value.str);
        else if (tokens->tokens[i].type == DOUBLE) {
            snprintf(buffer, sizeof(buffer), "%.17g", tokens->tokens[i].value.num);
            total_len += strlen(buffer);
        }
    }
    char *result = malloc(total_len + 1);
    if (!result) return NULL;
    result[0] = '\0';
    for (int i = first; i < eof; i++) {
        if (tokens->tokens[i].type == STRING) strcat(result, tokens->tokens[i].value.str);
        else if (tokens->tokens[i].type == DOUBLE) {
            snprintf(buffer, sizeof(buffer), "%.17g", tokens->tokens[i].value.num);
            strcat(result, buffer);
        }
    }
    return result;
}
object lexlist(list *l, tokenslist *toks, int idx, int eof) {
    object obj = *(object *)malloc(sizeof(object));
    tokentype t = toks->tokens[idx].type;
    switch (t) {
        case DOUBLE:{
            object *o = malloc(sizeof(object));
            o->type = NUM;
            o->val.num = toks->tokens[idx].value.num;
            listadd(l, *o);
            obj = *o;
            break;
        } case STRING:{
            object *o = malloc(sizeof(object));
            o->type = STR;
            o->val.num = toks->tokens[idx].value.num;
            listadd(l, *o);
            obj = *o;
            break;
        } case YESNO:{
            object *o = malloc(sizeof(object));
            o->type = BOOL;
            o->val.boolean = toks->tokens[idx].value.boolean;
            listadd(l, *o);
            obj = *o;
            break;
        } case LEFT_PAREN:{
            int current = 0;
            list li = *(list *)malloc(sizeof(list));
            li.size = 0;
            li.cap = 256;
            li.o = malloc(li.cap * sizeof(object));
            for (int i = eof; i > idx; i--) {
                current++;
                if (toks->tokens[i + 1].type == RIGHT_PAREN) break;
            }
            for (int i = idx; i < current; i++) listadd(&li, lexlist(l, toks, idx, current));
            object *o = malloc(sizeof(object));
            o->type = LI;
            o->val.l = li;
            listadd(l, *o);
            obj = *o;
            break;
        } case COMMA:
        case NEWLINE:
        case WHITESPACE:
            break;
        default:{
            printf("[LEXLIST] Unknown token: %s\n", nameof(t));
            break;
        }
    }
    return obj;
}
astnode **create_ast(tokenslist *tokens) {
    printf("[CREATE_AST] Creating AST...\n");
    astnode **node = malloc(sizeof(astnode));
    tokentype priority[5][7] = {
        {ACTION, NUMBER, TEXT, LIST, YESNO, -1},
        {IF, OTHERWISE, REPEAT, FOR, UNTIL, -1},
        {GIVE, -1},
        {EQUALS_EQ, BANG_EQ, MORE, LESS, MORE_EQ, LESS_EQ, -1},
        {-1}
    };
    for (int i = 0; i < tokens->size && !isnull(tokens->tokens[i]); i++) {
        token *tok = &tokens->tokens[i];
        int group = -1;
        for (int p = 0; priority[p][0] != -1; p++) {
            const tokentype *level = priority[p];
            for (int x = 0; level[x] != -1; x++) {
                if (tok->type == level[x]) {
                    group = p;
                    break;
                }
            }
            if (group != -1) break;
        }
        if (group == -1) printf("[PARSE] tokentype %s not in any prioroties group\n", nameof(tok->type));
        switch (tok->type) {
            // TODO: Finish AST node creation
            case ACTION: {
                printf("[CREATE_AST] Found expression of type: ACTION, initialising variables...\n");
                int argc, retval, index, is;
                astnode **args;
                char *name;
                tokenslist *actiontoks = malloc(sizeof(tokenslist));
                actiontoks->cap = 256;
                actiontoks->size = 0;
                actiontoks->tokens = malloc(actiontoks->cap * sizeof(token));
                printf("[CREATE_AST] Done!\n[CREATE_AST] Creating AST node...\n");
                printf("[CREATE_AST] Lexing ACTION token line...\n");
                for (int i = 0; i < tokens->size; i++) if (tokens->tokens[i].ID == tok->ID) index = i - 1;
                for (int i = index; i < tokens->size; i++) if (tokens->tokens[i].type == IS && tokens->tokens[i].line == tok->line) is = i;
                for (int i = index; i < is; i++) add(actiontoks, &tokens->tokens[i]);
                printf("[CREATE_AST] Done!\n");
                printf("[CREATE_AST] Getting return value of ACTION...\n");
                for (int i = 0; i < tokens->size; i++) {
                    tokentype t = tokens->tokens[i].type;
                    if (t == ACTION) {
                        t = tokens->tokens[i - 1].type;
                        if (t == NUMBER || t == TEXT || t == YESNO || t == LIST) retval = t;
                        else retval = -1;
                    }
                }
                printf("[CREATE_AST] Done! Return value: %s\n", (retval != -1) ? nameof(retval) : "void");
                printf("[CREATE_AST] Finding number of arguments...\n");
                argc = (is - index) - 3;
                if (retval >= 0) argc--;
                if (argc != 0) argc /= 2;
                printf("[CREATE_AST] Done! Number of arguments: %d\n", argc);
                printf("[CREATE_AST] Finding the name of the ACTION...\n");
                name = tokens->tokens[index + 2].value.str;
                printf("[CREATE_AST] Done! Name of ACTION: %s\n", name);
                printf("[CREATE_AST] Getting code block...\n");
                int end = 0;
                int line;
                int prevl;
                for (int i = is + 1; i < tokens->size; i++) {
                    if (i > is + 1) prevl = line;
                    else prevl = -1;
                    line = tokens->tokens[i].line;
                    if (line != prevl) {
                        if (tokens->tokens[i].type == WHITESPACE) end++;
                        else break;
                    }
                }
                tokenslist *actionblock = malloc(sizeof(tokenslist));
                actionblock->cap = 256;
                actionblock->size = 0;
                actionblock->tokens = malloc(actionblock->cap * sizeof(token));
                for (int i = is + 1; i < end + is; i++) {if (!isnull(tokens->tokens[i])) {add(actionblock, &tokens->tokens[i]); printf("[CREATE_AST] Added token %s to action block array.\n", nameof(tokens->tokens[i].type));}}
                args = create_ast(actionblock);
                printf("[CREATE_AST] Done!\n");
                printf("[CREATE_AST] Finalising...\n");
                astnode *actionnode = make_action(argc, args, name, retval);
                node = &actionnode;
                printf("[CREATE_AST] Successfully created AST node of type ACTION.\n");
                break;
            } case NUMBER: {
                printf("[CREATE_AST] Found expression of type: NUMBER, initialising variables...\n");
                char *name;
                int index, eof, isinit;
                double val;
                tokenslist *numtoks = malloc(sizeof(tokenslist));
                numtoks->cap = 256;
                numtoks->size = 0;
                numtoks->tokens = malloc(numtoks->cap * sizeof(token));
                printf("[CREATE_AST] Done!\n[CREATE_AST] Creating AST node...\n");
                printf("[CREATE_AST] Lexing NUMBER token line...\n");
                for (int i = 0; i < tokens->size; i++) if (tokens->tokens[i].ID == tok->ID) index = i;
                for (int i = index; i < tokens->size; i++) {
                    if ((tokens->tokens[i].type == DOUBLE || tokens->tokens[i].type == VAR) && tokens->tokens[i].line == tok->line) {
                        eof = i; 
                        isinit = tokens->tokens[i].type == DOUBLE;
                        if (tokens->tokens[i].type == DOUBLE || (tokens->tokens[i].type == VAR && tokens->tokens[i + 2].type != DOUBLE)) break;
                    }
                }
                for (int i = index; i < eof + 1; i++) add(numtoks, &tokens->tokens[i]);
                printf("[CREATE_AST] Done!\n");
                printf("[CREATE_AST] Getting value of NUMBER...\n");
                if (isinit) {
                    int first;
                    for (int i = index; i < eof; i++) {if (strcmp(tokens->tokens[i].value.str, "=") == 0) {first = i + 1; break;}}
                    char *eq = strntok(tokens, first, eof);
                    val = tokens->tokens[eof].value.num;
                }
                printf("[CREATE_AST] Done! Value: %f\n", val);
                printf("[CREATE] Getting name of NUMBER...\n");
                name = tokens->tokens[index + 2].value.str;
                printf("[CREATE_AST] Done! Name of NUMBER: %s\n", name);
                printf("[CREATE_AST] Finalising...\n");
                astnode *numnode = make_num(val, name);
                node = &numnode;
                printf("[CREATE_AST] Successfully created AST node of type NUMBER.\n");
                break;
            } case TEXT: {
		        printf("[CREATE_AST] Found expression of type: TEXT, initialising variables...\n");
		        char *name, *val;
		        int index, eof, isinit;
		        tokenslist *txttoks = malloc(sizeof(tokenslist));
		        txttoks->cap = 256;
		        txttoks->size = 0;
		        txttoks->tokens = malloc(txttoks->cap * sizeof(token));
		        printf("[CREATE_AST] Done!\n[CREATE_AST] Creating AST node...\n");
		        printf("[CREATE_AST] Lexing TEXT token line...\n");
		        for (int i = 0; i < tokens->size; i++) if (tokens->tokens[i].ID == tok->ID) index = i;
		        for (int i = index; i < tokens->size; i++) {
                    if ((tokens->tokens[i].type == STRING || tokens->tokens[i].type == VAR) && tokens->tokens[i].line == tok->line) {
                        eof = i;
                        isinit = tokens->tokens[i].type == STRING;
                        if (tokens->tokens[i].type == STRING || (tokens->tokens[i].type == VAR && tokens->tokens[i].type != STRING)) break;
                    }
                }
		        for (int i = index; i < eof + 1; i++) add(txttoks, &tokens->tokens[i]);
		        printf("[CREATE_AST] Done!\n");
		        if (isinit) val = tokens->tokens[eof].value.str;
		        printf("[CREATE_AST] Done! Value: \"%s\"\n", val);
		        printf("[CREATE_AST] Getting name of TEXT...\n");
		        name = tokens->tokens[index + 2].value.str;
		        printf("[CREATE_AST] Done! Name of TEXT: %s\n", name);
                printf("[CREATE_AST] Finalising...\n");
		        astnode *txtnode = make_txt(val, name);
		        node = &txtnode;
		        printf("[CREATE_AST] Successfully created AST node of type TEXT.\n");
		        break;
	        } case YESNO: {
                printf("[CREATE_AST] Found expression of type: YESNO, initialising variables...\n");
                char *name;
                int index, eof, isinit, val;
                tokenslist *booltoks = malloc(sizeof(tokenslist));
                booltoks->cap = 256;
                booltoks->size = 0;
                booltoks->tokens = malloc (booltoks->cap * sizeof(token));
                printf("[CREATE_AST] Done!\n[CREATE_AST] Creating AST node...\n");
                printf("[CREATE_AST] Lexing YESNO token line...\n");
                for (int i = 0; i < tokens->size; i++) {if (tokens->tokens[i].ID == tok->ID) {index = i; break;}}
                for (int i = index; i < tokens->size; i++) {
                    if ((tokens->tokens[i].type == YES || tokens->tokens[i].type == NO || tokens->tokens[i].type == VAR) && tokens->tokens[i].line == tok->line) {
                        eof = i; 
                        isinit = tokens->tokens[i].type == YES || tokens->tokens[i].type == NO;
                        if ((tokens->tokens[i].type == YES || tokens->tokens[i].type == NO) || (tokens->tokens[i].type == VAR && (tokens->tokens[i + 2].type != YES && tokens->tokens[i + 2].type != NO))) break;
                    }
                }
                for (int i = index; i <= eof; i++) add(booltoks, &tokens->tokens[i]);
                printf("[CREATE_AST] Done!\n[CREATE_AST] Getting value of YESNO...\n");
                if (isinit) val = tokens->tokens[eof].value.boolean;
                printf("[CREATE_AST] Done! Value: %d\n", val);
                printf("[CREATE_AST] Getting name of YESNO...\n");
                name = tokens->tokens[index + 2].value.str;
                printf("[CREATE_AST] Done! Name of YESNO: %s\n", name);
                printf("[CREATE_AST] Finalising...\n");
                astnode *boolnode = make_bool(val, name);
                node = &boolnode;
                printf("[CREATE_AST] Successfully created AST node of type YESNO\n");
                break;
            } case LIST: {
                printf("[CREATE_AST] Found expression of type LIST, initialising variables...\n");
                char *name;
                int index, eof, arrind, arreof, isinit;
                list val;
                tokenslist *listtoks = malloc(sizeof(tokenslist));
                listtoks->cap = 256;
                listtoks->size = 0;
                listtoks->tokens = malloc(listtoks->cap * sizeof(token));
                printf("[CREATE_AST] Done!\n[CREATE_AST] Creating AST node...\n");
                printf("[CREATE_AST] Lexing LIST token line...\n");
                for (int i = 0; i < tokens->size; i++) {if (tokens->tokens[i].ID == tok->ID) {index = i; break;}}
                for (int i = index; i < tokens->size; i++) {
                    if ((tokens->tokens[i].type == ARRAY || tokens->tokens[i].type == VAR) && tokens->tokens[i].line == tok->line) {
                        isinit = tokens->tokens[i].type == LEFT_PAREN;
                        if (tokens->tokens[i].type == LEFT_PAREN || (tokens->tokens[i].type == VAR && tokens->tokens[i + 2].type != LEFT_PAREN)) break;
                    }
                }
                for (int i = index; i < tokens->size; i++) {if (tokens->tokens[i].type == NEWLINE) {eof = i; break;}}
                printf("%d\n",eof);
                for (int i = index; i < eof; i++) add(listtoks, &tokens->tokens[i]);
                printf("[CREATE_AST] Done!\n[CREATE_AST] Getting value of LIST...\n");
                if (isinit) {
                    int leftparen, rightparen;
                    for (int i = index; i < tokens->size; i++) {if (tokens->tokens[i].type == LEFT_PAREN) {leftparen = i; break;}}
                    for (int i = eof; i < tokens->size; i--) {if (tokens->tokens[i].type == RIGHT_PAREN) {rightparen = i; break;}}
                    for (int i = leftparen + 1; i < rightparen; i++) lexlist(&val, tokens, i, rightparen);
                }
                printf("[CREATE_AST] Done!\n[CREATE_AST] Getting name of LIST...\n");
                name = tokens->tokens[index + 2].value.str;
                printf("[CREATE_AST] Done! Name of LIST: %s\n", name);
                printf("[CREATE_AST] Finalising...\n");
                astnode *listnode = make_list(val, name);
                node = &listnode;
                printf("[CREATE_AST] Successfully created AST node of type LIST\n");
                break;
            } default:
                break;
        }
    }
    return node;
}
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("[MAIN] Usage: emr <file.emr>\n");
        return 1;
    }
    if (!isemr(argv[1])) {
        printf("[MAIN] Specified file does not have the emerald extention.\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("[MAIN] Could not open file %s\n", argv[1]);
        return 1;
    }
    printf("[MAIN] Reading file...\n");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    char *src = malloc(fsize + 1);
    fread(src, 1, fsize, f);
    src[fsize] = '\0';
    fclose(f);
    printf("[MAIN] Finished reading file, initialising tokens array\n");
    tokenslist *tokens = malloc(sizeof(tokenslist));
    tokens->tokens = malloc(256 * sizeof(token));
    tokens->size = 0;
    tokens->cap = 256;
    printf("[MAIN] Finished initialising tokens array, lexing\n");
    lex(src, tokens);
    token null = {NULL, TOK_EOF};
    add(tokens, &null);
    printf("[MAIN] Finished lexing, found tokens:\n");
    for (int i = 0; i < tokens->size; i++) {
        if (isnull(tokens->tokens[i])) break;
        print_token(tokens->tokens[i]);
    }
    printf("[MAIN] Current tokens capacity: %d, current tokens array size: %d\n", (int)tokens->cap, tokens->size);
    astnode **ast = create_ast(tokens);
    return 0;
}
