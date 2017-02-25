#!/usr/bin/python3

import urllib.request
import tarfile
import platform
import os
import shutil
from multiprocessing import Pool

def dlAndExtract(tup):
	url = tup[0]
	filename = tup[1]
	extractto = tup[2]
	
	print("downloading " + filename)
	
	if not os.path.isfile(filename):
		# download
		with urllib.request.urlopen(url) as response, open(filename, 'wb') as out_file:
			shutil.copyfileobj(response, out_file)
	
	print("extracting " + filename + " to " + extractto)
	
	# extract
	tar = tarfile.open(filename)
	tar.extractall(extractto)


chigraphDir = os.path.dirname(os.path.realpath(__file__))
thirdPartyDir = os.path.join(chigraphDir, "third_party")

urls=[]

if platform.system() == "Linux":
	
	urls.append(('https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-debug-gcc6.3-linux64.tar.xz', os.path.join(thirdPartyDir, "kf5-debug.tar.xz"), thirdPartyDir))
	urls.append(('https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-release-gcc6.3-linux64.tar.xz', os.path.join(thirdPartyDir, "kf5-release.tar.xz"), thirdPartyDir))
	
	urls.append(('https://github.com/chigraph/chigraph/releases/download/dependencies/llvm-4.0-release-gcc6.3-linux64.tar.xz', os.path.join(thirdPartyDir, "llvm-release.tar.xz"), thirdPartyDir))
	urls.append(('https://github.com/chigraph/chigraph/releases/download/dependencies/llvm-4.0-debug-gcc6.3-linux64.tar.xz', os.path.join(thirdPartyDir, "llvm-debug.tar.xz"), thirdPartyDir))
	
elif platform.system() == "Windows":
	urls.append(('https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-release-gcc6.3-win64.tar.xz', os.path.join(thirdPartyDir, "kf5-release.tar.xz"), thirdPartyDir)))
	urls.append(('https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-debug-gcc6.3-win64.tar.xz', os.path.join(thirdPartyDir, "kf5-debug.tar.xz"), thirdPartyDir)))
elif platform.system() == "Darwin":
	urls.append(('https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-debug-appleclang8-darwin64.tar.xz', os.path.join(thirdPartyDir, "kf5-debug.tar.xz"), thirdPartyDir))
	urls.append(('https://github.com/chigraph/chigraph/releases/download/dependencies/kf5-5.31.0-release-appleclang8-darwin64.tar.xz', os.path.join(thirdPartyDir, "kf5-release.tar.xz"), thirdPartyDir))


p = Pool(len(urls))
p.map(dlAndExtract, urls)
