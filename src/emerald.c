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
    IDENTIFIER, STRING, DOUBLE,
    YES, NO,
    AND, OR, NOT, OTHERWISE, REPEAT, UNTIL, FOR, NOTHING, SAY, COMMENT, ACTION, GIVE, HAVE, IF, TEXT, NUMBER, YESNO, LIST, VAR, FUNC, IS,
    TOK_EOF
} tokentype;
typedef struct {
    union {
        char *str;
        double num;
        int bool, isnull;
    } value;
    tokentype type;
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
typedef struct {
    tokentype type;
    union {
        double num;
        char *str;
        int bool;
        struct {
            struct astnode **elements;
            size_t size;
        } list;
        struct {
            struct astnode *left;
            char *op;
            struct astnode *right;
        } binary;
        struct {
            struct astnode *value;
        } give;
        struct {
            struct astnode *condition, *then, *otherwise;
        } ifstmt;
        struct {
            struct astnode *times, *body;
        } repeat;
        struct {
            struct astnode *condition, *body;
        } until;
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
} astnode;
hashmap keywords[20] = {
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
    {NULL, TOK_EOF}
};
hashmap booleans[3] = {
    {"yes", YES},
    {"no", NO},
    {NULL, TOK_EOF}
};
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
void add(tokenslist *l, token item) {
    printf("[ADD] Adding item to array\n");
    if (l->size >= l->cap) {
        l->cap *= 2;
        l->tokens = realloc(l->tokens, l->cap * sizeof(token));
        printf("[ADD] Increased array capacity for safety\n");
    }
    l->tokens[l->size++] = item;
    printf("[ADD] Added item to array\n");
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
            return "YES";
        case 22:
            return "NO";
        case 23:
            return "AND";
        case 24:
            return "OR";
        case 25:
            return "NOT";
        case 26:
            return "OTHERWISE";
        case 27:
            return "REPEAT";
        case 28:
            return "UNTIL";
        case 29:
            return "FOR";
        case 30:
            return "NOTHING";
        case 31:
            return "SAY";
        case 32:
            return "COMMENT";
        case 33:
            return "ACTION";
        case 34:
            return "GIVE";
        case 35:
            return "HAVE";
        case 36:
            return "IF";
        case 37:
            return "TEXT";
        case 38:
            return "NUMBER";
        case 39:
            return "YESNO";
        case 40:
            return "LIST";
        case 41:
            return "VAR";
        case 42:
            return "FUNC";
        case 43:
            return "IS";
        case 44:
            return "EOF";
        default:
            return NULL;
    }
}
void lex(const char *src, tokenslist *tokens) {
    printf("[LEX] Initialising important debuggin variables, start = 0, current = 0, line = 1\n");
    int start = 0, current = 0, line = 1;
    while (src[current] != '\0') {
        start = current;
        char c = src[current++];
        if (c != '\n' && c != '\t' && c != '\r' && c != ' ') printf("[LEX] Lexing character: %c\n", c);
        token token;
        switch(c) {
            case '(':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = "(";
                token.type = LEFT_PAREN;
                add(tokens, token);
                break;
            case ')':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = ")";
                token.type = RIGHT_PAREN;
                add(tokens, token);
                break;
            case '[':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = "[";
                token.type = LEFT_BRACE;
                add(tokens, token);
                break;
            case ']':
                printf("[LEX] Found character %c, adding to tokens array\n", c);               
                token.value.str = "]";
                token.type = RIGHT_BRACE;
                add(tokens, token);
                break;
            case ',':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = ",";
                token.type = COMMA;
                add(tokens, token);
                break;
            case '.':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = ".";
                token.type = DOT;
                add(tokens, token);
                break;
            case '+':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = "+";
                token.type = PLUS;
                add(tokens, token);
                break;
            case '-':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = "-";
                token.type = MINUS;
                add(tokens, token);
                break;
            case '*':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = "*";
                token.type = STAR;
                add(tokens, token);
                break;
            case '/':
                printf("[LEX] Found character %c, adding to tokens array\n", c);
                token.value.str = "/";
                token.type = SLASH;
                add(tokens, token);
                break;
            case '!':
                printf("[LEX] Found character %c, scanning...\n", c);
                if (src[current + 1] == '=') {
                    printf("[LEX] Found token !=, adding to tokens array\n");
                    token.value.str = "!=";
                    token.type = BANG_EQ;
                    add(tokens, token);
                    current++;
                } else {
                    printf("[LEX] Tokenising as !\n");
                    token.value.str = "!";
                    token.type = BANG;
                    add(tokens, token);
                }
                break;
            case '<':
                printf("[LEX] Found character %c, scanning...\n", c);
                if (src[current + 1] == '=') {
                    printf("[LEX] Found token <=, adding to tokens array\n");
                    token.value.str = "<=";
                    token.type = LESS_EQ;
                    add(tokens, token);
                    current++;
                } else {
                    printf("[LEX] Tokenising as <\n");
                    token.value.str = "<";
                    token.type = LESS;
                    add(tokens, token);
                }
                break;
            case '>':
                printf("[LEX] Found character %c, scanning...\n", c);
                if (src[current + 1] == '=') {
                    printf("[LEX] Found token >=, adding to tokens array\n");
                    token.value.str = ">=";
                    token.type = MORE_EQ;
                    add(tokens, token);
                    current++;
                } else { 
                    printf("[LEX] Tokenising as >\n");
                    token.value.str = ">";
                    token.type = MORE;
                    add(tokens, token);
                }
                break;
            case '=':
                printf("[LEX] Found character %c, scanning...\n", c);
                if (src[current + 1] == '=') {
                    printf("[LEX] Found token ==, adding to tokens array\n");
                    token.value.str = "==";
                    token.type = EQUALS_EQ;
                    add(tokens, token);
                    current++;
                } else {
                    printf("[LEX] Tokenising as =\n");
                    token.value.str = "=";
                    token.type = EQUALS;
                    add(tokens, token);
                }
                break;
            case '\t':
            case '\r':
            case ' ':
                printf("[LEX] Skipping whitespace\n");
                break;
            case '\n':
                printf("[LEX] Skipping whitespace\n");
                line++;
                break;
            case '"':
                printf("[LEX] Found string literal, scanning\n");
                while (src[current] != '"' && current < strlen(src)) {
                    if (src[current] == '\n') line++;
                    current++;
                }
                if (src[current] >= strlen(src)) {
                    printf("[LEX] Error: Unterminated string, line: %d\n", line);
                    exit(1);
                }
                current++;
                printf("[LEX] Finished tokenizing string, start index: %d, end index: %d\n[LEX] Adding string literal to tokens array\n", start, current);
                char value[256];
                strncpy(value, src + start, current - start);
                value[current - start] = '\0';
                token.value.str = strdup(value);
                token.type = STRING;
                add(tokens, token);
                break;
            default:
                if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
                    printf("[LEX] Found an identifier, scanning\n");
                    while ((src[current] >= 'A' && src[current] <= 'Z') || (src[current] >= 'a' && src[current] <= 'z') || (src[current] >= '0' && src[current] <= '9') || src[current] == '_') current++;
                    printf("[LEX] Finished tokenising identifier, start index: %d, end index: %d\n[LEX] Adding identifier to tokens array\n", start, current);
                    char txt[256];
                    int len = current - start;
                    strncpy(txt, src + start, len);
                    txt[len] = '\0';
                    tokentype type = get(booleans, txt);
                    if (type == TOK_EOF) {
                        type = get(keywords, txt);
                        if (type == TOK_EOF) type = IDENTIFIER;
                    }
                    if (type == COMMENT) {
                        while (src[current] != '\n' && current <= strlen(src)) current++;
                        break;
                    }
                    if (type == YES || type == NO) token.value.bool = type == YES;
                    else token.value.str = strdup(txt);
                    printf("[LEX] tokentype for identifier: %s\n", nameof(type));
                    token.type = type;
                    add(tokens, token);
                    if (token.type == NUMBER || token.type == TEXT || token.type == YESNO) {
                        while (src[current] == ' ') current++;
                        start = current;
                        while (((src[current] >= 'A' && src[current] <= 'Z') || (src[current] >= 'a' && src[current] <= 'z') || (src[current] >= '0' && src[current] <= '9') || src[current] == '_') && src[current] != '\n') current++;
                        printf("[LEX] Found variable name, start index: %d, end index: %d\n", start, current);
                        len = current - start;
                        strncpy(txt, src + start, len);
                        txt[len] = '\0';
                        token.value.str = strdup(txt);
                        token.type = VAR;
                        add(tokens, token);
                    } else if (token.type == ACTION) {
                        while (src[current] == ' ') current++;
                        start = current;
                        while (((src[current] >= 'A' && src[current] <= 'Z') || (src[current] >= 'a' && src[current] <= 'z') || (src[current] >= '0' && src[current] <= '9') || src[current] == '_') && src[current] != '\n') current++;
                        printf("[LEX] Found variable name, start index: %d, end index: %d\n", start, current);
                        len = current - start;
                        strncpy(txt, src + start, len);
                        txt[len] = '\0';
                        token.value.str = strdup(txt);
                        token.type = FUNC;
                        add(tokens, token);
                    }
                } else if (c >= '0' && c <= '9') {
                    printf("[LEX] Found a number, scanning\n");
                    while (src[current] >= '0' && src[current] <= '9') current++;
                    if (src[current] == '.' && (src[current + 1] >= '0' && src[current + 1] <= '9')) do current++; while (src[current] >= '0' && src[current] <= '9');
                    printf("[LEX] Finished tokenising number, start index: %d, end index: %d\n[LEX] Adding number to tokens array\n", start, current);
                    char value[256];
                    strncpy(value, src + start, current - start);
                    value[current - start] = '\0';
                    token.value.num = atof(value);
                    token.type = DOUBLE;
                    add(tokens, token);
                }
                else {
                    printf("[LEX] Unknown token: %c, line: %d\n", c, line);
                    exit(1);
                }
                break;
        }
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
            printf("%d\n", t.value.bool);
            break;
        default:
            if (t.value.str)
                printf("'%s'\n", t.value.str);
            else
                printf("NULL\n");
            break;
    }
}
astnode *make_num(double val) {
    astnode *node = malloc(sizeof(astnode));
    node->type = DOUBLE;
    node->num = val;
    return node;
}
astnode *make_give(struct astnode *val) {
    astnode *node = malloc(sizeof(astnode));
    node->type = GIVE;
    node->give.value = val;
    return node;
}
astnode *make_txt(char *val) {
    astnode *node = malloc(sizeof(astnode));
    node->type = STRING;
    node->str = val;
    return node;
}
astnode *make_bool(int val) {
    astnode *node = malloc(sizeof(astnode));
    node->type = (val) ? YES : NO;
    node->bool = val;
    return node;
}
astnode *make_action(int argc, struct astnode **args, char *name, tokentype retval) {
    astnode *node = malloc(sizeof(astnode));
    node->type = ACTION;
    node->action.argc = argc;
    node->action.args = args;
    node->action.name = name;
    node->action.retval = retval;
    return node;
}
void debug(const char *funcname) {printf("[%s] Do you see this?\n", funcname);}
void parse(tokenslist *tokens) {
    printf("[PARSE] Parsing...\n");
    tokentype priority[4][6] = {
        {ACTION, NUMBER, TEXT, LIST, YESNO, -1},
        {IF, OTHERWISE, REPEAT, FOR, UNTIL, -1},
        {GIVE, -1},
        {-1}
    };
    for (int i = 0; i < tokens->size && !isnull(tokens->tokens[i]); i++) {
        token *token = &tokens->tokens[i];
        int group = -1;
        for (int p = 0; priority[p][0] != -1; p++) {
            const tokentype *level = priority[p];
            for (int x = 0; level[x] != -1; x++) {
                if (token->type == level[x]) {
                    group = p;
                    break;
                }
            }
            if (group != -1) break;
        }
        if (group == -1) printf("[PARSE] tokentype %s not in any prioroties group\n", nameof(token->type));
        switch (token->type) {
            // TODO: PARSING SETUP DONE (HARD PART)
            // YOU DO THE REST ToT
            default:
                break;
        }
    }
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
    add(tokens, null); 
    printf("[MAIN] Finished lexing, found tokens:\n");
    for (int i = 0; i < tokens->size; i++) {
        if (isnull(tokens->tokens[i])) break;
        print_token(tokens->tokens[i]);
    }
    printf("[MAIN] Current tokens capacity: %d, current tokens array size: %d\n", (int)tokens->cap, tokens->size);
    parse(tokens);
    return 0;
}
