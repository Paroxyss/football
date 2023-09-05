import numpy as np
import matplotlib.pyplot as plt


def nonlinear_function(x, a, b, c):
    return a * np.exp(-b * x) + c


# mutation: 0.05, new blood: 0.05
n1 = {"uniform": [], "average": [], "onepointer": []}
# crossover: uniform, new blood: 0.05
n2 = {"0,05": [], "0,01": [], "0,005": [], "0,0001": [], "0,001": []}
# crossover: uniform, mutation: 0.0001
n3 = {"nb-0,05": [], "nb-0,1": [], "nb-0,01": [], "nb-0": []}
# crossover: uniform, mutation: 0.0001, new blood: 0
n4 = {"cb-0,9": [], "cb-0,95": [], "cb-0,8": [], "cb-0,6": [], "cb-1": []}

colors = ["blue", "red", "green", "pink", "yellow", "brown"]

# Read the file and process each line


def show(numbers, titre):
    for i, k in enumerate(numbers):
        with open("data/" + k + ".txt", "r") as file:
            for line in file:
                try:
                    num = float(line.strip())
                    numbers[k].append(num)
                except ValueError:
                    pass

            x_values = list(range(1, len(numbers[k]) + 1))
            y_values = numbers[k]

            plt.plot(
                x_values, y_values, color=colors[i], label=k, marker="", linestyle="-"
            )

    plt.xlabel("Generations")
    plt.ylabel("Nombre de touches")
    plt.grid(True)

    plt.title(titre)

    plt.legend()

    plt.show()


show(n1, "Crossover function")
show(n2, "Mutation rate")
show(n3, "New blood rate")
show(n4, "Crossover Probability")
