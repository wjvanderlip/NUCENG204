'''
Code built with Margo and Darrell
'''

import matplotlib.pyplot as plt
import numpy as np

#Need to parse data
f=open('../lab0_spectral_data.txt')
lines=f.readlines()[1:]

Am241=[] # 59.54 keV
Ba133=[] # 80.997 keV (34%) 302.853 keV (18%) 356.017 keV (62%)
Cs137=[] # 661.66 keV
Co60=[] # 1173.2 keV and 1332.51 keV
Eu152=[] # 344.28 keV (27%) 1112.1 keV (14%) 1408.1 keV (21%) 121.78 keV (26%)

for x in lines:
    x.split('\t')
    y=[int(s) for s in x.split() if s.isdigit()]
    Am241.append(y[0])
    Ba133.append(y[1])
    Cs137.append(y[2])
    Co60.append(y[3])
    Eu152.append(y[4])
# testing that input vectors have the same length
f.close()

chan=list(range(1,len(Cs137)+1)) #number of channels:8192

centroid_Cs=np.argmax(Cs137) # 661.66 keV
centroid_Am=np.argmax(Am241) # 59.54 keV

x1=centroid_Am
x2=centroid_Cs

y1=59.5409
y2=661.657

energies=[]
m=float(y2-y1)/(x2-x1)
b=float(-m*x1+y1)
print(m,b)

chan_array=np.array(chan)

for i in chan:
    energies.append(np.multiply(m, chan_array[i-1])+b)

#Makes raw data plot
plt.semilogy(chan, Am241, label='Am-241')
plt.semilogy(chan, Ba133, label='Ba-133')
plt.semilogy(chan, Cs137, label='Cs-137')
plt.semilogy(chan, Co60, label='Co-60')
plt.semilogy(chan, Eu152, label='Eu-152')
plt.legend()
plt.title('Uncalibrated Raw Data')
plt.xlabel('Channel Number')
plt.ylabel('Counts')
plt.savefig('../images/Raw_Spec.png')
plt.show()

#Make calibrated data plot for Cs and Am
plt.semilogy(energies, Am241, label='Am-241')
plt.semilogy(energies, Cs137, label='Cs-137')
plt.legend()
plt.title('Calibrated Energy Plot of Am-241 and Cs-137')
plt.xlabel('Energy (KeV)')
plt.ylabel('Counts')
plt.savefig('../images/Cal_AmCs_Spec.png')
plt.show()

#Gamma Energies of Ba-133, ignored 79.6142 because too close to 80.9979 which is 10x more likely
from find_nearest import find_near

Ba_a=[80.9979,276.3989,302.8508, 356.0129,383.8485]
Ba_e=[]
dE=10

percent_diff=[]
table=[]

#this loop finds the max energy in the range around each Ba_a value (or the centroid)
for x in range(0,5):
    minE=Ba_a[int(x)]-dE
    maxE=Ba_a[int(x)]+dE

    nearestmin=find_near(energies,minE)
    nearestmax=find_near(energies,maxE)

    Erange=energies[nearestmin[0]:nearestmax[0]]
    Countrange=Ba133[nearestmin[0]:nearestmax[0]]
    #print(Erange, Countrange)
    maxcounts=max(Countrange)
    ECentroid=Erange[Countrange.index(maxcounts)]
    Ba_e.append(ECentroid)
    percent_diff.append((Ba_e[x]-Ba_a[x])/Ba_a[x])
    table.append([Ba_a[x], Ba_e[x], percent_diff[x]])


np.savetxt('../images/peakdiffquant.csv', table)
