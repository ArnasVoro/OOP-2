#pragma once

#include <string>
#include <vector>
#include <list>

// Studentas yra pilnaverte klase: duomenys privatūs,
// o skaitymui naudojami getteriai.
class Studentas
{
public:
    Studentas() noexcept;

    Studentas(std::string vardas, std::string pavarde,
        std::vector<int> namuDarbai, int egzaminas);

    Studentas(const Studentas& kitas);
    Studentas(Studentas&& kitas) noexcept;

    Studentas& operator=(const Studentas& kitas);
    Studentas& operator=(Studentas&& kitas) noexcept;

    ~Studentas();

    const std::string& vardas() const noexcept;
    const std::string& pavarde() const noexcept;
    const std::vector<int>& namuDarbai() const noexcept;

    int egzaminoPazymys() const noexcept;
    double galutinisVidurkis() const noexcept;
    double galutineMediana() const noexcept;

    static double skaiciuotiVidurki(
        const std::vector<int>& pazymiai);

    static double skaiciuotiMediana(
        const std::vector<int>& pazymiai);

private:
    std::string vardas_;
    std::string pavarde_;
    std::vector<int> namuDarbai_;

    int egzaminas_;
    double galutinisVid_;
    double galutinisMed_;

    void apskaiciuotiGalutinius();
};

bool rikiuotiPagalVarda(
    const Studentas& a,
    const Studentas& b);

void nuskaitytiIsFailo(
    const std::string& failas,
    std::vector<Studentas>& grupe);

void nuskaitytiIsFailo(
    const std::string& failas,
    std::list<Studentas>& grupe);

void issaugotiIFaila(
    const std::string& failas,
    const std::vector<Studentas>& grupe);

void issaugotiIFaila(
    const std::string& failas,
    const std::list<Studentas>& grupe);
