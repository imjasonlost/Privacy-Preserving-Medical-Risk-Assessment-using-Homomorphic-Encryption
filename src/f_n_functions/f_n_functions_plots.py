import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


#Fn(x) functions, fours functions highest degree first
Coefficient_f1=[-1/2,0,3/2,0]
Coefficient_f2=[3/8,0, -10/8,0,15/8, 0]
Coefficient_f3=[-5/16, 0, 21/16,0,-35/16,0,35/16,0]
Coefficient_f4=[35/128,0,-180/128,0,378/128,0,-420/128,0,315/128,0]
Coefficient_f6=[231/1024,0,-1638/1024,0,5005/1024,0,-8580/1024,0,9009/1024,0,-6006/1024,0,3003/1024,0]

def polynomial(coefficient):
    return np.poly1d(coefficient)
       
f1=polynomial(Coefficient_f1)
f2=polynomial(Coefficient_f2)
f3=polynomial(Coefficient_f3)
f4=polynomial(Coefficient_f4)
f6=polynomial(Coefficient_f6)

x_values=np.linspace(-1, 1, 1000)

# Add labels, title, and grid
def add_lable_title_grid():   

    plt.xlabel(r"$x \in [-1, 1] \subset \mathbb{R}$")
    plt.ylabel(r"$f_n(x)$")
    plt.axhline(0, color='black', linewidth=0.5)
    plt.axvline(0, color='black', linewidth=0.5)
    plt.axhline(1, color='black', linewidth=0.5)
    plt.axvline(1, color='black', linewidth=0.5)
    plt.axvline(-1, color='black', linewidth=0.5)
    plt.axhline(-1, color='black', linewidth=0.5)
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.6)
    
def plotfunction(y,color,label,line):
    #plt.scatter(x_values, y, color='red', label='Polynomial Points')
    # Connect points with a line (optional)
    plt.plot(x_values, y,label=label, linestyle=line, color=color, alpha=0.5)

# Define signum function (Predicted Curve) using smooth approximation (tanh)
y_signum = np.sign(x_values)  # Tanh function as a smooth approximation
plotfunction(y_signum,"black","Signum Function","solid")
add_lable_title_grid()

y_values = f1(x_values)
plotfunction(y_values,"pink",r"$f_1(x)$","-")
add_lable_title_grid()

#Plot f2   
y_values = f2(x_values)

plotfunction(y_values,"blue",r"$f_2(x)$","--")
add_lable_title_grid()

#Plot f3   
y_values = f3(x_values)
plotfunction(y_values,"green",r"$f_3(x)$","-.")
add_lable_title_grid()

#Plot f4   
y_values = f4(x_values)
plotfunction(y_values,"purple",r"$f_4(x)$","-")
add_lable_title_grid()

#Plot f6   
y_values = f6(x_values)
plotfunction(y_values,"red",r"$f_6(x)$","--")
add_lable_title_grid()

#Show plots
plt.show()
