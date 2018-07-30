#!/usr/bin/env python

import os
import sys
from datetime import datetime
from optparse import OptionParser
from DPSimJobConf import DPSimJobConf
import GridUtil

## simple function to run a command
def runCmd(cmd):
    print cmd
    os.system(cmd)

## parse the command line controls
parser = OptionParser('Usage: %prog [options]')
parser.add_option('-p', '--prog', type = 'string', dest = 'prog', help = 'program to run', default = 'DPSim')
parser.add_option('-t', '--template', type = 'string', dest = 'template', help = 'Input template for the conf file gen', default = 'example.conf')
parser.add_option('-i', '--input', type = 'string', dest = 'input', help = 'input files', default = '')
parser.add_option('-o', '--output', type = 'string', dest = 'output', help = 'output file prefix', default = 'out_%s')
parser.add_option('-w', '--workdir', type = 'string', dest = 'workdir', help = 'Working dir of all temporary/permanent files', default = '')
parser.add_option('-n', '--nJobs', type = 'int', dest = 'nJobs', help = 'Number of jobs to make', default = 8)
parser.add_option('-s', '--seed', type = 'int', dest = 'seed', help = 'Seed offset of each job', default = 0)
parser.add_option('-a', '--addition', type = 'string', dest = 'addition', help = 'additional scan instruction', default = '')
parser.add_option('-l', '--local', action = 'store_true', dest = 'local', help = 'Start running the jobs locally', default = False)
parser.add_option('-g', '--grid', action = 'store_true', dest = 'grid', help = 'Submit the jobs to grid', default = False)
(options, args) = parser.parse_args()

if len(sys.argv) < 2:
    parser.parse_args(['--help'])

if not os.path.exists(options.template):
    print 'Template file not found!'
    sys.exit()

if options.local and options.grid:
    print 'Cannot be in both local and grid mode!'
    sys.exit()

if options.grid:
    GridUtil.gridInit()

if options.workdir == '':
    options.workdir = os.getcwd()

## parse the addition instructions
reservedKeys = []
reservedValFormula = []
if options.addition != '':
    items = [item.strip() for item in options.addition.split(',')]
    for item in items:
        reservedKeys.append(item.split(':')[0].strip())
        reservedValFormula.append(item.split(':')[1].strip())

## initialize the conf file generator, and the executable DPSim
tconf = DPSimJobConf(options.template, reservedKeys)
prog = os.path.join(os.getenv('DPSIM_ROOT'), options.prog)

## set up the working directories
workdir = os.path.abspath(options.workdir)
outputdir = os.path.join(workdir, 'output')
confdir = os.path.join(workdir, 'conf')
logdir = os.path.join(workdir, 'log')
wrapperdir = os.path.join(workdir, 'wrapper') if options.grid else workdir

for d in (wrapperdir, outputdir, confdir, logdir):
    if not os.path.exists(d):
        os.makedirs(d)

## prepare the file names
confs = [os.path.join(confdir, '%s.conf' % (options.output % str(i))) for i in range(options.nJobs)]
logs = [os.path.join(logdir, '%s.log' % (options.output % str(i))) for i in range(options.nJobs)]
outputs = [os.path.join(outputdir, '%s.root' % (options.output % str(i))) for i in range(options.nJobs)]
goutputs = ['$CONDOR_DIR_OUTPUT/%s.root' % (options.output % str(i)) for i in range(options.nJobs)] # local output file on node
wrappers = [os.path.join(wrapperdir, '%s.sh' % (options.output % str(i))) for i in range(options.nJobs)]

inputs = []
ginputs = []
if len(options.input) > 0:  # used in external input mode only
    reservedKeys.append('externalInput')
    inputs = [os.path.abspath(options.input % str(i)) for i in range(options.nJobs)]
    ginputs = ['$CONDOR_DIR_INPUT/%s' % (options.input.split('/')[-1] % str(i)) for i in range(options.nJobs)]  # local input file on node

for i, conf in enumerate(confs):
    reservedVals = [eval(formula) for formula in reservedValFormula]
    if options.local:
        if len(options.input) > 0:
            reservedVals.append(inputs[i])
        tconf.generate(conf, seed = options.seed + i, outputName = outputs[i], reserved = dict(zip(reservedKeys, reservedVals)), cmdargs = sys.argv)
    if options.grid:
        if len(options.input) > 0:
            reservedVals.append(ginputs[i])
        tconf.generate(conf, seed = options.seed + i, outputName = goutputs[i], reserved = dict(zip(reservedKeys, reservedVals)), cmdargs = sys.argv)

## if in local mode, make everything locally in the background
if options.local:
    for i in range(len(confs)):
        cmd = '%s %s > %s &' % (prog, confs[i], logs[i])
        if len(options.input) > 0  and (not os.path.exists(inputs[i])):
            print 'External input file', inputs[i], 'does not exist!'
            continue
        runCmd(cmd)

## if in grid mode, assume running on gpvm machines
if options.grid:

    # point everything to cvmfs
    prog = prog.replace('/e906/app/software/osg', '/cvmfs/seaquest.opensciencegrid.org/seaquest')
    gSetup = os.path.join(os.getenv('SEAQUEST_DISTRIBUTION_ROOT'), 'setup/setup.sh').replace('/e906/app/software/osg', '/cvmfs/seaquest.opensciencegrid.org/seaquest')
    lSetup = os.path.join(os.getenv('DPSIM_ROOT'), 'setup.sh').replace('/e906/app/software/osg', '/cvmfs/seaquest.opensciencegrid.org/seaquest')

    # write wrapper files first
    for i in range(len(confs)):
        fout = open(wrappers[i], 'w')

        fout.write('echo\n')
        fout.write('echo --------------------------------------------------------------------\n')
        fout.write('echo Wrapper script: %s\n' % wrappers[i])
        fout.write('echo Wrapper script generated by %s\n' % ' '.join(sys.argv))
        fout.write('echo Timestamp: %s\n' % datetime.now())
        fout.write('echo Working directory: %s\n' % workdir)
        fout.write('echo Conf file: %s\n' % confs[i])
        fout.write('echo Log file: %s\n' % logs[i])
        fout.write('echo Site: ${GLIDEIN_ResourceName}\n')
        fout.write('echo Node: `hostname`\n')
        fout.write('echo OS: `uname -a`\n')
        fout.write('echo User: `whoami`\n')
        fout.write('echo --------------------------------------------------------------------\n')
        fout.write('echo\n')

        fout.write('source ' + gSetup + '\n')
        fout.write('source ' + lSetup + '\n')
        fout.write('source /cvmfs/seaquest.opensciencegrid.org/seaquest/software/r1.7.0/externals/geant/setup410.sh\n')  #temp fix to use geant4.10
        fout.write('cd $_CONDOR_SCRATCH_DIR\n')
        fout.write('start_sec=$(date +%s)\n')
        fout.write('start_time=$(date +%F_%T)\n')
        fout.write('%s $CONDOR_DIR_INPUT/%s\n' % (prog, confs[i].split('/')[-1]))
        #fout.write('conf=$( ifdh fetchInput %s)\n' % confs[i])
        #fout.write('echo ${conf}\n')
        #fout.write('%s ${conf}\n' % prog)
        fout.write('status=$?\n')
        fout.write('stop_time=$(date +%F_%T)\n')
        fout.write('stop_sec=$(date +%s)\n')
        fout.write('duration=$(expr $stop_sec "-" $start_sec)\n')
        fout.write('echo Executable exit code: $status\n')
        fout.write('echo Job duration in seconds: $duration\n')
        fout.write('exit $status\n')

        fout.close()
        os.system('chmod u+x %s' % wrappers[i])

    # change permissions for grid node access
    runCmd('chmod -R 01755 ' + options.workdir)

    # make jobsub commands and submit
    cmds = []
    for i in range(len(confs)):
        cmd = 'jobsub_submit -g --OS=SL6 -expected-lifetime=2h --use_gftp --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE -e IFDHC_VERSION'
        cmd = cmd + ' -L %s' % logs[i]
        cmd = cmd + ' -f %s' % confs[i]
        if len(options.input) > 0:
            cmd = cmd + ' -f %s' % inputs[i]
        cmd = cmd + ' -d OUTPUT %s' % outputdir
        cmd = cmd + ' file://`which %s`' % wrappers[i]

        if len(options.input) > 0 and (not os.path.exists(inputs[i])):
            print 'External input file', inputs[i], 'does not exist!'
            continue

        cmds.append(cmd)

    #submit all
    GridUtil.submitAllJobs(cmds, 5)
    GridUtil.stopGridGuard()
