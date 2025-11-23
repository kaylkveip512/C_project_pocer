#include "poker.h"

Card deck[NUM_CARDS];
const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
const char *suits[] = {"(S)", "(H)", "(D)", "(C)"};

void initialize_deck() {
    int index = 0;
    for (int suit = 0; suit < NUM_SUITS; suit++) {
        for (int rank = 0; rank < NUM_RANKS; rank++) {
            deck[index].rank = rank + 2;
            deck[index].suit = suit;
            index++;
        }
    }
}

void shuffle_deck() {
    for (int i = NUM_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void deal_cards(Card *source_deck, int *current_card, Card *hand, int size) {
    for (int i = 0; i < size; i++) {
        hand[i] = source_deck[*current_card];
        (*current_card)++;
    }
}

void print_card(Card card) {
    printf("%s%s", ranks[card.rank-2], suits[card.suit]);
}

const char* combination_name(Combination combo) {
    switch(combo) {
        case HIGH_CARD: return "High Card";
        case PAIR: return "Pair";
        case TWO_PAIR: return "Two Pair";
        case THREE_OF_A_KIND: return "Three of a Kind";
        case STRAIGHT: return "Straight";
        case FLUSH: return "Flush";
        case FULL_HOUSE: return "Full House";
        case FOUR_OF_A_KIND: return "Four of a Kind";
        case STRAIGHT_FLUSH: return "Straight Flush";
        case ROYAL_FLUSH: return "Royal Flush";
        default: return "Unknown";
    }
}

int compare_cards(const void *a, const void *b) {
    Card *cardA = (Card *)a;
    Card *cardB = (Card *)b;
    return cardB->rank - cardA->rank;
}

int find_straight_high(Card *cards, int num_cards) {
    int ranks[15] = {0};
    
    for (int i = 0; i < num_cards; i++) {
        ranks[cards[i].rank] = 1;
        if (cards[i].rank == 14) ranks[1] = 1;
    }
    
    for (int i = 14; i >= 5; i--) {
        if (ranks[i] && ranks[i-1] && ranks[i-2] && ranks[i-3] && ranks[i-4]) {
            return i;
        }
    }
    
    if (ranks[14] && ranks[2] && ranks[3] && ranks[4] && ranks[5]) {
        return 5;
    }
    
    return -1;
}

bool is_straight_flush(Card *cards, int num_cards, int flush_suit, int *high_card) {
    Card suited_cards[num_cards];
    int suited_count = 0;
    
    for (int i = 0; i < num_cards; i++) {
        if (cards[i].suit == flush_suit) {
            suited_cards[suited_count++] = cards[i];
        }
    }
    
    if (suited_count < 5) return false;
    
    *high_card = find_straight_high(suited_cards, suited_count);
    return (*high_card != -1);
}

HandEvaluation evaluate_hand(Card *hand, Card *community_cards, int community_size) {
    Card all_cards[7];
    HandEvaluation eval = {HIGH_CARD, 0, 0, 0, 0, 0};
    
    int total_cards = 0;
    for (int i = 0; i < HAND_SIZE; i++) all_cards[total_cards++] = hand[i];
    for (int i = 0; i < community_size; i++) all_cards[total_cards++] = community_cards[i];
    
    qsort(all_cards, total_cards, sizeof(Card), compare_cards);
    
    int rank_count[15] = {0};
    int suit_count[4] = {0};
    
    for (int i = 0; i < total_cards; i++) {
        rank_count[all_cards[i].rank]++;
        suit_count[all_cards[i].suit]++;
    }
    
    int flush_suit = -1;
    for (int suit = 0; suit < 4; suit++) {
        if (suit_count[suit] >= 5) {
            flush_suit = suit;
            break;
        }
    }
    
    int straight_high = find_straight_high(all_cards, total_cards);
    
    if (flush_suit != -1 && straight_high != -1) {
        if (is_straight_flush(all_cards, total_cards, flush_suit, &straight_high)) {
            eval.combination = (straight_high == 14) ? ROYAL_FLUSH : STRAIGHT_FLUSH;
            eval.highest_card = straight_high;
            return eval;
        }
    }
    
    for (int rank = 14; rank >= 2; rank--) {
        if (rank_count[rank] == 4) {
            eval.combination = FOUR_OF_A_KIND;
            eval.highest_card = rank;
            for (int i = 0; i < total_cards; i++) {
                if (all_cards[i].rank != rank) {
                    eval.second_highest_card = all_cards[i].rank;
                    break;
                }
            }
            return eval;
        }
    }
    
    int three_rank = -1, two_rank = -1;
    for (int rank = 14; rank >= 2; rank--) {
        if (rank_count[rank] == 3) {
            if (three_rank == -1) three_rank = rank;
            else if (two_rank == -1) two_rank = rank;
        } else if (rank_count[rank] == 2) {
            if (two_rank == -1) two_rank = rank;
        }
    }
    if (three_rank != -1 && two_rank != -1) {
        eval.combination = FULL_HOUSE;
        eval.highest_card = three_rank;
        eval.second_highest_card = two_rank;
        return eval;
    }
    
    if (flush_suit != -1) {
        eval.combination = FLUSH;
        int flush_cards = 0;
        for (int i = 0; i < total_cards && flush_cards < 5; i++) {
            if (all_cards[i].suit == flush_suit) {
                switch(flush_cards) {
                    case 0: eval.highest_card = all_cards[i].rank; break;
                    case 1: eval.second_highest_card = all_cards[i].rank; break;
                    case 2: eval.third_highest_card = all_cards[i].rank; break;
                    case 3: eval.fourth_highest_card = all_cards[i].rank; break;
                    case 4: eval.fifth_highest_card = all_cards[i].rank; break;
                }
                flush_cards++;
            }
        }
        return eval;
    }
    
    if (straight_high != -1) {
        eval.combination = STRAIGHT;
        eval.highest_card = straight_high;
        return eval;
    }
    
    for (int rank = 14; rank >= 2; rank--) {
        if (rank_count[rank] == 3) {
            eval.combination = THREE_OF_A_KIND;
            eval.highest_card = rank;
            int kickers = 0;
            for (int i = 0; i < total_cards && kickers < 2; i++) {
                if (all_cards[i].rank != rank) {
                    if (kickers == 0) eval.second_highest_card = all_cards[i].rank;
                    else eval.third_highest_card = all_cards[i].rank;
                    kickers++;
                }
            }
            return eval;
        }
    }
    
    int pairs[2] = {-1, -1};
    for (int rank = 14; rank >= 2; rank--) {
        if (rank_count[rank] == 2) {
            if (pairs[0] == -1) pairs[0] = rank;
            else if (pairs[1] == -1) pairs[1] = rank;
        }
    }
    if (pairs[0] != -1 && pairs[1] != -1) {
        eval.combination = TWO_PAIR;
        eval.highest_card = pairs[0];
        eval.second_highest_card = pairs[1];
        for (int i = 0; i < total_cards; i++) {
            if (all_cards[i].rank != pairs[0] && all_cards[i].rank != pairs[1]) {
                eval.third_highest_card = all_cards[i].rank;
                break;
            }
        }
        return eval;
    }
    
    for (int rank = 14; rank >= 2; rank--) {
        if (rank_count[rank] == 2) {
            eval.combination = PAIR;
            eval.highest_card = rank;
            int kickers = 0;
            for (int i = 0; i < total_cards && kickers < 3; i++) {
                if (all_cards[i].rank != rank) {
                    switch(kickers) {
                        case 0: eval.second_highest_card = all_cards[i].rank; break;
                        case 1: eval.third_highest_card = all_cards[i].rank; break;
                        case 2: eval.fourth_highest_card = all_cards[i].rank; break;
                    }
                    kickers++;
                }
            }
            return eval;
        }
    }
    
    eval.combination = HIGH_CARD;
    eval.highest_card = all_cards[0].rank;
    eval.second_highest_card = all_cards[1].rank;
    eval.third_highest_card = all_cards[2].rank;
    eval.fourth_highest_card = all_cards[3].rank;
    eval.fifth_highest_card = all_cards[4].rank;
    
    return eval;
}

int compare_evaluations(HandEvaluation eval1, HandEvaluation eval2) {
    if (eval1.combination != eval2.combination) {
        return eval1.combination - eval2.combination;
    }
    
    if (eval1.highest_card != eval2.highest_card) {
        return eval1.highest_card - eval2.highest_card;
    }
    
    if (eval1.second_highest_card != eval2.second_highest_card) {
        return eval1.second_highest_card - eval2.second_highest_card;
    }
    
    if (eval1.third_highest_card != eval2.third_highest_card) {
        return eval1.third_highest_card - eval2.third_highest_card;
    }
    
    if (eval1.fourth_highest_card != eval2.fourth_highest_card) {
        return eval1.fourth_highest_card - eval2.fourth_highest_card;
    }
    
    return eval1.fifth_highest_card - eval2.fifth_highest_card;
}

bool is_hand_better(Card *hand1, Card *hand2, Card *community_cards, int community_size) {
    HandEvaluation eval1 = evaluate_hand(hand1, community_cards, community_size);
    HandEvaluation eval2 = evaluate_hand(hand2, community_cards, community_size);
    return compare_evaluations(eval1, eval2) > 0;
}

int calculate_win_probability(Card *player_hand, Card *community_cards, int community_size) {
    int wins = 0, ties = 0;
    
    for (int sim = 0; sim < MONTE_CARLO_SIMULATIONS; sim++) {
        Card temp_deck[NUM_CARDS];
        int temp_deck_size = 0;
        
        for (int i = 0; i < NUM_CARDS; i++) {
            bool card_used = false;
            
            for (int j = 0; j < HAND_SIZE; j++) {
                if (deck[i].rank == player_hand[j].rank && deck[i].suit == player_hand[j].suit) {
                    card_used = true;
                    break;
                }
            }
            
            for (int j = 0; j < community_size; j++) {
                if (deck[i].rank == community_cards[j].rank && deck[i].suit == community_cards[j].suit) {
                    card_used = true;
                    break;
                }
            }
            
            if (!card_used) {
                temp_deck[temp_deck_size++] = deck[i];
            }
        }
        
        for (int i = temp_deck_size - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Card temp = temp_deck[i];
            temp_deck[i] = temp_deck[j];
            temp_deck[j] = temp;
        }
        
        Card opponent_hand[HAND_SIZE];
        Card simulated_community[5];
        memcpy(simulated_community, community_cards, community_size * sizeof(Card));
        
        int current_card = 0;
        deal_cards(temp_deck, &current_card, opponent_hand, HAND_SIZE);
        
        for (int i = community_size; i < 5; i++) {
            simulated_community[i] = temp_deck[current_card++];
        }
        
        HandEvaluation player_eval = evaluate_hand(player_hand, simulated_community, 5);
        HandEvaluation opponent_eval = evaluate_hand(opponent_hand, simulated_community, 5);
        
        int result = compare_evaluations(player_eval, opponent_eval);
        if (result > 0) wins++;
        else if (result == 0) ties++;
    }
    
    return (wins * 100 + ties * 50) / MONTE_CARLO_SIMULATIONS;
}
