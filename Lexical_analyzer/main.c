#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int main(void) {
    // file of read
    FILE *psiFile = fopen("code.psi", "r");
    // file of write
    FILE *lexFile = fopen("code.lex", "w");

    // line of read
    char lines[256];
    // reading piece
    char *piece;
    // pointer of line
    char *pointer_line;

    if (psiFile == 0) {
        printf("File open error !");
        return 1;
    }

    if (lexFile == 0) {
        printf("File open error");
        return 1;
    }

    while (fgets(lines, 256, psiFile)) {
        pointer_line = lines;
        while (*pointer_line) {


            // skip spaces
            for (; isspace(*pointer_line); ++pointer_line) {
            }

            // remove comment line
            if (strncmp(pointer_line, "/*", 2) == 0) {
                int isEnd = 0;
                while (*pointer_line != '\0') {
                    if (strncmp(pointer_line, "*/", 2) == 0) {
                        isEnd = 1;
                        pointer_line += 2;
                        break;
                    }
                    pointer_line++;
                    continue;
                }
                if (!isEnd) {
                    printf("Error: unclosed comment\n");
                    exit(EXIT_FAILURE);
                    return 0;
                }
            }

        // Check all read by file
        switch (*pointer_line) {
            case '\"':
                if (is_string(pointer_line, &piece)) {
                    fprintf(lexFile, "String(%s)\n", piece);
                    pointer_line += strlen(piece) + 2;
                    continue;
                }
                break;
            case '+':
            case '-':
            case '*':
            case '/':
            case ':':
            case '=':
                if (is_operator(pointer_line, &piece)) {
                    fprintf(lexFile, "Opetator(%s)\n", piece);
                    pointer_line += strlen(piece);
                    continue;
                }
                break;
            case '(':
            case '[':
            case '{':
                if (is_bracket(pointer_line, &piece)) {
                    fprintf(lexFile, "%s\n", piece);
                    pointer_line += 1;
                    continue;
                }
                break;
            case ')':
                fprintf(lexFile, "RightPar\n");
                pointer_line += 1;
                continue;
            case ']':
                fprintf(lexFile, "RightSquareBracket\n");
                pointer_line += 1;
                continue;
            case '}':
                fprintf(lexFile, "RightCurlyBracket\n");
                pointer_line += 1;
                continue;
            case ';':
                fprintf(lexFile, "EndOfLine\n");
                pointer_line += 1;
                continue;
            default:
                if (isdigit(*pointer_line)) {
                    // maximum integer size
                    char max_num[10];
                    int count = 0;
                    while (isdigit(*pointer_line)) {
                        max_num[count++] = *pointer_line++;
                    }
                    // check if maximum integer size is 10 digits
                    if (count < 9) {
                        max_num[count] = '\0';
                        fprintf(lexFile, "IntConst(%s)\n", max_num);

                    } else {
                        printf("Error: Integer size must be max 10 digits\n");
                        exit(EXIT_FAILURE);
                    }
                    continue;
                } else if (isalpha(*pointer_line)) {
                    // maximum  id size
                    char words[31];
                    int count = 0;
                    while (isalnum(*pointer_line) || *pointer_line == '_') {
                        if (isalpha(*pointer_line)) {
                            words[count] = tolower(*pointer_line);
                        } else {
                            words[count] = *pointer_line;
                        }
                        count++;
                        // check if maximum identifier size larger than 30 characters
                        if (count >= 30) {
                            printf("Error: Maximum identifier size must be 30 characters\n");
                            exit(EXIT_FAILURE);
                        }
                        pointer_line++;
                    }
                    words[count] = '\0';
                    processWord(words, lexFile);
                    continue;
                }
                break;
                }

            }

        }
        // opened files are closing
        fclose(psiFile);
        fclose(lexFile);

        return 0;
}

// checks if it is a string
int is_string(char *ch, char **pointer) {
    if (ch[0] == '"') {
        char words[256];
        int string_ended = 0;
        int i = 0;

        for (int j = 1; ch[j] != '\0'; j++) {
            if (ch[j] == '"') {
                string_ended = 1;
                break;
            }
            words[i++] = ch[j];
        }

        // take an error if terminate before the file end
        if (!string_ended) {
            printf("Error: string not ended\n");
            exit(EXIT_FAILURE);
        } else {
            words[i] = '\0';
            *pointer = strdup(words);
            return 1;
        }
    }

    return 0;
}

// checks if it is a operator in the operators table
int is_operator(char *ch, char **pointer) {
    // operators table
    char *operators[] = {"++", "--", "*", "/", "+", "-", ":="};
    for (int i = 0; i < 7; i++) {
        if (strncmp(operators[i], ch, strlen(operators[i])) == 0) {
            *pointer = operators[i];
            return 1;
        }
    }
    return 0;
}

// checks if it is a bracket with condition
int is_bracket(char *ch, char **pointer) {
    if (*ch == '(') {
        *pointer = "LeftPar";
        return 1;
    } else if (*ch == ')') {
        *pointer = "RightPar";
        return 1;
    } else if (*ch == '[') {
        *pointer = "LeftSquareBracket";
        return 1;
    } else if (*ch == ']') {
        *pointer = "RightSquareBracket";
        return 1;
    } else if (*ch == '{') {
        *pointer = "LeftCurlyBracket";
        return 1;
    } else if (*ch == '}') {
        *pointer = "RightCurlyBracket";
        return 1;
    } else {
        return 0;
    }
}


void processWord(char *word, FILE *lexFile) {
    if (is_keyword(word)) {
        fprintf(lexFile, "Keyword(%s)\n", word);
    } else {
        fprintf(lexFile, "Identifier(%s)\n", word);
    }
}

// checks if it is a word in the keyword table
int is_keyword(char *pointer) {
    // keyword table
    char *keywords[] = {
        "break", "case", "char", "const", "continue",
        "do", "else", "enum", "float", "for",
        "goto", "if", "int", "long", "record",
        "return", "static", "while"};
    for (int i = 0; i < 18; i++) {
        if (strcmp(keywords[i], pointer) == 0) {
            return 1;
        }
    }
    return 0;
}
