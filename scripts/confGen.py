#!/usr/bin/env python

import os
import sys
from optparse import OptionParser

def runCmd(cmd):
    print cmd
    os.system(cmd)

## parse the command line controls
parser = OptionParser('Usage: %prog [options]')
parser.add_option('-t', '--template', type = 'string', dest = 'template', help = 'Input template for the conf file gen', default = 'example.conf')
parser.add_option('-o', '--output', type = 'string', dest = 'output', help = 'output file name', default = 'out_%s.conf')
parser.add_option('-n', '--nJobs', type = 'int', dest = 'nJobs', help = 'Number of jobs to make', default = 8)
parser.add_option('-s', '--seed', type = 'int', dest = 'seed', help = 'Seed offset of each job', default = 0)
parser.add_option('-r', '--run', action = 'store_true', dest = 'run', help = 'Automatically start running the conf', default = False)
(options, args) = parser.parse_args()

if not os.path.exists(options.template):
    print 'Template file not found!'
    sys.exit()

outputConfs = [options.output % i for i in range(options.nJobs)]
for i, output in enumerate(outputConfs):
    fout = open(output, 'w')
    fin = open(options.template, 'r')
    for line in fin.readlines():
        vals = line.strip().split()
        if len(vals) == 0:
            fout.write(line)
        elif vals[0] == 'seed':
            fout.write('seed %d\n' % (options.seed + i))
        elif vals[0] == 'outputFileName':
            fout.write('outputFileName %s\n' % output.replace('.conf', '.root'))
        else:
            fout.write(line)
    fout.close()
fin.close()

if not options.run:
    sys.exit()

for i, output in enumerate(outputConfs):
    runCmd('./DPSim %s > log_%d &' % (output, i))
