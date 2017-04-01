#!/usr/bin/python

import os
import sys
import time
from array import array
from optparse import OptionParser
import ROOT
from math import sqrt
import copy

def get_eloss(mom, length):
	'''get muon energy loss per cm in iron'''

	length_fmag = 502.92
	eloss = 7.18274 + 0.0361447*mom - 0.000718127*mom*mom + 7.97312e-06*mom*mom*mom - 3.05481e-08*mom*mom*mom*mom

	return eloss/length_fmag*length

def fastswim(charge, mom_i, pos_i):
	'''Fast track swimming using simplified geometry, for acceptance only'''

	## define some geometry constants
	z_station = [666.626, 1420.99, 1958.45, 2242.57]
	x_min_station = [-150, -81.588, -100.44, -113.65, -152.13]
	x_max_station = [150, 80.419, 102.79, 113.86, 152.38]
	y_min_station = [-150, -69.966, -152.05, -167.97, -183.59]
	y_max_station = [150, 69.733, 151.94,  167.30, 182.16]
	z_fmag_end = 502.92
	z_kmag_center = 1064.26
	length_absorber = 99.568
	pt_kick_fmag = 2.909/z_fmag_end
	pt_kick_kmag = 0.4016
	step = 10.

	## list of results
	mom_all = []
	pos_all = []
	accepted = False

	## swim through reminder of fmag
	mom_curr = ROOT.TVector3(mom_i.Px(), mom_i.Py(), mom_i.Pz())
	pos_curr = ROOT.TVector3(pos_i.X(), pos_i.Y(), pos_i.Z())
	tx_curr = mom_curr.Px()/mom_curr.Pz()
	ty_curr = mom_curr.Py()/mom_curr.Pz()
	if debug: print '--- charge  tx   ty   x    y    z    px     py     pz'
	if debug: print '--- 0', charge, tx_curr, ty_curr, pos_curr.X(), pos_curr.Y(), pos_curr.Z(), mom_curr.X(), mom_curr.Y(), mom_curr.Z()
	if pos_i.Z() < z_fmag_end:

		#decide the number of steps
		nSteps = int((z_fmag_end - pos_curr.Z())/step) + 1
		step = (z_fmag_end - pos_curr.Z())/nSteps/2.

		#step by step swim, apply the pt kick at the center and energy loss in each half step
		ty_curr = mom_curr.Py()/mom_curr.Pz()
		for i in range(nSteps):
			
			# swim to the center of the step
			tx_curr = mom_curr.Px()/mom_curr.Pz()
			pos_curr = pos_curr + ROOT.TVector3(tx_curr*step, ty_curr*step, step)

			if debug: print '--- ---', i, charge, tx_curr, ty_curr, pos_curr.X(), pos_curr.Y(), pos_curr.Z(), mom_curr.X(), mom_curr.Y(), mom_curr.Z()

			# apply 1/2 energy loss and then full pt_kick
			p_tot_curr = mom_curr.Mag() - get_eloss(mom_curr.Mag(), ROOT.TVector3(tx_curr*step, ty_curr*step, step).Mag())
			tx_curr = tx_curr - 2.*charge*pt_kick_fmag*step/sqrt(mom_curr.Px()*mom_curr.Px() + mom_curr.Pz()*mom_curr.Pz())

			# swim to the end of the step and apply another 1/2 energy loss
			pos_curr = pos_curr + ROOT.TVector3(tx_curr*step, ty_curr*step, step)
			p_tot_curr = p_tot_curr - get_eloss(p_tot_curr, ROOT.TVector3(tx_curr*step, ty_curr*step, step).Mag())
			pz_curr = p_tot_curr/sqrt(1. + tx_curr*tx_curr + ty_curr*ty_curr)
			mom_curr.SetXYZ(pz_curr*tx_curr, pz_curr*ty_curr, pz_curr)

			if pos_curr.X() < x_min_station[0] or pos_curr.X() > x_max_station[0] or pos_curr.Y() < y_min_station[0] or pos_curr.Y() > y_max_station[0] or mom_curr.Mag() < 1E-3:
				if debug: print '--- --- Killed!'
				return (mom_all, pos_all, accepted)
			

	## get projections at station 1
	dist = z_station[0] - pos_curr.Z()
	pos_curr = pos_curr + ROOT.TVector3(mom_curr.Px()/mom_curr.Pz()*dist, mom_curr.Py()/mom_curr.Pz()*dist, dist)
	mom_all.append(copy.deepcopy(mom_curr))
	pos_all.append(copy.deepcopy(pos_curr))
	if debug: print '--- 1', charge, tx_curr, ty_curr, pos_curr.X(), pos_curr.Y(), pos_curr.Z(), mom_curr.X(), mom_curr.Y(), mom_curr.Z()

	if pos_curr.X() < x_min_station[1] or pos_curr.X() > x_max_station[1] or pos_curr.Y() < y_min_station[1] or pos_curr.Y() > y_max_station[1] or mom_curr.Mag() < 1E-3:
		if debug: print '--- 1 Killed!'
		return (mom_all, pos_all, accepted)

	## get projections at station 2
	# 1. swim to kmang center first
	dist = z_kmag_center - pos_curr.Z()
	pos_curr = pos_curr + ROOT.TVector3(mom_curr.Px()/mom_curr.Pz()*dist, mom_curr.Py()/mom_curr.Pz()*dist, dist)

	# 2. then apply pt kick
	tx_curr = mom_curr.Px()/mom_curr.Pz() - charge*pt_kick_kmag/sqrt(mom_curr.Px()*mom_curr.Px() + mom_curr.Pz()*mom_curr.Pz())
	ty_curr = mom_curr.Py()/mom_curr.Pz()
	pz_curr = mom_curr.Mag()/sqrt(1. + tx_curr*tx_curr + ty_curr*ty_curr)
	mom_curr.SetXYZ(pz_curr*tx_curr, pz_curr*ty_curr, pz_curr)

	# 3. then swim to station 2
	dist = z_station[1] - pos_curr.Z()
	pos_curr = pos_curr + ROOT.TVector3(mom_curr.Px()/mom_curr.Pz()*dist, mom_curr.Py()/mom_curr.Pz()*dist, dist)
	mom_all.append(copy.deepcopy(mom_curr))
	pos_all.append(copy.deepcopy(pos_curr))
	if debug: print '--- 2', charge, tx_curr, ty_curr, pos_curr.X(), pos_curr.Y(), pos_curr.Z(), mom_curr.X(), mom_curr.Y(), mom_curr.Z()

	if pos_curr.X() < x_min_station[2] or pos_curr.X() > x_max_station[2] or pos_curr.Y() < y_min_station[2] or pos_curr.Y() > y_max_station[2] or mom_curr.Mag() < 1E-3:
		if debug: print '--- 2 Killed!'
		return (mom_all, pos_all, accepted)

	## get the projections at station 3
	dist = z_station[2] - pos_curr.Z()
	pos_curr = pos_curr + ROOT.TVector3(mom_curr.Px()/mom_curr.Pz()*dist, mom_curr.Py()/mom_curr.Pz()*dist, dist)
	mom_all.append(copy.deepcopy(mom_curr))
	pos_all.append(copy.deepcopy(pos_curr))
	if debug: print '--- 3', charge, tx_curr, ty_curr, pos_curr.X(), pos_curr.Y(), pos_curr.Z(), mom_curr.X(), mom_curr.Y(), mom_curr.Z()

	if pos_curr.X() < x_min_station[3] or pos_curr.X() > x_max_station[3] or pos_curr.Y() < y_min_station[3] or pos_curr.Y() > y_max_station[3] or mom_curr.Mag() < 1E-3:
		if debug: print '--- 3 Killed!'
		return (mom_all, pos_all, accepted)

	## get the projections at station 4
	# 1. apply the energy loss in absorber
	tx_curr = mom_curr.Px()/mom_curr.Pz()
	ty_curr = mom_curr.Py()/mom_curr.Pz()
	pz_curr = (mom_curr.Mag() - get_eloss(mom_curr.Mag(), length_absorber*sqrt(1. + tx_curr*tx_curr + ty_curr*ty_curr)))/sqrt(1. + tx_curr*tx_curr + ty_curr*ty_curr)
	mom_curr.SetXYZ(pz_curr*tx_curr, pz_curr*ty_curr, pz_curr)

	# 2. swim to station 4
	dist = z_station[3] - pos_curr.Z()
	pos_curr = pos_curr + ROOT.TVector3(mom_curr.Px()/mom_curr.Pz()*dist, mom_curr.Py()/mom_curr.Pz()*dist, dist)
	mom_all.append(copy.deepcopy(mom_curr))
	pos_all.append(copy.deepcopy(pos_curr))
	if debug: print '--- 4', charge, tx_curr, ty_curr, pos_curr.X(), pos_curr.Y(), pos_curr.Z(), mom_curr.X(), mom_curr.Y(), mom_curr.Z()

	if pos_curr.X() < x_min_station[4] or pos_curr.X() > x_max_station[4] or pos_curr.Y() < y_min_station[4] or pos_curr.Y() > y_max_station[4] or mom_curr.Mag() < 1E-3:
		if debug: print '--- 4 Killed!'
		return (mom_all, pos_all, accepted)

	accepted = True
	for i in range(4):
		accepted = accepted and pos_all[i].X() > x_min_station[i+1] and pos_all[i].X() < x_max_station[i+1]
		accepted = accepted and pos_all[i].Y() > y_min_station[i+1] and pos_all[i].Y() < y_max_station[i+1]
		accepted = accepted and mom_all[i].Pz() > 0.

	if debug: print '--- 5', accepted
	return (mom_all, pos_all, accepted)

def digitize(pos_all, detectorID, elementID):
	'''Digitization of the track hit positions into detectorID/elementID as defined in E906 convention.'''

	# define the hodoscoep specs
	detectorID_Top = [26, 32, 34, 40]
	detectorID_Bot = [25, 31, 33, 39]
	width = [7.0025, 12.6825, 14.27, 19.33]
	nElements = [23, 16, 16, 16]
	overlap = 0.3175

	# get the elementID hit on each pos
	if debug: print '--- 6 Digitize to: ',
	for i in range(4):
		#print i, pos_all[i].X(), pos_all[i].Y(), pos_all[i].Z()
		if pos_all[i].Y() > 0:
			detectorID[i] = detectorID_Top[i]
		else:
			detectorID[i] = detectorID_Bot[i]

		elementID[i] = int((pos_all[i].X() + 0.5*nElements[i]*(width[i] - overlap))/(width[i] - overlap)) + 1
		if debug: print elementID[i], ', ',
	if debug: print ' '


def calcTriggerID(detectorID, elementID):
	'''Calculate the trigger road ID as defined in standard E906 way'''
	roadID = (elementID[0]-1)*16*16*16 + (elementID[1]-1)*16*16 + (elementID[2]-1)*16 + elementID[3]

	if detectorID[0] % 2 == 0:
		return roadID
	else:
		return -1*roadID

def loadTriggerSet():
	'''load the trigger matrix setup'''

	roadFiles = ['roads_plus_top.txt', 'roads_plus_bottom.txt', 'roads_minus_top.txt', 'roads_minus_bottom.txt']
	for i in range(4):
		for line in open(roadFiles[i], 'r').readlines():
			if i < 2:
				plusRoads.append(int(line.strip().split()[0]))
			else:
				minusRoads.append(int(line.strip().split()[0]))


def testTrigger(roadID1, roadID2):
	'''see if the trigger matrix is satisfied'''
	return roadID1 in plusRoads and roadID2 in minusRoads and roadID1*roadID2 < 0

## input control parameters
print 'Dark photon -> dimuon phase space generator'
parser = OptionParser('%prog [options]')
parser.add_option('--pt', type = 'string', dest = 'pt', help = 'dark photon pT range in GeV/c [default: %default]', default = '0-5')
parser.add_option('--pz', type = 'string', dest = 'pz', help = 'dark photon pz range in GeV/c [default: %default]', default = '0-20')
parser.add_option('--mass', type = 'string', dest = 'mass', help = 'dark photon mass in GeV/c2 [default: %default]', default = '0.22-1')
parser.add_option('--zvtx', type = 'string', dest = 'zvtx', help = 'dark photon z_vertex range in cm [default: %default]', default = '300-600')
parser.add_option('--nevt', type = 'int', dest = 'nEvents', help = 'number of events [default: %default]', default = 100)
parser.add_option('--output', type = 'string', dest = 'output', help = 'output file name [default: %default]', default = 'output.root')
parser.add_option('--seed', type = 'int', dest = 'seed', help = 'random seed [default: %default]', default = 0)
parser.add_option('--log', type = 'string', dest = 'logfile', help = 'Redirect all screen output to log, to the screen if not specified', default = '')

parser.add_option('--host', type = 'string', dest = 'host', help = 'MySQL server address [default: %default]', default = 'e906-db1.fnal.gov')
parser.add_option('--port', type = 'int', dest = 'port', help = 'MySQL server port [default: %default]', default = 3306)
parser.add_option('--schema', type = 'string', dest = 'schema', help = 'MySQL output schema [default: %default]', default = '')
parser.add_option('--user', type = 'string', dest = 'user', help = 'MySQL user name [default: %default]', default = 'seaguest')
parser.add_option('--passwd', type = 'string', dest = 'passwd', help = 'MySQL user password [default: %default]', default = '')

parser.add_option('--acc', action = 'store_true', dest = 'accepted', default = False, help = 'If specified, will only save accepted events, turned off by default')
parser.add_option('--trig', action = 'store_true', dest = 'trig', default = False, help = 'If specified, will load trigger matrix and pass event through')
parser.add_option('-v', action = 'store_true', dest = 'debug', default = False, help = 'If specified, will output debugging information')

(options, args) = parser.parse_args()

(pt_min, pt_max) = [float(val) for val in options.pt.split('-')]
(pz_min, pz_max) = [float(val) for val in options.pz.split('-')]
(mass_min, mass_max) = [float(val) for val in options.mass.split('-')]
(zvtx_min, zvtx_max) = [float(val) for val in options.zvtx.split('-')]
nEvents = options.nEvents

debug = options.debug
accOnly = options.accepted

if options.logfile != '':
	sys.stdout = open(options.logfile, 'w')

print ''
print '-------------- Dark photon kinematics --------------'
print 'Dark photon pT: %f - %f GeV' % (pt_min, pt_max)
print 'Dark photon pz: %f - %f GeV' % (pz_min, pz_max)
print 'Dark photon mass: %f - %f GeV' % (mass_min, mass_max)
print 'Dark photon vertex: %f - %f cm' % (zvtx_min, zvtx_max)
print ''
print '-------------------    I/O   -----------------------'
print 'Random seed: %d' % options.seed 
print 'Number of events: %d' % options.nEvents
print 'ROOT output file: ' + options.output
if options.schema != '':
	print 'MySQL output server: ' + options.host
	print 'MySQL output schema: ' + options.schema

## initlize the trigger road bank
if options.trig:
	plusRoads = []
	minusRoads = []
	loadTriggerSet()

## book ROOT output structure
eventID = array('i', [0])
weight = array('d', [0.])
px0 = array('d', [0.])    # initiali 4-momentum of dark photon
py0 = array('d', [0.])
pz0 = array('d', [0.])
E0 = array('d', [0.])
mass = array('d', [0.])   # mass of dark photon
z0 = array('d', [0.])     # z vertex of dark photon
px1 = array('d', [0.])    # initial momentum of mu+
py1 = array('d', [0.])
pz1 = array('d', [0.])
px2 = array('d', [0.])    # initial momentum of mu-
py2 = array('d', [0.])
pz2 = array('d', [0.])
pxp = array('d', [0.]*5)  # 3-momentum of mu+, index = 0, 1, 2, 3, 4 indicate vertex, station-1/2/3/4 hodo
pyp = array('d', [0.]*5)
pzp = array('d', [0.]*5)
xp = array('d', [0.]*5)   # position of mu+, index = 0, 1, 2, 3, 4 indicate vertex, station-1/2/3/4 hodo
yp = array('d', [0.]*5)
zp = array('d', [0.]*5)
pxm = array('d', [0.]*5)  # 3-momentum of mu-, index = 0, 1, 2, 3, 4 indicate vertex, station-1/2/3/4 hodo
pym = array('d', [0.]*5)
pzm = array('d', [0.]*5)
xm = array('d', [0.]*5)   # position of mu-, index = 0, 1, 2, 3, 4 indicate vertex, station-1/2/3/4 hodo
ym = array('d', [0.]*5)
zm = array('d', [0.]*5)
detectorID1 = array('i', [0]*4)    # fired hodoscope detector ID on each station, index = 0, 1, 2, 3 indicate station-1/2/3/4 hodo
detectorID2 = array('i', [0]*4)
elementID1 = array('i', [0]*4)     # fired hodoscope element ID on each station, index = 0, 1, 2, 3 indicate station-1/2/3/4 hodo
elementID2 = array('i', [0]*4)
triggerID1 = array('i', [0])       # trigger road ID as defined in E906
triggerID2 = array('i', [0])
fired = array('i', [-1])  # flag of dimuon trigger
accp = array('i', [-1])   # flag of mu+/- being accepted or not
accm = array('i', [-1])

saveFile = ROOT.TFile(options.output, 'recreate')
saveTree = ROOT.TTree('save', 'save')

saveTree.Branch('eventID', eventID, 'eventID/I')
saveTree.Branch('weight', weight, 'weight/D')
saveTree.Branch('px0', px0, 'px0/D')
saveTree.Branch('py0', py0, 'py0/D')
saveTree.Branch('pz0', pz0, 'pz0/D')
saveTree.Branch('E0', E0, 'E0/D')
saveTree.Branch('px1', px1, 'px1/D')
saveTree.Branch('py1', py1, 'py1/D')
saveTree.Branch('pz1', pz1, 'pz1/D')
saveTree.Branch('px2', px2, 'px2/D')
saveTree.Branch('py2', py2, 'py2/D')
saveTree.Branch('pz2', pz2, 'pz2/D')
saveTree.Branch('z0', z0, 'z0/D')
saveTree.Branch('mass', mass, 'mass/D')
saveTree.Branch('pxp', pxp, 'pxp[5]/D')
saveTree.Branch('pyp', pyp, 'pyp[5]/D')
saveTree.Branch('pzp', pzp, 'pzp[5]/D')
saveTree.Branch('xp', xp, 'xp[5]/D')
saveTree.Branch('yp', yp, 'yp[5]/D')
saveTree.Branch('zp', zp, 'zp[5]/D')
saveTree.Branch('pxm', pxm, 'pxm[5]/D')
saveTree.Branch('pym', pym, 'pym[5]/D')
saveTree.Branch('pzm', pzm, 'pzm[5]/D')
saveTree.Branch('xm', xm, 'xm[5]/D')
saveTree.Branch('ym', ym, 'ym[5]/D')
saveTree.Branch('zm', zm, 'zm[5]/D')
saveTree.Branch('detectorID1', detectorID1, 'detectorID1[4]/I')
saveTree.Branch('detectorID2', detectorID2, 'detectorID2[4]/I')
saveTree.Branch('elementID1', elementID1, 'elementID1[4]/I')
saveTree.Branch('elementID2', elementID2, 'elementID2[4]/I')
saveTree.Branch('triggerID1', triggerID1, 'triggerID1/I')
saveTree.Branch('triggerID2', triggerID2, 'triggerID2/I')
saveTree.Branch('fired', fired, 'fired/I')
saveTree.Branch('accp', accp, 'accp/I')
saveTree.Branch('accm', accm, 'accm/I')

## book MySQL output structure
writeMySQL = False
if options.schema != '':
	writeMySQL = True

	import MySQLdb
	server = MySQLdb.connect(host = options.host, port = options.port, user = options.user, passwd = options.passwd)
	cur = server.cursor()

	cur.execute('DROP DATABASE IF EXISTS ' + options.schema)
	cur.execute('CREATE DATABASE ' + options.schema)
	server.select_db(options.schema)

	createQuery = '''CREATE TABLE eTrack(
		id           int           NOT NULL,
		eventID      int           NOT NULL,
		particleID   int           NOT NULL,
		px           double        NOT NULL,
		py           double        NOT NULL,
		pz           double        NOT NULL,
		x            double        NOT NULL,
		y            double        NOT NULL,
		z            double        NOT NULL)'''
	cur.execute(createQuery)

	insertQuery = 'INSERT INTO eTrack(id,eventID,particleID,px,py,pz,x,y,z) VALUES(%d,%d,%d,%f,%f,%f,%f,%f,%f)'


## initialize the random generator
rndm = ROOT.TRandom()
rndm.SetSeed(options.seed)

## initialize the phase space generator
gen = ROOT.TGenPhaseSpace()
masses = array('d', [0.1056583715, 0.1056583715])

## generate N events
for i in range(nEvents):

	eventID[0] = i

	# initialize the kinematics
	px0[0] = rndm.Uniform(pt_min, pt_max)
	py0[0] = rndm.Uniform(pt_min, pt_max)
	pz0[0] = rndm.Uniform(pz_min, pz_max)
	mass[0] = rndm.Uniform(mass_min, mass_max)
	if rndm.Rndm() < 0.5:
		px0[0] = -px0[0]
	if rndm.Rndm() < 0.5:
		py0[0] = -py0[0]
	z0[0] = rndm.Uniform(zvtx_min, zvtx_max)

	p_A = ROOT.TLorentzVector()
	p_A.SetXYZM(px0[0], py0[0], pz0[0], mass[0])
	E0[0] = p_A.E()
	gen.SetDecay(p_A, 2, masses)
	if debug: print '------------- eventID = ', i, p_A.Px(), p_A.Py(), p_A.Pz(), p_A.E(), mass[0], z0[0]

	# get the pos/mom at vertex
	weight[0] = gen.Generate()
	pMup = gen.GetDecay(0)
	pMum = gen.GetDecay(1)

	pxp[0] = pMup.Px()
	px1[0] = pxp[0]
	pyp[0] = pMup.Py()
	py1[0] = pyp[0]
	pzp[0] = pMup.Pz()
	pz1[0] = pzp[0]
	pxm[0] = pMum.Px()
	px2[0] = pxm[0]
	pym[0] = pMum.Py()
	py2[0] = pym[0]
	pzm[0] = pMum.Pz()
	pz2[0] = pzm[0]
	xp[0] = 0.
	yp[0] = 0.
	zp[0] = z0[0]
	xm[0] = 0.
	ym[0] = 0.
	zm[0] = z0[0]	

	# fill the MySQL schema
	if writeMySQL:
		cur.execute(insertQuery % (2*i,   eventID[0], -13, pxp[0], pyp[0], pzp[0], xp[0], yp[0], zp[0]))
		cur.execute(insertQuery % (2*i+1, eventID[0],  13, pxm[0], pym[0], pzm[0], xm[0], ym[0], zm[0]))
		server.commit()

	# get the pos/mom at detectors and digitize
	momp_all, posp_all, accp[0] = fastswim( 1, ROOT.TVector3(pxp[0], pyp[0], pzp[0]), ROOT.TVector3(xp[0], yp[0], zp[0]))
	if accp[0] == 1: 
		digitize(posp_all, detectorID1, elementID1)
		triggerID1[0] = calcTriggerID(detectorID1, elementID1)
	else:
		triggerID1[0] = -1
		for j in range(4):
			detectorID1[j] = -1
			elementID1[j] = -1

	momm_all, posm_all, accm[0] = fastswim(-1, ROOT.TVector3(pxm[0], pym[0], pzm[0]), ROOT.TVector3(xm[0], ym[0], zm[0]))
	if accm[0] == 1: 
		digitize(posm_all, detectorID2, elementID2)
		triggerID2[0] = calcTriggerID(detectorID2, elementID2)
	else:
		triggerID2[0] = -1
		for j in range(4):
			detectorID2[j] = -1
			elementID2[j] = -1

	# fill the final results
	for j in range(len(momp_all)):
		pxp[j+1] = momp_all[j].Px()
		pyp[j+1] = momp_all[j].Py()
		pzp[j+1] = momp_all[j].Pz()
		xp[j+1] = posp_all[j].X()
		yp[j+1] = posp_all[j].Y()
		zp[j+1] = posp_all[j].Z()

	for j in range(len(momm_all)):
		pxm[j+1] = momm_all[j].Px()
		pym[j+1] = momm_all[j].Py()
		pzm[j+1] = momm_all[j].Pz()
		xm[j+1] = posm_all[j].X()
		ym[j+1] = posm_all[j].Y()
		zm[j+1] = posm_all[j].Z()

	# now fill the rest if the track is killed halfway
	for j in range(len(momp_all)+1, 5):
		pxp[j] = -9999.;
		pyp[j] = -9999.;
		pzp[j] = -9999.;
		xp[j] = -9999.;
		yp[j] = -9999.;
		zp[j] = -9999.;

	for j in range(len(momm_all)+1, 5):
		pxm[j] = -9999.;
		pym[j] = -9999.;
		pzm[j] = -9999.;
		xm[j] = -9999.;
		ym[j] = -9999.;
		zm[j] = -9999.;

	if options.trig: fired[0] = testTrigger(triggerID1[0], triggerID2[0])
	if accOnly:
		if accp[0] == 1 and accm[0] == 1:
			saveTree.Fill()
	else:
		saveTree.Fill()

## finalize
saveTree.Write()
saveFile.Close()

if options.logfile != '':
	sys.stdout.close()