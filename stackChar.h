#pragma once
#include <gtk-3.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// Создание структуры для представления стека символов
typedef struct {
    int capacity;   // Максимальная емкость стека
    int top;        // Индекс верхнего элемента в стеке
    char **data;    // Указатель на массив, который будет содержать элементы стека
} CharStack;

CharStack* createCharStack(int capacity);

int isCharStackEmpty(CharStack* stack);

void pushChar(CharStack* stack, char *value);

char *popChar(CharStack* stack, gpointer user_data);

char *peekChar(CharStack* stack, gpointer user_data);

void pushChar(CharStack* stack, char* value);

int isOperator(const char *ch);

int isFunction(const char* token);

int precedence(const char* op);

void infixToPostfix(char* tokens[], int size, char** postfix, int* postfixSize, gpointer user_data);