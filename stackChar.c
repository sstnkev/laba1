#include "stackChar.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Функция для создания стека для символов с заданной емкостью
CharStack *createCharStack(int capacity) {
    CharStack *stack = (CharStack *) malloc(sizeof(CharStack)); // Выделяем память под структуру стека
    stack->capacity = capacity;                                 // Устанавливаем максимальную емкость
    stack->top = -1;                                            // Инициализируем индекс верхнего элемента (-1 означает, что стек пуст)
    stack->data = (char **) malloc(stack->capacity * sizeof(char *)); // Выделяем память под массив для элементов стека
    return stack; // Возвращаем указатель на созданный стек
}

// Функция для проверки, пуст ли стек для символов
int isCharStackEmpty(CharStack *stack) {
    return stack->top == -1; // Если индекс верхнего элемента равен -1, стек пуст
}

// Функция для добавления элемента в стек для символов
void pushChar(CharStack *stack, char *value) {
    stack->data[++stack->top] = value; // Увеличиваем индекс верхнего элемента и добавляем значение в этот индекс
}

// Функция для удаления элемента из стека для символов и возвращения его значения
char *popChar(CharStack *stack, gpointer user_data) {
    if (isCharStackEmpty(stack)) { // Проверяем, пуст ли стек
        /*printf("Ошибка: Попытка извлечения из пустого стека\n"); // Выводим сообщение об ошибке
        exit(EXIT_FAILURE); // Завершаем выполнение программы с ошибкой*/
        GtkWidget *entry = GTK_WIDGET(user_data);
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }
    return stack->data[stack->top--]; // Возвращаем верхний элемент и уменьшаем индекс верхнего элемента
}

// Функция для получения верхнего элемента стека для символов без его удаления
char *peekChar(CharStack *stack, gpointer user_data) {
    if (isCharStackEmpty(stack)) { // Проверяем, пуст ли стек
        /*printf("Ошибка: Попытка просмотра верхнего элемента пустого стека\n"); // Выводим сообщение об ошибке
        exit(EXIT_FAILURE); // Завершаем выполнение программы с ошибкой*/
        GtkWidget *entry = GTK_WIDGET(user_data);
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }
    return stack->data[stack->top]; // Возвращаем верхний элемент без изменения индекса
}

// Функция для проверки, является ли строка оператором
int isOperator(const char *token) {
    return strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
           strcmp(token, "*") == 0 || strcmp(token, "/") == 0 ||
           strcmp(token, "^") == 0; // Возвращаем истину, если строка совпадает с любым из операторов
}

// Функция для проверки, является ли строка функцией
int isFunction(const char *token) {
    return strcmp(token, "sin") == 0 || strcmp(token, "cos") == 0 ||
           strcmp(token, "tan") == 0 || strcmp(token, "log") == 0 ||
           strcmp(token, "asin") == 0 || strcmp(token, "acos") == 0 ||
           strcmp(token, "atan") == 0 || strcmp(token, "ln") == 0 ||
           strcmp(token, "√") == 0 || strcmp(token, "exp") == 0 ||
           strcmp(token, "∛") == 0 || strcmp(token, "!") == 0 ||
           strcmp(token, "abs") == 0; // Возвращаем истину, если строка совпадает с любым из функций
}

// Функция для определения приоритета операторов
int precedence(const char *op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    return 0; // Возвращаем значение приоритета для каждого оператора
}

// Функция для конвертации инфиксной записи в постфиксную
void infixToPostfix(char *tokens[], int size, char **postfix, int *postfixSize, gpointer user_data) {
    CharStack *stack = createCharStack(size); // Создаем стек для символов с размером, равным количеству токенов
    *postfixSize = 0; // Инициализируем размер постфиксной записи

    for (int i = 0; i < size; ++i) {
        if (strcmp(tokens[i], "e") == 0) { // Замена символа 'e' на его числовое значение
            postfix[(*postfixSize)++] = "2.71828182846";
        } else if (strcmp(tokens[i], "π") == 0) { // Замена символа 'π' на его числовое значение
            postfix[(*postfixSize)++] = "3.14159265359";
        } else if (isdigit(tokens[i][0]) || (tokens[i][0] == '-' && isdigit(tokens[i][1]))) {
            // Если токен является числом или отрицательным числом, добавляем его в постфиксную запись
            postfix[(*postfixSize)++] = tokens[i];
        } else if (strcmp(tokens[i], "!") == 0) {
            // Если токен является факториалом, добавляем его в постфиксную запись
            postfix[(*postfixSize)++] = "!";
        } else if (isFunction(tokens[i])) {
            // Если токен является функцией, помещаем его в стек
            pushChar(stack, tokens[i]);
        } else if (strcmp(tokens[i], "(") == 0) {
            // Если токен является открывающей скобкой, помещаем его в стек
            pushChar(stack, tokens[i]);
        } else if (strcmp(tokens[i], ")") == 0) {
            // Если токен является закрывающей скобкой, извлекаем элементы из стека до открывающей скобки
            while (!isCharStackEmpty(stack) && strcmp(peekChar(stack, user_data), "(") != 0) {
                postfix[(*postfixSize)++] = popChar(stack, user_data);
            }
            popChar(stack, user_data); // Удаляем открывающую скобку из стека
            if (!isCharStackEmpty(stack) && isFunction(peekChar(stack, user_data))) {
                // Если верхний элемент стека является функцией, перемещаем его в постфиксную запись
                postfix[(*postfixSize)++] = popChar(stack, user_data);
            }
        } else if (isOperator(tokens[i])) {
            // Если токен является оператором, перемещаем из стека в постфиксную запись все операторы с большим или равным приоритетом
            while (!isCharStackEmpty(stack) && precedence(peekChar(stack,user_data)) >= precedence(tokens[i])) {
                postfix[(*postfixSize)++] = popChar(stack, user_data);
            }
            pushChar(stack, tokens[i]); // Помещаем текущий оператор в стек
        }
    }

    // Перемещаем оставшиеся операторы из стека в постфиксную запись
    while (!isCharStackEmpty(stack)) {
        postfix[(*postfixSize)++] = popChar(stack, user_data);
    }

    // Освобождаем память, выделенную под стек
    free(stack->data);
    free(stack);
}
