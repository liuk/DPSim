#!/usr/bin/env python

from xml.etree.ElementTree import Element, SubElement, Comment, tostring
from xml.dom import minidom

query_materials = 'SELECT material,density,nComponents,receipt FROM Materials'
query_box_solid = (
    'SELECT detectorGroupName,if(max(z0+deltaZ)-min(z0+deltaZ)<1.,1.,max(z0+deltaZ)-min(z0+deltaZ)), '
    '1.05*(max(x0+deltaX)-min(x0+deltaX)+planeWidth),1.05*(max(y0+deltaY)-min(y0+deltaY)+planeHeight) '
    'FROM Planes,Alignments WHERE Planes.detectorID=Alignments.detectorID GROUP BY detectorGroupName '
    'UNION '
    'SELECT volumeName,zLength,xLength,yLength FROM Nondetectors')
query_tube_solid = (
    'SELECT targetName,0.,radius,length FROM Targets '
    'UNION '
    'SELECT instruName,0.,radius,length FROM Instrumentation')
query_logic = (
    'SELECT detectorGroupName,material,1,"Blue" FROM Planes GROUP BY detectorGroupName '
    'UNION '
    'SELECT volumeName,material,0,"Red" FROM Nondetectors '
    'UNION '
    'SELECT targetName,material,0,"Black" FROM Targets '
    'UNION '
    'SELECT instruName,material,0,"Green" FROM Instrumentation')
query_phys = (
    'SELECT detectorGroupName,0.5*(max(z0+deltaZ)+min(z0+deltaZ)),0.5*(max(x0+deltaX)+min(x0+deltaX)),'
    '0.5*(max(y0+deltaY)+min(y0+deltaY)) FROM Planes,Alignments WHERE Planes.detectorID=Alignments.detectorID '
    'GROUP BY detectorGroupName '
    'UNION '
    'SELECT volumeName,zPos,xPos,yPos FROM Nondetectors '
    'UNION '
    'SELECT instruName,zPos,xPos,yPos FROM Instrumentation')
query_target = "SELECT targetName,nPieces,spacing,z0 FROM Targets WHERE targetName='%s'"

def prettify(elem):
    rough_string = tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent = '  ')

def addnode(mother, childname, keys, vals):
    node = SubElement(mother, childname)
    for i in range(len(keys)):
        node.set(keys[i], str(vals[i]))
    return node

def addproperty(mother, propertyName, value):
    node = SubElement(mother, propertyName)
    node.set('value', str(value))

def addreference(mother, referenceName, ref):
    node = SubElement(mother, referenceName + 'ref')
    node.set('ref', str(ref))

def parseMaterial(mother, info):
    node = addnode(mother, 'material', ['name'], [info[0]])
    addproperty(node, 'D', info[1])

    receipts = [item for item in info[3].replace('(', '').replace(')', '').split(',')]
    for item in receipts:
        vals = [i.strip() for i in item.split(':')]
        addnode(node, 'fraction', ['n', 'ref'], [vals[1], vals[0]])
    return node

def parseSolidBox(mother, info):
    node = addnode(mother, 'box', ['lunit', 'name', 'x', 'y', 'z'], ['cm', info[0]+'Solid', info[2], info[3], info[1]])
    return node

def parseSolidTube(mother, info):
    node = addnode(mother, 'tube', ['lunit', 'name', 'rmin', 'rmax', 'z', 'aunit', 'deltaphi'], ['cm', info[0]+'Solid', info[1], info[2], info[3], 'degree', 360.])
    return node

def parseLogical(mother, info):
    node = addnode(mother, 'volume', ['name'], [info[0]+'Logical'])
    addreference(node, 'material', info[1])
    addreference(node, 'solid', info[0]+'Solid')
    addnode(node, 'auxiliary', ['auxtype', 'auxvalue'], ['Color', info[3]])
    if info[2] != 0:
        addnode(node, 'auxiliary', ['auxtype', 'auxvalue'], ['SensDet', info[0]])
    return node

def parsePhysical(mother, info):
    node = addnode(mother, 'physvol', ['name'], [info[0]])
    addreference(node, 'volume', info[0]+'Logical')
    addnode(node, 'position', ['name', 'unit', 'x', 'y', 'z'], [info[0]+'Pos', 'cm', info[2], info[3], info[1]])
    return node
