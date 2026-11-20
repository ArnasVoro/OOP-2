
#include "Testavimas.h"
#include "Failai.h"
#include "Studentas.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

namespace
{
    using Laikrodis = std::chrono::steady_clock;

    template <typename F>
    double matuoti(F&& funkcija)
    {
        const auto pradzia = Laikrodis::now();

        funkcija();

        const auto pabaiga = Laikrodis::now();

        return std::chrono::duration<double>(
            pabaiga - pradzia
        ).count();
    }

    // =========================================
    // STRUCT VERSIJA (v1.0)
    // =========================================

    struct StudentasStruct
    {
        std::string vardas;
        std::string pavarde;
        std::vector<int> pazymiai;

        double galutinisVid = 0.0;
        double galutineMed = 0.0;
    };

    double apskaiciuotiVidurki(
        const std::vector<int>& pazymiai)
    {
        if (pazymiai.empty())
            return 0.0;

        double suma = 0.0;

        for (int p : pazymiai)
            suma += p;

        return suma / pazymiai.size();
    }

    double apskaiciuotiMediana(
        std::vector<int> pazymiai)
    {
        if (pazymiai.empty())
            return 0.0;

        std::sort(
            pazymiai.begin(),
            pazymiai.end()
        );

        const std::size_t vidurys =
            pazymiai.size() / 2;

        if (pazymiai.size() % 2 == 1)
            return pazymiai[vidurys];

        return (
            pazymiai[vidurys - 1] +
            pazymiai[vidurys]
            ) / 2.0;
    }

    void nuskaitytiStructIsFailo(
        const std::string& failas,
        std::vector<StudentasStruct>& studentai)
    {
        std::ifstream f(failas);

        if (!f)
        {
            throw std::runtime_error(
                "Nepavyko atidaryti failo: " + failas
            );
        }

        studentai.clear();

        std::string eilute;

        // Praleidziama failo antraste.
        std::getline(f, eilute);

        while (std::getline(f, eilute))
        {
            if (eilute.empty())
                continue;

            std::stringstream ss(eilute);

            StudentasStruct s;

            if (!(ss >> s.vardas >> s.pavarde))
                continue;

            int pazymys;
            bool neteisingas = false;

            while (ss >> pazymys)
            {
                if (pazymys < 1 || pazymys > 10)
                {
                    neteisingas = true;
                    break;
                }

                s.pazymiai.push_back(pazymys);
            }

            if (neteisingas || !ss.eof() ||
                s.pazymiai.empty())
            {
                continue;
            }

            // Paskutinis pazymys yra egzamino pazymys.
            const int egzaminas = s.pazymiai.back();

            s.pazymiai.pop_back();

            s.galutinisVid =
                0.4 * apskaiciuotiVidurki(s.pazymiai)
                + 0.6 * egzaminas;

            s.galutineMed =
                0.4 * apskaiciuotiMediana(s.pazymiai)
                + 0.6 * egzaminas;

            studentai.push_back(std::move(s));
        }
    }

    // =========================================
    // PAGRINDINIAI TESTAVIMO METODAI
    // =========================================

    template <typename Container>
    double vienasTestas(
        const std::string& failas,
        int kriterijus,
        int strategija,
        std::size_t& vargsiukuKiekis,
        std::size_t& kietiakuKiekis)
    {
        Container studentai;
        Container vargsiukai;
        Container kietiakiai;

        nuskaitytiIsFailo(
            failas,
            studentai
        );

        const double laikas = matuoti([&]()
            {
                skirstytiIGrupes(
                    studentai,
                    vargsiukai,
                    kietiakiai,
                    kriterijus,
                    strategija
                );
            });

        vargsiukuKiekis = vargsiukai.size();

        kietiakuKiekis =
            strategija == 1
            ? kietiakiai.size()
            : studentai.size();

        return laikas;
    }

    template <typename Container>
    void vykdytiStrategijuTestus(
        const std::string& konteineris,
        const std::string& failas,
        int kartai,
        int kriterijus)
    {
        std::cout
            << "\n========================================\n"
            << "Konteineris: " << konteineris
            << "\nFailas: " << failas
            << "\n========================================\n";

        std::cout
            << "Strategija | Vargsiukai | Kietiakai | Vidutinis laikas (s)\n";

        std::ofstream csv(
            "v1_1_rezultatai.csv",
            std::ios::app
        );

        if (!csv)
        {
            throw std::runtime_error(
                "Nepavyko atidaryti v1_1_rezultatai.csv"
            );
        }

        if (csv.tellp() == 0)
        {
            csv << "konteineris,failas,kriterijus,strategija,"
                "kartai,vargsiukai,kietiakai,vidurkis_s\n";
        }

        for (int strategija = 1; strategija <= 3; ++strategija)
        {
            double suma = 0.0;

            std::size_t vKiekis = 0;
            std::size_t kKiekis = 0;

            for (int i = 0; i < kartai; ++i)
            {
                std::size_t v = 0;
                std::size_t k = 0;

                suma += vienasTestas<Container>(
                    failas,
                    kriterijus,
                    strategija,
                    v,
                    k
                );

                vKiekis = v;
                kKiekis = k;
            }

            const double vidurkis =
                suma / kartai;

            std::cout
                << "     " << strategija
                << "     | " << vKiekis
                << " | " << kKiekis
                << " | " << std::fixed
                << std::setprecision(6)
                << vidurkis << '\n';

            csv
                << konteineris << ','
                << failas << ','
                << kriterijus << ','
                << strategija << ','
                << kartai << ','
                << vKiekis << ','
                << kKiekis << ','
                << vidurkis << '\n';
        }
    }

    // =========================================
    // VECTOR STL ALGORITMU TESTAVIMAS
    // =========================================

    template <typename Predicate>
    double testuotiVectorAlgoritma(
        const std::vector<Studentas>& pradinis,
        Predicate salyga,
        int algoritmas,
        std::size_t& vargsiukuKiekis)
    {
        std::vector<Studentas> darbinis = pradinis;

        std::vector<Studentas> vargsiukai;
        std::vector<Studentas> kietiakiai;

        const double laikas = matuoti([&]()
            {
                if (algoritmas == 1)
                {
                    std::copy_if(
                        darbinis.begin(),
                        darbinis.end(),
                        std::back_inserter(vargsiukai),
                        salyga
                    );

                    std::copy_if(
                        darbinis.begin(),
                        darbinis.end(),
                        std::back_inserter(kietiakiai),
                        [&](const Studentas& s)
                        {
                            return !salyga(s);
                        }
                    );
                }
                else if (algoritmas == 2)
                {
                    std::copy_if(
                        darbinis.begin(),
                        darbinis.end(),
                        std::back_inserter(vargsiukai),
                        salyga
                    );

                    const auto riba = std::remove_if(
                        darbinis.begin(),
                        darbinis.end(),
                        salyga
                    );

                    darbinis.erase(
                        riba,
                        darbinis.end()
                    );
                }
                else if (algoritmas == 3)
                {
                    const auto riba = std::partition(
                        darbinis.begin(),
                        darbinis.end(),
                        salyga
                    );

                    vargsiukai.assign(
                        darbinis.begin(),
                        riba
                    );
                }
                else
                {
                    const auto riba = std::stable_partition(
                        darbinis.begin(),
                        darbinis.end(),
                        salyga
                    );

                    vargsiukai.assign(
                        darbinis.begin(),
                        riba
                    );
                }
            });

        vargsiukuKiekis = vargsiukai.size();

        return laikas;
    }
}

// =========================================
// VECTOR STL ALGORITMU PALYGINIMAS
// =========================================

void palygintiVectorAlgoritmus(
    int kiekisPaleidimu,
    const std::string& failoVardas,
    int kriterijus)
{
    std::vector<Studentas> pradinis;

    nuskaitytiIsFailo(
        failoVardas,
        pradinis
    );

    std::cout
        << "\nPapildomas vector STL algoritmu palyginimas\n"
        << "Failas: " << failoVardas
        << "\nAlgoritmas | Vargsiukai | Vidutinis laikas (s)\n";

    const std::string pavadinimai[] =
    {
        "copy_if",
        "remove_if",
        "partition",
        "stable_partition"
    };

    std::ofstream csv(
        "v1_1_vector_algoritmai.csv",
        std::ios::app
    );

    if (!csv)
    {
        throw std::runtime_error(
            "Nepavyko atidaryti v1_1_vector_algoritmai.csv"
        );
    }

    if (csv.tellp() == 0)
    {
        csv << "failas,algoritmas,kartai,vargsiukai,vidurkis_s\n";
    }

    for (int alg = 1; alg <= 4; ++alg)
    {
        double suma = 0.0;

        std::size_t kiekis = 0;

        for (int i = 0; i < kiekisPaleidimu; ++i)
        {
            std::size_t v = 0;

            const double laikas = testuotiVectorAlgoritma(
                pradinis,
                [kriterijus](const Studentas& s)
                {
                    return (
                        kriterijus == 2
                        ? s.galutineMediana()
                        : s.galutinisVidurkis()
                        ) < 5.0;
                },
                alg,
                v
            );

            suma += laikas;
            kiekis = v;
        }

        const double vidurkis =
            suma / kiekisPaleidimu;

        std::cout
            << pavadinimai[alg - 1]
            << " | " << kiekis
            << " | " << std::fixed
            << std::setprecision(6)
            << vidurkis << '\n';

        csv
            << failoVardas << ','
            << pavadinimai[alg - 1] << ','
            << kiekisPaleidimu << ','
            << kiekis << ','
            << vidurkis << '\n';
    }
}

// =========================================
// STRUCT IR CLASS PALYGINIMAS
// =========================================

void palygintiStructIrClass(
    const std::string& failas,
    int kartai,
    int kriterijus)
{
    double structSkaitymas = 0.0;
    double structSkirstymas = 0.0;

    double classSkaitymas = 0.0;
    double classSkirstymas = 0.0;

    std::size_t structVargsiukuKiekis = 0;
    std::size_t classVargsiukuKiekis = 0;

    for (int i = 0; i < kartai; ++i)
    {
        // STRUCT nuskaitymas.

        std::vector<StudentasStruct> structStudentai;

        structSkaitymas += matuoti([&]()
            {
                nuskaitytiStructIsFailo(
                    failas,
                    structStudentai
                );
            });

        // STRUCT skirstymas.

        std::vector<StudentasStruct> structVargai;

        structSkirstymas += matuoti([&]()
            {
                const auto riba = std::stable_partition(
                    structStudentai.begin(),
                    structStudentai.end(),
                    [kriterijus](const StudentasStruct& s)
                    {
                        const double galutinis =
                            kriterijus == 2
                            ? s.galutineMed
                            : s.galutinisVid;

                        return galutinis < 5.0;
                    }
                );

                structVargai.reserve(
                    static_cast<std::size_t>(
                        std::distance(
                            structStudentai.begin(),
                            riba
                        )
                        )
                );

                std::move(
                    structStudentai.begin(),
                    riba,
                    std::back_inserter(structVargai)
                );

                structStudentai.erase(
                    structStudentai.begin(),
                    riba
                );
            });

        structVargsiukuKiekis =
            structVargai.size();

        // CLASS nuskaitymas.

        std::vector<Studentas> classStudentai;

        classSkaitymas += matuoti([&]()
            {
                nuskaitytiIsFailo(
                    failas,
                    classStudentai
                );
            });

        // CLASS skirstymas.

        std::vector<Studentas> classVargai;

        classSkirstymas += matuoti([&]()
            {
                const auto riba = std::stable_partition(
                    classStudentai.begin(),
                    classStudentai.end(),
                    [kriterijus](const Studentas& s)
                    {
                        const double galutinis =
                            kriterijus == 2
                            ? s.galutineMediana()
                            : s.galutinisVidurkis();

                        return galutinis < 5.0;
                    }
                );

                classVargai.reserve(
                    static_cast<std::size_t>(
                        std::distance(
                            classStudentai.begin(),
                            riba
                        )
                        )
                );

                std::move(
                    classStudentai.begin(),
                    riba,
                    std::back_inserter(classVargai)
                );

                classStudentai.erase(
                    classStudentai.begin(),
                    riba
                );
            });

        classVargsiukuKiekis =
            classVargai.size();
    }

    // Apskaiciuojami vidutiniai laikai.

    structSkaitymas /= kartai;
    structSkirstymas /= kartai;

    classSkaitymas /= kartai;
    classSkirstymas /= kartai;

    const double structViso =
        structSkaitymas + structSkirstymas;

    const double classViso =
        classSkaitymas + classSkirstymas;

    // Rezultatai rodomi terminale.

    std::cout
        << "\n====================================================\n"
        << "             STRUCT IR CLASS PALYGINIMAS\n"
        << "====================================================\n";

    std::cout
        << "Failas: " << failas << '\n'
        << "Studentu kiekis: " << structVargsiukuKiekis
        << " vargsiuku is " << classVargsiukuKiekis
        << " class teste\n"
        << "Testu kartojimu: " << kartai << "\n\n";

    std::cout
        << std::left
        << std::setw(18) << "Matavimas"
        << std::setw(18) << "STRUCT (v1.0)"
        << std::setw(18) << "CLASS (v1.1)"
        << '\n';

    std::cout << std::string(54, '-') << '\n';

    std::cout
        << std::setw(18) << "Nuskaitymas (s)"
        << std::setw(18) << structSkaitymas
        << std::setw(18) << classSkaitymas
        << '\n';

    std::cout
        << std::setw(18) << "Skirstymas (s)"
        << std::setw(18) << structSkirstymas
        << std::setw(18) << classSkirstymas
        << '\n';

    std::cout
        << std::setw(18) << "Bendras (s)"
        << std::setw(18) << structViso
        << std::setw(18) << classViso
        << '\n';

    std::cout << std::string(54, '-') << '\n';

    std::cout
        << "STRUCT vargsiukai: "
        << structVargsiukuKiekis << '\n';

    std::cout
        << "CLASS vargsiukai:  "
        << classVargsiukuKiekis << '\n';

    std::cout
        << "====================================================\n";
}

// =========================================
// PAGRINDINIS TESTAVIMO REZIMAS
// =========================================

void testuotiKeliskart(int kiekisPaleidimu)
{
    const std::string failai[] =
    {
        "studentai_1000.txt",
        "studentai_10000.txt",
        "studentai_100000.txt",
        "studentai_1000000.txt",
        "studentai_10000000.txt"
    };

    // 1. Failu pasirinkimas.

    std::cout
        << "\nPasirinkite, kuriuos failus norite tirti:\n";

    for (int i = 0; i < 5; ++i)
    {
        std::cout
            << i + 1 << " - "
            << failai[i] << '\n';
    }

    std::cout
        << "0 - Visi failai\n"
        << "Pasirinkite numerius, atskirtus tarpais: ";

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::string eilute;

    std::getline(
        std::cin,
        eilute
    );

    std::stringstream ss(eilute);

    std::vector<int> pasirinktiFailai;

    int pasirinkimas;

    while (ss >> pasirinkimas)
    {
        if (pasirinkimas == 0)
        {
            pasirinktiFailai = { 0, 1, 2, 3, 4 };
            break;
        }

        if (pasirinkimas < 1 || pasirinkimas > 5)
        {
            std::cout
                << "Neteisingas failo numeris: "
                << pasirinkimas << '\n';

            return;
        }

        const int indeksas = pasirinkimas - 1;

        if (std::find(
            pasirinktiFailai.begin(),
            pasirinktiFailai.end(),
            indeksas
        ) == pasirinktiFailai.end())
        {
            pasirinktiFailai.push_back(indeksas);
        }
    }

    if (pasirinktiFailai.empty())
    {
        std::cout
            << "Nepasirinktas ne vienas failas.\n";

        return;
    }

    // 2. Kriterijaus pasirinkimas.

    int kriterijus;

    std::cout
        << "\nSkirstymo kriterijus:\n"
        << "1 - Galutinis vidurkis\n"
        << "2 - Galutine mediana\n"
        << "Pasirinkite: ";

    if (!(std::cin >> kriterijus) ||
        (kriterijus != 1 && kriterijus != 2))
    {
        std::cout
            << "Neteisingas kriterijus.\n";

        return;
    }

    // 3. Konteinerio pasirinkimas.

    int konteineris;

    std::cout
        << "\nPasirinkite konteineri:\n"
        << "1 - vector\n"
        << "2 - list\n"
        << "3 - Abu\n"
        << "Pasirinkite: ";

    if (!(std::cin >> konteineris) ||
        konteineris < 1 || konteineris > 3)
    {
        std::cout
            << "Neteisingas konteinerio pasirinkimas.\n";

        return;
    }

    // 4. Automatiskai generuojami pasirinkti testiniai failai.

    std::cout
        << "\nGeneruojami pasirinkti testiniai failai...\n";

    generuotiTestiniusFailus(pasirinktiFailai);

    // 5. Vykdomi visi pasirinkti testai.

    for (int idx : pasirinktiFailai)
    {
        std::cout
            << "\n\n############################################\n"
            << "TESTUOJAMAS FAILAS: "
            << failai[idx]
            << "\n############################################\n";

        if (konteineris == 1 || konteineris == 3)
        {
            vykdytiStrategijuTestus<std::vector<Studentas>>(
                "vector",
                failai[idx],
                kiekisPaleidimu,
                kriterijus
            );

            palygintiVectorAlgoritmus(
                kiekisPaleidimu,
                failai[idx],
                kriterijus
            );
        }

        if (konteineris == 2 || konteineris == 3)
        {
            vykdytiStrategijuTestus<std::list<Studentas>>(
                "list",
                failai[idx],
                kiekisPaleidimu,
                kriterijus
            );
        }

        // STRUCT ir CLASS palyginimas kiekvienam failui.

        palygintiStructIrClass(
            failai[idx],
            kiekisPaleidimu,
            kriterijus
        );
    }

    std::cout
        << "\nTestavimas baigtas.\n"
        << "Pagrindiniai rezultatai irasyti i CSV failus.\n";
}
