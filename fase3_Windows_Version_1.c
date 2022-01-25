/*
  AAU CPH, SW1 Group 2, P1 Project
  Windows Version 1.0
  
  Reference list:
  - makeFileKeywords function was inspired by Pankaj Prakash at https://tinyurl.com/2cst5cdh
  - makeFileWithLineBreaks function was inspired by CProgrammer at https://tinyurl.com/2p9b6act
  - readFileKeywords function was inspired by Haccks at https://tinyurl.com/4jkjxat5
  - sentenceFinder function was inspired by Amanda Fawcett at https://tinyurl.com/yc6zj2z4 and https://tinyurl.com/mv322rd3
*/

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Symbolic constants
#define PHONE_OR_EMAIL_LENGTH 4 // Number of phone or email addresses
#define TEXT_NUM 12             // Number of texts to import and analyze
#define STRING_NUM 20           // Number of strings in a string arrays for keywords and sentences
#define WORD_LENGTH 50          // Length of keywords
#define SENTENCE_LENGTH 200     // Length of a sentence or email
#define MAX 9999                // Symbolic constant for any relatively big number
#define PRINT_LENGTH 99999      // Length of output in file

// Function prototypes
typedef struct
{
    int rating;                                         // Stores rating
    int keywordCounter;                                 // Stores number of keywords
    int sentenceCounter;                                // Stores number of sentences with keywords
    char filename[WORD_LENGTH];                         // Stores filename
    char keywords[STRING_NUM][WORD_LENGTH];             // Stores keywords
    char sentences[STRING_NUM][SENTENCE_LENGTH];        // Stores sentences with keywords
    char email[PHONE_OR_EMAIL_LENGTH][SENTENCE_LENGTH]; // Stores all email addresses
    long phone[PHONE_OR_EMAIL_LENGTH];                  // Stores all phone numbers
} text_file;

void makeFileKeywords(void);                                                         // Creates keyword file containing user input
void makeFileWithLineBreaks(int i, text_file *texts);                                // Creates a new temporary file containing the original text file without punctuation
void readFileKeywords(int fileNumber, int *totalKeywordCount, text_file *texts);     // Puts keywords into string array and also calls keywordFinder and sentenceFinder
void keywordFinder(char keyword[], int *keywordExists);                              // Extracts all keywords from temporary text file
void sentenceFinder(char keyword[], int fileNum, text_file *texts); // Extracts all sentences from temporary text file
void contactFinder(int i, text_file *texts);                                         // Extracts contact information from text file
void contactRating(text_file *texts);                                                // Checks for contact information and calculates ratings accordingly
void outputInFile(int wordCount, text_file *texts);                                  // Deletes previous data and prints results in a new text file
void validation(FILE *file);                                                         // Checks if a file exists
int qsortComparison(const void *a, const void *b);                                   // Compares text_file structs by their ratings

int main(void)
{
    text_file *texts = malloc((TEXT_NUM + 1) * sizeof(text_file));
    if (texts == NULL)
    {
        printf("Memory could not be allocated.\n");
        exit(EXIT_FAILURE);
    }

    int totalKeywordCount; // Stores all keywords
    makeFileKeywords();

    for (int i = 1; i <= TEXT_NUM; i++)
    {
        printf("Text %2d: ", i);
        makeFileWithLineBreaks(i, texts);
        contactFinder(i, texts);
        readFileKeywords(i, &totalKeywordCount, texts);
    }

    contactRating(texts);
    outputInFile(totalKeywordCount, texts);
    free(texts);

    printf("Successfully finished all operations.\n\n");
    return 0;
}

// Creates keyword file containing user input
void makeFileKeywords()
{
    char data[MAX]; // Stores terminal input

    FILE *keywords = fopen("keywords.txt", "w");
    validation(keywords);

    printf("\n---------------------------------------------------------------------------------------------\n");
    printf("Please enter keywords to store in file in sequential priority.\nDisclaimer: max 20 words w/ spaces in between!\nWrite here: ");
    fgets(data, MAX, stdin); // Puts keywords from terminal to array
    fputs(data, keywords);   // Puts keywords from array to file

    // Close file to save file data
    fclose(keywords);
    printf("\nKeyword file created and saved successfully.\n\n");
}

// Creates a new temporary file containing the original text file without punctuation
void makeFileWithLineBreaks(int i, text_file *texts)
{
    char punctuation[] = {'.', '?', '!', ':', ';',
                          '-', '(', ')', '[', ']',
                          ',', '"', '/'};

    // Input is used to store the original text, and output is the same text without punctuation
    char input[MAX], output[MAX];
    int outputChar = 0;

    // Open file
    char filenameFormat[] = "text%d.txt";                    // All text files have the format of "text*number*.txt"
    char filename[sizeof(filenameFormat) + 4];               // 4 is for length of text, which varies with decimals in file name
    snprintf(filename, sizeof(filename), filenameFormat, i); // Puts data in filename

    FILE *text = fopen(filename, "r");
    validation(text);

    strcpy(texts[i].filename, filename); // Inserts filename into struct

    // Get strings from file
    while (fgets(input, MAX, text) != NULL)
    {
        // INCLUDE THE FOLLOWING LINE ON WINDOWS
        puts(input);

        for (int i = 0; input[i] != '\0'; i++)
        {

            // Flag is a boolean used to mark if punctuation has been spotted
            int flag = 0;

            // Change char subsequent to period to line break in input string
            if (input[i] == '.')
            {
                input[i + 1] = '\n';
            }

            // Compare to all the different punctations
            for (int j = 0; j < 13; j++)
            {
                // Check all punctuation
                if (input[i] == punctuation[j])
                {
                    // Flag is true
                    flag = 1;
                    break;
                }
            }

            // If flag is not true, put input array value into output list
            if (!flag)
            {
                output[outputChar++] = input[i];
            }
        }
    }

    fclose(text);

    // Put keywords from array into file
    FILE *textTemp = fopen("text_temp.txt", "w");
    validation(textTemp);
    fputs(output, textTemp);
    fclose(textTemp);

    printf("Temporary text file created and saved successfully.\n\n");
}

// Puts keywords into string array and also calls keywordFinder and sentenceFinder
void readFileKeywords(int fileNumber, int *totalKeywordCount, text_file *texts)
{
    // Find size for keyword string array
    int wordCount = 1; //Number of keywords
    char ch;           // Stores current char from keyword file

    FILE *keywords = fopen("keywords.txt", "r");
    validation(keywords);

    while ((ch = fgetc(keywords)) != EOF)
    {
        // If the current char is a space then iterate wordcount
        if (ch == ' ')
        {
            wordCount++;
        }
    }
    *totalKeywordCount = wordCount;

    // File pointer is reset to be able to run fscanf
    rewind(keywords);

    char keyword[wordCount][WORD_LENGTH]; // Empty string array for the keywords

    // Putting keywords into keyword array
    for (int i = 0; i < wordCount; i++)
    {
        // Do not need pointer because fscanf specifies it
        fscanf(keywords, "%s", keyword[i]);
    }

    // The keywords are now stored in a string array
    fclose(keywords);

    // Counts unique keywords
    int keywordExists = 0; // keywordExists is boolean

    // Check match for every keyword
    for (int i = 0; i < wordCount; i++)
    {
        keywordFinder(keyword[i], &keywordExists);

        // Checks if new match has been found, if unique keyword was found (keyword counter != keyword exists) add points
        if (keywordExists == 1)
        {
            texts[fileNumber].rating += 1;
            texts[fileNumber].keywordCounter += 1;
            if (i < 8)
            {
                texts[fileNumber].rating += 10 - i;
            }
            else
            {
                texts[fileNumber].rating += 2;
            }
        }

        sentenceFinder(keyword[i], fileNumber, texts);
    }
}

// Extracts all keywords from temporary text file
void keywordFinder(char keyword[], int *keywordExists)
{
    // Creating char array to store data of file and keyword
    char fullText[MAX];
    int keywordNum = 0; // Number of actual keyword
    *keywordExists = 0; // Boolean

    FILE *text = fopen("text_temp.txt", "r");
    validation(text);

    // Checks number of keywords
    while (fscanf(text, "%s", fullText) != EOF)
    {
        // IT MUST BE strstr ON WINDOWS BECAUSE OF MISSING SYSTEM PACKAGE
        // FOR MAC: strcasestr ignores case, thus gives more accurate results
        // Checks if keyword exists
        if (strstr(fullText, keyword) > 0)
        {
            keywordNum++;
        }
    }

    if (keywordNum > 0)
    {
        *keywordExists = 1;
    }

    //Close file
    fclose(text);
}

// Extracts all sentences from temporary text file
void sentenceFinder(char keyword[], int fileNum, text_file *texts)
{
    // Creating char array to store data of file and keyword
    char line[SENTENCE_LENGTH];

    FILE *text = fopen("text_temp.txt", "r");
    validation(text);

    // Checking for substring
    while (fgets(line, sizeof(line), text))
    {

        // IT MUST BE strstr ON WINDOWS BECAUSE OF MISSING SYSTEM PACKAGE
        // FOR MAC: strcasestr ignores case, thus gives more accurate results
        if (strstr(line, keyword) > 0) // warning: ordered comparison between pointer and zero
        {
            strcpy(texts[fileNum].sentences[texts[fileNum].sentenceCounter++], line);
            strcpy(texts[fileNum].keywords[texts[fileNum].sentenceCounter - 1], keyword);
        }
    }

    // Close file
    fclose(text);
}

// Extracts contact information from text file
void contactFinder(int i, text_file *texts)
{
    // Creating char array to store data of file and keyword
    char filenameFormat[] = "text%d.txt";
    char filename[sizeof(filenameFormat) + 3];
    char fullText[MAX];
    char *point;
    snprintf(filename, sizeof(filename), filenameFormat, i);

    FILE *text = fopen(filename, "r");
    int phoneNumber = 0;
    int phoneCounter = 0;
    // Search until end of document
    while (fscanf(text, "%s", fullText) != EOF)
    {
        // --------THIS FOR PHONE--------
        // Check if scanned word is 8 characters long
        if (strlen(fullText) == 8 || strlen(fullText) == 10)
        {
            // Check if scanned word contains digits, if not 0 is returned, sometimes it returns some of the digits, causes problems...
            phoneNumber = strtol(fullText, &point, 10);

            // Checks validity of phone number
            if (phoneNumber >= 10000000)
            {
                // Add phone number to array
                texts[i].phone[phoneCounter] = phoneNumber;
                phoneCounter++;
            }
        }
    }

    // Resets pointer
    rewind(text);

    // Finds email address(es)
    char mail[5][SENTENCE_LENGTH];
    int mailCounter = 0;

    while (fscanf(text, "%s", fullText) != EOF)
    {
        if (strrchr(fullText, '@'))
        {
            strcpy(mail[mailCounter], fullText);
            mailCounter++;
        }
    }

    for (int j = 0; j < mailCounter; j++)
    {
        int mailLength = strlen(mail[j]);
        if (mail[j][mailLength - 1] == '.')
        {
            mail[j][mailLength - 1] = '\0';
        }
        strcpy(texts[i].email[j], mail[j]);
    }
    // Close file
    fclose(text);
}

// Checks for contact information and calculates ratings accordingly
void contactRating(text_file *texts)
{
    for (int j = 0; j <= TEXT_NUM; j++)
    {
        if (strlen(texts[j].email[0]) < 5) // If there is no mail, then mail is "None", so strlen returns 4
        {
            texts[j].rating -= 10;
        }

        if (texts[j].phone[0] == 0) // If there is no phone number, then phone is "0"
        {
            texts[j].rating -= 10;
        }

        if ((strlen(texts[j].email[0]) < 5) && (texts[j].phone[0] == 0))
        {
            texts[j].rating -= 100; // Text is disqualified if there is not contact information
        }
    }
}

// Deletes previous data and prints results in a new text file
void outputInFile(int totalKeywordCount, text_file *texts)
{
    // Empty shortlist.txt
    FILE *shortlistTemp = fopen("shortlist.txt", "w");
    validation(shortlistTemp);
    fclose(shortlistTemp);

    // Append to shortlists
    FILE *shortlist = fopen("shortlist.txt", "a");
    validation(shortlist);

    int maxPoints = 0;
    int minPoints;

    for (int i = 0; i < totalKeywordCount; i++)
    {
        maxPoints += 1;
        if (i < 8)
        {
            maxPoints += 10 - i;
        }
        else
        {
            maxPoints += 2;
        }
    }
    printf("\nPlease enter minimum score for application files (Max score is: %d):\nWrite here: ", maxPoints);
    scanf("%d", &minPoints);
    printf("\n");

    // Sorts struct list
    qsort(texts, TEXT_NUM + 1, sizeof(text_file), qsortComparison);

    //print header to file
    fprintf(shortlist, "---------------------------------------------------------------------------------------------\n --- Job applications from most to least matches --- \n---------------------------------------------------------------------------------------------\n");

    for (int i = TEXT_NUM; i > 0; i--)
    {
        //textfiles's rating must at least be equal to minPoints
        if (texts[i].rating >= minPoints)
        {

            // Create an empty string
            char textToPrint[PRINT_LENGTH];
            snprintf(textToPrint, PRINT_LENGTH, "\nFile Name: %s\nRating: %d / %d\nUnique Keywords: %d\n",
                     texts[i].filename, texts[i].rating, maxPoints, texts[i].keywordCounter);

            // Add phone numbers to text to print
            char phoneTemp[50];
            for (int j = 0; j < 4; j++)
            {
                strcpy(phoneTemp, "");
                if (texts[i].phone[j] > 0)
                {
                    snprintf(phoneTemp, 50, "Phone no. %d: %ld\n", j + 1, texts[i].phone[j]);
                    strcat(textToPrint, phoneTemp);
                }
            }
            // Add emails to text to print
            char emailTemp[200];
            for (int j = 0; j < 4; j++)
            {
                strcpy(emailTemp, "");
                if (strlen(texts[i].email[j]) > 0)
                {
                    snprintf(emailTemp, 200, "Email no. %d: %s\n", j + 1, texts[i].email[j]);
                    strcat(textToPrint, emailTemp);
                }
            }

            char sentenceTemp[50];
            snprintf(sentenceTemp, 50, "\nsentences(%d):\n", texts[i].sentenceCounter);
            strcat(textToPrint, sentenceTemp);

            for (int j = 0; j < texts[i].sentenceCounter; j++)
            {
                strcat(textToPrint, "- ");
                strcat(textToPrint, texts[i].keywords[j]);
                strcat(textToPrint, ":\n");

                texts[i].sentences[j][strlen(texts[i].sentences[j]) - 1] = '.';
                texts[i].sentences[j][strlen(texts[i].sentences[j])] = '\0';

                strcat(textToPrint, texts[i].sentences[j]);
                strcat(textToPrint, "\n\n");
            }
            textToPrint[strlen(textToPrint) - 1] = '\0';
            strcat(textToPrint, "---------------------------------------------------------------------------------------------\n");
            fputs(textToPrint, shortlist);
        }
    }
    fclose(shortlist);
    printf("File shortlist.txt with key information about applicants created successfully.\n\n");
}

// Checks if a file exists
void validation(FILE *file)
{
    if (file == NULL)
    {
        // File not located hence exit
        printf("Unable to locate file.\n");
        exit(EXIT_FAILURE);
    }
}

// Compares text_file structs by their ratings
int qsortComparison(const void *a, const void *b)
{
    const text_file *text1 = a;
    const text_file *text2 = b;

    if (text1->rating > text2->rating)
    {
        return 1;
    }
    else if (text1->rating < text2->rating)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
