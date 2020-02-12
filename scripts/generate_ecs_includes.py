#!/usr/bin/env python3

from os import listdir
from os.path import isfile, isdir, join
import hashlib

ECS_PATH = 'src/ecs'

# group directories
ecs_dirs = [d for d in listdir(ECS_PATH) if isdir(join(ECS_PATH, d))]

def make_include_all_file(subdir):
	dirpath = join(ECS_PATH, subdir)
	files = (dirpath.replace('src/', ''), [f for f in listdir(dirpath) if isfile(join(dirpath, f))])
	return files

def include_lines(subdir):
	files = make_include_all_file(subdir)
	hfiles = []
	for file in files[1]:
		if file.endswith('.h') or file.endswith('.hpp'):
			hfiles.append('#include <%s>' % join(files[0], file))
	return hfiles

# compare the hash stored in file at header_fn to new_hash
def has_changed(header_fn, new_hash):
	file = open(header_fn, 'r')
	file.seek(3) # skip comment
	cur_hash = file.readline().strip()
	file.close()
	
	return cur_hash != new_hash


for d in ecs_dirs:
	header_fn = '%s.hpp' % join(ECS_PATH, d)
	lines = include_lines(d)
	hashed_includes = hashlib.sha1('\n'.join(lines).encode('utf-8')).hexdigest()
	
	# don't generate a new header file with identical content so CMake doesn't
	# trigger a rebuild
	if not has_changed(header_fn, hashed_includes):
		continue
	
	header = open(header_fn, 'w+')
	header.write('// ' + hashed_includes + '\n');
	header.write('#pragma once\n\n')
	for include in lines:
		header.write(include + '\n')
	
	header.close()

