#include <gtest/gtest.h>
#include <vector>
#include <string>

extern "C" {
    #include "poker.h"
}

class PokerTest : public ::testing::Test {
protected:
    void SetUp() override {
        initialize_deck();
        srand(42);
    }

    bool cards_equal(Card card1, Card card2) {
        return (card1.rank == card2.rank && card1.suit == card2.suit);
    }

    bool is_deck_unique() {
        for (int i = 0; i < NUM_CARDS; i++) {
            for (int j = i + 1; j < NUM_CARDS; j++) {
                if (cards_equal(deck[i], deck[j])) {
                    return false;
                }
            }
        }
        return true;
    }
};

TEST_F(PokerTest, DeckInitializationCreatesUniqueCards) {
    EXPECT_TRUE(is_deck_unique()) << "Всі карти в колоді повинні бути унікальними";
}

TEST_F(PokerTest, DealCardsProvidesCorrectNumberOfCards) {
    shuffle_deck();
    int current_card = 0;
    Card test_hand1[HAND_SIZE];
    Card test_hand2[HAND_SIZE];
    
    deal_cards(deck, &current_card, test_hand1, HAND_SIZE);
    deal_cards(deck, &current_card, test_hand2, HAND_SIZE);
    
    for (int i = 0; i < HAND_SIZE; i++) {
        EXPECT_GE(test_hand1[i].rank, 2) << "Ранг карти повинен бути >= 2";
        EXPECT_LE(test_hand1[i].rank, 14) << "Ранг карти повинен бути <= 14";
        EXPECT_GE(test_hand1[i].suit, 0) << "Масть карти повинна бути >= 0";
        EXPECT_LE(test_hand1[i].suit, 3) << "Масть карти повинна бути <= 3";
    }
}

TEST_F(PokerTest, PlayersReceiveDifferentCards) {
    shuffle_deck();
    int current_card = 0;
    Card player_hand[HAND_SIZE];
    Card computer_hand[HAND_SIZE];
    
    deal_cards(deck, &current_card, player_hand, HAND_SIZE);
    deal_cards(deck, &current_card, computer_hand, HAND_SIZE);
    
    bool cards_are_different = true;
    for (int i = 0; i < HAND_SIZE; i++) {
        for (int j = 0; j < HAND_SIZE; j++) {
            if (cards_equal(player_hand[i], computer_hand[j])) {
                cards_are_different = false;
            }
        }
    }
    
    EXPECT_TRUE(cards_are_different) << "Гравці не повинні отримувати однакові карти";
}

TEST_F(PokerTest, StrongHandHasHighWinProbability) {
    Card strong_hand[HAND_SIZE] = {{14, 0}, {14, 1}};
    Card no_community[5] = {{0, 0}};
    
    int probability = calculate_win_probability(strong_hand, no_community, 0);
    
    EXPECT_GT(probability, 70) << "Пара тузів повинна мати високу ймовірність виграшу (>70%)";
}

TEST_F(PokerTest, WeakHandHasLowWinProbability) {
    Card weak_hand[HAND_SIZE] = {{2, 0}, {7, 1}};
    Card no_community[5] = {{0, 0}};
    
    int probability = calculate_win_probability(weak_hand, no_community, 0);
    
    EXPECT_LT(probability, 40) << "Слабка рука повинна мати низьку ймовірність виграшу (<40%)";
}

TEST_F(PokerTest, ProbabilityCalculationHandlesDifferentScenarios) {
    Card hand[HAND_SIZE] = {{10, 0}, {10, 1}};
    Card community[3] = {{2, 0}, {5, 1}, {8, 2}};
    
    int probability = calculate_win_probability(hand, community, 3);
    
    EXPECT_GE(probability, 0) << "Ймовірність не може бути від'ємною";
    EXPECT_LE(probability, 100) << "Ймовірність не може перевищувати 100%";
}

TEST_F(PokerTest, PairBeatsHighCard) {
    Card pair_hand[HAND_SIZE] = {{10, 0}, {10, 1}};
    Card high_card_hand[HAND_SIZE] = {{14, 0}, {9, 1}};
    Card community[5] = {{2, 0}, {5, 1}, {7, 2}, {8, 3}, {3, 0}};
    
    bool result = is_hand_better(pair_hand, high_card_hand, community, 5);
    
    EXPECT_TRUE(result) << "Пара повинна перемагати старшу карту";
}

TEST_F(PokerTest, FlushBeatsStraight) {
    Card flush_hand[HAND_SIZE] = {{2, 0}, {6, 0}};
    Card straight_hand[HAND_SIZE] = {{7, 1}, {8, 2}};
    Card community[5] = {{3, 0}, {4, 0}, {5, 0}, {9, 0}, {10, 1}};
    
    bool result = is_hand_better(flush_hand, straight_hand, community, 5);
    
    EXPECT_TRUE(result) << "Флеш повинен перемагати стріт";
}

TEST_F(PokerTest, TwoPairBeatsOnePair) {
    Card two_pair_hand[HAND_SIZE] = {{10, 0}, {9, 1}};
    Card one_pair_hand[HAND_SIZE] = {{8, 0}, {7, 1}};
    Card community[5] = {{10, 2}, {9, 3}, {5, 0}, {3, 1}, {2, 2}};
    
    bool result = is_hand_better(two_pair_hand, one_pair_hand, community, 5);
    
    EXPECT_TRUE(result) << "Дві пари повинні перемагати одну пару";
}

TEST_F(PokerTest, IdentifiesRoyalFlush) {
    Card hand[HAND_SIZE] = {{14, 0}, {13, 0}};
    Card community[5] = {{12, 0}, {11, 0}, {10, 0}, {5, 1}, {2, 2}};
    
    HandEvaluation eval = evaluate_hand(hand, community, 5);
    
    EXPECT_EQ(eval.combination, ROYAL_FLUSH) << "Повинен бути ідентифікований роял-флеш";
    EXPECT_EQ(eval.highest_card, 14) << "Старша карта роял-флеша повинна бути тузом";
}

TEST_F(PokerTest, IdentifiesStraight) {
    Card hand[HAND_SIZE] = {{10, 0}, {11, 1}};
    Card community[5] = {{12, 2}, {13, 3}, {14, 0}, {5, 1}, {2, 2}};
    
    HandEvaluation eval = evaluate_hand(hand, community, 5);
    
    EXPECT_EQ(eval.combination, STRAIGHT) << "Повинен бути ідентифікований стріт";
    EXPECT_EQ(eval.highest_card, 14) << "Старша карта стріту повинна бути тузом";
}

TEST_F(PokerTest, IdentifiesFullHouse) {
    Card hand[HAND_SIZE] = {{8, 0}, {8, 1}};
    Card community[5] = {{8, 2}, {5, 0}, {5, 1}, {10, 2}, {2, 3}};
    
    HandEvaluation eval = evaluate_hand(hand, community, 5);
    
    EXPECT_EQ(eval.combination, FULL_HOUSE) << "Повинен бути ідентифікований фул-хаус";
}

TEST_F(PokerTest, IdentifiesFourOfAKind) {
    Card hand[HAND_SIZE] = {{7, 0}, {7, 1}};
    Card community[5] = {{7, 2}, {7, 3}, {10, 0}, {5, 1}, {2, 2}};
    
    HandEvaluation eval = evaluate_hand(hand, community, 5);
    
    EXPECT_EQ(eval.combination, FOUR_OF_A_KIND) << "Повинно бути ідентифіковане каре";
}

TEST_F(PokerTest, FindStraightHighIdentifiesHighestCard) {
    Card cards[5] = {{10, 0}, {11, 1}, {12, 2}, {13, 3}, {14, 0}};
    
    int high_card = find_straight_high(cards, 5);
    
    EXPECT_EQ(high_card, 14) << "Функція повинна знаходити туза як найвищу карту стріту";
}

TEST_F(PokerTest, IsStraightFlushIdentifiesCorrectly) {
    Card cards[5] = {{10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}};
    int flush_suit = 0;
    int high_card;
    
    bool result = is_straight_flush(cards, 5, flush_suit, &high_card);
    
    EXPECT_TRUE(result) << "Повинен бути виявлений стрейт-флеш";
    EXPECT_EQ(high_card, 14) << "Старша карта стрейт-флеша повинна бути тузом";
}

TEST_F(PokerTest, EmptyCommunityCards) {
    Card hand[HAND_SIZE] = {{10, 0}, {11, 1}};
    Card empty_community[5] = {{0, 0}};
    
    HandEvaluation eval = evaluate_hand(hand, empty_community, 0);
    
    EXPECT_EQ(eval.combination, HIGH_CARD) << "Без спільних карт повинна бути старша карта";
}

TEST_F(PokerTest, SameHandsAreEqual) {
    Card hand1[HAND_SIZE] = {{10, 0}, {11, 1}};
    Card hand2[HAND_SIZE] = {{10, 0}, {11, 1}};
    Card community[5] = {{2, 0}, {3, 1}, {4, 2}, {5, 3}, {6, 0}};
    
    HandEvaluation eval1 = evaluate_hand(hand1, community, 5);
    HandEvaluation eval2 = evaluate_hand(hand2, community, 5);
    
    int result = compare_evaluations(eval1, eval2);
    
    EXPECT_EQ(result, 0) << "Ідентичні руки повинні бути рівними";
}

TEST_F(PokerTest, HandComparison_PairVsHighCard) {
    Card pair_hand[HAND_SIZE] = {{10, 0}, {10, 1}};
    Card high_card_hand[HAND_SIZE] = {{14, 2}, {9, 3}};
    
    Card community[3] = {{2, 0}, {3, 1}, {4, 2}};
    
    bool result = is_hand_better(pair_hand, high_card_hand, community, 3);
    EXPECT_TRUE(result) << "Пара повинна перемагати старшу карту";
}

TEST_F(PokerTest, HandComparison_FlushVsStraight) {
    Card flush_hand[HAND_SIZE] = {{2, 0}, {6, 0}};
    Card straight_hand[HAND_SIZE] = {{7, 1}, {8, 2}};
    Card community[5] = {{3, 0}, {4, 0}, {5, 0}, {9, 0}, {10, 1}};
    
    bool result = is_hand_better(flush_hand, straight_hand, community, 5);
    EXPECT_TRUE(result) << "Флеш повинен перемагати стріт";
}

TEST_F(PokerTest, CombinationNamesAreCorrect) {
    EXPECT_STREQ(combination_name(HIGH_CARD), "High Card");
    EXPECT_STREQ(combination_name(PAIR), "Pair");
    EXPECT_STREQ(combination_name(TWO_PAIR), "Two Pair");
    EXPECT_STREQ(combination_name(THREE_OF_A_KIND), "Three of a Kind");
    EXPECT_STREQ(combination_name(STRAIGHT), "Straight");
    EXPECT_STREQ(combination_name(FLUSH), "Flush");
    EXPECT_STREQ(combination_name(FULL_HOUSE), "Full House");
    EXPECT_STREQ(combination_name(FOUR_OF_A_KIND), "Four of a Kind");
    EXPECT_STREQ(combination_name(STRAIGHT_FLUSH), "Straight Flush");
    EXPECT_STREQ(combination_name(ROYAL_FLUSH), "Royal Flush");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
