#include "poker.h"

Card player_hand[HAND_SIZE];
Card computer_hand[HAND_SIZE];
Card community_cards[5];
int community_size = 0;

void print_hand(Card *hand, int size, const char *name) {
    printf("%s: ", name);
    for (int i = 0; i < size; i++) {
        print_card(hand[i]);
        printf(" ");
    }
    printf("\n");
}

void print_game_state(int stage) {
    printf("\n=== TEXAS HOLD'EM POKER ===\n");
    
    printf("Your hand: ");
    print_card(player_hand[0]);
    printf(" ");
    print_card(player_hand[1]);
    
    HandEvaluation player_eval = evaluate_hand(player_hand, community_cards, community_size);
    printf(" [%s]", combination_name(player_eval.combination));
    printf("\n");
    
    if (community_size > 0) {
        printf("Community cards: ");
        for (int i = 0; i < community_size; i++) {
            print_card(community_cards[i]);
            printf(" ");
        }
        printf("\n");
    }
    
    const char *stage_names[] = {"Pre-flop", "Flop", "Turn", "River"};
    printf("Stage: %s\n", stage_names[stage]);
    
    int win_prob = calculate_win_probability(player_hand, community_cards, community_size);
    printf("Win probability: %d%%\n", win_prob);
}

char get_player_action() {
    char action;
    printf("\nChoose action: (C)ontinue or (F)old? ");
    scanf(" %c", &action);
    return action;
}

void determine_winner() {
    printf("\n=== FINAL HANDS ===\n");
    
    HandEvaluation player_eval = evaluate_hand(player_hand, community_cards, 5);
    HandEvaluation computer_eval = evaluate_hand(computer_hand, community_cards, 5);
    
    printf("Your hand: ");
    print_card(player_hand[0]);
    printf(" ");
    print_card(player_hand[1]);
    printf(" [%s]", combination_name(player_eval.combination));
    printf("\n");
    
    printf("Computer hand: ");
    print_card(computer_hand[0]);
    printf(" ");
    print_card(computer_hand[1]);
    printf(" [%s]", combination_name(computer_eval.combination));
    printf("\n");
    
    printf("Community cards: ");
    for (int i = 0; i < 5; i++) {
        print_card(community_cards[i]);
        printf(" ");
    }
    printf("\n\n");
    
    int result = compare_evaluations(player_eval, computer_eval);
    
    if (result > 0) {
        printf("🎉 YOU WIN! 🎉\n");
    } else if (result < 0) {
        printf("💻 COMPUTER WINS!\n");
    } else {
        printf("🤝 IT'S A TIE! (Split pot)\n");
    }
}

int main() {
    srand(time(NULL));
    
    printf("TEXAS HOLD'EM POKER - LET'S HAVE FUN TODAY!\n");
    printf("Are you ready?\n");
    
    initialize_deck();
    shuffle_deck();
    
    int current_card = 0;
    
    deal_cards(deck, &current_card, player_hand, HAND_SIZE);
    deal_cards(deck, &current_card, computer_hand, HAND_SIZE);
    
    community_size = 0;
    print_game_state(0);
    
    char action = get_player_action();
    if (action == 'F' || action == 'f') {
        printf("You folded. Computer wins!\n");
        return 0;
    }
    
    deal_cards(deck, &current_card, community_cards, FLOP_SIZE);
    community_size = FLOP_SIZE;
    print_game_state(1);
    
    action = get_player_action();
    if (action == 'F' || action == 'f') {
        printf("You folded. Computer wins!\n");
        return 0;
    }
    
    deal_cards(deck, &current_card, &community_cards[FLOP_SIZE], TURN_RIVER_SIZE);
    community_size = FLOP_SIZE + TURN_RIVER_SIZE;
    print_game_state(2);
    
    action = get_player_action();
    if (action == 'F' || action == 'f') {
        printf("You folded. Computer wins!\n");
        return 0;
    }
    
    deal_cards(deck, &current_card, &community_cards[FLOP_SIZE + TURN_RIVER_SIZE], TURN_RIVER_SIZE);
    community_size = 5;
    print_game_state(3);
    
    determine_winner();
    
    return 0;
}
