# Programy wielowątkowe w języku C

Repozytorium zawiera dwa programy demonstracyjne implementujące klasyczne problemy synchronizacji procesów z użyciem wątków POSIX (`pthread`) i mechanizmów IPC (pamięć współdzielona i semafory System V).

## Spis plików

- **`tasmociag.c`** – symulacja taśmociągu, na którym producent umieszcza cegły, a konsument je odbiera, z uwzględnieniem ograniczeń wagowych i liczby cegieł.
- **`palacze.c`** – rozszerzona wersja problemu palaczy, w której każdy z palaczy posiada tylko jeden składnik i dokonuje zakupu pozostałych dwóch na rynku kontrolowanym przez agenta ustalającego losowe ceny.

## Kompilacja

Wymagany jest kompilator C z obsługą `pthread` i `IPC` (np. `gcc`):

```bash
gcc -o tasmociag tasmociag.c -lpthread
gcc -o palacze palacze.c -lpthread
```

## Uruchamianie

```bash
./tasmociag
./palacze
```
Programy działają w trybie ciągłym (lub do momentu przetworzenia określonej liczby elementów w przypadku tasmociag.c). W obu przypadkach można zaobserwować współdziałanie wątków oraz efekty synchronizacji.

## Opis działania  *tasmociag.c*
Producent generuje cegły o wadze 1 lub 2 kg.
Taśma może przenosić maksymalnie 10 cegieł lub maksymalnie 10 kg łącznej wagi.
Konsument zdejmuje cegły z taśmy.
Używane są mutexy i zmienne warunkowe do synchronizacji.

## Opis działana: *palacze.c*
Każdy z trzech palaczy posiada jeden składnik: tytoń, papier lub zapałki.
Agent co pewien czas ustala losowe ceny pozostałych składników.
Palacze kupują brakujące składniki tylko jeśli ich na nie stać.
Synchronizacja opiera się na semaforach Systemu V.
