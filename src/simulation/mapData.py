import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import sys
from readcsv import *

n = int(sys.argv[1])

tx, ty = [],[]

for i in range(n):
    data = read("map" + str(i+1) + ".csv");
    x, y = zip(*data)
    tx.append(x), ty.append(y)

fig, ax = plt.subplots()

line, = ax.plot(tx[0], ty[0], linestyle='None', marker=".");


def animate(i):
    line.set_xdata(tx[i%n])
    line.set_ydata(ty[i%n])  # update the data.
    return line,


ani = animation.FuncAnimation(
    fig, animate, interval=500, blit=True, save_count=n, repeat=False)

# To save the animation, use e.g.
#
#ani.save("map.mp4", writer = FFwriter)
#
# or
#
writer = animation.FFMpegWriter(
    fps=15, metadata=dict(artist='Me'), bitrate=6000)
ani.save("movie.mp4", writer=writer)

#plt.show()
