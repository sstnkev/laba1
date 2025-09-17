#include <gtk-3.0/gtk/gtk.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "stack.h"
#include "stackChar.h"

#define MAX_TOKENS 100

const GtkListStore *list_store; // Global for entering any event

void add_to_table(char *first, double second) { // количество строк равно 7
    GtkTreeIter iter;

    FILE *file;

    gint num_rows = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(list_store), NULL);
    if (num_rows == 7) { // max size
        file = fopen("../operations.txt", "r");
        char **temp_strings = (char **) malloc(12 * sizeof(char *));
        if (temp_strings == NULL) {
            printf("Ошибка при выделении памяти.\n");
            fclose(file);
            return;
        }

        // Чтение содержимого файла во временный массив строк
        char buffer[MAX_TOKENS];

        fgets(buffer, sizeof(buffer), file);
        fgets(buffer, sizeof(buffer), file);

        int j;
        for (j = 0; j < 12 && fgets(buffer, sizeof(buffer), file) != NULL; ++j) {
            temp_strings[j] = strdup(buffer);
        }

        fclose(file);

        file = fopen("../operations.txt", "w");

        // Запись содержимого временного массива строк обратно в файл
        for (int i = 0; i < j; ++i) {
            fprintf(file, "%s", temp_strings[i]);
            free(temp_strings[i]);
        }
        free(temp_strings);

        fclose(file);

        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter)) {
            gtk_list_store_remove(list_store, &iter);
        }
    }

    file = fopen("../operations.txt", "a");
    // Запись новой строки в файл
    fprintf(file, "%s\n", first);
    fprintf(file, "%lf\n", second);
    fclose(file);

    // Добавление новой строки в GtkListStore
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set(list_store, &iter, 0, first, 1, second, -1); // в 0 колонку 1, в 1 колонку 2, колонок больше нет
}

void set_start() { // функция вызывается для инициализации или обновления таблицы данных при запуске
    FILE *file = fopen("../operations.txt", "r");
    char **temp_strings = (char **) malloc(14 * sizeof(char *));
    char buffer[MAX_TOKENS];
    int j;
    for (j = 0; j < 14 && fgets(buffer, sizeof(buffer), file) != NULL; ++j) {
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        temp_strings[j] = strdup(buffer);
    }

    GtkTreeIter iter; //новую переменную iter,используемую для хранения итератора, указывающего на строку в таблице GtkListStore

    for (int i = 0; i < j; i += 2) {
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter, 0, temp_strings[i], 1, atof(temp_strings[i + 1]), -1);
        free(temp_strings[i]);
        free(temp_strings[i + 1]);
    }
    free(temp_strings);
}


double factorial(int n) {
    if (n == 0 || n == 1) return 1;
    double result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

void free_tokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; ++i) {
        free(tokens[i]); // Освобождаем память, выделенную под каждый токен
    }
    free(tokens); // Освобождаем память, выделенную под массив указателей на токены
}

bool is_number(const char *str) {
    // Если строка пуста, она не является числом
    if (str == NULL || *str == '\0') {
        return false;
    }

    // Проверяем каждый символ строки
    while (*str) {
        if (!isdigit(*str) && *str != ',') {
            return false; // Если символ не является цифрой, строка не является числом
        }
        if (*str == '-' && !isdigit(*(str + 1))) return false;
        ++str; // Переходим к следующему символу
    }

    return true; // Если все символы - цифры, строка является числом
}

//
bool isValidExpression(char **tokens) {
    int i = -1;
    while (tokens[++i] != NULL) {
        if (!i) {
            if (!is_number(tokens[i])) {
                if (!((isFunction(tokens[i]) || strcmp(tokens[i], "(") == 0) && strcmp(tokens[i], "!") != 0)) {
                    return false;
                }
            }
        } else {
            if (strcmp(tokens[i], "(") == 0) {
                if (tokens[i + 1] == NULL) {
                    return false;
                } else if (strcmp(tokens[i + 1], ")") == 0) {
                    return false;
                }
            } else if (strcmp(tokens[i], "!") == 0) {
                if (strcmp(tokens[i - 1], ")") == 0) {
                    return false;
                } else if (!is_number(tokens[i - 1])) {
                    return false;
                }
            } else if (strcmp(tokens[i], ")") == 0) {
                if (!(is_number(tokens[i - 1]) || (strcmp(tokens[i - 1], "!")) == 0 ||
                      (strcmp(tokens[i - 1], ")")) == 0)) {
                    return false;
                }
            } else if (is_number(tokens[i]) || strcmp(tokens[i], "e") == 0 || strcmp(tokens[i], "π") == 0) {
                if (!(precedence(tokens[i - 1]) || (isFunction(tokens[i - 1]) && strcmp(tokens[i - 1], "!") == 0) ||
                      (strcmp(tokens[i - 1], "(") == 0))) {
                    return false;
                }
            } else if (isFunction(tokens[i])) {
                if (!(precedence(tokens[i - 1]) || strcmp(tokens[i - 1], "(") == 0)) {
                    return false;
                }
            } else if (precedence(tokens[i])) {
                if (!(is_number(tokens[i - 1]) || strcmp(tokens[i - 1], ")") == 0 || strcmp(tokens[i - 1], "!") == 0)) {
                    return false;
                }
            }
        }
    }

    return true;
}

char **split_string(const char *input_string, gpointer user_data) {// разбиение строки на элементы
    char **tokens = (char **) malloc(MAX_TOKENS * sizeof(char *));
    if (tokens == NULL) {
        GtkWidget *entry = GTK_WIDGET(user_data);
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }
    int token_count = 0;

    char *input_copy = strdup(input_string);
    if (input_copy == NULL) {
        GtkWidget *entry = GTK_WIDGET(user_data);
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }
    char *token = strtok(input_copy, " ");

    while (token != NULL && token_count < MAX_TOKENS) {
        tokens[token_count++] = strdup(token);
        token = strtok(NULL, " ");
    }

    tokens[token_count] = NULL;

    free(input_copy); // Освобождаем память, выделенную под копию входной строки

    return tokens;
}

// Функция для вычисления значения выражения в обратной польской записи
double evaluateRPN(char *tokens[], int size, gpointer user_data) {
    Stack *stack = createStack(size);

    for (int i = 0; i < size; ++i) {
        if (isOperator(tokens[i])) {
            double b = pop(stack, user_data);
            double a = pop(stack, user_data);
            if (strcmp(tokens[i], "+") == 0) {
                push(stack, a + b);
            } else if (strcmp(tokens[i], "-") == 0) {
                push(stack, a - b);
            } else if (strcmp(tokens[i], "*") == 0) {
                push(stack, a * b);
            } else if (strcmp(tokens[i], "/") == 0) {
                if (b == 0) {
                    GtkWidget *entry = GTK_WIDGET(user_data);
                    gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
                }
                push(stack, a / b);
            } else if (strcmp(tokens[i], "^") == 0) {
                push(stack, pow(a, b));
            }
        } else if (isFunction(tokens[i])) {
            double a = pop(stack, user_data);
            if (strcmp(tokens[i], "sin") == 0) {
                push(stack, sin(a));
            } else if (strcmp(tokens[i], "cos") == 0) {
                push(stack, cos(a));
            } else if (strcmp(tokens[i], "tan") == 0) {
                push(stack, tan(a));
            } else if (strcmp(tokens[i], "asin") == 0) {
                push(stack, asin(a));
            } else if (strcmp(tokens[i], "acos") == 0) {
                push(stack, acos(a));
            } else if (strcmp(tokens[i], "atan") == 0) {
                push(stack, atan(a));
            } else if (strcmp(tokens[i], "log") == 0) {
                if (a <= 0) {
                    GtkWidget *entry = GTK_WIDGET(user_data);
                    gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
                }
                push(stack, log10(a));
            } else if (strcmp(tokens[i], "ln") == 0) {
                if (a <= 0) {
                    GtkWidget *entry = GTK_WIDGET(user_data);
                    gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
                }
                push(stack, log(a));
            } else if (strcmp(tokens[i], "√") == 0) {
                if (a < 0) {
                    GtkWidget *entry = GTK_WIDGET(user_data);
                    gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
                }
                push(stack, sqrt(a));
            } else if (strcmp(tokens[i], "∛") == 0) {
                push(stack, cbrt(a));
            } else if (strcmp(tokens[i], "exp") == 0) {
                push(stack, exp(a));
            } else if (strcmp(tokens[i], "abs") == 0) {
                push(stack, fabs(a));
            } else if (strcmp(tokens[i], "!") == 0) {
                if (a != (int) a || a < 0) {
                    GtkWidget *entry = GTK_WIDGET(user_data);
                    gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
                }
                push(stack, factorial((int) a));
            }
        } else {
            // Если токен не оператор и не функция, то это число
            push(stack, atof(tokens[i]));
        }
    }

    if (stack->top != 0) {
        GtkWidget *entry = GTK_WIDGET(user_data);
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }

    double result = pop(stack, user_data);
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (strcmp(text, "ERROR") != 0) {
        free(stack->data);
    }
    free(stack);
    return result;
}

void on_equal_clicked(GtkWidget *button, gpointer user_data) { // функция при клике на кнопку равно
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
    char **tokens = split_string(text, user_data);

    if (isValidExpression(tokens)) {
        char **new_tokens = (char **) malloc(MAX_TOKENS * sizeof(char *));
        if (new_tokens == NULL) {
            gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
        }

        int size = 0;
        int open_brackets = 0;
        int close_brackets = 0;
        int sizePostfix = 0;

        // Определение размера массива tokens и подсчет скобок
        while (tokens[size] != NULL) {
            if (strcmp(tokens[size], "(") == 0) ++open_brackets;
            if (strcmp(tokens[size], ")") == 0) ++close_brackets;
            size++;
        }

        // Обработка ошибок в скобках
        if (close_brackets > open_brackets) {
            gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
        }

        // Добавление недостающих закрывающих скобок
        int total_tokens = size + (open_brackets - close_brackets);
        if (total_tokens >= MAX_TOKENS) {
            gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
        }

        for (int i = size; i < total_tokens; ++i) {
            tokens[i] = strdup(")");
            if (tokens[i] == NULL) {
                gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
            }
        }
        tokens[total_tokens] = NULL;

        // Конвертация инфиксного выражения в постфиксное
        infixToPostfix(tokens, total_tokens, new_tokens, &sizePostfix, user_data);

        // Вычисление результата
        double result = evaluateRPN(new_tokens, sizePostfix, user_data);
        if (strcmp(text, "ERROR") != 0) {
            char *temp = (char *) calloc((strlen(text)), sizeof(char));
            int brackets_counter = 0;
            for (int i = 0; i < size - 1; ++i) {
                if (strcmp(tokens[i], "(") == 0) { ++brackets_counter; }
                else if (strcmp(tokens[i], ")") == 0) { --brackets_counter; }
                strcat(temp, tokens[i]);
            }
            for (int i = 0; i < brackets_counter; ++i) {
                strcat(temp, ")");
            }
            add_to_table(temp, result);

            gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%g", result));
            free(temp);
        }
    } else {
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }

    // Освобождаем память, выделенную под токены
    free_tokens(tokens);
}


void on_operator_clicked(GtkWidget *button, gpointer user_data) { // обработчик событий при клике
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *text = gtk_button_get_label(GTK_BUTTON(button));
    const char *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(entry_text, text, NULL));
}

void on_dot_clicked(GtkWidget *, gpointer user_data) { // клик по точке
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    char **tokens = split_string(entry_text, user_data);
    int i = 0;
    while (tokens[i] != NULL) {
        i++;
    }
    if (tokens[0] == NULL) {
        gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(entry_text, "0", NULL));
    } else if (!is_number(tokens[i - 1])) {
        gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(entry_text, "0,", NULL));
    } else {
        gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(entry_text, ",", NULL));
    }
    free_tokens(tokens);
}

int is_button_function(const char *token) {
    return strcmp(token, " sin ") == 0 || strcmp(token, " cos ") == 0 ||
           strcmp(token, " tan ") == 0 || strcmp(token, " log ") == 0 ||
           strcmp(token, " asin ") == 0 || strcmp(token, " acos ") == 0 ||
           strcmp(token, " atan ") == 0 || strcmp(token, " ln ") == 0 ||
           strcmp(token, " √ ") == 0 || strcmp(token, " exp ") == 0 ||
           strcmp(token, " ∛ ") == 0 || strcmp(token, " abs ") == 0;
}

void on_button_clicked(GtkWidget *button, gpointer user_data) { // добавление ( к тригонометрическим функциям
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    const char *text = gtk_button_get_label(GTK_BUTTON(button));
    char *check_text = (char *) malloc(6 * sizeof(char));
    strncpy(check_text, entry_text, 5);
    check_text[5] = '\0';
    if (strcmp(check_text, "ERROR") == 0 && strcmp(text, " = ") != 0) {
        gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(text, NULL));
        if (is_button_function(text)) {
            const char *new_entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
            gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(new_entry_text, "( ", NULL));
        }
    }
}

void on_operation_clicked(GtkWidget *button, gpointer user_data) { // обратобтчик событий на математические функции
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *text = gtk_button_get_label(GTK_BUTTON(button));
    const char *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    char **tokens = split_string(entry_text, user_data);
    int i = 0;
    while (tokens[i] != NULL) {
        i++;
    }
    const char *new_text = text;
    if (tokens[0] != NULL) {
        if ((strcmp(tokens[i - 1], "*") == 0) + (strcmp(new_text, " * ") == 0) +
            (strcmp(new_text, " ! ") == 0) +
            (strcmp(tokens[i - 1], "-") == 0) + (strcmp(new_text, " - ") == 0) +
            (strcmp(tokens[i - 1], "/") == 0) + (strcmp(new_text, " / ") == 0) +
            (strcmp(tokens[i - 1], "+") == 0) + (strcmp(new_text, " + ") == 0) +
            (strcmp(new_text, " ^ ") == 0) >= 2) {
            if (strcmp(tokens[i - 1], "-") != 0 && strcmp(new_text, " - ") == 0)
                new_text = " -";
            else
                new_text = "";
        }
        if (strcmp(tokens[i - 1], "(") == 0 && strcmp(new_text, " - ") == 0)
            new_text = " -";
        if (i > 1)
            if (strcmp(tokens[i - 1], "(") == 0 && strcmp(tokens[i - 2], "^") == 0 && strcmp(new_text, " ^ ") == 0)
                new_text = "";
    } else {
        if (strcmp(new_text, " - ") == 0)
            new_text = " -";
        else
            new_text = "";
    }
    gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(entry_text, new_text, NULL));
    if (strcmp(new_text, " ^ ") == 0) {
        const char *new_entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
        gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(new_entry_text, "( ", NULL));
    }
    free_tokens(tokens);
}

void on_function_clicked(GtkWidget *button, gpointer user_data) { // добавление ( к тригонометрическим функциям
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *text = gtk_button_get_label(GTK_BUTTON(button));
    const char *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    char *text_to_label = (char *) calloc(7, sizeof(char));
    if (text_to_label == NULL) {
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    }
    strcat(text_to_label, text);
    strcat(text_to_label, "( ");
    gtk_entry_set_text(GTK_ENTRY(entry), g_strconcat(entry_text, text_to_label, NULL));
    free(text_to_label);
}

void on_clear_clicked(GtkWidget *, gpointer user_data) {
    GtkWidget *entry = GTK_WIDGET(user_data);
    gtk_entry_set_text(GTK_ENTRY(entry), "");
}

void on_clear_history_button(GtkWidget *, gpointer user_data) {
    FILE *file = fopen("../operations.txt", "w");
    fclose(file);

    GtkTreeIter iter;

    for (int i = 0; i < 7; ++i) {
        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter)) {
            gtk_list_store_remove(list_store, &iter);
        }
    }
}

char *decimal_to_binary(int n) {
    if (n < 0) {
        n *= -1;
        char *negative_sign = "-";
        char *binary_str = decimal_to_binary(n);
        size_t len_negative_sign = strlen(negative_sign);
        size_t len_binary_str = strlen(binary_str);
        char *result = (char *) malloc(len_negative_sign + len_binary_str + 1);
        strcpy(result, negative_sign);
        strcat(result, binary_str);
        free(binary_str);
        return result;
    }
    if (n / 2 != 0) {
        char *binary_str = decimal_to_binary(n / 2);
        size_t len_binary_str = strlen(binary_str);
        char *result = (char *) malloc(len_binary_str + 2);
        sprintf(result, "%s%d", binary_str, n % 2);
        free(binary_str);
        return result;
    }
    char *result = (char *) malloc(2);
    sprintf(result, "%d", n % 2);
    return result;
}

char *fraction_to_binary(double fraction, int precision) {
    char *binary_fraction = (char *) malloc(precision + 2);
    strcpy(binary_fraction, ",");

    while (precision--) {
        fraction *= 2;
        int bit = fraction;
        if (bit == 1) {
            fraction -= bit;
            strcat(binary_fraction, "1");
        } else {
            strcat(binary_fraction, "0");
        }
    }
    return binary_fraction;
}

void on_to_binary_button(GtkWidget *, gpointer user_data) {
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (strlen(entry_text) > 15) {
        entry = GTK_WIDGET(user_data);
        gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%s", "ERROR"));
    } else if (is_number(entry_text)) {
        double decimal = atof(entry_text);
        int int_part = (int) decimal;
        double fraction_part = decimal - int_part;

        char *int_binary_str = decimal_to_binary(int_part);
        char *fraction_binary_str = fraction_to_binary(fraction_part, 7);

        size_t len_int_binary_str = strlen(int_binary_str);
        size_t len_fraction_binary_str = strlen(fraction_binary_str);

        char *binary_str = (char *) malloc(len_int_binary_str + len_fraction_binary_str + 1);
        strcpy(binary_str, int_binary_str);
        strcat(binary_str, fraction_binary_str);

        gtk_entry_set_text(GTK_ENTRY(entry), binary_str);

        free(int_binary_str);
        free(fraction_binary_str);
        free(binary_str);
    } else {
        gtk_entry_set_text(GTK_ENTRY(entry), "ERROR");
    }
}

void create_window(GtkWidget *window) {
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 7, 1);

    const char *buttons[5][7] = {
            {" sin ",  " cos ",  " tan ",  "C",   " ( ", " ) ", " / "},
            {" asin ", " acos ", " atan ", "7",   "8",   "9",   " * "},
            {" ln ",   " log ",  " abs ",  "4",   "5",   "6",   " - "},
            {" √ ",    " ∛ ",    " ^ ",    "1",   "2",   "3",   " + "},
            {" exp ",  " e ",    " π ",    " ! ", "0",   ",",   " = "},
    };

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 7; j++) {
            GtkWidget *button = gtk_button_new_with_label(buttons[i][j]);
            gtk_widget_set_name(button, "custom-button");
            gtk_grid_attach(GTK_GRID(grid), button, j, i + 1, 1, 1);

            if (strlen(buttons[i][j]) > 0) {
                if (strcmp(buttons[i][j], " * ") == 0 ||
                    strcmp(buttons[i][j], " - ") == 0 ||
                    strcmp(buttons[i][j], " / ") == 0 ||
                    strcmp(buttons[i][j], " + ") == 0 ||
                    strcmp(buttons[i][j], " ! ") == 0 ||
                    strcmp(buttons[i][j], " ^ ") == 0) {
                    g_signal_connect(button, "clicked", G_CALLBACK(on_operation_clicked), entry);
                } else if (strcmp(buttons[i][j], " ln ") == 0 ||
                           strcmp(buttons[i][j], " log ") == 0 ||
                           strcmp(buttons[i][j], " cos ") == 0 ||
                           strcmp(buttons[i][j], " sin ") == 0 ||
                           strcmp(buttons[i][j], " tan ") == 0 ||
                           strcmp(buttons[i][j], " acos ") == 0 ||
                           strcmp(buttons[i][j], " asin ") == 0 ||
                           strcmp(buttons[i][j], " abs ") == 0 ||
                           strcmp(buttons[i][j], " exp ") == 0 ||
                           strcmp(buttons[i][j], " atan ") == 0 ||
                           strcmp(buttons[i][j], " √ ") == 0 ||
                           strcmp(buttons[i][j], " ∛ ") == 0) {
                    g_signal_connect(button, "clicked", G_CALLBACK(on_function_clicked), entry);
                } else if (strcmp(buttons[i][j], ",") == 0) {
                    g_signal_connect(button, "clicked", G_CALLBACK(on_dot_clicked), entry);
                } else {
                    g_signal_connect(button, "clicked", G_CALLBACK(on_operator_clicked), entry);
                }
            }

            if (strcmp(buttons[i][j], " = ") == 0) {
                g_signal_connect(button, "clicked", G_CALLBACK(on_equal_clicked), entry);
            }

            if (strcmp(buttons[i][j], "C") == 0) {
                g_signal_connect(button, "clicked", G_CALLBACK(on_clear_clicked), entry);
            }
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
        }
    }

    list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_DOUBLE);
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));

    // Создаем столбец для строковых значений
    GtkCellRenderer *renderer_text = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column_text = gtk_tree_view_column_new_with_attributes("Equation", renderer_text, "text", 0,
                                                                              NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column_text);

    // Создаем столбец для целочисленных значений
    GtkCellRenderer *renderer_number = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column_number = gtk_tree_view_column_new_with_attributes("Answer", renderer_number, "text", 1,
                                                                                NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column_number);

    gtk_grid_attach(GTK_GRID(grid), tree_view, 8, 0, 80, 10);

    GtkWidget *clear_history_button = gtk_button_new_with_label("Clear history");
    gtk_grid_attach(GTK_GRID(grid), clear_history_button, 3, 8, 4, 1);
    g_signal_connect(clear_history_button, "clicked", G_CALLBACK(on_clear_history_button), entry);

    GtkWidget *to_binary_button = gtk_button_new_with_label("To binary");
    gtk_grid_attach(GTK_GRID(grid), to_binary_button, 0, 8, 3, 1);
    g_signal_connect(to_binary_button, "clicked", G_CALLBACK(on_to_binary_button), entry);

    set_start();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Калькулятор");

    create_window(window);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
