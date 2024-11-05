#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>

using namespace std;

// Classe TicTacToe
class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo

public:
    TicTacToe() : current_player('O'), game_over(false), winner(' ') {
        // Inicializar o tabuleiro e as variáveis do jogo
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                board[i][j] = ' ';
            }
        }
    }

    void display_board() {
        // Exibir o tabuleiro no console
        std::this_thread::sleep_for(std::chrono::seconds(2));
        #ifdef _WIN32
            system("cls");  // Para Windows
        #else
            system("clear");  // Para Linux/Mac
        #endif
        cout << "\n";
        cout << "Jogador atual: " << current_player;
        cout << "\n";
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                cout << " " << board[i][j];
                if (j < 2) {
                    cout << " |";
                }
            }
            cout << "\n";
            if (i < 2) {
                cout << "---|---|---\n";
            }
        }
        cout << "\n";
    }

    bool make_move(char player, int row, int col) {
        // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        // Utilizar variável de condição para alternância de turnos
        std::unique_lock lock(board_mutex);
        
        while (current_player != player && !game_over) {
            turn_cv.wait(lock);
        }

        if (board[row][col] != ' ' || game_over) {
            return false;
        }

        board[row][col] = player;
        display_board();

        if (check_win(player)) {
            turn_cv.notify_all();
            return true;
        }

        if (check_draw()) {
            turn_cv.notify_all();
            return true;
        }

        current_player = (player == 'X') ? 'O' : 'X';
        turn_cv.notify_all();
        return true;
    }

    bool check_win(char player) {
        // Verificar se o jogador atual venceu o jogo
        for (int i = 0; i < 3; i++) {
            if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
                (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
                game_over = true;
                winner = player;
                return true;
            }
        }
        if((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
               (board[0][2] == player && board[1][1] == player && board[2][0] == player)){
                game_over = true;
                winner = player;
                return true;
        }
        return false;
    }

    bool check_draw() {
        // Verificar se houve um empate
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') return false;
            }
        }
        game_over = true;
        winner = 'D';
        return true;
    }

    bool is_game_over() {
        // Retornar se o jogo terminou
        return game_over;
    }

    char get_winner() {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        return winner;
    }
};

// Classe Player
class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

public:
    Player(TicTacToe& g, char s, std::string strat) 
        : game(g), symbol(s), strategy(strat) {}

    void play() {
        // Executar jogadas de acordo com a estratégia escolhida
        while (!game.is_game_over()) {
            if (strategy == "sequential") {
                play_sequential();
            } else if (strategy == "random") {
                play_random();
            }
             if (game.is_game_over()) {
                return;
            }
        }
    }

private:
    void play_sequential() {
        // Implementar a estratégia sequencial de jogadas
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (game.make_move(symbol, i, j)) {
                    return;
                }
            }
        }
    }

    void play_random() {
        // Implementar a estratégia aleatória de jogadas
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 2);
        while (true) {
            int row = distrib(gen);
            int col = distrib(gen);
            if (game.make_move(symbol, row, col) || game.is_game_over()) {
                return;
            }
        }
    }
};

// Função principal
int main() {
    // Inicializar o jogo e os jogadores

    // Criar as threads para os jogadores

    // Aguardar o término das threads

    // Exibir o resultado final do jogo
    TicTacToe jogoDaVelha;

    Player player_X(jogoDaVelha, 'X', "sequential");
    Player player_O(jogoDaVelha, 'O', "random");

    std::thread thread_X(&Player::play, &player_X);
    std::thread thread_O(&Player::play, &player_O);

    thread_X.join();
    thread_O.join();

    cout << "Fim de jogo!\n";

    char winner = jogoDaVelha.get_winner();
    if (winner == 'D') {
        cout << "O jogo terminou em empate.\n";
    } else {
        cout << "O jogador " << winner << " venceu o jogo!\n";
    }
    return 0;
}