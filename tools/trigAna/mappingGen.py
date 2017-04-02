import sys

ports = ['a_din_l' for i in range(32)]
ports = ports + ['b_din_l' for i in range(32)]
ports = ports + ['d_din_l' for i in range(32)]
ports = ports + ['e_din_l' for i in range(32)]
ports = ports + ['f_din_l' for i in range(32)]
channels = range(32) + range(32) + range(32) + range(32) + range(32)
detectorNames = ['DP1TL' for i in range(80)]
detectorNames = detectorNames + ['DP2TL' for i in range(50)]
detectorNames = detectorNames + ['H4Y1Ll' for i in range(8)]
detectorNames = detectorNames + ['H4Y1Lr' for i in range(8)]
elementIDs = range(1,81) + range(1,51) + range(9,17) + range(9,17)

fout = open(sys.argv[1], 'w')
for i in range(len(elementIDs)):
    fout.write('%s   %d   %s   %d\n' % (detectorNames[i], elementIDs[i], ports[i], channels[i]))
fout.close()
