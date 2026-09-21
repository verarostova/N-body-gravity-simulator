import csv
import matplotlib.pyplot as plt

time = []
x = []
y = []
z = []

file = open("trajectory.csv", "r")
reader = csv.DictReader(file)

for row in reader:
    time.append(float(row["time"]))
    x.append(float(row["x"]))
    y.append(float(row["y"]))
    z.append(float(row["z"]))

file.close()

print("Number of data points:", len(time))
print("First position:", x[0], y[0], z[0])
print("Last position:", x[-1], y[-1], z[-1])

plt.plot(x, y)
plt.xlabel("x (AU)")
plt.ylabel("y (AU)")
plt.title("Earth Orbit")
plt.axis("equal")
plt.grid()

plt.savefig("orbit.png")