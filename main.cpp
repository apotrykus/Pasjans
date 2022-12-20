#include <iostream>
#include <vector>
#include <string>
#include <chrono>   //do generowania liczb losowych
#include <random>   // też
#include <iomanip>  // do manipulacji wyjścia - funkcja setw(), ustawia szerokość następnego wyświetlenia tekstu

// Enum dla kolorow karty - enum to typ, last - ostatni typ klasy enum żeby łatwo można było przejść po każdym enumie
enum class Color
{red, black, last};

// Enum dla figur kart
enum class Figure
{ace, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, last};

// Enum dla typow kart
enum class Type
{spades, hearts, diamonds, clubs, last};

// Enum dla pozycji kart stos rezerwowy, 4 stosy na których można by było układać ostateczne, kolumny
enum class Position
{reserve, stack1, stack2, stack3, stack4, col1, col2, col3, col4, col5, col6, col7, last};

// Klasa dla kart
class Card
{
public:                     //mogę to zmieniać
    Color color;
    Figure figure;
    Type type;
    Position pos;
    bool visible = false;
    bool picked1 = false; //wykorzystywane do zamiany kart
    bool picked2 = false;
public:
    Card(){};
    Card(Figure f, Type t) : figure{f},type{t} {};
    void printColor();
    void printFigure();
    void printType();
    void printCard();
    std::string getColor();
    std::string getFigure();
    std::string getType();
    std::string getCard();
};

// Funkcja do mieszania talii kart (vectora kart) Fisher–Yates shuffle
void shuffleDeck(std::vector<Card>& deck)
{
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed1);

    for(int i=deck.size()-1; i>=0; i--)
    {
        std::uniform_int_distribution<int> distribution(0, i);
        int j = distribution(generator);
        std::swap(deck.at(i), deck.at(j));
    }
}

// Funkcja przypisujaca pozycje dla kart z talii
void prepareBoard(std::vector<Card>& deck)
{
    int idx = 0;
    int colHeight = 1;
    //jak sie przechodzi przez każdy typ enum po pozycjach od col1-7 jak natrafimy na last to kończymy pętle
    for(int posInt = static_cast<int>(Position::col1); posInt != static_cast<int>(Position::last); posInt++)
    {
        // Przypisuje 28 kartom pozycje w kolumnach, zaczynam od wysokosci kolumny i tam jest przypisana col1, wtedy mamy wysokość kolumny 2
        for(int j=0; j<colHeight; j++)
        {
            Position newPos = static_cast<Position>(posInt); //tworzona jest nowa pozycja
            deck.at(idx).pos = newPos; //przypisuje aktualnej karcie pozycje
            idx++; // zwiększa indeks o 1
        }
        colHeight++; //zwiększa wysokość kolumny o 1
    }
    // Przypisujemy 24 pozostalym kartom pozycje rezerwy
    for(int i=0; i<24; i++)
    {
        deck.at(idx).pos = Position::reserve;
        idx++;
    }
}

// Funkcja do wypelnienia pustej talii kart, jedna karta kazdego rodzaju
void fillDeck(std::vector<Card>& deck)
{
    // Przychodzimy przez wszystkie enumy typu
    for(int typeInt = static_cast<int>(Type::spades); typeInt != static_cast<int>(Type::last); typeInt++)
    {
        Type newType = static_cast<Type>(typeInt);
        // Iterujemy przez kazdy enum figury
        for(int figureInt = static_cast<int>(Figure::ace); figureInt != static_cast<int>(Figure::last); figureInt++)
        {
            Figure newFigure = static_cast<Figure>(figureInt);
            Card newCard(newFigure, newType);
            deck.push_back(newCard);
        }
    }
}

// Funkcja do wyswietlania planszy, przyjmuje kolumny, 4 stosy i stos kart rezerwowych
void displayBoard(std::vector<std::vector<Card>> &cols, std::vector<std::vector<Card>> &stacks, std::vector<Card> &reserve) {
    // Wyswietlamy karte na gorze stosu rezerwowego
    reserve.back().printCard();

    // Wyznaczamy rozmiar najwiekszego stosu
    int maxStackSize = stacks.at(0).size();
    for (int i = 1; i < stacks.size(); i++) {
        if (stacks.at(i).size() > maxStackSize)
            maxStackSize = stacks.at(i).size();
    }

    // Wyswietlamy stosy
    for (int i = 0; i < maxStackSize; i++) {
        for (int j = 0; j < 4; j++) {
            if (i < stacks.at(j).size())
                std::cout << std::setw(10) << stacks.at(j).at(i).getCard();
            else
                std::cout << "        ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;

    // Wyznaczamy rozmiar najwiekszej kolumny
    int maxColSize = cols.at(0).size();
    for (int i = 1; i < cols.size(); i++) {
        if (cols.at(i).size() > maxColSize)
            maxColSize = cols.at(i).size();
    }

    // Wyswietlamy karty z wszystkich kolumn jesli powinny byc widoczne
    for (int i = 0; i < maxColSize; i++) {
        for (int j = 0; j < 7; j++) {
            if (i < cols.at(j).size())
                if (cols.at(j).at(i).visible)
                    std::cout << std::setw(10) << cols.at(j).at(i).getCard();
                else
                    std::cout << std::setw(8) << "??";
            else
                std::cout << "        ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

int main(void)
{
    // Tworzymy zmienna playing do kontrolowania czy gra ciagle trwa
    bool playing = true;

    // Tworzymy dwie zmienne input1, input2 typu string aby przyjmowac polecenia gracza
    std::string input1{};
    std::string input2{};

    // Tworzymy pusty wektor kart
    std::vector<Card> deckOfCards;

    // Wypelniamy wektor kartami
    fillDeck(deckOfCards);

    // Mieszamy talie kart
    shuffleDeck(deckOfCards);

    // Przypisujemy kartom odpowiednie pozycje
    prepareBoard(deckOfCards);

    // Tworzymy puste wektory kart
    std::vector<Card> reserve{};
    std::vector<Card> stack1{};
    std::vector<Card> stack2{};
    std::vector<Card> stack3{};
    std::vector<Card> stack4{};
    std::vector<Card> col1 {};
    std::vector<Card> col2 {};
    std::vector<Card> col3 {};
    std::vector<Card> col4 {};
    std::vector<Card> col5 {};
    std::vector<Card> col6 {};
    std::vector<Card> col7 {};

    // Wypelniamy puste wektory kart odpowiednimi kartami w zaleznosci od pozycji
    for(auto c : deckOfCards)
    {
        switch(c.pos)
        {
            case Position::reserve : reserve.push_back(c); break;
            case Position::col1    : col1.push_back(c);    break;
            case Position::col2    : col2.push_back(c);    break;
            case Position::col3    : col3.push_back(c);    break;
            case Position::col4    : col4.push_back(c);    break;
            case Position::col5    : col5.push_back(c);    break;
            case Position::col6    : col6.push_back(c);    break;
            case Position::col7    : col7.push_back(c);    break;
        }
    }

    // Tworzymy wektory wszystkich kolumn oraz wszystkich stosow
    std::vector<std::vector<Card>> cols { col1, col2, col3, col4, col5, col6, col7 };
    std::vector<std::vector<Card>> stacks { stack1, stack2, stack3, stack4 };

    // Ustawiamy ostatnia karte z kazdej kolumny jako widoczna
    for(auto &col : cols)
    {
        if(!col.empty())
            col.back().visible = true;
    }

    // Glowna petla gry
    while(playing)
    {
        // Wyswietlamy plansze
        displayBoard(cols, stacks, reserve);

        // Wyswietlamy instrukcje dla gracza
        std::cout << "Podaj dwie karty, które chcesz przełożyć, aby wykonać ruch np. 9 serce potem 10pik" << std::endl;
        std::cout << "Podaj R aby sprawdzic nastepna karte ze stosu rezerwowego, lub Q aby zakonczyc grę" << std::endl;
        std::cout << "Podaj S1, S2, S3, S4 aby przelozyc karte na jeden z gornych stosow kart" << std::endl;

        // Przyjmujemy od gracza input
        std::getline(std::cin, input1);
        std::getline(std::cin, input2);

        // Jesli jedna z dwoch inputow to "Q" to konczymy rozgrywke
        if(input1 == "Q" || input2 == "Q") playing = false;

        // Jeśli jedna z dwóch inputów to R to sprawdzamy kolejną kartę ze stosu rezerowego
        if(input1 == "R" || input2 == "R")
        {
            Card temp = reserve.back();
            reserve.pop_back();
            reserve.insert(reserve.begin(), temp);
        }



        std::string figure1s{};
        std::string type1s{};
        std::string figure2s{};
        std::string type2s{};
        if(input1.length() > 3)
        {
            figure1s = std::string (input1, 0, 2);
            type1s = std::string (input1, 2, input1.length()-2);
        }

        if(input2.length() > 3)
        {
            figure2s = std::string(input2, 0, 2);
            type2s = std::string (input2, 2, input2.length()-2);
        }


        Type type1 = Type::last;
        Type type2 = Type::last;

        if(type1s == "pik") type1 = Type::spades;
        if(type1s == "serce") type1 = Type::hearts;
        if(type1s == "karo") type1 = Type::diamonds;
        if(type1s == "trefl") type1 = Type::clubs;

        if(type2s == "pik") type2 = Type::spades;
        if(type2s == "serce") type2 = Type::hearts;
        if(type2s == "karo") type2 = Type::diamonds;
        if(type2s == "trefl") type2 = Type::clubs;

        Figure figure1 = Figure::last;
        Figure figure2 = Figure::last;


        if(figure1s == "A ") figure1 = Figure::ace  ;
        if(figure1s == "2 ") figure1 = Figure::two  ;
        if(figure1s == "3 ") figure1 = Figure::three;
        if(figure1s == "4 ") figure1 = Figure::four ;
        if(figure1s == "5 ") figure1 = Figure::five ;
        if(figure1s == "6 ") figure1 = Figure::six  ;
        if(figure1s == "7 ") figure1 = Figure::seven;
        if(figure1s == "8 ") figure1 = Figure::eight;
        if(figure1s == "9 ") figure1 = Figure::nine ;
        if(figure1s == "10") figure1 = Figure::ten  ;
        if(figure1s == "J ") figure1 = Figure::jack ;
        if(figure1s == "Q ") figure1 = Figure::queen;
        if(figure1s == "K ") figure1 = Figure::king ;

        if(figure2s == "A ") figure2 = Figure::ace  ;
        if(figure2s == "2 ") figure2 = Figure::two  ;
        if(figure2s == "3 ") figure2 = Figure::three;
        if(figure2s == "4 ") figure2 = Figure::four ;
        if(figure2s == "5 ") figure2 = Figure::five ;
        if(figure2s == "6 ") figure2 = Figure::six  ;
        if(figure2s == "7 ") figure2 = Figure::seven;
        if(figure2s == "8 ") figure2 = Figure::eight;
        if(figure2s == "9 ") figure2 = Figure::nine ;
        if(figure2s == "10") figure2 = Figure::ten  ;
        if(figure2s == "J ") figure2 = Figure::jack ;
        if(figure2s == "Q ") figure2 = Figure::queen;
        if(figure2s == "K ") figure2 = Figure::king ;


        Card temp1{};
        Card temp2{};

        // Find and mark cards to switch
        for(auto &col : cols)
        {
            if(col.back().figure == figure1 && col.back().type == type1)
            {
                col.back().picked1 = true;
                temp1 = col.back();
                col.pop_back();
            }
            if(col.back().figure == figure2 && col.back().type == type2)
            {
                col.back().picked2 = true;
                temp2 = col.back();
                // col.pop_back();
            }
        }
        if(reserve.back().figure == figure1 && reserve.back().type == type1)
        {
            reserve.back().picked1 = true;
            temp1 = reserve.back();
            reserve.pop_back();
        }
        if(reserve.back().figure == figure2 && reserve.back().type == type2)
        {
            reserve.back().picked2 = true;
            temp2 = reserve.back();
            //reserve.pop_back();
        }



        //for(auto &col : cols)
        //{
        //    if(col.back().picked1)
        //    {
        //        col.push_back(temp2);
        //        col.back().picked1 = false;
        //        temp1 = {};
        //    }
        //}
        for(auto &col : cols)
        {
            if(col.back().picked2)
            {
                col.push_back(temp1);
                col.back().picked2 = false;
                temp2 = {};
            }
        }
        if(reserve.back().picked1)
        {
            reserve.push_back(temp1);
            reserve.back().picked1 = false;
            temp1 = {};
        }
        if(reserve.back().picked2)
        {
            reserve.push_back(temp2);
            reserve.back().picked2 = false;
            temp2 = {};
        }
        for(auto &col : cols)
        {
            if(!col.empty())
                col.back().visible = true;
        }

    }


    return 0;
}

void Card::printColor() {
    switch (color) {
        case Color::red : std::cout << "red"; break;
        case Color::black : std::cout << "black"; break;
    }
}

std::string Card::getColor() {
    switch (color) {
        case Color::red : return "red";
        case Color::black : return "black";
    }
}

void Card::printFigure() {
    switch (figure) {
        case Figure::ace    : std::cout << "A"; break;
        case Figure::two    : std::cout << "2"; break;
        case Figure::three  : std::cout << "3"; break;
        case Figure::four   : std::cout << "4"; break;
        case Figure::five   : std::cout << "5"; break;
        case Figure::six    : std::cout << "6"; break;
        case Figure::seven  : std::cout << "7"; break;
        case Figure::eight  : std::cout << "8"; break;
        case Figure::nine   : std::cout << "9"; break;
        case Figure::ten    : std::cout << "10"; break;
        case Figure::jack   : std::cout << "J"; break;
        case Figure::queen  : std::cout << "Q"; break;
        case Figure::king   : std::cout << "K"; break;
    }
}

std::string Card::getFigure() {
    switch (figure) {
        case Figure::ace    : return "A";
        case Figure::two    : return "2";
        case Figure::three  : return "3";
        case Figure::four   : return "4";
        case Figure::five   : return "5";
        case Figure::six    : return "6";
        case Figure::seven  : return "7";
        case Figure::eight  : return "8";
        case Figure::nine   : return "9";
        case Figure::ten    : return "10";
        case Figure::jack   : return "J";
        case Figure::queen  : return "Q";
        case Figure::king   : return "K";
    }
}



void Card::printType() {
    switch (type)
    {
        case Type::spades   : std::cout << "\u2664"; break;
        case Type::hearts   : std::cout << "\u2665"; break;
        case Type::diamonds : std::cout << "\u2666"; break;
        case Type::clubs    : std::cout << "\u2667"; break;
    }
}

std::string Card::getType() {
    switch (type)
    {
        case Type::spades   : return "\u2664";
        case Type::hearts   : return "\u2665";
        case Type::diamonds : return "\u2666";
        case Type::clubs    : return "\u2667";
    }
}

void Card::printCard() {
    Card::printFigure();
    Card::printType();
}

std::string Card::getCard() {
    return Card::getFigure() + Card::getType();
}