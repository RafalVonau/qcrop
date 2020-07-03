#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os
import sys
import errno
import subprocess
import time
import hashlib
import random
import atexit
import pathlib
import re
from pathlib import Path, PureWindowsPath

def getKargosVersion():
	s = open("../src/qcrop.pro").readlines()
	p = re.compile(r'VERSION\s*=\s*(.+)$')
	for line in s:
		z = p.findall(line)
		if z:
			return z[0]

def runit(cmd):
	os.system(cmd)


idir=pathlib.Path(__file__).parent.absolute()
kdir=pathlib.Path(idir).parent.absolute()
version = getKargosVersion()

print(idir)
print(kdir)
print("Kargos v" + version)

s = open("WinInstaller.iss.in").read()
s = s.replace('%KVERSION', version)
s = s.replace('%IPATH', str(idir).replace('/', '\\'))
s = s.replace('%KPATH', str(kdir).replace('/', '\\'))
f = open("WinInstaller.iss", 'w')
f.write(s)
f.close()

runit("wine32 \"c:/Program Files/Inno Setup 5/ISCC.exe\" ./WinInstaller.iss")