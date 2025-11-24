USER INSTRUCTION:

# 1. Clone or download the project
git clone <repository>
cd C_project_pocer

# 2. Compile and run the game
make
./poker

# 3. Run tests
make test

# 4. Clean build files
make clean

SETUP ENVIRONMENT:

GCC (GNU Compiler Collection)
G++ (C++ compiler)
Google Test framework

INSTALLATION COMMANDS:

Linux(or WSL)

sudo apt update
sudo apt install gcc g++ make
sudo apt install libgtest-dev cmake

macOS:

brew install gcc make
brew install googletest

HIGH-LEVEL ARCHITECTURAL DIAGRAM:

┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   poker_main.c  │    │     poker.h      │    │  poker_tests.cpp│
│   (UI Layer)    │◄───┤ (Interface Layer)├───►│  (Test Layer)   │
│                 │    │                  │    │                 │
│ - User input    │    │ - Data structures│    │ - Unit tests    │
│ - Game flow     │    │ - Function proto │    │ - Validation    │
│ - Display       │    │ - Constants      │    │ - Verification  │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              ▲
                              │
                      ┌─────────────────┐
                      │    poker.c      │
                      │  (Logic Layer)  │
                      │                 │
                      │ - Game logic    │
                      │ - Card evaluation│
                      │ - Probability   │
                      └─────────────────┘

FUNCTION DESCRIPTION:

Deck Managment:

initialize_deck()  --  Creates 52-card deck with all suits/rank
shuffle_deck()  --  Randomizes card order using Fisher-Yates
deal_card()  --  Distributes cards to players

Hand Evaluation:

evaluate_hand()  --  Analyzes 7 cards (2 hand + 5 community)
compare_evaluations()  --  Compares two hand evaluations
is_hand_better()  --  Determines winning hand
combination_name()  --  Returns string name of combination

Probability:

calculate_win_probability()  --  Monte Carlo simulation (1000 iterations)
find_straight_high()  --  Identifies highest card in straight
is_straight_flush()  --  Checks for straight flush

UI Functions (poker_main.c):

print_game_state()  --  Displays current game status
get_player_action()  --  Gets user input (Continue/Fold)
determine_winner()  --  Shows final results

KEY IMPLEMENTATION DETAILS:

Monte Carlo Probability:

Method: Simulates 1000 random games
Process: Removes known cards, deals random opponents
Accuracy: Improves with more simulations
Output: Win percentage (0-100%)

Hand Ranking System:

Primary: Combination type (Pair > High Card)
Secondary: Highest card in combination
Tertiary: Kicker cards for tie-breaking

Card Representation:

Ranks: 2-14 (2-10, J=11, Q=12, K=13, A=14)
Suits: 0=Spades(♠), 1=Hearts(♥), 2=Diamonds(♦), 3=Clubs(♣)
Display: "10(H)" = 10 of Hearts

Unit Test Coverage:

Card deck initialization and shuffling
Hand evaluation and combination detection
Hand comparison logic
Probability calculation
Edge cases and boundary conditions

Performance Characteristics:

Time Complexity: O(n log n) for hand evaluation
Space Complexity: O(1) - fixed memory usage
Monte Carlo: O(1000 × n) for probability calculation
Real-time: Suitable for interactive gameplay
