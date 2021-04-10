
# Demon synchronizujący dwa podkatalogi
<p>Program który otrzymuje co najmniej dwa argumenty: ścieżkę źródłową, ścieżkę docelowa . Jeżeli któraś ze ścieżek nie jest katalogiem program powraca natychmiast z komunikatem błędu. W przeciwnym wypadku staje się demonem. Demon wykonuje następujące czynności: śpi przez piec minut (czas spania można zmieniać przy pomocy dodatkowego opcjonalnego argumentu), po czym po obudzeniu się porównuje katalog źródłowy z katalogiem docelowym. Pozycje, które nie są zwykłymi plikami są ignorowane (np. katalogi i dowiązania symboliczne). Jeżeli demon (a) napotka na nowy plik w katalogu źródłowym, i tego pliku brak w katalogu docelowym lub (b) plik w katalogu docelowym ma późniejsza datę ostatniej modyfikacji demon wykonuje kopie pliku z katalogu źródłowego do katalogu docelowego - ustawiając w katalogu docelowym datę modyfikacji tak, aby przy kolejnym obudzeniu nie trzeba było wykonać kopii (chyba ze plik w katalogu źródłowym zostanie ponownie zmieniony). Jeżeli zaś odnajdzie plik w katalogu docelowym, którego nie ma w katalogu źródłowym to usuwa ten plik z katalogu docelowego. Możliwe jest również natychmiastowe obudzenie się demona poprzez wysłanie mu sygnału SIGUSR1. Wyczerpująca informacja o każdej akcji typu uśpienie/obudzenie się demona (naturalne lub w wyniku sygnału), wykonanie kopii lub usuniecie pliku jest przesłana do logu systemowego. Informacja ta powinna zawierać aktualna datę. Operacje kopiowania mają być wykonane za pomocą niskopoziomowych operacji read/write. (14p).  </p>
Dodatkowo:
<ol>
<li>
Opcja -R pozwalająca na rekurencyjną synchronizację katalogów (teraz pozycje będące katalogami nie są ignorowane). W szczególności jeżeli demon stwierdzi w katalogu docelowym podkatalog którego brak w katalogu źródłowym powinien usunąć go wraz z zawartością. (8p)
</li>
<li>
W zależności od rozmiaru plików dla małych plików wykonywane jest kopiowanie przy pomocy read/write a w przypadku dużych używany jest bardziej efektywny mechanizm, np.: przy pomocy mmap/write (plik źródłowy zostaje zamapowany w całości w pamięci) lub za pomocą dedykowanego wywołania (np. sendfile czy copy_file_range). Próg dzielący pliki małe od dużych może być przekazywany jako opcjonalny argument. Wykonaj analizę wpływu danej metody na szybkość kopiowania plików i przedstaw wyniki w dokumentacji. (12p)
</li>
</ol>
