#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


using namespace std;
#define MAX_PLAYER_CARDS 2000
#define MAX_INPUT_TEXT_IN_LINE 100000
#define NUMBER_OF_COLORS 7
#define MAX_LETTERS_OF_COLORS 7

struct card {
    int value;
    int color_number;
};

void cout_color(int color_number, FILE* state) {
    int color_name_length[NUMBER_OF_COLORS] = { 5, 4, 3, 6, 6, 5, 5 };//ilość liter kolorów po kolei
    char color[NUMBER_OF_COLORS][MAX_LETTERS_OF_COLORS] = { {"green" }, { "blue" }, { "red" }, { "violet" }, { "yellow" }, { "white" }, { "black" } };

    for (int j = 0; j < color_name_length[color_number]; j++)
        fprintf(state, "%c", color[color_number][j]);//wypisujemy każdą literę po kolei chcianego koloru
}


void randomize(card* deck, int o, int k, int g) {
    srand(time(NULL));

    int i1, i2; card x;
    for (int i = 0; i < (o * k + g); i++)
    {
        i1 = rand() % (o * k + g);
        i2 = rand() % (o * k + g);

        x = deck[i1];
        deck[i1] = deck[i2];
        deck[i2] = x;
    }
}


void create_deck(int k, int g, int gv, int o, int* number, card* deck) {
    for (int i = 0; i < g; i++) {//najpierw tworzymy karty zielone 
        deck[i].value = gv;
        deck[i].color_number = 0;//kolory będziemy oznaczać cyframi (0-green, 1-blue, 2-red...) a przy wypisywaniu używać funkcji cout_color
    }

    int pom = g;//aby nasze następne karty nie zastąpiły już wpisanych ustawiamy wartość na g, jest to pierwszy wolny indeks
    for (int j = 1; j <= k; j++) {//tworzymy karty nie zielone, gdy utworzymy wszystkie karty o danym kolorze zmieniamy kolor na następny
        for (int i = 0; i < o; i++) {
            deck[pom].value = number[i];
            deck[pom].color_number = j;
            pom++;
        }
    }
    randomize(deck, o, k, g);
}


void card_deal(int o, int k, int g, int n, card* deck, card** players_hand) {
    int pom = 0;
    for (int i = 0; i < (o * k + g) / n + n; i++) {
        for (int j = 0; j < n; j++) {
            if (pom < o * k + g) {//dopóki nie rozdamy wszystich kart rozdajemy każdemu graczowi po kolei jedną kartę
                players_hand[j][i] = deck[pom];
                pom++;
            }
            else
                break;
        }
    }
}


int explosion_threshold(char* m, int i, int j) {//nie wiadomo ile cyfr będzie miało e, dlatego przy każdej kolejnej cyfrze liczbę już wcześniej utworzoną mnożymy przez 10 i dodajemy do niej kolejną cyfrę 
    if (m[i] <= 57 && m[i] >= 48)
        return explosion_threshold(m, i + 1, 10 * j + m[i] - 48);
    else
        return j;
}


void load_basic_info(int* active_player, int* n, char* m, int* e, FILE* state) {
    fgets(m, MAX_INPUT_TEXT_IN_LINE, state);
    *active_player = m[16] - 48;
    fgets(m, MAX_INPUT_TEXT_IN_LINE, state);
    *n = m[17] - 48;
    fgets(m, MAX_INPUT_TEXT_IN_LINE, state);
    *e = explosion_threshold(m, 23, m[22] - 48);
}


void load_cards(card* cards, char* m, int i, FILE* state) {
    if (i == 21) {//nie ma podanej ilości kociołków dlatego trzeba je wpisywać za pomocą while co koliduje z cin.getline użytym w tej funkcji, dla kociołków i = 14
        for (int g = 0; g < MAX_INPUT_TEXT_IN_LINE; g++)
            m[g] = ',';// "," jest to znak nie występujący nigdzie we wczytywanym tekście, ustawiamy ten znak aby móc ustalić ilość liter wczytanej linii tekstu
        fgets(m, MAX_INPUT_TEXT_IN_LINE, state);
    }

    int pom = 0, size = 0;
    while (m[size] != ',')
        size++;

    for (i; i < size; i++) {
        if (m[i] <= 57 && m[i] >= 48) {
            if (cards[pom].value != 0)
                cards[pom].value = cards[pom].value * 10 + m[i] - 48;
            else
                cards[pom].value = m[i] - 48;
        }
        else if (m[i] != ' ' && m[i - 1] == ' ') {//szukamy pierwszej litery koloru, w przypadku black i blue patrzymy na 3 literę aby je odróżnić
            if (m[i] == 'g')
                cards[pom].color_number = 0;
            if (m[i] == 'b' && m[i + 2] == 'u')
                cards[pom].color_number = 1;
            if (m[i] == 'r')
                cards[pom].color_number = 2;
            if (m[i] == 'v')
                cards[pom].color_number = 3;
            if (m[i] == 'y')
                cards[pom].color_number = 4;
            if (m[i] == 'w')
                cards[pom].color_number = 5;
            if (m[i] == 'b' && m[i + 2] == 'a')
                cards[pom].color_number = 6;
            pom++;

        }
    }
}


void load_game(int n, card** players_hand, card** players_deck, char* m, card** piles_cards, int* piles_amount, FILE* state) {
    for (int j = 0; j < n; j++) {
        load_cards(players_hand[j], m, 21, state);//od 22 litery zaczynają się karty
        load_cards(players_deck[j], m, 21, state);
    }

    for (int g = 0; g < MAX_INPUT_TEXT_IN_LINE; g++)
        m[g] = ',';

    while (fgets(m, MAX_INPUT_TEXT_IN_LINE, state)) {
        load_cards(piles_cards[*piles_amount], m, 14, state);//dla kociołków od 15 litery zaczynają się karty
        *piles_amount = *piles_amount + 1;
        for (int g = 0; g < MAX_INPUT_TEXT_IN_LINE; g++)
            m[g] = ',';
    }

}


void green_cards(int n, card** cards, int* value, int* amount) {
    for (int i = 0; i < n; i++) {
        int j = 0;
        while (cards[i][j].value != 0) {
            if (cards[i][j].color_number == 0) {
                if (*value == 0)//nie wystąpiła jeszcze żadna wartość zielonej karty
                    *value = cards[i][j].value;
                else if (cards[i][j].value != *value)
                    *value = -1;//wystąpiły różne wartości kart
                *amount = *amount + 1;
            }
            j++;
        }
    }
}


void check_green(card** players_hand, card** players_deck, int n, card** piles_cards, int piles_amount, bool* end) {
    int green_cards_value = 0, green_cards_amount = 0;

    green_cards(n, players_hand, &green_cards_value, &green_cards_amount);
    green_cards(n, players_deck, &green_cards_value, &green_cards_amount);
    green_cards(piles_amount, piles_cards, &green_cards_value, &green_cards_amount);
    //szukamy zielonych kart w każdej tablicy kart

    if (green_cards_value == -1) {
        cout << "Different green cards values occurred";
        *end = true;
    }
}

int count_color_check(int n, card** cards, int color) {
    int number = 0;

    for (int i = 0; i < n; i++) {
        int j = 0;
        while (cards[i][j].value != 0) {
            if (cards[i][j].color_number == color)
                number++;
            j++;
        }
    }

    return number;
}


void check_card_amount(card** players_hand, card** players_deck, int n, card** piles_cards, int piles_amount, bool* end) {
    int colors_amount[NUMBER_OF_COLORS] = { 0 };//w tej tablicy nie będziemy używać 0 indeksu, tworzymy go w tablicy tylko po to aby łatwiej porównywać kolory (1 indeks będzie przechowywał ilość niebieskiego koloru a kolor niebieski oznaczany jest liczbą 1 itp)
    bool different = false;

    for (int i = 1; i < NUMBER_OF_COLORS; i++) {
        colors_amount[i] = count_color_check(n, players_hand, i) + count_color_check(n, players_deck, i) + count_color_check(piles_amount, piles_cards, i);
        if (colors_amount[i] != colors_amount[i - 1] && colors_amount[i - 1] != 0 && colors_amount[i] != 0 && i - 1 != 0)
            different = true;
    }

    if (different == true) {
        cout << "At least two colors with a different number of cards were found:" << endl;
        for (int i = 1; i < NUMBER_OF_COLORS; i++) {
            if (colors_amount[i] != 0) {
                cout_color(i, stdout);
                cout << " cards are " << colors_amount[i] << endl;
            }
        }
        *end = true;
    }
}


int cmp(const void* a, const void* b) {
    int arg1 = *(int*)a;
    int arg2 = *(int*)b;
    if (arg2 == 0)
        return -1;
    if (arg1 == 0)
        return 1;
    return arg1 - arg2;
}


void sort_values(int color_values[NUMBER_OF_COLORS][MAX_PLAYER_CARDS]) {
    for (int i = 1; i < NUMBER_OF_COLORS; i++) {
        if (color_values[i][0] != 0)
            qsort(color_values[i], MAX_PLAYER_CARDS, sizeof(int), cmp);
    }
}


void values(card** piles_cards, card** players_hand, card** players_deck, int color_values[NUMBER_OF_COLORS][MAX_PLAYER_CARDS], int n, int piles_amount) {
    int index[NUMBER_OF_COLORS] = { 0 };//tablica służy nam do pokazywania najbliższego pustego indeksu w danym kolorze aby w tablicy color_values nie zastępować już wpisanych liczb

    for (int j = 0; j < n; j++) {
        int i = 0;
        while (players_hand[j][i].value != 0 || players_deck[j][i].value != 0) {
            if (players_hand[j][i].color_number != 0) {
                color_values[players_hand[j][i].color_number][index[players_hand[j][i].color_number]] = players_hand[j][i].value;
                index[players_hand[j][i].color_number]++;
            }
            if (players_deck[j][i].color_number != 0) {
                color_values[players_deck[j][i].color_number][index[players_deck[j][i].color_number]] = players_deck[j][i].value;
                index[players_deck[j][i].color_number]++;
            }
            i++;
        }
    }

    for (int j = 0; j < piles_amount; j++) {
        int i = 0;
        while (piles_cards[j][i].value != 0) {
            if (piles_cards[j][i].color_number != 0) {
                color_values[piles_cards[j][i].color_number][index[piles_cards[j][i].color_number]] = piles_cards[j][i].value;
                index[piles_cards[j][i].color_number]++;
            }
            i++;
        }
    }
    sort_values(color_values);
}


bool check_if_identical(int color_values[NUMBER_OF_COLORS][MAX_PLAYER_CARDS]) {

    for (int i = 2; i < NUMBER_OF_COLORS; i++) {
        int j = 0;
        while (color_values[i][j] != 0)
        {
            if (color_values[i - 1][j] != color_values[i][j] && color_values[i - 1][0] != 0)
                return false;
            j++;
        }
    }

    return true;
}


void check_cards_values(card** players_hand, card** players_deck, int n, card** piles_cards, int piles_amount, bool* end) {
    int color_values[NUMBER_OF_COLORS][MAX_PLAYER_CARDS] = { 0 };//tablica będzie przechowywać występujące wartościdla każdego koloru (oprócz zielonego) 

    values(piles_cards, players_hand, players_deck, color_values, n, piles_amount);

    if (check_if_identical(color_values) == false) {
        cout << "The values of cards of all colors are not identical:" << endl;
        for (int i = 1; i < NUMBER_OF_COLORS; i++) {
            if (color_values[i][0] != 0) {
                cout_color(i, stdout);
                cout << " cards values: ";
                int j = 0;
                while (color_values[i][j] != 0) {
                    cout << color_values[i][j] << " ";
                    j++;
                }
                cout << endl;
            }
        }
        *end = true;
    }
}


bool different_colors(card* pile_cards) {
    int i = 1;

    while (pile_cards[i].value != 0) {
        if (pile_cards[i].color_number != pile_cards[i - 1].color_number && pile_cards[i].color_number != 0 && pile_cards[i - 1].color_number != 0)
            return true;
        i++;
    }

    return false;
}


int cards_amount(card* players_hand) {

    int pom = 0;
    while (players_hand[pom].value != 0)
        pom++;

    return pom;
}


bool cards_not_correct(card** players_hand, int n, int a_p) {
    int* player_cards_amount;
    player_cards_amount = new int[n];

    for (int i = 0; i < n; i++)
        player_cards_amount[i] = cards_amount(players_hand[i]);

    for (int i = n - 1; i > 0; i--) {
        if (i + 1 == a_p && player_cards_amount[i] != player_cards_amount[i - 1] + 1 && player_cards_amount[i] != player_cards_amount[i - 1])
            return true;//gracz który wykonuje ruch ma albo o jedną kartę więcej od gracza który już wykonał ruch albo mają po równo biorąc pod uwagę możliwość niesprawiedliwego podziału kart na początku
        else if (i + 1 != a_p && player_cards_amount[i] != player_cards_amount[i - 1] - 1 && player_cards_amount[i] != player_cards_amount[i - 1])
            return true;
    }

    return false;
}


bool if_explosion(card* pile_cards, int e, int pom) {
    int i = 0;

    while (pile_cards[i].value != 0) {
        pom += pile_cards[i].value;
        i++;
    }

    if (pom > e)
        return true;
    else
        return false;
}


void check_state(card** piles_cards, card** players_hand, int n, int piles_amount, int e, int active_player, bool* end) {
    bool mistakes = false;

    if (cards_not_correct(players_hand, n, active_player) == true) {
        cout << "The number of players cards on hand is wrong" << endl;
        mistakes = true;
    }

    for (int i = 0; i < piles_amount; i++) {
        if (different_colors(piles_cards[i]) == true) {
            cout << "Two different colors were found on the " << i + 1 << " pile" << endl;
            mistakes = true;
        }
    }

    for (int i = 0; i < piles_amount; i++) {
        if (if_explosion(piles_cards[i], e, 0) == true) {
            cout << "Pile number " << i + 1 << " should explode earlier" << endl;
            mistakes = true;
        }
    }

    if (mistakes == true)
        *end = true;
}


void explosion(card* pile, card* deck) {

    int i = 0;
    while (deck[i].value != 0)
        i++;

    int j = 0;
    while (pile[j].value != 0) {
        deck[i + j] = pile[j];
        pile[j].value = 0;
        pile[j].color_number = 0;
        j++;
    }
}


void add_to_pile(card* pile_cards, card card) {

    int j = 0;
    while (pile_cards[j].value != 0)
        j++;

    pile_cards[j].color_number = card.color_number;
    pile_cards[j].value = card.value;
}


int count_values(card* cards) {
    int pom = 0;

    int i = 0;
    while (cards[i].value != 0) {
        pom += cards[i].value;
        i++;
    }

    return pom;
}


int count_color(card* cards, int color) {
    int number = 0;
    int j = 0;
    while (cards[j].value != 0) {
        if (cards[j].color_number == color)
            number++;
        j++;
    }

    return number;
}


int select_min_value(card* card) {
    int min_value = card[0].value, index = 0;

    int i = 1;
    while (card[i].value != 0) {
        if (min_value > card[i].value) {
            min_value = card[i].value;
            index = i;
        }
        i++;
    }

    return index;
}

int select_max_value(card* card) {
    int max_value = card[0].value, index = 0;

    int i = 1;
    while (card[i].value != 0) {
        if (max_value < card[i].value) {
            max_value = card[i].value;
            index = i;
        }
        i++;
    }

    return index;
}


int select_pile(card** piles_cards, int player_card_color, int amount, bool search_min_value) {
    int select = 0;
    int pile_amount_plus_end = amount + 1; //wielkość tablic musi być o 1 większa aby wartość pod ostatnim indeksem równała się 0
    card* pile;//sumy wartości kart w kociołkach (zadanie wybór najmniejszej karty)
    card* pile_min_amount;//ilość kart w kociołkach (zadanie pozbywanie się największych kart)
    pile = new card[pile_amount_plus_end]();
    pile_min_amount = new card[pile_amount_plus_end]();
    //powyższe tablice są tablicami struktury card aby ich typ pasował do wcześniej napisanych funkcji

    for (int i = 0; i < amount; i++) {
        if (i < pile_amount_plus_end) {
            if (player_card_color != 0) {
                pile_min_amount[i].value = -1;
                pile[i].value = -1;
                //tylko ostatnia wartość tablic może się równać 0 (wartość karty równa 0 oznacza koniec danych w tablicy)

                int j = 0;
                while (piles_cards[i][j].value != 0) {
                    if (player_card_color == piles_cards[i][j].color_number) {
                        delete[] pile;
                        return i;
                    }
                    if (piles_cards[i][j].color_number != 0) {//ustawiamy duże wartości aby potem wyszukując najmniejszej nie trafić do kociołka na który nie może pójść karta gracza
                        pile[i].value = 10000;
                        pile_min_amount[i].value = 10000;
                        break;
                    }
                    pile_min_amount[i].value = cards_amount(piles_cards[i]);
                    pile[i].value = count_values(piles_cards[i]);

                    j++;
                }
            }
            else {
                pile[i].value = count_values(piles_cards[i]);
                pile_min_amount[i].value = cards_amount(piles_cards[i]);
            }
            if (pile[i].value == 0)//gdy jest pusty kociołek aby wartość nie była równa 0 ustawiamy ją na inną (wartość karty równa 0 oznacza koniec danych w tablicy)
                pile[i].value = -1;
            if (pile_min_amount[i].value == 0)//gdy jest pusty kociołek aby wartość nie była równa 0 ustawiamy ją na inną (wartość karty równa 0 oznacza koniec danych w tablicy)
                pile_min_amount[i].value = -1;
        }
    }

    for (int i = 0; i < amount; i++) {

        if (select_min_value(pile) == i && search_min_value == true) {
            select = i;
            break;
        }
        if (select_min_value(pile_min_amount) == i && search_min_value == false) {
            select = i;
            break;
        }
    }

    delete[] pile;
    return select;
}


void select(card* player_hand, int* selected_pile, int* index, card** piles_cards, int piles_amount, int e, bool search_min) {
    if (search_min == true)
        *index = select_min_value(player_hand);
    else
        *index = select_max_value(player_hand);
    int i = 0, min = 10000;

    while (player_hand[i].value != 0)
    {
        if (player_hand[i].value == player_hand[*index].value) {
            if (count_values(piles_cards[select_pile(piles_cards, player_hand[i].color_number, piles_amount, search_min)]) < min && search_min == true) {
                //liczymy wartości kart w wybranym kociołku dla każdej karty o najmniejszej wartości i wybieramy taką dla której wartości kart w kociołku są najmniejsze
                min = count_values(piles_cards[select_pile(piles_cards, player_hand[i].color_number, piles_amount, search_min)]);
                *index = i;
            }
            else if (cards_amount(piles_cards[select_pile(piles_cards, player_hand[i].color_number, piles_amount, search_min)]) < min && search_min == false) {
                //liczymy ilość kart w wybranym kociołku dla każdej karty o największej wartości i wybieramy taką dla której ilość kart w kociołku jest najmniejsza
                min = cards_amount(piles_cards[select_pile(piles_cards, player_hand[i].color_number, piles_amount, search_min)]);
                *index = i;
            }
        }
        i++;
    }

    *selected_pile = select_pile(piles_cards, player_hand[*index].color_number, piles_amount, search_min);
    if (if_explosion(piles_cards[*selected_pile], e, player_hand[*index].value) == true && search_min == true)//jeżeli wybór najmniejszej karty prowadzi do eksplozji przechodzimy do pozbywania się największych kart
        select(player_hand, selected_pile, index, piles_cards, piles_amount, e, false);
}


void move(card* player_hand, card* player_deck, card** piles_cards, int piles_amount, int e) {
    int selected_pile = 0, selected_card_index;
    bool search_min = true;//gdy search_min = true - zadanie wybór najmniejszej karty, gdy false - zadanie pozbywanie się największych kart

    select(player_hand, &selected_pile, &selected_card_index, piles_cards, piles_amount, e, search_min);
    add_to_pile(piles_cards[selected_pile], player_hand[selected_card_index]);

    if (if_explosion(piles_cards[selected_pile], e, 0) == true)
        explosion(piles_cards[selected_pile], player_deck);

    int i = selected_card_index;
    while (player_hand[i].value != 0) {
        player_hand[i].value = player_hand[i + 1].value;
        player_hand[i].color_number = player_hand[i + 1].color_number;
        i++;
    }
}

void print_cards(card* players_hand, FILE* state) {
    int i = 0;

    while (players_hand[i].value != 0) {
        fprintf(state, "%d ", players_hand[i].value);
        cout_color(players_hand[i].color_number, state);
        fprintf(state, " ");
        i++;
    }
}

void immune(card** players_deck, int n, int** player_immune) {
    for (int j = 1; j < NUMBER_OF_COLORS; j++) {
        for (int i = 0; i < n; i++)
            player_immune[i][j] = count_color(players_deck[i], j);
    }

    for (int j = 1; j < NUMBER_OF_COLORS; j++) {//nie patrzymy na kolor zielony
        int max = player_immune[0][j]; int max_i = 0;
        for (int i = 1; i < n; i++) {//szukamy największej liczby w tablicy, resztę liczb zerujemy (na każdy kolor tylko jedna osoba może być odporna)
            if (player_immune[i][j] > max) {
                player_immune[max_i][j] = 0;
                max = player_immune[i][j];
                max_i = i;
            }
            else if (player_immune[i][j] == max) {
                player_immune[i][j] = 0;
                player_immune[max_i][j] = 0;
            }
            else
                player_immune[i][j] = 0;
        }
    }
}


int player_score(card* player_deck, int* player_immune) {
    int j = 0; int score = 0;

    while (player_deck[j].value != 0) {
        if (player_deck[j].color_number == 0)
            score += 2;
        else if (player_immune[player_deck[j].color_number] == 0)//podliczamy tylko kary na które gracz nie jest odporny
            score++;
        j++;
    }

    return score;
}


void end_of_the_game(card** players_deck, int n, FILE* state) {
    int** player_immune;
    player_immune = new int* [n];//w tej tablicy będziemy zapisywać dla każdego gracza ilość kart danego koloru
    for (int i = 0; i < n; i++)
        player_immune[i] = new int[NUMBER_OF_COLORS]();

    immune(players_deck, n, player_immune);//szukamy graczy odpornych na kolory

    cout << "GRA ZAKONCZONA, WYNIKI:" << endl;
    for (int j = 1; j < NUMBER_OF_COLORS; j++) {
        for (int i = 0; i < n; i++) {
            if (player_immune[i][j] != 0) {
                cout << "Na kolor ";
                cout_color(j, stdout);
                cout << " odporny jest gracz " << i + 1 << endl;
            }
        }
    }

    for (int i = 0; i < n; i++)
        cout << "Wynik gracza " << i + 1 << " = " << player_score(players_deck[i], player_immune[i]) << endl;

    delete[] player_immune;
}


void print(card** piles_cards, card** players_hand, card** players_deck, int n, int piles_amount, int e, int active_player, FILE* state, bool* end, bool ruch) {

    if (players_hand[active_player - 1][0].value == 0) {
        *end = true;
        end_of_the_game(players_deck, n, state);
    }
    else if (ruch == true)
        move(players_hand[active_player - 1], players_deck[active_player - 1], piles_cards, piles_amount, e);

    fseek(state, 0, 0);

    fprintf(state, "active player = ");
    if (active_player == n)
        fprintf(state, "%d\n", 1);
    else
        fprintf(state, "%d\n", active_player + 1);
    fprintf(state, "players number = %d\nexplosion threshold = %d\n", n, e);

    for (int i = 0; i < n; i++) {
        fprintf(state, "%d player hand cards: ", i + 1);
        print_cards(players_hand[i], state);
        fprintf(state, "\n");
        fprintf(state, "%d player deck cards: ", i + 1);
        print_cards(players_deck[i], state);
        fprintf(state, "\n");
    }
    for (int i = 0; i < piles_amount; i++) {
        fprintf(state, "%d pile cards: ", i + 1);
        print_cards(piles_cards[i], state);
        fprintf(state, "\n");
    }

}

void print2(card** players_hand, int n, int k, FILE* state, int e) {
    fseek(state, 0, 0);
    fprintf(state, "active player = 1\nplayers number = %d\nexplosion threshold = %d\n", n, e);

    for (int m = 0; m < n; m++) {
        fprintf(state, "%d player hand cards: ", m + 1);
        print_cards(players_hand[m], state);
        fprintf(state, "\n");
        fprintf(state, "%d player deck cards: ", m + 1);
        fprintf(state, "\n");
    }

    for (int i = 0; i < k; i++) {
        fprintf(state, "%d pile cards: ", i + 1);
        if (i != k - 1)fprintf(state, "\n");
    }
}


void check(card** players_hand, card** players_deck, card** piles_cards, int n, int piles_amount, int e, bool* end, int active_player) {
    check_green(players_hand, players_deck, n, piles_cards, piles_amount, end);
    check_cards_values(players_hand, players_deck, n, piles_cards, piles_amount, end);
    check_card_amount(players_hand, players_deck, n, piles_cards, piles_amount, end);
    check_state(piles_cards, players_hand, n, piles_amount, e, active_player, end);
}


int main()
{
    int active_player = 1;
    bool end = false, first_move = true;

    cout << "Gracz " << active_player << endl;
    cout << "GRACZ 1 WPISUJE DANE" << endl;

    if (active_player == 1) {
        FILE* begin;
        begin = fopen("StateOfTheGame.txt", "w+");
        FILE* write;
        write = fopen("ruchy.txt", "a");

        int n, k, g, gv, o, e;

        card* deck;
        card** players_hand;
        int* number;

        cout << endl << "Wprowadz dane aby przeprowadzic rozgrywke" << endl << "Liczba graczy: ";
        cin >> n;
        cout << "Liczba kociolkow: ";
        cin >> k;
        cout << "Liczba zielonych kart: ";
        cin >> g;
        cout << "Wartosc zielonych kart: ";
        cin >> gv;
        cout << "Ilosc kart kazdego innego koloru niz zielony: ";
        cin >> o;
        cout << "Wartosci niezielonych kart:" << endl;

        number = new int[o];
        deck = new card[o * k + g];
        players_hand = new card * [n];
        for (int i = 0; i < n; i++) {
            players_hand[i] = new card[o * k + g]();//każdy gracz ma własną tablicę kart
        }

        for (int i = 0; i < o; i++)
            cin >> number[i];

        cout << "Prog eksplozji kociolka: ";
        cin >> e;

        create_deck(k, g, gv, o, number, deck);
        card_deal(o, k, g, n, deck, players_hand);
        print2(players_hand, n, k, begin, e);
        print2(players_hand, n, k, write, e);


        fclose(write); fclose(begin);
        delete[] deck;
        delete[] number;
        delete[] players_hand;
    }

    while (1) {

        Sleep(10);
        FILE* ruch;
        ruch = fopen("ruchy.txt", "a");
        FILE* state;
        state = fopen("StateOfTheGame.txt", "r+");

        if (state != NULL) {
            int n, piles_amount = 0, e, a_p = 0;
            char* input;
            input = new char[MAX_INPUT_TEXT_IN_LINE];//tworzymy jedną tablicę którą będziemy używać do każdego wczytywania linii tekstu(aby za każdym razem nie tworzyć nowej)
            card** players_hand;
            card** players_deck;
            card** piles_cards;

            load_basic_info(&a_p, &n, input, &e, state);

            if (active_player == a_p) {
                if (first_move == true)
                    cout << endl << "DANE ZOSTALY WPISANE" << endl << endl;

                players_hand = new card * [n];
                players_deck = new card * [n];
                for (int i = 0; i < n; i++) {//każdy gracz ma swoją tablicę kart na ręce oraz kart leżących na stole
                    players_hand[i] = new card[MAX_PLAYER_CARDS]();
                    players_deck[i] = new card[MAX_PLAYER_CARDS]();
                }

                piles_cards = new card * [NUMBER_OF_COLORS - 1];
                for (int i = 0; i < NUMBER_OF_COLORS - 1; i++)//maksymalna ilość kociołków to liczba kolorów bez zielonego
                    piles_cards[i] = new card[MAX_PLAYER_CARDS]();//każdy kociołek ma swoją tablicę kart
                //tablice graczy oraz kociołków są wyzerowane co oznacza że kartą jest każda struktura 'card' której wartość nie jest równa 0

                load_game(n, players_hand, players_deck, input, piles_cards, &piles_amount, state);
                check(players_hand, players_deck, piles_cards, n, piles_amount, e, &end, a_p);
                if (end == true) {//gdy zostaną wykryte nieprawidłowości w grze zmienna end przyjmie wartość true i gra zostanie zakończona
                    fclose(state);
                }
                print(piles_cards, players_hand, players_deck, n, piles_amount, e, a_p, state, &end, true);
                print(piles_cards, players_hand, players_deck, n, piles_amount, e, a_p, ruch, &end, false);
                delete[] players_hand;
                delete[] players_deck;
                delete[] input;
                delete[] piles_cards;
            }

            first_move = false;
            fclose(ruch);
            fclose(state);
        }

        if (end == true)
            break;

    }

    return 0;
}