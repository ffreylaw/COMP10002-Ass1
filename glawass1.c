/* The University of Melbourne
 * COMP10002 Foundations of Algorithms, Semester 2, 2015
 * Project 1 written by Geoffrey Law (Student ID: 759218), September 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define DEBUG 0

#define MAXCHARS_LINE 1000  /* max characters of a line */
#define MAXCHARS_QUERY 100  /* max characters of a query */
#define MAXRANKS 10         /* max numbers of rank to show */

typedef struct {
    int lineNum;            /* line number */
    char *lineStr;          /* line of text (string) */
    double simpleScore;     /* simple score */
    double betterScore;     /* better score */
} line_t;

int getLine(char *oneLine, int limit);
void printStageZero(char *query);
void printStageOne(char *line, int i);
void printStageTwo(double score, int i);
void printStageThree(line_t *rank);
void printStageFour(double score, int i);
void ranking(line_t *rank, int limit);
void lineSwap(line_t *line1, line_t *line2);
double simpleScoring(char *query, char *line);
double betterScoring(char *query, char *line);
double logBase2(double n);

int main(int argc, char *argv[]) {
    char oneLine[MAXCHARS_LINE+1];
    line_t *allLines=NULL;
    int i, n=0;
    char *query;
    
    query = argv[1];
    if (!query) {
        printf("Usage: %s string\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if (strlen(query)>MAXCHARS_QUERY) {
        printf("Query is too long (>%d), please try again\n", MAXCHARS_LINE);
        exit(EXIT_FAILURE);
    }
    
    /* Constructing the array of sturcture */
    allLines = (line_t*)malloc(sizeof(line_t));     /* initialize memory */
    while (getLine(oneLine, MAXCHARS_LINE) != EOF) {
        /* re-allocate memory of one sturcture in the array
         * and allocate memory of the string in the structure 
         */
        allLines = (line_t*)realloc(allLines, (n+1)*sizeof(line_t));
        allLines[n].lineStr = (char*)malloc((strlen(oneLine)+1)*sizeof(char));
        /* store corresponding components into allLines structure */
        allLines[n].lineNum = n+1;
        strcpy(allLines[n].lineStr, oneLine);
        n++;
    }
    
    /* Print all stages and calculate the scores */
    printStageZero(query);
    for (i=0; i<n; i++) {
        /* iterate through every structure in the array */
        if (allLines[i].lineStr[0]=='\0') {
            /* if an empty line detected, skip to next loop */
            allLines[i].simpleScore = 0.0;
            allLines[i].betterScore = 0.0;
            continue;
        } else {
            printf("---\n");
            printStageOne(allLines[i].lineStr, i);
            allLines[i].simpleScore = 
                simpleScoring(query, allLines[i].lineStr);
            printStageTwo(allLines[i].simpleScore, i);
            allLines[i].betterScore = 
                betterScoring(query, allLines[i].lineStr);
            /* function betterScoring includes debug section */
            printStageFour(allLines[i].betterScore, i);
        }
    }
    ranking(allLines, n);   /* function to sort allLines by score */
    printStageThree(allLines);
    
    return 0;
}

/*****************************************************************************/

/* Get a string of one line from stdin and store into oneLine
 * refer to getword.c (Moffat, 2012) 
 */

int getLine(char *oneLine, int limit) {
    int ch, len=0;
    while((ch=getchar())!=EOF && len<limit) {
        if (ch == '\n') {
            /* finish reading character if a new line being detected */
            break;
        } else {
            oneLine[len++] = ch;
        }
    }
    if (ch==EOF) {
        return EOF;
    }
    oneLine[len] = '\0';    /* close string */
    
    return 0;
}

/*****************************************************************************/

/* Functions to print stages */

void printStageZero(char *query) {
    printf("S0: query = %s \n", query);
}

void printStageOne(char *line, int i) {
    printf("%s\nS1: line %5d, bytes = %2lu\n", line, i+1, strlen(line));
}

void printStageTwo(double score, int i) {
    printf("S2: line %5d, score = %6.3f\n", i+1, score);
}

void printStageThree(line_t *rank) {
    int i;
    printf("------------------------------------------------\n");
    for (i=0; i<MAXRANKS; i++) {
        if (rank[i].betterScore == 0.0) {
            continue;
        } else {
            printf("S3: line %5d, score = %6.3f\n%s\n---\n",
                   rank[i].lineNum, rank[i].betterScore, rank[i].lineStr);
        }
    }
}

void printStageFour(double score, int i) {
    printf("S4: line %5d, score = %6.3f\n", i+1, score);
}

/*****************************************************************************/

/* Insertion sort and a function swap addresses of structure line_t
 * refer to insertionsort.c (Moffat, 2012) 
 */

void ranking(line_t *rank, int limit) {
    int i, j;
    for (i=1; i<limit; i++) {
        for (j=i-1; j>=0 && rank[j+1].betterScore>rank[j].betterScore; j--) {
            /* sort in descending order */
            lineSwap(&rank[j], &rank[j+1]);
        }
    }
}

void lineSwap(line_t *line1, line_t *line2) {
    line_t tmp;
    tmp = *line1;
    *line1 = *line2;
    *line2 = tmp;
}

/*****************************************************************************/

/* Calculate simple score of a line */

double simpleScoring(char *query, char *line) {
    int scoreArr[strlen(line)], i, j, u=0, v=0, currScore, tmpScore;
    double maxScore;
    
    /* Calculate the score by brute-force string matching */
    for (i=0; i<strlen(line); i++) {
        currScore = 0;
        tmpScore = 0;
        for (j=0; j<strlen(query); j++) {
            /* comparing line[i] to every character of the query */
            currScore = 0;
            if (line[i]==query[j]) {
                /* if they do match, increment the current score */
                currScore++;
                if ((line[i+1]!='\0') && (query[j+1]!='\0')) {
                    /* if next position of both are not null,
                     * start a loop which shifting both strings
                     * and check whether they matched
                     */
                    u = i+1;
                    v = j+1;
                    while (line[u]==query[v]) {
                        /* if they matched then shift to next position */
                        currScore++;
                        u++;
                        v++;
                        if ((line[u]!='\0') && (query[v]!='\0')) {
                            /* if current shifted positions are not null
                             * then continue shifting
                             */
                            continue;
                        } else {
                            break;
                        }
                    }
                }
            }
            if (tmpScore < currScore) {
                tmpScore = currScore;
            }
        }
        if (currScore < tmpScore) {
            currScore = tmpScore;
        }
        scoreArr[i] = currScore;
        /* store the maximum score of the current character
         * into the score array
         */
    }
    
    /* Find the maximum score of the score array */
    maxScore = 0.0;
    for (i=0; i<strlen(line); i++) {
        if (scoreArr[i] > maxScore) {
            maxScore = scoreArr[i];
        }
    }
    
    return 1.0*maxScore;
}

/*****************************************************************************/

/* Calculate better score of a line */

double betterScoring(char *query, char *line) {
    int i, j, u, v, len=0, **scoreTable;
    unsigned long m=0, n=0;
    double iSum, jSum, score=0.0;
    char *queryIsalnum;
    
    /* Generate the new string called queryIsalnum that
     * removed all non-alnum characters from the original query 
     */
    queryIsalnum = (char*)malloc(sizeof(char));
    for (i=0; i<strlen(query); i++) {
        if (isalnum(query[i])) {
            queryIsalnum =
                (char*)realloc(queryIsalnum, (len+1)*sizeof(char));
            queryIsalnum[len++] = tolower(query[i]);
        }
    }
    queryIsalnum = (char*)realloc(queryIsalnum, (len+1)*sizeof(char));
    queryIsalnum[len] = '\0';   /* close string */
    
    m = strlen(queryIsalnum);
    n = strlen(line);
    
    /* Initialize the score table */
    scoreTable = (int**)malloc(m*sizeof(int*));
    for (i=0; i<m; i++) {
        scoreTable[i] = (int*)malloc(m*sizeof(int));
        for (j=0; j<m; j++) {
            scoreTable[i][j] = 0;
        }
    }
    
    /* Generate the score table by brute-force string matching */
    for (i=0; i<m; i++) {
        for (j=0; j<n; j++) {
            if (queryIsalnum[i]==tolower(line[j])) {
                /* if they do match, increment the score 
                 * of corresponding position
                 */
                scoreTable[i][i]++;
                if ((queryIsalnum[i+1]!='\0') && (line[j+1]!='\0')) {
                    /* if next position of both are not null, 
                     * start a loop which shifting both strings
                     * and check whether they matched 
                     */
                    u = i+1;
                    v = j+1;
                    while (queryIsalnum[u]==tolower(line[v])) {
                        /* if they matched then shift to next position */
                        scoreTable[i][u]++;
                        u++;
                        v++;
                        if ((queryIsalnum[u]!='\0') && (line[v]!='\0')) {
                            /* if current shifted positions are not null
                             * then continue shifting
                             */
                            continue;
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }
    
#if DEBUG
    /* Print the score table */
    printf("DB: ");
    for (i=0; i<m; i++) {
        printf("%3c", queryIsalnum[i]);
    }
    printf("\n");
    for (i=0; i<m; i++) {
        printf("DB: ");
        for (j=0; j<m; j++) {
            printf("%3d", scoreTable[i][j]);
        }
        printf("\n");
    }
#endif
    
    /* Calculate the score by the summation formula,
     * obtaining values from the score table 
     */
    iSum = 0.0;
    for (i=0; i<=m-3; i++) {
        jSum = 0.0;
        for (j=i+2; j<=m-1; j++) {
            if (scoreTable[i][j]==0) {
                continue;
            } else {
                jSum = jSum +
                    1.0*(j-i-1)*(j-i-1)*logBase2(2+scoreTable[i][j]);
            }
        }
        iSum = iSum + jSum;
    }
    score = 1.0*iSum/logBase2(30+n);
    
    return score;
}

/*****************************************************************************/

/* Calculate log base 2 */

double logBase2(double n) {
    return 1.0*log(n)/log(2);
}

/*****************************************************************************/

/* ALGORITHMS ARE FUN! */

/* Reference
 Moffat, A. (2012). Programming, Problem Solving, and Abstraction with C.
 Sydney, Australia: Pearson Custom Books. */
