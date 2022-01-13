
ecgViewer : visualiseur de signal électro-cardiogramme avec des fonctionnalités très limités


DEPENDANCES

 - Qt 6.2 / QtQuick 2
 - G++ 9.3.0
 - boost 1.71 histogram (header only)
testé sur Ubuntu 20.04.3 LTS:


INPUT

  Ensemble de fichiers "ecg-*.txt" contenu dans le répertoire ./data du dépôt

  Sans ce répertoire, le programme restera bloqué sur une pop-up modale contenant qu'un bouton
  "Load Sequence". Une fois le répertoire renseigné, le programme lira automatiquement les fichiers
  et affichera la vue avec les boutons et le graphique correspondant aux données du répertoire.


GRAPHIQUE

  L'abscisse représente le temps en secondes, par défaut, on affiche 6 secondes de données
  à la fois (variable 'windowWidthInSeconds' modifiable).
  L'ordonnée représente l'amplitude du signal, les varibles sont bornées entre -32768 et 32767


LECTURE

  On propose un mini-player pour permettre à l'utilisateur de jouer les données

  On permet à l'utilisateur de faire les actions suivantes: Play, Pause, Resume et Stop

  On doit noter que la lecture est basé sur le temps écoulé côté C++ et non au niveau du
  Timer côté QML car celui-ci à des limitations et ne permet pas de descendre en dessous
  d'un rafraîchissement de 10 ms.

  
CORRECTION

  Fonction basique de correction

  Une simple correction est appliquée sur le signal basée sur la moyenne selon une fenêtre réglable
  par l'utilisateur (par défaut fixée à 30). En effet, chaque valeur Y seront moyennée avec leur n-valeurs
  voisins. Ceci à l'avantage de produire un signal plus lisse.

  Possibilité d'appliquer une correction sur le signal d'entrée en faisant une moyenne sur valeur Y
  avec ses voisins antérieurs et postérieurs. Un champ texte en haut à gauche permet de faire
  varier le nombre de voisins avec lesquelles on veut calculer la moyenne (fixée par défaut à 30)


OUTLIERS

  Fonction utilisant boost::histogram permettant de réduire les valeurs aberrantes

  La fonctionalité "Remove Outliers" a été ajouté pour enlever les valeurs extrêmes faiblement représentées.
  En effet, grâce à un histogramme, nous allons s'intéresser uniquement aux valeurs les plus représentées.
  Toutes les autres seront ecrasées avec des valeurs bornes recalculées afin d'avoir une échelle selon l'axe Y
  plus lisible.

  
LIMITATIONS

  Pas de zoom : La fonctionalité zoom a été expérimenté avec un succès limité, existe dans la version
  native de la librairie Qt et non pas en QML

  Pas de tests unitaires : Faute de temps, je n'ai pas gérer cette dépendance ni écrit des tests.
  En revanche, la partie lecture des données à partir d'un répertoire est robuste et strict.
