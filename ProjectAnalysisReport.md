# Izveštaj analize Boost.Regex

## 1. Jedinični testovi

Za funkcionalno i API testiranje biblioteke Boost.Regex korišćen je okvir `Google Test`, uz alat `gcov\lcov` za generisanje izvetštaja o pokrivenosti.

Cilj testiranja nije samo provera funkcionalnosti parsera regularnih izraza. Značaj deo testova proverava ponašanje javnog API-ja biblioteke, tj. najznačajnijih modula poput `basic_regex`, `match_results`, `regex_match`, `regex_search` i slične.

Direktorijum `unit_tests` sadrži:
- 11 datoteka sa testovima
- sktipru `coverage.sh` za pokretanje testova sa opcijom praćenja pokrivenosti i generisanje izveštaja
- primer konfiguracione datoteke `coverage.cfg.example`
- rezultata pokretanja testova `reports/output.log` i izveštaje o pokrivenosti

### 1.1 Konfiguracija i izvršavanje

Pre pokretanja, neophodno je iskopirati datoteku `coverage.cfg.example` u `coverage.cfg` i navesti u neophodno polje `BOOST_ROOT` putanju do lokalne Boost instalacije. Ostale opcije se proizvoljne i menjaju se po potrebi. 

Pokretanjem skripte `coverage.sh` generiše se podirektorijum `reports` sa rezultatima testova `reports/output.log`. Po završetku skripte automatski se otvara izveštaj o pokrivenosti, ali ako to nije slučaj, on se može naći u `reports/html/index.html`. 
### 1.2 Organizacija i sadržaj testova 

| Datoteka | Broj testova | Glavni predmet provere |
|---|---:|---|
| `test_basic_regex.cpp` | 6 | Konstrukcija, dodela, poređenje i provera polja klase `basic_regex` |
| `test_match_results_sub_match.cpp` | 23 | `match_results`, `sub_match`, konstrukcija, kopiranje, dodela, pristup grupama i provera postuslova |
| `test_matching.cpp` | 17 | `regex_match`, `regex_search`, provera raznih konstrukcija i postuslova |
| `test_replace.cpp` | 17 | `regex_replace`, provera raznih konstrukcija i opcija za formatiranje |
| `test_iterators.cpp` | 21 | `regex_iterator`, `regex_token_iterator`, različite provere ponašanja iteratora i izdavajanja tokena |
| `test_match_flags.cpp` | 13 | Opcije za prevodjenje regularnih izraza i pronalaženja podudaranja |
| `test_groups.cpp` | 21 | Imenovane i neimenovane grupe, opcione grupe i referenciranje grupa |
| `test_character_classes_and_word_boundaries.cpp` | 13 | Klase karaktera, granice reči i linija, literali i wildcard karakter |
| `test_quantifiers_and_alternation.cpp` | 9 | Kvantifikatori, pohlepno/nepohlepno ponašanje i alternacija |
| `test_lookarounds.cpp` | 10 | Lookahead, lookbehind i atomične grupe |
| `test_regex_compilation.cpp` | 6 | Greske pri prevodjenju neispravnih regularnih izraza |


### 1.3 Rezultati

Analizom `reports/outputs.log` utvrdjuje se da je od 156 test slučajeva 156 uspešno izvršeno.

```console
[----------] Global test environment tear-down
[==========] 156 tests from 21 test suites ran. (153 ms total)
[  PASSED  ] 156 tests.
```

Izveštaj o pokrivenosti daje naredne infromacije: 

| Metrika | Pokriveno | Ukupno | Pokrivenost |
|---|---:|---:|---:|
| Linije | 2856 | 5771 | 49.5% |
| Funkcije | 583 | 790 | 73.8% |
| Grane | 1835 | 6057 | 30.3% |

Detaljnija statistika data je na slici:

[!Detaljna statistika pokrivenosti](images/coverage.png)

Najviša pokrivenosti ostvarena je u javnim funkcijama koja testovi direktno pozivaju. Datoteke `regex_match.hpp`, `regex_search.hpp`, `regex_replace.hpp`, `regex_iterator.hpp`, `regex_token_iterator.hpp`, `basic_regex.hpp` i `sub_match.hpp` imaju veoma visoku pokrivenost linija i funkcija, što je u skladu sa predmetima testiranja. Slabiju pokrivenost imaju moduli zaduženi za parsiranje regularnih izraza, a kako je naše testiranje obuhvatilo samo osnovne konstrukte regularnih izraza ovo bismo tumačili kao solidne rezultate. Primećuje se da je porkivenost grana u svim datotekama značajno gora od ostale dve metrike, čemu doprinose i brojni izuzeci u okviru biblioteke. 

## 2. Valgrind Memcheck

Za dinamičku analizu upravljanja memorijom korišćen je alat **Memcheck**. U ovom projektu alat je pokrenut nad izvršivom datotekom `regex_tests` koja sadrži skup jedinica koda prethodno opisanih, a glavni dikretorijum alata je `valgrind/memcheck`. 

Bitan detalj je onemogućena upotreba sanitajzera, što prouzrokuje preskaknje jednog testa koji se oslenja na prisustvo santijazera adresa.

### 2.1 Konfiguracija alata 

Za pokretanje alata koristi se se skripta `valgrind/memcheck/memcheck.sh`. Pre pokretanja neophodno je iskopirati `memcheck.cfg.example` u `memcheck.cfg` i popuniti odgovarajuće vrednosti. 

### 2.2 Rezultati

Valgrind Memcheck je pokrenut sa opcijama `--leak-check=full` i `--show-leak-kinds=all` koji imaju za cilj da za svako pronađeno curenje bilo koje vrste postoji detaljan izveštja o istom. 

Rezultat pokretanja testova se može naći u `reports/run_<timestamp>/output.log` unutar direktorijuma alata. Rezultati konkretnog pokretanja dostupno na repozitorijumu na putanji `reports/run_regex_tests/output.log` pokazuju da su testovi izvršeni u skladu sa očekivanjima: od 156 testova, 155 je uspešno, dok je 1 preskočen.

```console
[----------] Global test environment tear-down
[==========] 156 tests from 21 test suites ran. (1652 ms total)
[  PASSED  ] 155 tests.
[  SKIPPED ] 1 test, listed below:
[  SKIPPED ] RegexIterator.SourceStringMustOutliveIterator
```

Primetimo da je izvšavanje znatno sporije nego kod klasičnog pokretanja testova.

Rezultati Memcheck alata se nalaze u `reports/run_<timestamp>/memcheck_regex_tests.log`, a rezultati prethodno pomenutog konkretnog pokretanja su dati `reports/run_regex_tests/memcheck_regex_tests.log`. Alat tokom izvršavanja nije detektovao ni jednu grešku u rukovanju sa memorijom što je priloženu u pomenutoj log datoteci. Njen isečak dat je u nastavku:

```console
==117433== HEAP SUMMARY:
==117433==     in use at exit: 0 bytes in 0 blocks
==117433==   total heap usage: 4,612 allocs, 4,612 frees, 928,631 bytes allocated
==117433== 
==117433== All heap blocks were freed -- no leaks are possible
==117433== 
==117433== For lists of detected and suppressed errors, rerun with: -s
==117433== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

Eksperimenta radi, modifikovana je skripta za pokretanje alata tako da se poziva nad izvršivom datotekom `regex_app` sa argumentima `18 100`. Ovo je mala aplikacija čiji se izvorni kod može naći na putanji `src/main.cpp`. Unutar nje se pokreću tzv. zli regularni izrazi (eng. evil regex) nad dugačkim ulazima. Prvi argument `18` daje metriku za složenost ulaza regularnog izraza, a drugi argument je broj iteracija pretrage (`boost::regex_search` ili `boost::regex_match`) nad datim ulazom. Memcheck ni ovde nije našao greške. Rezultati ovog konkretnog pokretanja mogu se naći na putanji `reports/run_regex_app` a u nastavku je dat isečak memcheck izveštaja.

```console
==118136== HEAP SUMMARY:
==118136==     in use at exit: 0 bytes in 0 blocks
==118136==   total heap usage: 750 allocs, 750 frees, 197,968 bytes allocated
==118136== 
==118136== All heap blocks were freed -- no leaks are possible
==118136== 
==118136== For lists of detected and suppressed errors, rerun with: -s
==118136== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
