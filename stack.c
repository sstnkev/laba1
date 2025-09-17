#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

// Функция для создания стека с заданной емкостью
Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack)); // Выделяем память под структуру стека
    stack->capacity = capacity;                   // Устанавливаем максимальную емкость
    stack->top = -1;                              // Инициализируем индекс верхнего элемента (-1 означает, что стек пуст)
    stack->data = (double*)malloc(stack->capacity * sizeof(double)); // Выделяем память под массив для элементов стека
    return stack; // Возвращаем указатель на созданный стек
}

// Функция для проверки, пуст ли стек
int isEmpty(Stack* stack) {
    return stack->top == -1; // Если индекс верхнего элемента равен -1, стек пуст
}

// Функция для добавления элемента в стек
void push(Stack* stack, double value) {
    stack->data[++stack->top] = value; // Увеличиваем индекс верхнего элемента и добавляем значение в этот индекс
}

// Функция для удаления элемента из стека и возвращения его значения
double pop(Stack* stack, gpointer user_data) {
    if (isEmpty(stack)) { // Проверяем, пуст ли стек
        /*printf("Ошибка: Попытка извлечения из пустого стека\n"); // Выводим сообщение об ошибке
        exit(EXIT_FAILURE); // Завершаем выполнение программы с ошибкой*/
        GtkWidget *entry = GTK_WIDGET(user_data);
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }
    return stack->data[stack->top--]; // Возвращаем верхний элемент и уменьшаем индекс верхнего элемента
}

// Функция для получения верхнего элемента стека без его удаления
double peek(Stack* stack, gpointer user_data) {
    if (isEmpty(stack)) { // Проверяем, пуст ли стек
        /*printf("Ошибка: Попытка просмотра верхнего элемента пустого стека\n"); // Выводим сообщение об ошибке
        exit(EXIT_FAILURE); // Завершаем выполнение программы с ошибкой*/
        GtkWidget *entry = GTK_WIDGET(user_data);
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }
    return stack->data[stack->top]; // Возвращаем верхний элемент без изменения индекса
}
