#!/usr/bin/env python

import os
from datetime import datetime

class DPSimJobConf:
    """ Container of base DPSim job configuration, and generator of new job cards """

    def __init__(self, template, reserved = []):
        """ read the template conf file, ignore the key words that are passed in reserved list """

        self.attr = {}
        self.contents = []
        self.userdef = ['seed', 'outputFileName'] + reserved
        self.origin = os.path.abspath(template)

        if not os.path.exists(template):
            print 'Template file not found! '
            return

        for line in open(template, 'r').readlines():
            if len(line.strip()) == 0:
                continue

            if '#' not in line:
                key, val = line.strip().split()
                if key not in self.userdef:
                    self.attr[key] = val
                    self.contents.append(line.strip())

    def __getattr__(self, name):
        if name in self.attr:
            return self.attr[name]
        elif name[0] == '_' and int(name[1:]) < len(self.contents):
            return self.contents[int(name[1:])]
        return None

    def generate(self, confname, seed = 1, outputName = '', reserved = {}):

        if outputName == '':
            outputName = confname.replace('.conf', '.root')

        try:
            fout = open(confname, 'w')
        except EnvironmentError:
            print 'Cannot write conf file', confname
            return False

        fout.write('\n# ---- Generated at %s ----' % datetime.now())
        fout.write('\n# ---- Template: %s ----\n\n' % self.origin)

        fout.write('\n# ---- Job-specific settings ----\n')
        fout.write('seed %d\n' % seed)
        fout.write('outputFileName %s\n' % outputName)
        for item in reserved:
            fout.write('%s %s\n' % (item, reserved[item]))

        fout.write('\n# ---- Universal settings ----\n')
        for line in self.contents:
            fout.write('%s\n' % line)
        fout.close()

        return True

if __name__ == '__main__':
    a = DPSimJobConf('../conf/example.conf', reserved = ['nEvents'])
    print a.x1Min
    print a._10

    print a.generate(confname = 'test.conf', seed = 10, reserved = {'nEvents' : 100})
