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
  - [ ] Obrót (znany OPCODE, nie włączone)
- [ ] Informacje o graczu
  - [ ] HP (znany OPCODE, nie włączone)
  - [ ] Stamina (znany OPCODE, nie włączone)
  - [ ] Animacja

|         |  Gracz 1   |  Gracz 2   |
|:-------:|------------|------------|
| PosX    | 0x00E16974 | 0x00E16C64 |
| PosY    | 0x00E16978 | 0x00E16C68 |
| PosZ    | 0x00E1697C | 0x00E16C6C |
| RotZ    | 0x00E16998 | 0x00E16C88 |
| HP      | 0x00E169DC | 0x00E16CCC |
| Stamina | 0x00E16BFC | 0x00E16EEC |

## Kompilacja

Projekt obecnie nie wymaga dodatkowych bibliotek, wystarczą domyślne biblioteki kompilatora C++ dla Windowsa.

## Schemat działania 

![Schemat działania](https://raw.githubusercontent.com/ryczek02/TWMultiplayer/master/Untitled%20Diagram.svg)

Czytane z pamięci adresy pozycji są zamieniane na pakiety, a następnie przysyłane pakiety u drugiego gracza są zmieniane na wartości, które są wstrzykiwane do pamięci procesu gry. 

## Licencja
[MIT](https://choosealicense.com/licenses/mit/)
