#ifndef POKER_H
#define POKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define NUM_CARDS 52
#define NUM_RANKS 13
#define NUM_SUITS 4
#define HAND_SIZE 2
#define FLOP_SIZE 3
#define TURN_RIVER_SIZE 1
#define MONTE_CARLO_SIMULATIONS 1000

typedef enum {
    HIGH_CARD = 0,
    PAIR = 1,
    TWO_PAIR = 2,
    THREE_OF_A_KIND = 3,
    STRAIGHT = 4,
    FLUSH = 5,
    FULL_HOUSE = 6,
    FOUR_OF_A_KIND = 7,
    STRAIGHT_FLUSH = 8,
    ROYAL_FLUSH = 9
} Combination;

typedef struct {
    int rank;  // 2-14
    int suit;  // 0-3
} Card;

typedef struct {
    Combination combination;
    int highest_card;
    int second_highest_card;
    int third_highest_card;
    int fourth_highest_card;
    int fifth_highest_card;
} HandEvaluation;

#ifdef __cplusplus
extern "C" {
#endif

extern Card deck[NUM_CARDS];
extern const char *ranks[];
extern const char *suits[];

void initialize_deck();
void shuffle_deck();
void deal_cards(Card *source_deck, int *current_card, Card *hand, int size);
HandEvaluation evaluate_hand(Card *hand, Card *community_cards, int community_size);
int compare_evaluations(HandEvaluation eval1, HandEvaluation eval2);
bool is_hand_better(Card *hand1, Card *hand2, Card *community_cards, int community_size);
int calculate_win_probability(Card *player_hand, Card *community_cards, int community_size);
void print_card(Card card);
int find_straight_high(Card *cards, int num_cards);
bool is_straight_flush(Card *cards, int num_cards, int flush_suit, int *high_card);
const char* combination_name(Combination combo);

#ifdef __cplusplus
}
#endif


#endif
