import pandas as pd
import numpy as np
import json

c_light = 299792458
k = 1.380649e-23
h = 6.62607015e-34
C_1 = 2 * h * c_light**2
C_2 = h * c_light / k

def planks_law(T, l):
    """
    Returns the spectrum of light of a black body at temperature T.

    :param T: scalar value of the temperature in Kelvin.
    :param l: ndarray of the wavelengths at which to compute the
    spectrum, in nanometers. l stands for lambda.
    :return: a ndarray of the energy at the specified wavelengths.
    """
    return C_1 / (l**5 * (np.exp(C_2 / (l * T)) - 1))

def XYZ_to_sRGB(XYZ):
    """
    Converts a color from the CIE 1964 XYZ color space to sRGB.
    """
    # XYZ.shape = (3, N)
    transform = np.array([
        [+3.2406, -1.5372, -0.4986],
        [-0.9689, +1.8758, +0.0415],
        [+0.0557, -0.2040, +1.0570]
    ])
    values = np.matmul(transform, XYZ)
    # Gamma correction
    mask = values > 0.0031308
    values[~mask] = 12.92 * values[~mask]
    values[mask] = 1.055 * values[mask]**(1/2.4) - 0.055
    return np.clip(values, 0, 1)

CIE_1964 = pd.read_csv("CIE_1964.csv")
CIE_1964["lambda"] *= 1e-9 # Nanometers
min_T = 100 # Kelvins
max_T = 20000
T_range = range(min_T, max_T)

def temperature_to_XYZ(T):
    """Computes the CIE 1964 XYZ values of the color of the black body
    emission at temperature T in Kelvin.
    :param T: The temperature, a scalar.
    :return: A 3-tuple with the XYZ components of the color.
    """
    radiance = planks_law(T, CIE_1964["lambda"].values)
    return (
        np.trapz(radiance * CIE_1964["xbar"].values, CIE_1964["lambda"].values),
        np.trapz(radiance * CIE_1964["ybar"].values, CIE_1964["lambda"].values),
        np.trapz(radiance * CIE_1964["zbar"].values, CIE_1964["lambda"].values)
    )


XYZ = np.zeros((3, max_T - min_T))
for i, T in enumerate(T_range):
    XYZ[:, i] = temperature_to_XYZ(T)
M = np.sum(XYZ, 0)
sRGB = XYZ_to_sRGB(XYZ / M)

# x and y are the chromaticity components.
# Not used here but they can be plotted over the Plank's locus to make
# sure we are qualitatively close to the expected theoretical colors
x, y = XYZ[0:2,:] / M
Y = np.full(XYZ.shape[1], 0.3)

j = {
    "R": sRGB[0,:].tolist(),
    "G": sRGB[1,:].tolist(),
    "B": sRGB[2,:].tolist(),
    "T": list(T_range),
    "M": M.tolist()
}

with open("../resources/black_body.json", "w") as f:
    json.dump(j, f)
