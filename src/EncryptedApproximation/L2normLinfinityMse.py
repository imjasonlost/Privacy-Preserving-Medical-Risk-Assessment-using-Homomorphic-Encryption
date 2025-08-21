
import pandas as pd
import numpy as np

#Load the CSV file generated from "polynomialapprox.cpp"
actual_df = pd.read_csv('signum_output.csv')           #Actual output
approx_df = pd.read_csv('approximated_output.csv')     #Approximated output



actual = actual_df[' output'].to_numpy()
approx = approx_df[' output'].to_numpy()

#Calculate difference
#Note-Make sure when each point is not approximated near -1, 1 then leave these points before 
#calculating error otherwise it does not output desired L2, Linfinit and MSE. 
error = actual - approx

#  L2 Norm (Euclidean norm)
l2_norm = np.linalg.norm(error)

# L-infinity norm (Max absolute error)
l_inf_norm = np.max(np.abs(error))

# 3. Mean Squared Error (MSE)
mse = np.mean(np.square(error))

# Print the results
print(f"L2 Norm (Euclidean): {l2_norm}")
print(f"L-infinity Norm (Max error): {l_inf_norm}")
print(f"Mean Squared Error (MSE): {mse}")
