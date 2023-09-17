import matplotlib.pyplot as plt
import matplotlib.animation as animation

fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, layout="constrained")


def read_and_parse():
    graph_data = open("stats.csv", "r").read()
    data = graph_data.split("\n")[:-1]
    data = [line.split(",") for line in data]
    n = len(data)

    for i in range(n):
        data[i] = [float(x) for x in data[i]]

    return data


def animate(i):
    graph_data = read_and_parse()

    xs = [x[0] for x in graph_data]

    ys_1 = [x[1] for x in graph_data]
    ax1.clear()
    ax1.plot(xs, ys_1)
    ax1.set_title("Nombre moyen de collisions")
    ax1.set_xlabel("Générations")
    ax1.set_ylabel("Collisions / Nombre de matches")
    ax1.grid(axis="y")

    ys_2 = [x[2] for x in graph_data]
    ax2.clear()
    ax2.plot(xs, ys_2)
    ax2.set_title("Nombre moyen de touches")
    ax2.set_xlabel("Générations")
    ax2.set_ylabel("Touches / Nombre de matches")
    ax2.grid(axis="y")

    ys_3 = [x[3] for x in graph_data]
    ax3.clear()
    ax3.plot(xs, ys_3)
    ax3.set_title("Nombre moyen de buts")
    ax3.set_xlabel("Générations")
    ax3.set_ylabel("Butes / Nombre de matches")
    ax3.grid(axis="y")

    ys_4 = [x[4] for x in graph_data]
    ax4.clear()
    ax4.plot(xs, ys_4)
    ax4.set_title("Nombre de matches")
    ax4.set_xlabel("Générations")
    ax4.set_ylabel("Matches")
    ax4.grid(axis="y")


ani = animation.FuncAnimation(fig, animate, interval=1000)
plt.tight_layout()
plt.grid()
plt.show()
