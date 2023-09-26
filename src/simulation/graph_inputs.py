from readcsv import *
import matplotlib.pyplot as plt
import sys

# on récupère les données
filename = "game.csv"
data = read(filename)


# On récupère les infos
nb_joueurs = int(data[0][1] * 2)
data = data[1:]

# On parse le fichier
parsed = []

not_normalized = ["x", "y", "orientation"]

for i in range(len(data)):
    if sys.argv[-1] == "+":
        parsed.append(parse_normalized(data[i][3:], nb_joueurs))
    else:
        parsed.append(parse(data[i][3:], nb_joueurs))


# On affiche les courbes pour le joueurs 1
joueur = [x[0] for x in parsed]
n = len(joueur[0])


# On récupère la clé
keys = []

if len(sys.argv) >= 2:
    keys = sys.argv[1:]
else:
    print("Clés disponibles :")
    for k in joueur[0].keys():
        print(k)
    print("all")
    sys.exit(0)

for k in range(n):
    curr = list(joueur[0].keys())[k]

    if "all" in keys or curr in keys:
        if sys.argv[-1] != "+" or not curr in not_normalized:
            x = range(len(joueur))
            y = [j[curr] for j in joueur]
            plt.plot(x, y, label=curr)

# On affiche la courbe
plt.grid()
plt.legend()
plt.show()
