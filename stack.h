#pragma once
#include <gtk-3.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Создание структуры для представления стека
typedef struct {
    int capacity;   // Максимальная емкость стека
    int top;        // Индекс верхнего элемента в стеке
    double* data;   // Указатель на массив, который будет содержать элементы стека
} Stack;

Stack* createStack(int capacity);

int isEmpty(Stack* stack);

void push(Stack* stack, double value);

double pop(Stack* stack, gpointer user_data);

double peek(Stack* stack, gpointer user_data);