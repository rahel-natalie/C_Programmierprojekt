# C_Programmierprojekt

Lösung für das C Programmierprojekt des Moduls Digitale Systeme 2021 an der Humboldt Universität zu Berlin.

3. Platz beim Performance Wettkampf.


Die Lösung musste in einer einzelnen Datei eingereicht werden.


Für die Aufgabe sollte eine Ameise über Äste laufen, die übereinander liegen und Kreuzungen bilden. Die Äste sind Kanten und die Kreuzungen sind Knoten eines Graphen.
Jede Kreuzung weist eine Markierung auf. Die Ameise beginnt an einem ausgezeichneten Startpunkt und immer wenn sie einen Knoten (Kreuzung) verlässt, erhöht sich dessen Markierung um 1.
Die Nachbarn werden lexikographisch sortiert. Die Ameise geht als nächstes zum Knoten, der an dieser Stelle steht: momentane Markierung modulo Anzahl der Nachbarknoten.
Die Eingabe gibt den Graphen, den Startpunkt und die Anzahl der Schritte an, die die Ameise macht. Ausgegeben wird der Endpunkt der Ameise und die Knoten mit Markierungen.

Der Graph wird als dynamisches Array von Knoten gespeichert und jeder Knoten erhält eine nach Namen sortierte Nachbarliste als dynamisches Array aus Indizes in das Knotenarray.
