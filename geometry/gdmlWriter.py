#!/usr/bin/env python

import os
import sys
import MySQLdb
from optparse import OptionParser
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
from GDMLUtil import *

## parse the command line controls
parser = OptionParser('Usage: %prog [options]')
parser.add_option('-i', '--input', type = 'string', dest = 'input', help = 'Input geometry definition schema', default = 'geometry_ver1')
parser.add_option('-o', '--output', type = 'string', dest = 'output', help = 'Output GDML file name', default = 'test.gdml')
parser.add_option('-t', '--target', type = 'string', dest = 'target', help = 'Type of target in the beam', default = 'Tungsten')
parser.add_option('-s', '--server', type = 'string', dest = 'server', help = 'MySQL server', default = 'localhost')
parser.add_option('-p', '--port', type = 'int', dest = 'port', help = 'MySQL port', default = 3306)
(options, args) = parser.parse_args()

## connect the database
con = MySQLdb.connect(host = options.server, port = options.port, user = 'seaguest', passwd = 'qqbar2mu+mu-', db = options.input)
cur = con.cursor()

## initialize the GDML head node
root = Element('gdml')
root.set('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance')
root.set('xsi:noNamespaceSchemaLocation', 'http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd')

## a dummy define section
dummy = SubElement(root, "define")

## materials section, add a temporary D material
mat = SubElement(root, 'materials')
mat_D = addnode(mat, 'element', ['Z', 'name'], ['1', 'D'])
addproperty(mat_D, 'atom', 2.014)

cur.execute(query_materials)
for row in cur.fetchall():
    parseMaterial(mat, row)

## solids section, manually add world
sol = SubElement(root, 'solids')
addnode(sol, 'box', ['lunit', 'name', 'x', 'y', 'z'], ['cm', 'WorldSolid', 800., 600., 6000.])

cur.execute(query_box_solid)
for row in cur.fetchall():
    parseSolidBox(sol, row)

cur.execute(query_tube_solid)
for row in cur.fetchall():
    parseSolidTube(sol, row)

## volume section
struct = SubElement(root, 'structure')

cur.execute(query_logic)
for row in cur.fetchall():
    node_log = parseLogical(struct, row)

## physical volume part, manuall add world
world = addnode(struct, 'volume', ['name'], ['World'])
addreference(world, 'material', 'G4_AIR')
addreference(world, 'solid', 'WorldSolid')

cur.execute(query_phys)
for row in cur.fetchall():
    parsePhysical(world, row)

## special treatment of target, in particular because there might be multiple pieces
cur.execute(query_target % options.target)
targetInfo = cur.fetchone()
for i in range(targetInfo[1]):
    targetPiece = addnode(world, 'physvol', ['name'], [targetInfo[0]+'_'+str(i)])
    addreference(targetPiece, 'volume', targetInfo[0]+'Logical')
    addnode(targetPiece, 'position', ['name', 'unit', 'x', 'y', 'z'], ['%s_%dPos' % (targetInfo[0], i), 'cm', 0, 0, targetInfo[3] + (i-(targetInfo[1]-1)/2.)*targetInfo[2]])

## setup section -- target specific
setup = addnode(root, 'setup', ['name', 'version'], ['Default', '1.0'])
setup_world = addnode(setup, 'world', ['ref'], ['World'])

## final output, to both file and screen
fout = open(options.output, 'w')
fout.write(prettify(root))
fout.close()

print prettify(root)
