# master-rad
Master rad na temu "Implementacija tekst editora za pisanje koda"

## Uputsvo za pokretanje

Kada klonirate repozitorijum pokrenite **powershell** i pozicionirajte se u repozitorijum.

Zatim unesite inicijalizujte i ažurirajte podmodule

```
git submodule init 
```
```
git submodule update
```

Kada su podmoduli ažurirani možemo da krenemo sa izgradnjom.

Napravimo *build* datoteku i pozicioniramo se u nju.
```
mkdir build
cd build
```
Pokrenemo **cmake** alat da bi nam generisao fajlove za izgradnju
```
cmake ..
```

Pokrenemo izgradnju programa.

```
cmake --build .
```

Kada je ovo gotovo pozicioniramo se u *Debug* folder i pokrećemo program.
```
cd Debug
.\text-editor.exe
```
