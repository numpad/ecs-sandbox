#!/usr/bin/env python3

from os import listdir
from os.path import isfile, isdir, join

ECS_PATH = 'src/ecs'

# group directories
ecs_dirs = [d for d in listdir(ECS_PATH) if isdir(join(ECS_PATH, d))]

def make_include_all_file(subdir):
	dirpath = join(ECS_PATH, subdir)
	files = (dirpath.replace('src/', ''), [f for f in listdir(dirpath) if isfile(join(dirpath, f))])
	return files

def include_text(subdir):
	files = make_include_all_file(subdir)
	hfiles = []
	for file in files[1]:
		if file.endswith('.h') or file.endswith('.hpp'):
			hfiles.append('#include <%s>' % join(files[0], file))
	return hfiles

for d in ecs_dirs:
	header_fn = '%s.hpp' % join(ECS_PATH, d)
	header = open(header_fn, 'w+')
	
	header.write('#pragma once\n// auto-generated\n\n')
	for f in include_text(d):
		header.write(f + '\n')
	
	header.close()

