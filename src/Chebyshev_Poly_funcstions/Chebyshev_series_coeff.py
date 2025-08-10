import numpy as np
import matplotlib.pyplot as plt
from numpy.polynomial import Chebyshev, Polynomial

#Signum function
def signum(x):
    return np.sign(x)
#degrees=[5,9,11,13,15,17]
degrees = [5, 9]

#Scale to strict curve
#scale_factor=0.84 #(simpler)
scale_factor=0.85835555


x_plot = np.linspace(-1, 1, 100000) #100K points in interval [-1,1]
y_true = signum(x_plot)

#Chebyshev nodes for projection
n_samples = 100000
x_nodes = np.cos(np.pi * np.arange(n_samples) / (n_samples - 1))
y_nodes = signum(x_nodes)

# Plot setup
plt.figure(figsize=(14, 8))
plt.plot(x_plot, y_true, label='Original Sign Function', color='black', linewidth=2)
plt.axhline(y=1, color='gray', linestyle='--')
plt.axhline(y=-1, color='gray', linestyle='--')
plt.axvline(x=0, color='red', linestyle='--', linewidth=1)

#Loop over degrees
for deg in degrees:
    #Fit Chebyshev series
    cheb_fit = Chebyshev.fit(x_nodes, y_nodes, deg)

    #Odd symmetry
    coeffs = cheb_fit.coef.copy()
    for i in range(len(coeffs)):
        if i % 2 == 0:
            coeffs[i] = 0

    #Apply scaling
    scaled_cheb = Chebyshev(coeffs * scale_factor, domain=cheb_fit.domain)

    #Evaluate Plot
    y_scaled = scaled_cheb(x_plot)
    plt.plot(x_plot, y_scaled, label=f'Degree {deg}')

    #Converting standard polynomial
    poly_std = scaled_cheb.convert(kind=Polynomial)

    #Print standard polynomial coefficients (ascending order)
    print(f"Standard Polynomial Coefficients (ascending) for Degree {deg}:")
    for i, c in enumerate(poly_std.coef):
        print(f"  x^{i}: {c:.9f}")  #9 digit after decimal for much accuracy

#Final plot
plt.title(f'Scaled Chebyshev Series Approximations of sign(x)',fontsize=14)
plt.xlabel(r"$x \in [-1, 1] \subset \mathbb{R}$",fontsize=15)
plt.ylabel('f(x)',fontsize=15)
plt.grid(True)
plt.legend()
plt.show() 
