#include "Studentas.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

Studentas::Studentas() noexcept
    : vardas_(), pavarde_(), namuDarbai_(), egzaminas_(0),
    galutinisVid_(0.0), galutinisMed_(0.0)
{
}

Studentas::Studentas(std::string vardas, std::string pavarde,
    std::vector<int> namuDarbai, int egzaminas)
    : vardas_(std::move(vardas)), pavarde_(std::move(pavarde)),
    namuDarbai_(std::move(namuDarbai)), egzaminas_(egzaminas),
    galutinisVid_(0.0), galutinisMed_(0.0)
{
    if (vardas_.empty() || pavarde_.empty())
        throw std::invalid_argument(
            "Studento vardas ir pavarde negali buti tusti.");

    if (egzaminas_ < 1 || egzaminas_ > 10)
        throw std::invalid_argument(
            "Egzamino pazymys turi buti nuo 1 iki 10.");

    for (int pazymys : namuDarbai_)
        if (pazymys < 1 || pazymys > 10)
            throw std::invalid_argument(
                "Namu darbu pazymiai turi buti nuo 1 iki 10.");

    apskaiciuotiGalutinius();
}

Studentas::Studentas(const Studentas& kitas)
    : vardas_(kitas.vardas_), pavarde_(kitas.pavarde_),
    namuDarbai_(kitas.namuDarbai_), egzaminas_(kitas.egzaminas_),
    galutinisVid_(kitas.galutinisVid_),
    galutinisMed_(kitas.galutinisMed_)
{
}

Studentas::Studentas(Studentas&& kitas) noexcept
    : vardas_(std::move(kitas.vardas_)),
    pavarde_(std::move(kitas.pavarde_)),
    namuDarbai_(std::move(kitas.namuDarbai_)),
    egzaminas_(kitas.egzaminas_),
    galutinisVid_(kitas.galutinisVid_),
    galutinisMed_(kitas.galutinisMed_)
{
}

Studentas& Studentas::operator=(const Studentas& kitas)
{
    if (this != &kitas)
    {
        vardas_ = kitas.vardas_;
        pavarde_ = kitas.pavarde_;
        namuDarbai_ = kitas.namuDarbai_;
        egzaminas_ = kitas.egzaminas_;
        galutinisVid_ = kitas.galutinisVid_;
        galutinisMed_ = kitas.galutinisMed_;
    }

    return *this;
}

Studentas& Studentas::operator=(Studentas&& kitas) noexcept
{
    if (this != &kitas)
    {
        vardas_ = std::move(kitas.vardas_);
        pavarde_ = std::move(kitas.pavarde_);
        namuDarbai_ = std::move(kitas.namuDarbai_);
        egzaminas_ = kitas.egzaminas_;
        galutinisVid_ = kitas.galutinisVid_;
        galutinisMed_ = kitas.galutinisMed_;
    }

    return *this;
}

Studentas::~Studentas() = default;

const std::string& Studentas::vardas() const noexcept
{
    return vardas_;
}

const std::string& Studentas::pavarde() const noexcept
{
    return pavarde_;
}

const std::vector<int>& Studentas::namuDarbai() const noexcept
{
    return namuDarbai_;
}

int Studentas::egzaminoPazymys() const noexcept
{
    return egzaminas_;
}

double Studentas::galutinisVidurkis() const noexcept
{
    return galutinisVid_;
}

double Studentas::galutineMediana() const noexcept
{
    return galutinisMed_;
}

double Studentas::skaiciuotiVidurki(
    const std::vector<int>& pazymiai)
{
    if (pazymiai.empty())
        return 0.0;

    double suma = 0.0;

    for (int pazymys : pazymiai)
        suma += pazymys;

    return suma / pazymiai.size();
}

double Studentas::skaiciuotiMediana(
    const std::vector<int>& pazymiai)
{
    if (pazymiai.empty())
        return 0.0;

    std::vector<int> kopija = pazymiai;

    std::sort(kopija.begin(), kopija.end());

    const std::size_t vidurys = kopija.size() / 2;

    if (kopija.size() % 2 == 1)
        return kopija[vidurys];

    return (kopija[vidurys - 1] + kopija[vidurys]) / 2.0;
}

void Studentas::apskaiciuotiGalutinius()
{
    galutinisVid_ =
        0.4 * skaiciuotiVidurki(namuDarbai_) +
        0.6 * egzaminas_;

    galutinisMed_ =
        0.4 * skaiciuotiMediana(namuDarbai_) +
        0.6 * egzaminas_;
}

bool rikiuotiPagalVarda(const Studentas& a, const Studentas& b)
{
    if (a.vardas() != b.vardas())
        return a.vardas() < b.vardas();

    return a.pavarde() < b.pavarde();
}

template <typename Container>
void nuskaityti(const std::string& failas, Container& grupe)
{
    std::ifstream f(failas);

    if (!f)
        throw std::runtime_error(
            "Nepavyko atidaryti failo: " + failas);

    grupe.clear();

    std::string eilute;
    std::getline(f, eilute);

    while (std::getline(f, eilute))
    {
        if (eilute.empty())
            continue;

        std::stringstream ss(eilute);

        std::string vardas, pavarde;

        if (!(ss >> vardas >> pavarde))
            continue;

        std::vector<int> pazymiai;

        int pazymys;
        bool neteisingas = false;

        while (ss >> pazymys)
        {
            if (pazymys < 1 || pazymys > 10)
            {
                neteisingas = true;
                break;
            }

            pazymiai.push_back(pazymys);
        }

        if (neteisingas || !ss.eof() || pazymiai.empty())
            continue;

        const int egzaminas = pazymiai.back();

        pazymiai.pop_back();

        grupe.emplace_back(
            std::move(vardas),
            std::move(pavarde),
            std::move(pazymiai),
            egzaminas);
    }
}

void nuskaitytiIsFailo(
    const std::string& failas,
    std::vector<Studentas>& grupe)
{
    nuskaityti(failas, grupe);
}

void nuskaitytiIsFailo(
    const std::string& failas,
    std::list<Studentas>& grupe)
{
    nuskaityti(failas, grupe);
}

template <typename Container>
void issaugoti(
    const std::string& failas,
    const Container& grupe)
{
    std::ofstream f(failas);

    if (!f)
        throw std::runtime_error(
            "Nepavyko sukurti failo: " + failas);

    f << std::left
        << std::setw(15) << "Pavarde"
        << std::setw(15) << "Vardas"
        << std::setw(20) << "Galutinis (Vid.)"
        << std::setw(20) << "Galutinis (Med.)"
        << '\n';

    f << std::string(70, '-') << '\n';

    for (const Studentas& s : grupe)
    {
        f << std::left
            << std::setw(15) << s.pavarde()
            << std::setw(15) << s.vardas()
            << std::fixed << std::setprecision(2)
            << std::setw(20) << s.galutinisVidurkis()
            << std::setw(20) << s.galutineMediana()
            << '\n';
    }
}

void issaugotiIFaila(
    const std::string& failas,
    const std::vector<Studentas>& grupe)
{
    issaugoti(failas, grupe);
}

void issaugotiIFaila(
    const std::string& failas,
    const std::list<Studentas>& grupe)
{
    issaugoti(failas, grupe);
}
