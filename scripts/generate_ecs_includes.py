#!/usr/bin/env python3

from os import listdir
from os.path import isfile, isdir, join

ECS_PATH = 'src/ecs'

# group directories
ecs_dirs = [d for d in listdir(ECS_PATH) if isdir(join(ECS_PATH, d))]

def make_include_all_file(subdir):
	dirpath = join(ECS_PATH, subdir)
	files = (dirpath, [f for f in listdir(dirpath) if isfile(join(dirpath, f))])
	return files

def include_text(files):
	for file in files[1]:
		print(join(files[0], file))

for d in ecs_dirs:
	files = make_include_all_file(d)
	include_text(files)
