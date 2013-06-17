#!/usr/bin/env python3

from pylab import *


# import big data table
# consists of 320 consecutive 48x15 matrices
# columns = p_T, rows = phi
# each of the 320 matrices corresponds to a particle type
# the 320 particles and their order can be found in tables/pdg-App.dat
dN = vsplit(loadtxt('../results/dN_pTdpTdphidy.dat'), 320)

# define sampled particle info
# n:  number in the big dN array
# ID:  Monte Carlo ID
# label:  for display purposes in the plot
# style:  line/dot style in the plot
particles = (
        {'n': 1, 'ID': '211', 'label': r'$\pi^+$', 'style': 'b-o'},
        {'n': 4, 'ID': '321', 'label': r'$K^+$', 'style': 'g-s'},
        {'n': 17, 'ID': '2212', 'label': r'$p$', 'style': 'r-^'}
        )

# import gaussian weights for the phi columns
phi_weights = loadtxt('../tables/phi_gauss_table.dat', usecols=(1,))

# import the p_T bins
pT_values = loadtxt('../tables/pT_gauss_table.dat', usecols=(0,))

# enable latex fonts
rc('text', usetex=True)
rc('font', family='serif')

# make a plot for each particle
for p in particles:
    semilogy(pT_values, dot(dN[p['n']].T,phi_weights)/1000, p['style'], label=p['label'])

# axes labels, legend
xlabel(r'$p_T$')
ylabel(r'$dN/p_T\,dp_T\,dy$')
legend()

# save to pdf
savefig('dN_pTdpTdy.pdf')
