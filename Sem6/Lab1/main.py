# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np



data = np.loadtxt('Net.txt')



fig = plt.figure()
ax = fig.gca(projection='3d')

ysize = np.size(data) / np.size(data[0])
xsize = np.size(data[0])


# Make data.
X = np.arange(0, 1, 1 / xsize)
Y = np.arange(0, 1, 1 / ysize)
X, Y = np.meshgrid(X, Y)
R = np.sqrt(X**2 + Y**2)
Z = np.sin(R)

# Plot the surface.
surf = ax.plot_surface(X, Y, data, cmap=cm.coolwarm,
                       linewidth=0, antialiased=False, vmin=np.min(data), vmax = np.max(data), shade=3)

# Customize the z axis.
ax.set_zlim(np.min(data), np.max(data))
ax.zaxis.set_major_locator(LinearLocator(10))
ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()


# See PyCharm help at https://www.jetbrains.com/help/pycharm/
