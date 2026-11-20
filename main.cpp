#include "Studentas.h"
#include "Failai.h"
#include "Testavimas.h"

#include <iostream>
#include <limits>
#include <list>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
    template <typename Container>
    void iprastasRezymas(Container& studentai)
    {
        std::cout << "\n1 - Nuskaityti is kursiokai.txt\n"
            << "2 - Ivesti studentus rankiniu budu\nPasirinkite: ";

        int budas;
        if (!(std::cin >> budas))
            throw std::runtime_error("Neteisinga ivestis.");

        if (budas == 1)
        {
            nuskaitytiIsFailo("kursiokai.txt", studentai);
        }
        else if (budas == 2)
        {
            int n;
            std::cout << "Studentu skaicius: ";

            if (!(std::cin >> n) || n <= 0)
            {
                std::cout << "Neteisingas studentu skaicius.\n";
                return;
            }

            for (int i = 0; i < n; ++i)
            {
                std::string vardas, pavarde;

                std::cout << "\nVardas ir pavarde: ";
                if (!(std::cin >> vardas >> pavarde))
                    throw std::runtime_error("Neteisinga ivestis.");

                int kiek;
                std::cout << "Namu darbu pazymiu skaicius: ";

                if (!(std::cin >> kiek) || kiek < 0)
                    throw std::runtime_error("Neteisingas pazymiu skaicius.");

                std::vector<int> nd;
                nd.reserve(static_cast<std::size_t>(kiek));

                for (int j = 0; j < kiek; ++j)
                {
                    int pazymys;

                    do
                    {
                        std::cout << "Pazymys (1-10): ";

                        if (!(std::cin >> pazymys))
                            throw std::runtime_error("Neteisinga ivestis.");

                    } while (pazymys < 1 || pazymys > 10);

                    nd.push_back(pazymys);
                }

                int egzaminas;

                do
                {
                    std::cout << "Egzamino pazymys (1-10): ";

                    if (!(std::cin >> egzaminas))
                        throw std::runtime_error("Neteisinga ivestis.");

                } while (egzaminas < 1 || egzaminas > 10);

                studentai.emplace_back(
                    std::move(vardas),
                    std::move(pavarde),
                    std::move(nd),
                    egzaminas
                );
            }
        }
        else
        {
            std::cout << "Tokio pasirinkimo nera.\n";
            return;
        }

        if (studentai.empty())
        {
            std::cout << "Studentu sarasas tuscias.\n";
            return;
        }

        int kriterijus;

        std::cout << "\nSkirstyti pagal:\n"
            << "1 - Galutini vidurki\n"
            << "2 - Galutine mediana\n"
            << "Pasirinkite: ";

        if (!(std::cin >> kriterijus) ||
            (kriterijus != 1 && kriterijus != 2))
        {
            std::cout << "Tokio pasirinkimo nera.\n";
            return;
        }

        int strategija;

        std::cout << "\nPasirinkite skirstymo strategija:\n"
            << "1 - Kopijuoti i du naujus konteinerius\n"
            << "2 - Kopijuoti vargsiukus ir pasalinti is pradinio\n"
            << "3 - Optimizuotas skirstymas\n"
            << "Pasirinkite: ";

        if (!(std::cin >> strategija) ||
            strategija < 1 || strategija > 3)
        {
            std::cout << "Tokio pasirinkimo nera.\n";
            return;
        }

        Container vargsiukai, kietiakiai;

        skirstytiIGrupes(
            studentai,
            vargsiukai,
            kietiakiai,
            kriterijus,
            strategija
        );

        if (strategija != 1)
            kietiakiai = studentai;

        issaugotiIFaila("vargsiukai.txt", vargsiukai);
        issaugotiIFaila("kietiakiai.txt", kietiakiai);

        std::cout << "\nStudentai suskirstyti.\n"
            << "Vargsiukai: " << vargsiukai.size()
            << "\nKietiakai: " << kietiakiai.size()
            << "\nRezultatai issaugoti vargsiukai.txt ir kietiakiai.txt\n";
    }
}

int main()
{
    std::cout << "========== STUDENTU PROGRAMA v1.1 ==========\n\n"
        << "1 - Iprastas rezimas\n"
        << "2 - Testavimo rezimas\n"
        << "Pasirinkite rezima: ";

    int rezimas;

    if (!(std::cin >> rezimas))
    {
        std::cout << "Neteisinga ivestis.\n";
        return 1;
    }

    try
    {
        if (rezimas == 1)
        {
            int konteineris;

            std::cout << "\nPasirinkite konteineri:\n"
                << "1 - vector\n"
                << "2 - list\n"
                << "Pasirinkite: ";

            if (!(std::cin >> konteineris))
                throw std::runtime_error("Neteisinga ivestis.");

            if (konteineris == 1)
            {
                std::vector<Studentas> studentai;
                iprastasRezymas(studentai);
            }
            else if (konteineris == 2)
            {
                std::list<Studentas> studentai;
                iprastasRezymas(studentai);
            }
            else
            {
                std::cout << "Tokio pasirinkimo nera.\n";
            }
        }
        else if (rezimas == 2)
        {
            int kartai;

            std::cout << "\nKiek kartu kartoti kiekviena testa? ";

            if (!(std::cin >> kartai) || kartai <= 0)
            {
                std::cout << "Testu skaicius turi buti teigiamas.\n";
                return 1;
            }

            testuotiKeliskart(kartai);
        }
        else
        {
            std::cout << "Tokio pasirinkimo nera.\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nKLAIDA: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
