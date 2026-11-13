#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <pigpio.h>
#define PIN 17
#define MAX_TEMP_READS 5

int isOutOfRange(float temperature, float UpperBound, float LowerBound) 
{
    return (temperature < LowerBound || temperature > UpperBound) ? 1 : 0;
}

void checkReading(float reading, float UpperBound, float LowerBound, FILE* OutputFile) 
{
    
    printf("\n   Tempature  System Status     Warning LED\n");
        if (isOutOfRange(reading, UpperBound, LowerBound)) printf("%8.0f F       WARNING           ON\n", reading);
        gpioWrite(PIN, 1);
        else printf("%8.0f F       NORMAL            OFF\n", reading);
        gpioWrite(PIN, 0);

    fprintf(OutputFile, "\n   Tempature  System Status     Warning LED\n");
        if (isOutOfRange(reading, UpperBound, LowerBound)) fprintf(OutputFile, "%8.0f F       WARNING           ON\n", reading);
        else fprintf(OutputFile, "%8.0f F       NORMAL            OFF\n", reading);
    
}

double calculateAverage(float readings[], int count) {
    double total = 0;
    for (int i = 0; i < count; i++) total += readings[i];
    return total / count;
}

char PrintMenu(char MenuOption)
{
    printf("\nPress <T> to enter a temperature reading\n");
    printf("Press <C> to change the valid temperature range\n");
    printf("Press <Q> to end the program\n");
    printf("Enter the menu option: ");
    scanf(" %c", &MenuOption);
    return toupper(MenuOption);
}

int main(int argc, char* argv[]) 
{   
    float readings[MAX_TEMP_READS] = {};
    float UpperBound = 76.0;
    float LowerBound = 68.0;
    int TempsEntered = 0;
    char MenuOption;
    FILE *FH = NULL;

    FH = fopen("temperature_log.csv", "w+");
    if (FH == NULL)
    {
        printf("The output file did not open. Ending program...\n");
    }
    

    gpioInitialise();
    gpioSetMode(PIN, PI_OUTPUT);

    
    printf("Welcome to the Smart Classroom Comfort Monitor\n\n");
    printf("Enter up to 5 temperature readings and this program will let you know if they are in the valid range\n");
    printf("The valid range is 68 to 76 by default (Fahrenheit)\n");
    printf("See the menu below:\n\n");
    MenuOption = PrintMenu(MenuOption);

    while (MenuOption != 'Q' && TempsEntered <= 5)
    {
        switch(MenuOption)
        {
            case 'T':
                printf("Enter a temperature reading: ");
                scanf(" %f", &readings[TempsEntered]);
                checkReading(readings[TempsEntered], UpperBound, LowerBound);
                TempsEntered++;
                MenuOption = PrintMenu(MenuOption);
                break;
            case 'C':
                printf("Enter the new desired Upper Bound: ");
                scanf(" %f", &UpperBound);
                printf("Enter the new desired Lower Bound: ");
                scanf(" %f", &LowerBound);
                printf("Changes successfully saved\n");
                MenuOption = PrintMenu(MenuOption);
                break;
            default:
                printf("That Menu Option was not recognized. Please reenter: ");
                scanf(" %c", &MenuOption);
                MenuOption = toupper(MenuOption);
                break;
        }
    }
        printf("\nAverage temperature: %.1f F\n",
        calculateAverage(readings, MAX_TEMP_READS));
        gpioWrite(PIN, 0);

    return 0;
}

