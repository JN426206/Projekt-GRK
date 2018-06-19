<p style="text-align:center">&nbsp;</p>

<p><strong><span style="font-size:20.0pt">Dokumentacja projektu</span></strong></p>

<p><strong><span style="font-size:12.0pt">Autor: Jan Nowak</span></strong></p>

<p>Program używa API OpenGL oraz biblioteki free glut używanych do tworzenia grafiki tr&oacute;jwymiarowej.&nbsp; Tworzenie macierzy perspektywy przyczynia się do uzyskania przestrzennego widoku na monitorze 2D.</p>

<p>Program umożliwia interakcje z użytkownikiem za pomocą funkcji keyboard. Dzięki niej mamy możliwość poruszania obiektem &quot;shark&quot;.<br />
Możliwe ruchu to:</p>

<ul>
	<li>ruchy w bok,</li>
	<li>do przodu oraz do tyłu</li>
	<li>a także obr&oacute;t wok&oacute;ł własnej osi.</li>
</ul>

<p>Efekt ten uzyskany został za pomocą zmiany pozycji kamery w zależności jaki przycisk został wciśnięty, a efekt poruszającego się modelu realizuje przyczepienie rekina do kamery.</p>

<p>Program wczytuje modele z plik&oacute;w o rozszerzeniu obj.</p>

<p>W projekcie zastosowane są shadery dla efekt&oacute;w świetlnych.</p>

<p>Aby modele bardziej przypominały rzeczywiste obiekty wczytywane są odpowiednie tekstury.</p>

<p>Dzięki zmiennej time i pobieranie do niej czasu działania programu, macierzy rotacji oraz innych obliczeń, ustalane są takie macierze translacji aby obiekty poruszały się prawidłowo po wyznaczonej ścieżce oraz uzyskujemy animację ruchu min. takie jak pływające rybki.</p>

<p>Ilość generowanych rybek ustalana jest w programie w zmiennej NUM_FISHES. Pozycje rysowania rybek generowane są losowo w celu uzyskania lepszego odwzorowania rzeczywistości.</p>
