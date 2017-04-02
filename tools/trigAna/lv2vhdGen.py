#!/usr/bin/env python

import os
import sys
from datetime import datetime
from optparse import OptionParser

def writeline(filestr, line, indent = 0):
    for i in range(indent):
        filestr.write(' ')
    filestr.write(line + '\n')

def ifstatementGen(name, indices):
    line = 'if '
    for idx in indices[:-1]:
        line = line + "(%s(%d) = '1') or " % (name, idx)
    line = line + "(%s(%d) = '1') then" % (name, indices[-1])
    return line

class Road:
    def __init__(self, line):
        vals = line.strip().split()

        self.n = len(vals) - 5  ## 5 additional numbers
        self.quadrant = int(vals[0])
        self.roadID = vals[1]
        self.detectorIDs = [int(val[:2]) for val in vals[2:2+self.n]]
        self.elementIDs  = [int(val[2:]) for val in vals[2:2+self.n]]
        self.zavg = float(vals[2 + self.n])
        self.weight = float(vals[3 + self.n])
        self.zrms = float(vals[4 + self.n])
        self.ibin = 0
        self.index = 0
        #self.cuts = [float(val) for vals[2+self.n:]]

    def __str__(self):
        line = '%d  %s ' % (self.quadrant, self.roadID)
        for i in range(self.n):
            line = line + '%02d%03d  ' % (self.detectorIDs[i], self.elementIDs[i])
        line = line + '%f  %f  %f' % (self.weight, self.zavg, self.zrms)
        return line

class Mapping:
    def __init__(self, line):
        vals = line.strip().split()

        self.detector = vals[0]
        self.element = int(vals[1])
        self.port = vals[2]
        self.channel = int(vals[3])

    def __str__(self):
        line = '%s   %02d   %s   %02d' % (self.detector, self.element, self.port, self.channel)
        return line

    def key(self):
        return '%s%02d' % (self.detector, self.element)

## parse the commandline inputs
parser = OptionParser('Usage: %prog [options]')
parser.add_option('-i', '--input', type = 'string', dest = 'input', help = 'Input road list ascii file')
parser.add_option('-o', '--output', type = 'string', dest = 'output', help = 'Output vhd file name')
parser.add_option('-e', '--entity', type = 'string', dest = 'entity', help = 'Entity name')
parser.add_option('-m', '--mapping', type = 'string', dest = 'mapping', help = 'Mapping file')
parser.add_option('-q', '--quadrant', type = 'int', dest = 'quadrant', help = 'Quadrant selection')
parser.add_option('-n', '--nbins', type = 'int', dest = 'nbins', help = 'Number of bins to divide z-vtx range', default = 8)
parser.add_option('-u', '--upstream', type = 'float', dest = 'zup', help = 'Z-vtx upstream', default = 300.)
parser.add_option('-d', '--downstream', type = 'float', dest = 'zdown', help = 'Z-vtx downstream', default = 600.)
(options, args) = parser.parse_args()

## load the mapping file
detectorID2Name = {49 : 'DP1TL', 50 : 'DP1TR', 51 : 'DP1BL', 52 : 'DP1BR', 53 : 'DP1TL', 54 : 'DP1TR', 55 : 'DP1BL', 56 : 'DP1BR', 35 : 'H4Y1L', 36 : 'H4Y1R'}
detectorName2ID = {v : k for k, v in detectorID2Name.iteritems()}
detectorNames = [['DP1TL', 'DP1TR', 'DP1BL', 'DP1BR'], ['DP2TL', 'DP2TR', 'DP2BL', 'DP2BR'], ['H4Y1L', 'H4Y1R', 'H4Y1L', 'H4Y1R']]
offset = [8, 8, 0, 0]

## load the mapping info
mappingInfo = {}
for line in open(options.mapping).readlines():
    m = Mapping(line)
    mappingInfo[m.key()] = m

## load roads with given quadrant
nClocks = 8
roads = []
roadCounts = [[0 for i in range(options.nbins)] for j in range(nClocks)]
zbinsize = (options.zdown - options.zup)/options.nbins
for line in open(options.input).readlines():
    r = Road(line)
    if r.quadrant == options.quadrant and r.zavg > options.zup and r.zavg < options.zdown:
        r.ibin = int((r.zavg - options.zup)/zbinsize)
        r.index = roadCounts[0][r.ibin]
        roads.append(r)
        roadCounts[0][r.ibin] += 1
roads.sort(key = lambda x: x.ibin*10000 + x.index)
roadCounts[0] = [n if n > 0 else 1 for n in roadCounts[0]]

for i in range(1, nClocks):
    for j in range(options.nbins):
        roadCounts[i][j] = roadCounts[i-1][j]/4 + 1

## start real vhd generation
fout = open(options.output, 'w')

## write all necessary headers
writeline(fout, 'library ieee;')
writeline(fout, 'use ieee.std_logic_1164.all;')
writeline(fout, '')
writeline(fout, 'entity %s is' % options.entity)
writeline(fout, '  port(')
writeline(fout, '    clk     : in std_logic;')
writeline(fout, '    a_din_l : in std_logic_vector(31 downto 0);')
writeline(fout, '    b_din_l : in std_logic_vector(31 downto 0);')
writeline(fout, '    d_din_l : in std_logic_vector(31 downto 0);')
writeline(fout, '    e_din_l : in std_logic_vector(31 downto 0);')
writeline(fout, '    f_din_l : in std_logic_vector(31 downto 0);')
writeline(fout, '    c_dout_l : out std_logic_vector(31 downto 0)')
writeline(fout, '  );')
writeline(fout, 'end %s;' % options.entity)
writeline(fout, '')

writeline(fout, 'architecture rtl of %s is' % options.entity)
writeline(fout, '')
writeline(fout, '-- input signals, regrouped and on clock', 2)
writeline(fout, 'signal Y1 : std_logic_vector(79 downto 0);', 2)
writeline(fout, 'signal Y2 : std_logic_vector(49 downto 0);', 2)
writeline(fout, 'signal Y4 : std_logic_vector(7  downto 0);', 2)
writeline(fout, '')

for i in range(1, nClocks):
    writeline(fout, '-- grouping of the outputs lv-%02d' % i, 2)
    for j in range(options.nbins):
        # write the current level
        writeline(fout, 'signal zgrp_lv%02d_bin%02d : std_logic_vector(%d downto 0);' % (i, j, roadCounts[i-1][j]-1), 2)
    writeline(fout, '')

writeline(fout, '-- final grouping', 2)
writeline(fout, 'signal zgrp_lv%02d : std_logic_vector(%d downto 0);' % (nClocks, options.nbins-1), 2)
writeline(fout, '')

## starts
writeline(fout, 'begin')
writeline(fout, 'c_dout_l(%d downto 0) <= zgrp_lv%02d;' % (options.nbins-1, nClocks), 2)
writeline(fout, '')

## signal assignment to the read detectors
# DP1
st1Name = detectorNames[0][options.quadrant]
writeline(fout, 'sigAssignmentY1 : process(clk)', 2)
writeline(fout, 'begin', 2)
writeline(fout, 'if rising_edge(clk) then', 4)
for i in range(0, 80):
    phyName = '%s%02d' % (st1Name, i+1)
    writeline(fout, "if %s(%d) = '1' then" % (mappingInfo[phyName].port, mappingInfo[phyName].channel), 6)
    writeline(fout, "Y1(%d) <= '1';" % i, 8)
    writeline(fout, "else", 6)
    writeline(fout, "Y1(%d) <= '0';" % i, 8)
    writeline(fout, "end if;", 6)
writeline(fout, 'end if;', 4)
writeline(fout, 'end process;', 2)
writeline(fout, '')

# DP2
st2Name = detectorNames[1][options.quadrant]
writeline(fout, 'sigAssignmentY2 : process(clk)', 2)
writeline(fout, 'begin', 2)
writeline(fout, 'if rising_edge(clk) then', 4)
for i in range(0, 50):
    phyName = '%s%02d' % (st2Name, i+1)
    #print i, phyName, mappingInfo[phyName]
    writeline(fout, "if %s(%d) = '1' then" % (mappingInfo[phyName].port, mappingInfo[phyName].channel), 6)
    writeline(fout, "Y2(%d) <= '1';" % i, 8)
    writeline(fout, "else", 6)
    writeline(fout, "Y2(%d) <= '0';" % i, 8)
    writeline(fout, "end if;", 6)
writeline(fout, 'end if;', 4)
writeline(fout, 'end process;', 2)
writeline(fout, '')

# H4Y1
st4lName = detectorNames[2][options.quadrant] + 'l'
st4rName = detectorNames[2][options.quadrant] + 'r'
writeline(fout, 'sigAssignmentY4 : process(clk)', 2)
writeline(fout, 'begin', 2)
writeline(fout, 'if rising_edge(clk) then', 4)
for i in range(8):
    phyNamel = '%s%02d' % (st4lName, i+offset[options.quadrant]+1)
    phyNamer = '%s%02d' % (st4rName, i+offset[options.quadrant]+1)
    #print i, phyName, mappingInfo[phyName]
    writeline(fout, "if (%s(%d) = '1') or (%s(%d) = '1') then" % (mappingInfo[phyNamel].port, mappingInfo[phyNamel].channel, mappingInfo[phyNamer].port, mappingInfo[phyNamer].channel), 6)
    writeline(fout, "Y4(%d) <= '1';" % i, 8)
    writeline(fout, "else", 6)
    writeline(fout, "Y4(%d) <= '0';" % i, 8)
    writeline(fout, "end if;", 6)
writeline(fout, 'end if;', 4)
writeline(fout, 'end process;', 2)
writeline(fout, '')

## Lv-1 is for the real road combination
writeline(fout, 'lut_lv01 : process(clk)', 2)
writeline(fout, 'begin', 2)
writeline(fout, 'if rising_edge(clk) then', 4)
for i, road in enumerate(roads):
    writeline(fout, "if (Y1(%d) = '1') and (Y2(%d) = '1') and (Y4(%d) = '1') then" % (road.elementIDs[0]-1, road.elementIDs[1]-1, road.elementIDs[2] - 1 - offset[options.quadrant]), 6)
    writeline(fout, "zgrp_lv01_bin%02d(%d) <= '1';" % (road.ibin, road.index), 8)
    writeline(fout, "else", 6)
    writeline(fout, "zgrp_lv01_bin%02d(%d) <= '0';" % (road.ibin, road.index), 8)
    writeline(fout, "end if;", 6)
writeline(fout, 'end if;', 4)
writeline(fout, 'end process;', 2)
writeline(fout, '')

## lv 2 - nClock-1 is for merging the road combinations
for i in range(2, nClocks):
    writeline(fout, 'lut_lv%02d : process(clk)' % i, 2)
    writeline(fout, 'begin', 2)
    writeline(fout, 'if rising_edge(clk) then', 4)
    for j in range(options.nbins):
        zgroups = [range(k*4, k*4+4) for k in range(roadCounts[i-1][j])]
        zgroups[-1] = [k for k in zgroups[-1] if k < roadCounts[i-2][j]]
        for k, zgroup in enumerate(zgroups):
            if len(zgroup) == 0:
                continue
            writeline(fout, ifstatementGen('zgrp_lv%02d_bin%02d' % (i-1, j), zgroup), 6)
            writeline(fout, "zgrp_lv%02d_bin%02d(%d) <= '1';" % (i, j, k), 8)
            writeline(fout, 'else', 6)
            writeline(fout, "zgrp_lv%02d_bin%02d(%d) <= '0';" % (i, j, k), 8)
            writeline(fout, 'end if;', 6)
    writeline(fout, 'end if;', 4)
    writeline(fout, 'end process;', 2)
    writeline(fout, '')

## lv-nclock is the final output
writeline(fout, 'lut_lv%02d : process(clk)' % nClocks, 2)
writeline(fout, 'begin', 2)
writeline(fout, 'if rising_edge(clk) then', 4)
for i in range(options.nbins):
    writeline(fout, "if zgrp_lv%02d_bin%02d(0) = '1' then" % (nClocks-1, i), 6)
    writeline(fout, "zgrp_lv08(%d) <= '1';" % i, 8)
    writeline(fout, "else", 6)
    writeline(fout, "zgrp_lv08(%d) <= '0';" % i, 8)
    writeline(fout, "end if;", 6)

writeline(fout, 'end if;', 4)
writeline(fout, 'end process;', 2)
writeline(fout, '')

## combinatorial logic
writeline(fout, 'end rtl;')
fout.close()
