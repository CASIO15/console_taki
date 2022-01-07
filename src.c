#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME			  20 // Max name of a player
#define MAX_CARD_NAME		6 // The longest identifier is COLOR, its length is 0..5 + 1 space for '\0'
#define MAX_COLOR_LEN		2 // Max length of color, i.e R,G,B,W are of length 1, we need extra slot for '\0'
#define LEN_COLOR       5 // The length of the string 'COLOR'

#define WIDTH_CARD			9 // The width of the card printed to the screen
#define LENGTH_CARD			6 // The length of the card printed to the screen

#define INIT_QUAN			  4 // The initial quantity of card for each player
#define FIRST_PLAYER		0 // The index of the first player

// Tokens for each card, and one for indicating that the player drew a card from the deck.
#define TOKEN_PLUS			  10 
#define TOKEN_STOP			  11
#define TOKEN_CHANGE_DIR	12
#define TOKEN_TAKI			  13
#define TOKEN_CHANGE_COL	14
#define TOKEN_REG			    15
#define TOKEN_FROM_DEC		16

// Enumeration for each color
#define COLOR_Y			    1
#define COLOR_R				  2
#define COLOR_B				  3
#define COLOR_G				  4

// String literals for each type of card available in the game
#define STR_PLUS			    "+"
#define STR_STOP			    "STOP"
#define STR_CHANGE_DIR		"<->"
#define STR_TAKI			    "TAKI"
#define STR_CHANGE_COL		"COLOR"

// Char identifiers for each card number and the first letter of each type
#define CARD_1              '1'
#define CARD_2              '2'
#define CARD_3              '3'
#define CARD_4              '4'
#define CARD_5              '5'
#define CARD_6              '6'
#define CARD_7              '7'
#define CARD_8              '8'
#define CARD_9              '9'
#define CARD_PLUS           '+'
#define CARD_STOP           'S'
#define CARD_CH_DIR         '<'
#define CARD_CH_COL         'C'
#define CARD_TAKI           'T'

#define ZERO_CHAR			'0' // For converting numerical value to char
#define NO_COLOR			'\0' // Used when the color card is initialized, it has no color, used in the prinitng function for printing blank
#define NULL_CHAR			'\0' // String termination char

#define ERROR_OK			  0 // The operation succeeded 
#define ERROR_INVALID		1 // The opeartion failed
#define CARDS_RANGE			14 // 1 - 9, TAKI, <->, +, COLOR, STOP

// Typedefs for making the code a bit more redable, regarding the return type of several functions
typedef int errorCode;
typedef int token;

typedef struct card {
    char colour; // Can be R | G | B | Y | or none
    char type[MAX_CARD_NAME]; // Can be 1 - 9 | + | STOP | <-> | COLOR | TAKI
} Card;

/* Struct representing a player
   Contains the name, the deck of the player, the current quantity of cards in the deck and the total capacity of the deck.
*/
typedef struct player {
    char name[MAX_NAME];
    Card* deck;
    int handSize;
    int handCapacity;
} Player;

/*
    Struct representing the histogram/statistics of the game
    contatins the type of the card and a counter.
*/

typedef struct histogram {
    char type[MAX_CARD_NAME];
    int count;
} Histogram;

/*
    Holding metadata of the game, data that is not relevent for each player (except for the top card).
    Hold the number of players, the player currently playing, the rotation, the top card and a histogram
    that is updated at run-time.
*/

typedef struct info {
    int numOfPlayers;
    int currentlyPlaying;
    bool rotation;
    Card topCard;
    Histogram histogram[CARDS_RANGE];
} GameInfo;

void setSeed();
int getRandInRange(int n);
void welcomeMsg();
void enterNameMsg(int playerId);
void setPlayerName(char* name, int playerId);
void enterNumOfPlayersMsg();
void setNumOfPlayers(int* numOfPlayers);
void initPlayers(Player* players, int numOfPlayers);
void setNewCard(Card* newCard, char* type, char colour);
void makePlayerCard(Card* card, int choice);
void initTopCard(Card* topCard, int choice);
void displayCards(Card* card);
void showPlayerHand(Card* deck, int handSize);
errorCode validateMove(Card* topCard, Player* player, int choice);
token mapTopType(char* topType);
void swapCards(Card* c1, Card* c2);
token makeAMove(GameInfo* info, Player* player);
void setNewTopColor(Card* topCard);
void updateScreen(GameInfo* info, Player* player);
void changeGameState(GameInfo* info, Player* player, token tokenType, bool* isWinner);
void rotationHandler(GameInfo* info);
errorCode validateMoveOnTaki(GameInfo* info, char takiColour);
void takiHandler(GameInfo* info, Player* player, bool* isWinner);
void checkIfWinner(Player* player, bool* isGameOver);
void gameLoop(GameInfo* info, Player* players);
void checkCardAlloc(Card* newDeck);
void checkPlayerAlloc(Player* player);
void copyDeck(Card* dest, Card* source, int copySize);
Card* deckRealloc(Player* player, int newSize);
void initHistogram(GameInfo* info);
int returnCardIndex(Card* card);
void incHistogram(GameInfo* info, Card* drawnCard);
void initGameInfo(GameInfo* info);
void merge(Histogram* arr1, int size1, Histogram* arr2, int size2, Histogram* temp);
void copyDataFromTemp(Histogram* dest, Histogram* source, int size);
void mergeSort(Histogram* data, int size);
void sortHistogram(GameInfo* info);
void printHistogram(GameInfo* info);
void exitGame(GameInfo* info, Player* players);


void setSeed()
{
    // Sets the seed.

    srand(time(NULL));
}

int getRandInRange(int n)
{
    // Return value - A random number in the range 1 - n

    return (rand() % n) + 1;
}

void welcomeMsg()
{
    // Displays the initial hello message.

    printf("************ Welcome to TAKI game !!! ************\n");
}

void enterNameMsg(int playerId)
{
    // Displays the player a message to enter his name, displays the current players id.
    // int playerId - The current player identifier, the first player is #1 etc.

    printf("Please enter the first name of player #%d:\n", playerId);
}

void setPlayerName(char* name, int playerId)
{
    // Sets the name of the current player.
    // char *name - The name member of the Player struct.
    // int playerId - - The current player identifier, the first player is #1 etc.

    enterNameMsg(playerId);
    scanf("%s", name);
}

void enterNumOfPlayersMsg()
{
    // Displays a message to the player asking to enter the number of players.

    printf("Please enter the number of players:\n");
}

void setNumOfPlayers(int* numOfPlayers)
{
    // Receives the numOfPlayers member of the GameInfo struct and sets it to the value chosen by the user.

    enterNumOfPlayersMsg();
    scanf("%d", numOfPlayers);
}

void initPlayers(Player* players, int numOfPlayers)
{
    // Function for initalizing each player.
    // Player* players - An array of players, the array contains a pointer to each player.
    // int numOfPlayers - The amount of players that needs to be initialized.

    int i, j;

    for (i = 0; i < numOfPlayers; i++) {
        setPlayerName(players[i].name, i + 1);
        players[i].handSize = 0;

        players[i].deck = (Card*)malloc(sizeof(Card) * INIT_QUAN);
        checkCardAlloc(players[i].deck);

        for (j = 0; j < INIT_QUAN; j++) {
            makePlayerCard(&players[i].deck[j], getRandInRange(CARDS_RANGE));
        }
        players[i].handSize = INIT_QUAN;
        players[i].handCapacity = INIT_QUAN;
    }
}

void setNewCard(Card* newCard, char* type, char colour)
{
    // Setting the data of type and colour for a new card.

    newCard->colour = colour;
    strcpy(newCard->type, type);
}

void makePlayerCard(Card* card, int choice)
{
    // Function for making a card, makes one card at a time.
    // Card* card - Pointer to a card from the players deck.
    // int choice - A randomly generadted value from 1 - max(CARD_RANGE)

    char* validColors = "RGBY";
    int sizeCol = (int)strlen(validColors);
    int randCol = getRandInRange(sizeCol - 1); // Generating a number for choosing a random color.
    char chosenType[MAX_CARD_NAME] = { 0 };
    char choiceChar = (char)(choice + ZERO_CHAR);

    // If the choice is between 1 and 9 then we need to create a regular card
    // if not we need a case for each "special" card.
    switch (choice) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        chosenType[0] = choiceChar;
        chosenType[1] = NULL_CHAR;
        setNewCard(card, chosenType, validColors[randCol]);
        break;
    case TOKEN_PLUS:
        strcpy(chosenType, STR_PLUS);
        setNewCard(card, chosenType, validColors[randCol]);
        break;
    case TOKEN_CHANGE_COL:
        strcpy(chosenType, STR_CHANGE_COL);
        setNewCard(card, chosenType, NO_COLOR);
        break;
    case TOKEN_CHANGE_DIR:
        strcpy(chosenType, STR_CHANGE_DIR);
        setNewCard(card, chosenType, validColors[randCol]);
        break;
    case TOKEN_STOP:
        strcpy(chosenType, STR_STOP);
        setNewCard(card, chosenType, validColors[randCol]);
        break;
    case TOKEN_TAKI:
        strcpy(chosenType, STR_TAKI);
        setNewCard(card, chosenType, validColors[randCol]);
        break;
    default:
        break;
    }
}

void initTopCard(Card* topCard, int choice)
{
    // Initializing the top card acording to the instructions given in the PDF file.
    // Card* topCard - A pointer to the top card.
    // int choice - A random number between 1 - 9

    char* validColors = "RGBY";
    int sizeCol = (int)strlen(validColors);
    int randCol = getRandInRange(sizeCol - 1);
    char chosenType[MAX_COLOR_LEN] = { 0 };
    char choiceChar = (char)(choice + ZERO_CHAR);

    chosenType[0] = choiceChar;
    chosenType[1] = NULL_CHAR;
    setNewCard(topCard, chosenType, validColors[randCol]);
}

void displayCards(Card* card)
{
    // Function for printing a card
    // Card* card - A pointer to the card that need to be printed

    int i, j;
    int lengthType = (int)strlen(card->type); // The length of the type of the card, used for calculating an offset used later in the printing
    int offset1, offset2 = 1;

    if (lengthType > 1 && lengthType <= 4)
        offset1 = 2;
    else if (lengthType == LEN_COLOR)
        offset1 = 3;
    else
        offset1 = 1;

    for (i = 0; i < LENGTH_CARD; i++) {
        if (i == 0 || (i + 1) == LENGTH_CARD) {
            for (j = 0; j < WIDTH_CARD; j++)
                printf("*");
        }
        else {
            for (j = 0; j < WIDTH_CARD; j++) {
                if (j == 0 || (j + 1) == WIDTH_CARD) {
                    printf("*");
                }
                else {
                    printf(" ");
                }
                if (i == 2 && j == (WIDTH_CARD / 2) - offset1) {
                    printf("%s", card->type);
                    j += lengthType;
                }
                if (i == 3 && j == (WIDTH_CARD / 2) - offset2) {
                    if (card->colour == NULL_CHAR)
                        printf(" ");
                    else
                        printf("%c", card->colour);
                    j++;
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}

void showPlayerHand(Card* deck, int handSize)
{
    // Displaying the players hand
    // Card* deck - A pointer to the players deck.
    // int handSize - The physical size of the players deck.

    int i;

    for (i = 0; i < handSize; i++) {
        printf("Card #%d:\n", i + 1);
        displayCards(&deck[i]);
    }
}

errorCode validateMove(Card* topCard, Player* player, int choice)
{
    // Validating that the players move is indeed a valid one
    // Card* topCard - A pointer to the top card
    // Player* player - A pointer to the current player
    // int choice - The users choice of a card to place on top of the top card
    // Return value - If the users choice is a valid one a ERROR_OK is returned, if not then a ERROR_INVALID

    if (choice > player->handSize)
        return ERROR_INVALID;
    else if (choice == 0)
        return ERROR_OK;
    else if (*player->deck[choice - 1].type == *topCard->type)
        return ERROR_OK;
    else if (player->deck[choice - 1].colour == topCard->colour)
        return ERROR_OK;
    else if (mapTopType(player->deck[choice - 1].type) == TOKEN_CHANGE_COL)
        return ERROR_OK;
    else if (topCard->colour != player->deck[choice - 1].colour)
        return ERROR_INVALID;
    else if (choice < 0 || choice > player->handSize)
        return ERROR_INVALID;
}

token mapTopType(char* topType)
{
    // Mapping the type of the top card to the correct token
    // char* topType - A pointer to the char* representing the top card
    // Return value - A token representing the type

    switch (topType[0]) {
    case '+':
        return TOKEN_PLUS;
    case 'S':
        return TOKEN_STOP;
    case '<':
        return TOKEN_CHANGE_DIR;
    case 'C':
        return TOKEN_CHANGE_COL;
    case 'T':
        return TOKEN_TAKI;
    }
}

void swapCards(Card* c1, Card* c2)
{
    // Function for swapping cards

    Card temp = *c1;
    *c1 = *c2;
    *c2 = temp;
}

token makeAMove(GameInfo* info, Player* player)
{
    // One of the main function of the program, receives used input, validates it, and dispatch a call to other function based on the choice.
    // GameInfo* info - A pointer to the GameInfo, it controls the flow of the game.
    // Player* player - A pointer to the current player.
    // Return value - A token identifing the players 'move'

    int choice;
    token tokenType = TOKEN_REG;

    printf("Please enter 0 if you want to take a card from the deck\nor 1 - %d"
        " if you want to put one of your cards in the middle:\n", player->handSize);
    scanf(" %d", &choice);

    // Validating the users choice
    while (validateMove(&info->topCard, player, choice) != ERROR_OK) {
        printf("Invalid choice! Try again.\n");
        printf("Please enter 0 if you want to take a card from the deck\n"
            "or 1 - %d if you want to put one of your cards in the middle:\n", player->handSize);
        scanf("%d", &choice);
    }

    // If the user drew a card from the deck we will enter this branch.
    if (choice == 0) {
        // Verify wether the capacity of the deck need to be reallocated
        if (player->handCapacity < player->handSize + 1)
            player->deck = deckRealloc(player, player->handCapacity * 2);
        // Making a new card of the player and inserting it into the deck
        makePlayerCard(&player->deck[player->handSize++], getRandInRange(CARDS_RANGE));
        // Updating the histogram
        incHistogram(info, &player->deck[player->handSize - 1]);
        // Assaigning TOKEN_FROM_DECK to the return value
        tokenType = TOKEN_FROM_DEC;
    }
    else {
        /////////////////// Changing the type and color of the top card ////////////////////
        info->topCard.colour = player->deck[choice - 1].colour;
        strcpy(info->topCard.type, player->deck[choice - 1].type);
        ///////////////////////////////////////////////////////////////////////////////////
        // Determining the type of the new top card
        tokenType = mapTopType(info->topCard.type);
        // Decresing the hand size of the current player
        --(player->handSize);
        // Swapping the card chosen with the card placed in the handSize index.
        swapCards(&player->deck[choice - 1], &player->deck[player->handSize]);
    }
    return tokenType;
}

void setNewTopColor(Card* topCard)
{
    // If a COLOR card was chosen we need to assaign it a color, this function handles it.
    // Card* topCard - A pointer to the top card.

    int choice;

    printf("Please enter your color choice:\n"
        "1 - Yellow\n"
        "2 - Red\n"
        "3 - Blue\n"
        "4 - Green\n");
    scanf("%d", &choice);

    switch (choice) {
    case COLOR_Y:
        topCard->colour = 'Y';
        break;
    case COLOR_R:
        topCard->colour = 'R';
        break;
    case COLOR_G:
        topCard->colour = 'G';
        break;
    case COLOR_B:
        topCard->colour = 'B';
        break;
    }
}

void updateScreen(GameInfo* info, Player* player)
{
    // Updating the screen after each action
    // GameInfo* info - We need the topCard member from the info struct
    // Player* player - A pointer to the current players, whose deck we need to print

    printf("Upper card:\n\n");
    displayCards(&info->topCard);

    printf("%s's turn:\n\n", player->name);
    showPlayerHand(player->deck, player->handSize);
}

void changeGameState(GameInfo* info, Player* player, token tokenType, bool* isWinner)
{
    // A function for changing the game state accoring to the top card that was placed on the middle card stack.
    // GameInfo* info - Pointer to the info struct.
    // Player* player - Pointer to the current player.
    // token tokenType - The current token of the top card.
    // bool* isWinned - Pointer to the 'isGameOver' variable in the game loop function.


    // Dispatching to the correct handler based on the token of the top card.
    switch (tokenType) {
    case TOKEN_PLUS:
        if (player->handSize == 0) {
            makePlayerCard(player->deck, getRandInRange(CARDS_RANGE));
            (player->handSize)++;
            incHistogram(info, &player->deck[player->handSize - 1]);
            break;
        }
        else
            // If the card is +, thus the players has an extra turn, we don't need to update any component.
            return;
    case TOKEN_STOP:
        if (info->numOfPlayers == 1)
            return; // We dont need to do anything
        // Regular handler
        if (info->numOfPlayers == 2 && player->handSize == 1) {
            makePlayerCard(player->deck, getRandInRange(CARDS_RANGE));
        }
        else {
            rotationHandler(info);
            rotationHandler(info);
        }
        return;
    case TOKEN_CHANGE_DIR:
        // Changing the rotation using a ternary operator.
        // If the rotation is true, change it to false, else change to true.
        info->rotation = (info->rotation == true) ? false : true;
        break;
    case TOKEN_CHANGE_COL:
        setNewTopColor(&info->topCard);
        break;
    case TOKEN_TAKI:
        takiHandler(info, player, isWinner);
        return;
    }
    rotationHandler(info);
}

void rotationHandler(GameInfo* info)
{
    // Handler function for handling the rotation of the game
    // GameInfo* info - Pointer to the info of the game.

    if (info->rotation == true) { // Clockwise
        if (info->currentlyPlaying + 1 == info->numOfPlayers) {
            info->currentlyPlaying = 0;
        }
        else
            (info->currentlyPlaying)++;
    }
    else {
        if (info->currentlyPlaying == FIRST_PLAYER) { // Counter clockwise
            info->currentlyPlaying = info->numOfPlayers - 1;
        }
        else {
            (info->currentlyPlaying)--;
        }
    }
}

errorCode validateMoveOnTaki(GameInfo* info, char takiColour)
{
    // Function for validating the users move when the top card that was placed is the TAKI card.
    // Card* topCard - A pointer to the top card.
    // char takiColour - The colour of the TAKI card.
    // Return value - If the 'move' is valid it returns an ERROR_OK, else ERROR_INVALID

    if (strcmp(info->topCard.type, STR_CHANGE_COL) == 0) {
        return ERROR_OK;
    }
    if (info->topCard.colour != takiColour)
        return ERROR_INVALID;
    return ERROR_OK;
}

void takiHandler(GameInfo* info, Player* player, bool* isWinner)
{
    // The handler for the TAKI card.
    // GameInfo* info - A pointer to the info of the game.
    // Player* player - A pointer to the current player.
    // bool* isWinner - A pointer to the 'isGameOver' var in the gameLoop function, the player may win the game while in TAKI mode.

    int idx;
    char takiColour = info->topCard.colour; // Copy of the top cards colour.
    token returnedToken; // For storing the value returned from 'makeAMove'.

    while ((returnedToken = makeAMove(info, player)) != TOKEN_FROM_DEC && returnedToken != TOKEN_CHANGE_COL) {
        idx = player->handSize;
        checkIfWinner(player, isWinner);

        if (*isWinner == true && returnedToken != TOKEN_PLUS) {
            return;
        }

        if (validateMoveOnTaki(info, takiColour) == ERROR_INVALID) {
            printf("Invalid choice! Try again.\n");
            ++(player->handSize);
            swapCards(&info->topCard, &player->deck[idx + 1]);
        }
        else {
            info->topCard.colour = player->deck[idx].colour;
            strcpy(info->topCard.type, player->deck[idx].type);
            updateScreen(info, player);
        }
    }
    // Instead of creating another function for handling each top card placed at the end of the TAKI
    // we can make a recursive call (we are already in the call chain of changeGameState) to the changeGameState to update the gameInfo accordingly.
    changeGameState(info, player, returnedToken, isWinner);
}

void checkIfWinner(Player* player, bool* isGameOver)
{
    // If the physical handsize of the player is 0, he won the game, thus we need to notify the game loop that the game has ended.
    // Player* player - Pointer to the current player.
    // bool* isWinner - A pointer to the 'isGameOver' var in the gameLoop function.

    if (player->handSize == 0)
        *isGameOver = true;
}

void gameLoop(GameInfo* info, Player* players)
{
    // The loop of the game.
    // GameInfo* info - Pointer to the info of the game.
    // Players* players - Pointer to the array of players

    int i;
    token currentToken;
    bool isGameOver = false;

    printf("\n");

    info->currentlyPlaying = 0;
    info->rotation = true;

    // The actual loop of the game.
    while (isGameOver != true) {
        i = info->currentlyPlaying;
        printf("Upper card:\n\n");
        displayCards(&info->topCard);

        printf("%s's turn:\n\n", players[i].name);
        showPlayerHand(players[i].deck, players[i].handSize);
        currentToken = makeAMove(info, &players[info->currentlyPlaying]);

        if (currentToken == TOKEN_TAKI)
            updateScreen(info, &players[i]);

        changeGameState(info, &players[i], currentToken, &isGameOver);
        checkIfWinner(&players[i], &isGameOver);
    }

    printf("The winner is... %s! Congratulations !\n", players[i].name);
}

void checkCardAlloc(Card* newDeck)
{
    // Checking if the newDeck memory was allocated with no errors.

    if (newDeck == NULL) {
        printf("Error: Could not allocate memory !\n");
        exit(1);
    }
}

void checkPlayerAlloc(Player* player)
{
    // Checking if the player memory was allocated with no errors.

    if (player == NULL) {
        printf("Error: Could not allocate memory !\n");
        exit(1);
    }
}

void copyDeck(Card* dest, Card* source, int copySize)
{
    // Function for copying the old deck to the new one allocated.
    // Card* dest - Pointer to the new card deck allocated.
    // Card* source - Pointer to the source deck.
    // int copySize - The amount of cards we need to copy.

    // Storing the end address of the source deck
    Card* endAddr = source + copySize;

    // Copy operation
    while (source < endAddr) {
        *dest++ = *source++;
    }
}

Card* deckRealloc(Player* player, int newSize)
{
    // Reallocating the new deck.
    // Player* player - Pointer to the current player.
    // int newSize - The size of the new deck.

    Card* newDeck = NULL;

    if (newSize == 0) { // For freeing the decks later
        free(player->deck);
        return NULL;
    }

    newDeck = (Card*)malloc(sizeof(Card) * newSize);
    checkCardAlloc(newDeck);
    player->handCapacity = newSize;

    // Im copying the last to cards in the deck, because is use one of them in my taki handler.
    copyDeck(newDeck, player->deck, player->handSize + 2);
    free(player->deck);

    return newDeck;
}

void initHistogram(GameInfo* info)
{
    // Initializing the histogram
    // GameInfo* info - Pointer to the game info.

    int i;

    for (i = 0; i < CARDS_RANGE; i++) {
        if (i < 9) {
            info->histogram[i].type[0] = ((i + 1) + ZERO_CHAR);
            info->histogram[i].type[1] = NULL_CHAR;
        }
        else {
            switch ((i + 1)) {
            case TOKEN_CHANGE_COL:
                strcpy(info->histogram[i].type, "COLOR");
                break;
            case TOKEN_CHANGE_DIR:
                strcpy(info->histogram[i].type, "<->");
                break;
            case TOKEN_PLUS:
                strcpy(info->histogram[i].type, "+");
                break;
            case TOKEN_STOP:
                strcpy(info->histogram[i].type, "STOP");
                break;
            case TOKEN_TAKI:
                strcpy(info->histogram[i].type, "TAKI");
                break;
            }
        }
        info->histogram[i].count = 0;
    }
}

int returnCardIndex(Card* card)
{
    // Returning the index of the card based on his type
    // Card* card - Pointer to the card.

    // We only need the first char of the card.
    char type = card->type[0];

    switch (type) {
    case CARD_1:
        return 0;
    case CARD_2:
        return 1;
    case CARD_3:
        return 2;
    case CARD_4:
        return 3;
    case CARD_5:
        return 4;
    case CARD_6:
        return 5;
    case CARD_7:
        return 6;
    case CARD_8:
        return 7;
    case CARD_9:
        return 8;
    case CARD_PLUS:
        return 9;
    case CARD_STOP:
        return 10;
    case CARD_CH_DIR:
        return 11;
    case CARD_TAKI:
        return 12;
    case CARD_CH_COL:
        return 13;
    default:
        return -1;
    }
}

void incHistogram(GameInfo* info, Card* drawnCard)
{
    // Incrementing the histogram
    // GameInfo* info - Pointer to the game info.
    // Card* drawnCard - The new card created and placed in the current players deck.

    int cardIndex = returnCardIndex(drawnCard);
    info->histogram[cardIndex].count++;
}

void initGameInfo(GameInfo* info)
{
    // Initialzing the game info
    // GameInfo* info - Pointer to the game info.

    initTopCard(&info->topCard, getRandInRange(9));
    initHistogram(info);
}


///////////////////////////////// Functions for the merge sort ///////////////////////////////////////////

void merge(Histogram* arr1, int size1, Histogram* arr2, int size2, Histogram* temp)
{
    int i, j, k;

    i = j = k = 0;
    while (i < size1 && j < size2) {
        if (arr1[i].count >= arr2[j].count)
            temp[k++] = arr1[i++];
        else
            temp[k++] = arr2[j++];
    }

    while (i < size1)
        temp[k++] = arr1[i++];
    while (j < size2)
        temp[k++] = arr2[j++];
}

void copyDataFromTemp(Histogram* dest, Histogram* source, int size)
{
    while (size > 0) {
        *dest++ = *source++;
        size--;
    }
}

void mergeSort(Histogram* data, int size)
{
    // If the size <= 1 the data is sorted
    if (size <= 1)
        return;

    mergeSort(data, size / 2);
    mergeSort(data + size / 2, size - size / 2);

    Histogram* temp = (Histogram*)malloc(sizeof(Histogram) * size);

    if (temp == NULL) {
        printf("Error: Memory allocation failed !");
        exit(EXIT_FAILURE);
    }

    merge(data, size / 2, data + size / 2, size - size / 2, temp);
    copyDataFromTemp(data, temp, size);

    free(temp);
    temp = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void sortHistogram(GameInfo* info)
{
    mergeSort(info->histogram, CARDS_RANGE);
}

void printHistogram(GameInfo* info)
{
    // Function for printing the histogram
    // GameInfo* info - A pointer to the game info.
    int i;

    printf("\n************ Game Statistics ************\n"
        "Card # | Frequency\n"
        "__________________\n");

    for (i = 0; i < CARDS_RANGE; i++) {
        printf("%6s | %d\n", info->histogram[i].type, info->histogram[i].count);
    }
}

void exitGame(GameInfo* info, Player* players)
{
    // Function for exiting the game, i.e freeing the memory and for sorting and printing the histogram.
    // GameInfo* info - Pointer to the game info.
    // Player* players - Pointer to the array of players.

    int i;

    for (i = 0; i < info->numOfPlayers; i++)
        players[i].deck = deckRealloc(&players[i], 0);

    sortHistogram(info);
    printHistogram(info);
}

int main()
{
    Player* players = NULL;
    GameInfo info = { 0 };

    setSeed();
    welcomeMsg();

    setNumOfPlayers(&info.numOfPlayers);
    players = (Player*)malloc(sizeof(Player) * info.numOfPlayers);
    checkPlayerAlloc(players);

    initPlayers(players, info.numOfPlayers);
    initGameInfo(&info);

    gameLoop(&info, players);
    exitGame(&info, players);

    free(players);
    players = NULL;

    return 0;
}
