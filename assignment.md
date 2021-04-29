# Implementace algoritmu "Mesh Multiplication"
Pomocí knihovny Open MPI implementujte v jazyce C++ algoritmus Mesh Multiplication.

## Obecné informace k projektům
* Projekt je opravován automatizovaně, je proto nutné, abyste přesně dodržovali zadání, počet parametrů při spuštění programu, význam těchto parametrů, pojmenování jednotlivých souborů.
* Projekt musí být spustitelný na **Merlinovi**, během vypracovávání projektu **sledujte fórum** konkrétního projektu. Případné dotazy či připomínky také směřujte tam.
* Implementace algoritmů je poměrně triviální, velký důraz je proto kladen na dokumentaci, správnou analýzu složitosti, dále na komunikační protokol, způsob testování. Vedle funkčnosti programu a dokumentace bude hodnocena i přehlednost zdrojového kódu, komentáře, ...
* Komunikační protokol musí být obecný, tedy pro *n* procesů.
* U experimentů je dobré předem si stanovit cíl, za jakým účelem testování vlastně provádíte, pak zhodnotit výsledky. Výsledky testů musí být vhodně reprezentovány, např. grafem. Z desítek hodnot umístěných v tabulce se složitost ověří jen velmi obtížně. V dokumentaci se také očekává přesný popis postupu, který jste zvolili pro měření doby trvání běhu algoritmu.

## Užitečné informace
Knihovna MPI je určena zejména pro praktické použití a autoři nedoporučují spouštět na jednom stroji více procesorů. Jednak poté knihovna běží pomaleji, ale hlavně je každý procesor vytvořen jako samostatný proces v sytému a za běhu mohou být vytvářeny další procesy. Toto může vyústit až v neschopnost spuštění programu, protože mu systém nedovolí vytvořit tolik synovských procesů. Doporučuji proto testovat algoritmy tak, že použijete maximálně 20 procesorů. Pokud však uvidíte hlášku podobnou `merlin.fit.vutbr.cz:24462 [ 15882,0 ,0 ] ORTE_ERROR_LOG: The system limit on number of children a process can have was reached in file base/odls_base_`, nepropadejte panice! Zkušenost ukázala, že stačí nechat merlina chvíli odpočinout (nepřekládat znovu) a poté to zkusit s nižším počtem procesorů.

## Pro zvídavé
Některé limity si můžete zobrazit na svém oblíbeném linuxu pomocí `ulimit -a` a pomocí jednotlivých přepínačů je i měnit. Maximální hodnota je specifikována `ulimit -H -a`. Jsou to tzv. hard limity a pokud nejste administrátor, jejich hodnoty měnit nemůžete. Naše programy budou psány ve stylu SPMD (same program, multiple data), všechny procesory tedy dostanou stejný kód, kde jediným rozpoznávacím znakem daného procesoru je jeho rank. Pokud chcete projekty úspěšně vytvořit, musíte tuto filosofii respektovat. Open MPI neumí měnit po startu počet procesorů, proto je vhodné si vytvořit skript, který toto spočítá dopředu podle použitého algoritmu a počtu vstupních hodnot. Při vypracování projektů tedy můžete počítat s tím, že hodnoty budou při testování zadávány korektně. Pokud budete na merlinovi vytvářet spustitelné skripty, je třeba jim přidat příznak spustitelnosti: `chmod +x file_name`.

## Vstup a výstup
Vstupem jsou textové soubory `mat1` a `mat2`.

Výsledná matice, která je získána operací `mat1*mat2`, bude vypsána na standardní výstup ve formátu specifikovaném níže.

Jako oddělovač čísel na řádku použijte mezeru (na vstupu i výstupu), jako oddělovač jednotlivých řádků pak znak nového řádku "\n".

### mat1
První řádek mat1 obsahuje počet řádků.
```
3
1	-1
2	2
3	3
```

### mat2
První řádek mat2 udává počet sloupců.
```
4
1	-2	-2	-8
-1	-2	7	10
```

### Výstup obecně
```
radky:sloupce
a	b	c
d	e	f
g	h	i
```

### Příklad výstupu
```
2:3
1	2	3
4	5	6
```

## Postup

Užijte dodaný skript `test.sh`. Výsledný *program nebude přijímat žádné parametry* (jména souborů s maticemi zná a nic více nepotřebuje, načítání velikosti matic řešte v programu).

```
#!/bin/bash
 
mat1=$(head -n1 mat1)
mat2=$(head -n1 mat2)
 
cpus=$((mat1*mat2))
 
mpic++ --prefix /usr/local/share/OpenMPI -o mm mm.cpp -std=c++0x
mpirun --prefix /usr/local/share/OpenMPI -np $cpus mm
rm -f mm
```

## Dokumentace

* Obdobné jako v 1. projektu
* Hodnotí se i vzhled a jazyková úroveň.
* Obvyklý rozsah 3 strany (nepište zadání, úvodní stranu, obsah, pseudokódy).

Součástí řešení je dokumentace, která bude o rozsahu **maximálně 3 strany** (rozumné a odůvodněné překročení limitu stran není důvod k bodové srážce) funkčního textu.

Do dokumentace **nedávejte**:
* Úvodní stranu, obsah, popis zadání.

V dokumentaci **popište**:
* Rozbor a analýza algoritmu Mesh Multiplication, teoretická složitost - prostorová, časová náročnost a cena, sekvenční diagram (popis zasílání zpráv mezi procesy - jednoduchý a obecný).
* Experimenty s různě velkými maticemi pro ověření časové složitosti (očekává se graf, nikoliv tabulka), nikoliv měření počtu kroků algoritmu
* Graf - osa x (vodorovná) bude počet procesorů/prvků a osa y (svislá) bude čas, pozor na měřítka obou os, graf bude mít popisky os a bude z něj na první pohled zřejmý závěr
* Závěr - zhodnocení experimentů, zamyšlení nad reálnou složitostí.

## Implementace

* C++ (přeložitelné pomocí mpic++ kvůli jednotnému skriptu test.sh, ne nutně objektově)
* Open MPI

## Odevzdání
Do wisu se odevzdává jeden archiv xlogin00.{tar|tgz|zip}, který bude velký do 1MB, a který obsahuje:
* zdrojový kód- mm.cpp,
* hlavička- mm.h (pokud ji využijete),
* dokumentaci- xlogin00.pdf,
* nic jiného... (binárky, obrázky, matice, test.sh, ...)

Platí, že kvalita každé z části vzhledem k požadavkům má vliv na bodové ohodnocení . Počítejte s tím, že veškerá uvedená jména souborů jsou *case sensitive*.

## Doplňkové informace

* Výstupy programu budou strojově kontrolovány, mějte to prosím na paměti při implementaci.
* Budou testovány především korektní vstupy.
* Při implementaci se zaměřte zejména na důkladné pochopení toho, jak algoritmus funguje a co jednotlivé procesy dělají.
