Aby uruchomić program należy:
1) Przenieść plik .las do folderu data
2) Wykonać w terminalu: sudo docker build -t projekt-cpk .
Zbuduje to obraz Docker'a z używaną biblioteką pdal.
3) Następnie: sudo docker run   --rm   -v "$(pwd)":/app   moj-projekt-pdal
4) Plik wynikowy pojawi się pod nazwą output.las

Alternatywnie, jeżeli biblioteka pdal znajduje się na lokalnej maszynie można wywołać program poleceniem:
pdal pipeline classify.json --readers.las.filename=data/*.las --writers.las.filename=output.las 



Rozpoznawalne klasy:
classification = 1 -> nieklasyfikowane
classification = 2 -> grunt - ziemia
classification = 3 -> niska wegetacja
classification = 4 -> średnia wegetacja
classification = 5 -> wysoka wegetacja
classification = 7 -> szum
classification = 9 -> woda
classification = 18 -> słupy
classification = 20 -> ściany
classification = 21 -> dachy
classification = 28 -> chodnik
classification = 29 -> pas zieleni









