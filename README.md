# Analiza projekta Boost.Regex

Seminarski rad iz predmeta **Verifikacija softvera** posvećen analizi biblioteke **Boost.Regex**.

**Boost.Regex** je C++ biblioteka koja omogućava kompajliranje regularnih izraza, potpuno i parcijalno uparivanje teksta, pretragu, zamenu, iteriranje kroz poklapanja i rad sa različitim.

Više informacija se može naći na [zvanicnom repozitorijumu](https://github.com/boostorg/regex) bublioteke. Analiza je radjena na `master` grani (SHA komita: `f439e22ae41fa647bd7de16c2d92deb232593985`)

## Autor

Anja Cvetković 1009/2024 (mi241009@alas.matf.bg.ac.rs)

## Alati

| Alat ili tehnika | Namena | Direktorijum |
|---|---|---|
| GoogleTest with LCOV | Funkcionalno i API testiranje sa merenjem pokrivenosti | `unit_tests/` |
| Valgrind Memcheck | Otkrivanje curenja memorije i neispravnih pristupa memoriji | `valgrind/memcheck/` |
| AddressSanitizer| Otkrivanje grešaka pri radu sa memorijom | `sanitizers/` |
| UndefinedBehaviorSanitizer | Otkrivanje nedefinisanog ponašanja | `sanitizers/` |
| Google FuzzTest | Generisanje velikog broja neočekivanih ulaza | `fuzztest/` |
| Mull | Procena kvaliteta testova mutacionim testiranjem | `mull/` |
| perf | Merenje izvršavanja i pronalaženje skupih funkcija | `perf/` |
| Cppcheck | Statička analiza izvornog koda | `cppcheck/` |

Svaki alat ima poseban direktorijum koji sadrži:

1. skriptu za konfiguraciju, izgradnju i pokretanje alata
2. primer lokalnog konfiguracionog fajla sa nastavkom `.cfg.example`
3. direktorijum `reports/` za dobijene rezultate

Lokalni `.cfg` fajlovi i generisani izveštaji nisu namenjeni verzionisanju.

## Predusolvi 

Analiza je razvijana i pokretana u Linux okruzenju uz korišćenje lokalno izrađene statičke biblioteke Boost.Regex. 

### Preuzimanje i izgradnja Boost.Regex biblitoteke

```bash
git clone --recursive https://github.com/boostorg/boost.git
cd boost
git submodule update --init --recursive

git -C libs/regex checkout \
    f439e22ae41fa647bd7de16c2d92deb232593985
```

```bash
./bootstrap.sh

./b2 libs/regex \
    toolset=gcc \
    link=static \
    variant=debug \
    cxxflags="-O0 -g" \
    --prefix="$HOME/path_to_boost" \
    install
```
### Opšti obrazac konfiguracije alata

Primer konfiguracionog fajla se kopira u isti direktorijum bez nastavka `.example`, a zatim se u kopiji unose lokalne putanje i željene opcije.

Primer:

```bash
cp scripts/project.cfg.example scripts/project.cfg
nano scripts/project.cfg
```

U konfiguracionim fajlovima je neophodno navesti vrednosti polja označenih sa **Required**, sto je upravo putanja do lokalne Boost.Regex instalacije:

```bash
BOOST_ROOT=$HOME/path_to_boost
```
