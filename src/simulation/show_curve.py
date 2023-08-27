import numpy as np
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt


def nonlinear_function(x, a, b, c):
    return a * np.exp(-b * x) + c


file_path = "data/c1.txt"
numbers = []

# Read the file and process each line
with open(file_path, "r") as file:
    for line in file:
        try:
            num = float(line.strip())
            numbers.append(num)
        except ValueError:
            pass

x_values = list(range(1, len(numbers) + 1))
y_values = numbers


plt.grid(True)
plt.legend()

x_values = np.array(x_values).reshape(-1, 1)
y_values = np.array(y_values).reshape(-1, 1)

model = LinearRegression()
model.fit(x_values, y_values)

x_fit = np.linspace(min(x_values), max(x_values), 100).reshape(-1, 1)
y_fit = model.predict(x_fit)

plt.plot(x_fit, y_fit, label="Linear Regression", marker="", color="orange")
plt.plot(x_values, y_values, color="b", label="Curve", marker="", linestyle="-")
plt.xlabel("Generations")
plt.ylabel("Average Goal Per Match")
plt.title("Progression Curve")


plt.show()
