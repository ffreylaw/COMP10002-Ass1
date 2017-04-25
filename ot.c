#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int ch1, ch2, line=1, col=1;
    FILE *fp1, *fp2;
    fp1 = fopen(argv[1], "r");
    fp2 = fopen(argv[2], "r");
    if ((!argv[1])||(!argv[2])) {
        fprintf(stderr, "USAGE: %s FILE1 FILE2\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if ((!fp1)||(!fp2)) {
        fprintf(stderr, "FAILED OPENING FILE(s)\n");
        exit(EXIT_FAILURE);
    } else {
        printf("TWO FILES SUCCESSFULLY READ\nMATCHING STARTED...\n\n");
    }
    while (((ch1=getc(fp1))!=EOF)&&((ch2=getc(fp2))!=EOF)) {
        if (ch1==ch2) {
            printf("%c", ch2);
            if (ch2=='\n') {
                line++;
                col = 1;
            } else {
                col++;
            }
        } else {
            printf("\n\nMISMATCHING DETECTED ON\n");
            printf("LINE:   %15d\nCOLUMN: %15d \n", line, col);
            printf("MISMATCHED CHARACTER:\nFILE1 = '%c' FILE2 = '%c' \n\n", ch1, ch2);
            break;
        }
    }
    fclose(fp1);
    fclose(fp2);
    return 0;
}