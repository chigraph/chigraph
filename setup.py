#!/usr/bin/python3

import urllib.request
import tarfile
import platform

def dlAndExtract(url, filename, extractto):
	
	# download
	with urllib.request.urlopen(url) as response, open(filename, 'wb') as out_file:
		shutil.copyfileobj(response, out_file)
	
	# extract
	tar = tarfile.open(filename)
	tar.extractall(extractto)

if platform.system() == "Linux":
	
elif platform.system() == "Windows":
	
elif platform.system() == "OSX":
	

