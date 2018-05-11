/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Abd El Rahman
 *
 * Created on May 11, 2018, 1:28 PM
 */

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<assert.h>

#define MAX_D 256  // Buffer Size of Most Arrays

#define LP 10
#define RP 20
#define OPERATOR 30
#define OPERAND 40

#define LPP 0       // Left parentheses precedence. Minimum of all
#define AP 1        // Addition Subtraction precedence. Minimum among all operator precedence
#define SP AP       // Addition Subtraction precedence. Minimum among all operator precedence
#define MP 2        // Multiplication divisor precedence.
#define DP MP       // Multiplication divisor precedence.
#define REMP 2      // Remainder precedence.
#define NONE 9

/** START OF FUNCTIONS DECLARATION **/
// START OF LINKEDLIST [MAIN-DATASTRUCTURE]

/*
 * 
 */

typedef struct Node {
    char* key;
    char* value;
    struct Node *next;

} Node;

typedef struct LinkedList {
    Node *head;
    Node *tail;

} LinkedList;

char* find(LinkedList *list, char *key);
int size(LinkedList *list);
void init(LinkedList *list);
Node * CreateNode(char* key, char* value);
void addNode(LinkedList *list, char* key, char* value);
char* find(LinkedList *list, char *key);
void update(LinkedList *list, char *key, char *value);
// END OF LINKEDLIST [MAIN-DATASTRUCTURE]

// START OF INFIX TO POSTFIX CONVERTION FUNCTIONS
static char stackToPostfix[MAX_D];
static int top = -1;

void infixtopostfix(); /** POSTFIX CONVERSION FUNCTION **/
int gettype(char); /** TYPE OF EXPRESSION GENERATOR **/
void pushPostfix(char); /** PUSHPOSTFIX FUNCTION **/
char popPostfix(void); /** POPPOSTFIX FUNCTION **/
int getprec(char); /** PRECEDENCE CHECKER FUNCTION **/
// END OF INFIX TO POSTFIX CONVERTION FUNCTIONS

// START OF POSTFIX EVALUATION FUNCTIONS

void die(const char*);
void push(double);
double pop();
char* rpn(char*);

// END OF POSTFIX EVALUATION FUNCTIONS

// START OF VALIDATION FUNCTIONS

int checkName(char*);
char* preparaName(char*);
int checkEqual(char*);
char* extractFirstPart(char*);
char* extractSecondPart(char*);
int ValidateExpression(char*);
char* checkVariables(char*, LinkedList*);
char infix[MAX_D];
char postfix[MAX_D];
char* CHECK_VARIABLES(char*, LinkedList*);
char* preparePost(char*);

// END OF VALIDATION FUNCTIONS


// START OF BASIC FUNCTIONS

char* scan();
char *str_replace(char *orig, char *rep, char *with);
int gettypec(char sym);
// END OF BASIC FUNCTIONS

/** END OF FUNCTIONS DECLARATION **/
int main() {
    // LAUNCHING THE PROGRAM
    LinkedList *data = (LinkedList*) malloc(sizeof (LinkedList));
    init(data);
    char *input;
    printf("Enter a string (or \"end\" to exit): ");
    input = scan();

    while (strcmp(input, "end")) {

        // CHECK FOR '='
        if (!checkEqual(input)) {
            die("Invalid Expression");
            goto getNewInput;
        }
        // SPLIT INTO first & second half
        char *firsthalf = extractFirstPart(input);
        char *secondhalf = strchr(input, '=');
        secondhalf++;
        // REMOVE EXCESS SPACES
        firsthalf = preparaName(firsthalf);
        if (!checkName(firsthalf)) {
            die(str_replace("[variable_Name] is Not A Valid Variable Name", "variable_Name", firsthalf));
            goto getNewInput;
        }
        // VALIDATE VARIABLES
        secondhalf = CHECK_VARIABLES(secondhalf, data);
        if (strpbrk(secondhalf, "?")) {
            die("Error - cannot find the variable");
            goto getNewInput;
        }
        // PREEPARE VALUE
        strcpy(infix, secondhalf);
        // MOST FUNNY MOMENTS - SCIENCE HAS BEEN UNABLE TO EXPLAIN
        //infixtopostfix();
        infixtopostfix();
        // PRINT FINAL RESULT
        char *newArr = preparePost(postfix);
        //double result = rpn(postfix);
        char* result = (char*) malloc(257 * sizeof (char));
        result = rpn(newArr);
        // ADD IT INTO A DATA [LINKEDLIST]- OR - UPDATE IT IF IT'S ALREADY EXIST
        //sprintf(resultToString,"%.2f",result);
        if (strcmp(result, "?") == 0) goto getNewInput;
        if (strcmp(find(data, firsthalf), "?") == 0)
            addNode(data, firsthalf, result);
        else
            update(data, firsthalf, result);
        printf("\n%s = %s\n", firsthalf, result);

        /** NEW CYCLE **/
        // GET INPUT
getNewInput:
        printf("\nEnter a string (or \"end\" to exit): ");
        free(input);
        input = scan();
    }
    return 0;
}

/** START FUNCTIONS IMPLEMENTATION **/

// START OF INFIX TO POSTFIX CONVERTION FUNCTIONS

int gettype(char sym) {
    switch (sym) {
        case '(':
            return (LP);
        case ')':
            return (RP);
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            return (OPERATOR);
        default:
            return (OPERAND);
    }
}

void pushPostfix(char sym) {
    if (top > MAX_D) {
        printf("\nStackToPostfix is full\n");
        exit(0);
    } else
        stackToPostfix[++top] = sym;
}

char popPostfix(void) {
    if (top <= -1) {
        printf("\nStackToPostfix is empty\n");
        exit(0);
    } else
        return (stackToPostfix[top--]);
}

int getprec(char sym) {
    switch (sym) {
        case '(':
            return (LPP);
        case '+':
            return (AP);
        case '-':
            return (SP);
        case '*':
            return (MP);
        case '/':
            return (DP);
        case '%':
            return (REMP);
        default:
            return (NONE);
    }
}

void infixtopostfix() {

    int i, p, l, type, prec;
    char next;
    i = p = 0;
    l = strlen(infix);
    while (i < l) {
        type = gettype(infix[i]);
        switch (type) {
            case LP:
                pushPostfix(infix[i]);
                break;
            case RP:
                while ((next = popPostfix()) != '(') {
                    postfix[p++] = next;
                    postfix[p++] = ' ';
                }
                break;
            case OPERAND:
                postfix[p++] = infix[i];
                break;
            case OPERATOR:
                if (infix[i] == '-' && (i == 0 || gettype(infix[i - 1]) == OPERATOR || gettype(infix[i - 1]) == RP || gettype(infix[i - 1]) == LP))
                    postfix[p++] = infix[i];
                else {
                    prec = getprec(infix[i]);
                    while (top>-1 && prec <= getprec(stackToPostfix[top])) {
                        postfix[p++] = popPostfix();
                        postfix[p++] = ' ';
                    }
                    pushPostfix(infix[i]);
                }
                break;
        }
        if (type != gettype(infix[i + 1]) && infix[i] != '-')
            postfix[p++] = ' ';
        i++;
    }
    while (top>-1) {
        postfix[p++] = ' ';
        postfix[p++] = popPostfix();
    }
    postfix[p] = '\0';

    //printf("[%s]",postfix);


}
// END OF INFIX TO POSTFIX CONVERTION FUNCTIONS



// START OF POSTFIX EVALUATION FUNCTIONS
double stackToValue[MAX_D];
int depth;

void die(const char *msg) {
    fprintf(stderr, "\n%s\n", msg);
}

void push(double v) {
    if (depth >= MAX_D) die("stackToValue overflow\n");
    stackToValue[depth++] = v;
}

double pop() {
    if (!depth) die("stackToValue underflow\n");
    return stackToValue[--depth];
}

char* rpn(char *s) {
    double a, b;
    int i;
    char *e, *w = " \t\n\r\f";

    for (s = strtok(s, w); s; s = strtok(0, w)) {
        a = strtod(s, &e);
        if (e > s) /*printf(" :"),*/ push(a);
#define binop(x) /*printf("%c:", *s),*/ b = pop(), a = pop(), push(x)
        else if (*s == '+') binop(a + b);
        else if (*s == '-') binop(a - b);
        else if (*s == '*') binop(a * b);
        else if (*s == '/') binop(a / b);
            //else if (*s == '^')	binop(pow(a, b));
#undef binop
        else {
            fprintf(stderr, "'%c': ", *s);
            die("unknown oeprator\n");
            return "?";
        } // '0' 0+
        /*for (i = depth; i-- || 0 * putchar('\n'); )
            printf(" %g", stackToValue[i]);*/
    }

    if (depth != 1) {
        die("stackToValue leftover\n");
        return "?";
    }

    double result = pop();
    //printf("\n%f :",result);
    char* resultToString = (char*) malloc(257 * sizeof (char));
    sprintf(resultToString, "%.2f", result);

    while (top>-1)
        pop();
    return resultToString;
}

// END OF POSTFIX EVALUATION FUNCTIONS




// START OF VALIDATION FUNCTIONS

int checkName(char* Name) {
    if (strlen(Name) == 0)
        return 0;
    if (!((Name[0] >= 'A' && Name[0] <= 'Z') || (Name[0] >= 'a' && Name[0] <= 'z')))
        return 0;
    int i;
    for (i = 1; i < strlen(Name); i++)
        if (!((Name[i] >= 'A' && Name[i] <= 'Z') || (Name[i] >= 'a' && Name[i] <= 'z') || (Name[i] >= '0' && Name[i] <= '9') || Name[i] == '_'))
            return 0;
    return 1;
}

char* preparaName(char *Name) {
    // CONVERT " ABC   " -> "ABC"
    // CONVERT "  AB C " -> "AB C"

    int i;
    for (i = strlen(Name) - 1; i > 0; i--) // REMOVE SPACES AFTER
        if (Name[i] == ' ')
            Name[i] = '\0';
        else
            break;

    while (Name[0] == ' ') // REMOVE SPACE BEFORE
        Name++;

    return Name;
}

int checkEqual(char *Exp) {
    int counter = 0;
    int i;
    if (Exp[0] == '=' || Exp[strlen(Exp) - 1] == '=')
        return 0;
    for (i = 1; i < strlen(Exp); i++)
        if (Exp[i] == '=')
            counter++;

    if (counter != 1)
        return 0;

    return 1;
}

char* extractFirstPart(char *Exp) {
    int i;
    char* Key = malloc(1);

    for (i = 0; i < strlen(Exp); i++)
        if (Exp[i] == '=')
            break;
        else {
            Key = realloc(Key, sizeof (Key) + 1);
            Key[i] = Exp[i];
        }
    Key[i] = '\0';

    return Key;
}

char* CHECK_VARIABLES(char* exp, LinkedList* list) {

    char *newExp = malloc(256 * sizeof (char));
    char *variable = malloc(32 * sizeof (char));
    int i, j, k;

    i = j = k = 0;
    while (i < strlen(exp)) {
        if ((exp[i] >= '0' && exp[i] <= '9') || gettypec(exp[i]) || exp[i] == ' ')
            newExp[j++] = exp[i++];
        else {
            variable = malloc(32 * sizeof (char));
            while (!gettypec(exp[i]) && exp[i] != '\0')
                variable[k++] = exp[i++];
            variable[k] = '\0';
            variable = preparaName(variable);
            variable = strcpy(variable, find(list, variable));
            for (k = 0; k < strlen(variable);)
                newExp[j++] = variable[k++];
            k = 0;
            free(variable);
        }
    }
    newExp[j] = '\0';
    return newExp;

}
// END OF VALIDATION FUNCTIONS

// START OF BASIC FUNCTIONS

char* scan() {

    int i = 0;
    char *input = malloc(1);
    char reader;
    while ((reader = getchar()) != '\n') {
        input[i++] = reader;
        input = realloc(input, i + 1);
    }
    input[i] = '\0';
    return input;
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins; // the next insert point
    char *tmp; // varies
    int len_rep; // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count; // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

int gettypec(char sym) {
    switch (sym) {

        case '(':
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case '.':
            return (1);
        default:
            return (0);
    }
}

char* preparePost(char* post) {

    char* newArray = malloc(sizeof (post));
    int i, j;
    for (i = j = 0; i < strlen(post);) {
        if (post[i] == '-' && post[i + 1] == '-')
            i += 2;
        newArray[j++] = post[i++];
    }
    newArray[j] = '\0';

    //printf("\n[%s]",newArray);
    return newArray;

}
// END OF BASIC FUNCTIONS


// START OF LINKEDLIST [MAIN-DATASTRUCTURE]

int size(LinkedList *list) {

    int elements = 0;
    Node *node = list->head;
    while (node) {
        elements++;
        node = node->next;
    }
    return elements;
}

void init(LinkedList *list) {
    list->head = list->tail = NULL;
}

Node * CreateNode(char* key, char* value) {

    Node* n;
    n = (Node*) malloc(sizeof (Node));
    n->key = key;
    n->value = value;
    n->next = NULL;
    return n;
} // END-Create-

void addNode(LinkedList *list, char* key, char* value) {

    Node *n = CreateNode(key, value);
    if (list->head == NULL) {
        list->head = list->tail = n;
    } else {
        list->tail->next = n;
        list->tail = n;
    }
} // END-ADD-

char* find(LinkedList *list, char *key) {

    Node *node = list->head;
    while (node) {
        if (strcmp(key, node->key) == 0)
            return node->value;
        node = node->next;
    }
    return "?";
}

void update(LinkedList *list, char *key, char *value) {

    Node *node = list->head;

    while (node) {
        if (strcmp(key, node->key) == 0)
            node->value = value;
        node = node->next;
    }
}
// END OF LINKEDLIST [MAIN-DATASTRUCTURE]

/** END FUNCTIONS IMPLEMENTATION **/




