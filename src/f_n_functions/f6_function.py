from sympy import symbols, binomial, expand, simplify, collect, Poly, pprint, Rational

#Variable in form
x = symbols('x')

# Function to compute f_n(x)
def f_n(n):
    total = 0
    for i in range(n + 1):
        coeff = Rational(binomial(2 * i, i), 4**i)
        term = coeff * x * (1 - x**2)**i
        total += term
    return simplify(expand(total))

#Compute f_6(x)
f6 = f_n(6)

#Printing f6
print(f"\nExpanded f₆(x):")
pprint(f6)

#Extract and Print coefficients in fraction form

print(r"Fractional Coefficients of f₆(x):")

#Poly class will output (power, Coefficients)

f6_poly = Poly(f6, x)
for power, coeff in sorted(f6_poly.terms()):
    print(f"x^{power}: {coeff}")
