#include "Failai.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <stdexcept>
#include <utility>

namespace
{
    constexpr int dydziai[] = {
        1000, 10000, 100000, 1000000, 10000000
    };

    double galutinis(const Studentas& s, int kriterijus)
    {
        return kriterijus == 2
            ? s.galutineMediana()
            : s.galutinisVidurkis();
    }

    template <typename Container>
    void strategija1(
        const Container& grupe,
        Container& vargsiukai,
        Container& kietiakiai,
        int kriterijus)
    {
        vargsiukai.clear();
        kietiakiai.clear();

        for (const Studentas& s : grupe)
        {
            if (galutinis(s, kriterijus) < 5.0)
                vargsiukai.push_back(s);
            else
                kietiakiai.push_back(s);
        }
    }

    void strategija2(
        std::vector<Studentas>& grupe,
        std::vector<Studentas>& vargsiukai,
        int kriterijus)
    {
        vargsiukai.clear();

        for (auto it = grupe.begin(); it != grupe.end();)
        {
            if (galutinis(*it, kriterijus) < 5.0)
            {
                vargsiukai.push_back(std::move(*it));
                it = grupe.erase(it);
            }
            else
                ++it;
        }
    }

    void strategija2(
        std::list<Studentas>& grupe,
        std::list<Studentas>& vargsiukai,
        int kriterijus)
    {
        vargsiukai.clear();

        for (auto it = grupe.begin(); it != grupe.end();)
        {
            if (galutinis(*it, kriterijus) < 5.0)
            {
                auto dabartinis = it++;

                vargsiukai.splice(
                    vargsiukai.end(),
                    grupe,
                    dabartinis
                );
            }
            else
                ++it;
        }
    }

    void strategija3(
        std::vector<Studentas>& grupe,
        std::vector<Studentas>& vargsiukai,
        int kriterijus)
    {
        vargsiukai.clear();

        auto riba = std::stable_partition(
            grupe.begin(),
            grupe.end(),
            [kriterijus](const Studentas& s)
            {
                return galutinis(s, kriterijus) < 5.0;
            }
        );

        vargsiukai.reserve(
            static_cast<std::size_t>(
                std::distance(grupe.begin(), riba)
                )
        );

        std::move(
            grupe.begin(),
            riba,
            std::back_inserter(vargsiukai)
        );

        grupe.erase(grupe.begin(), riba);
    }

    void strategija3(
        std::list<Studentas>& grupe,
        std::list<Studentas>& vargsiukai,
        int kriterijus)
    {
        vargsiukai.clear();

        for (auto it = grupe.begin(); it != grupe.end();)
        {
            auto dabartinis = it++;

            if (galutinis(*dabartinis, kriterijus) < 5.0)
            {
                vargsiukai.splice(
                    vargsiukai.end(),
                    grupe,
                    dabartinis
                );
            }
        }
    }
}

// TESTINIU FAILU GENERAVIMAS

void generuotiTestiniusFailus(
    const std::vector<int>& pasirinktiFailai)
{
    const std::string vardai[] = {
        "Jonas", "Petras", "Ona", "Ieva", "Lukas",
        "Mantas", "Greta", "Aiste", "Tomas", "Rokas"
    };

    const std::string pavardes[] = {
        "Kazlauskas", "Petrauskas", "Jankauskas",
        "Stankevicius", "Vasiliauskas", "Zukauskas",
        "Butkus", "Paulauskas", "Urbonas", "Savickas"
    };

    std::mt19937 gen(2026);

    std::uniform_int_distribution<int> pazymys(1, 10);
    std::uniform_int_distribution<int> indeksas(0, 9);
    std::uniform_int_distribution<int> ndKiekis(5, 10);

    for (int pasirinkimas : pasirinktiFailai)
    {
        if (pasirinkimas < 0 || pasirinkimas >= 5)
            throw std::invalid_argument(
                "Neteisingas testinio failo pasirinkimas."
            );

        const int n = dydziai[pasirinkimas];

        const std::string failoVardas =
            "studentai_" + std::to_string(n) + ".txt";

        std::ofstream f(failoVardas);

        if (!f)
            throw std::runtime_error(
                "Nepavyko sugeneruoti failo: " + failoVardas
            );

        f << "Vardas Pavarde ND1 ND2 ND3 ND4 ND5 "
            "ND6 ND7 ND8 ND9 ND10 Egzaminas\n";

        for (int i = 0; i < n; ++i)
        {
            f << vardai[indeksas(gen)] << ' '
                << pavardes[indeksas(gen)];

            const int kiek = ndKiekis(gen);

            for (int j = 0; j < kiek; ++j)
                f << ' ' << pazymys(gen);

            f << ' ' << pazymys(gen) << '\n';
        }

        std::cout << "Sugeneruotas failas: "
            << failoVardas << '\n';
    }
}

// VECTOR SKIRSTYMAS

void skirstytiIGrupes(
    std::vector<Studentas>& grupe,
    std::vector<Studentas>& vargsiukai,
    std::vector<Studentas>& kietiakiai,
    int kriterijus,
    int strategija)
{
    if (kriterijus != 1 && kriterijus != 2)
        throw std::invalid_argument("Neteisingas kriterijus.");

    kietiakiai.clear();

    if (strategija == 1)
        strategija1(grupe, vargsiukai, kietiakiai, kriterijus);

    else if (strategija == 2)
        strategija2(grupe, vargsiukai, kriterijus);

    else if (strategija == 3)
        strategija3(grupe, vargsiukai, kriterijus);

    else
        throw std::invalid_argument(
            "Neteisingas strategijos numeris."
        );
}

// LIST SKIRSTYMAS

void skirstytiIGrupes(
    std::list<Studentas>& grupe,
    std::list<Studentas>& vargsiukai,
    std::list<Studentas>& kietiakiai,
    int kriterijus,
    int strategija)
{
    if (kriterijus != 1 && kriterijus != 2)
        throw std::invalid_argument("Neteisingas kriterijus.");

    kietiakiai.clear();

    if (strategija == 1)
        strategija1(grupe, vargsiukai, kietiakiai, kriterijus);

    else if (strategija == 2)
        strategija2(grupe, vargsiukai, kriterijus);

    else if (strategija == 3)
        strategija3(grupe, vargsiukai, kriterijus);

    else
        throw std::invalid_argument(
            "Neteisingas strategijos numeris."
        );
}
