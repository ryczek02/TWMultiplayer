# TWMP

Klient oraz serwer multiplayer do gry Tajemnicza Wyspa studia Calaris.

## Instalacja

```bash
git clone https://github.com/ryczek02/TWMultiplayer.git
```
Plik .sln otwieramy jako rozwiązanie w VS2019.

Opcjonalnie można pobrać skompilowany plik z zakładki Releases.

## Użycie

Gracze powinni odkryć co najmniej dwie postacie (np. wybrana + słoń), na ten czas nie ma identyfikacji postaci. Serwer jest niezależny od gry, pierwsza osoba może wybrać swoją postać i uruchomić TWMPClient.exe. Drugi klient musi się przełączyć na słonia i również może otworzyć klienta. Powinna nastąpić próba łączenia.

## TODO

- [ ] Dane pozycji gracza
  - [x] Lokacja
  - [ ] Obrót
- [ ] Informacje o graczu
  - [ ] HP
  - [ ] Stamina
  - [ ] Animacja

## Kompilacja

W pliku klienta, main.cpp ustawiamy PORT oraz TICKRATE wychodzący.

```c++
#define DEFAULT_PORT 21370
#define MP_TICKRATE 128
```
W pliku serwera, Server.cpp ustawiamy IP oraz TICKRATE.

```c++
#define SERVER_IP "127.0.0.1"
#define MP_TICKRATE 128
```
W pliku serwera, main.cpp ustawiamy port oraz wielkość bufora.

```c++
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 21370
```

Projekt obecnie nie wymaga dodatkowych bibliotek, wystarczą domyślne biblioteki kompilatora C++ dla Windowsa.

## Schemat działania 

![Schemat działania](https://raw.githubusercontent.com/ryczek02/TWMultiplayer/master/Untitled%20Diagram.svg)

Czytane z pamięci adresy pozycji są zamieniane na pakiety, a następnie przysyłane pakiety u drugiego gracza są zmieniane na wartości, które są wstrzykiwane do pamięci procesu gry. 

## Licencja
[MIT](https://choosealicense.com/licenses/mit/)
