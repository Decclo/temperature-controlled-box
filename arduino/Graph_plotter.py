#!/home/locus/anaconda3/envs/tbox/bin/python

from matplotlib import pyplot as plt
import numpy as np
import argparse
import re

# Introducing argparse to more easily select the correct logfile.
parser = argparse.ArgumentParser(description='Reads a logfile for the Temperated Box and creates relevant graphs.')
parser.add_argument('filepath', metavar='PATH', type=str,
                    help='Path to file to be read')
#parser.add_argument('--sum', dest='accumulate', action='store_const',
#                    const=sum, default=max,
#                    help='sum the integers (default: find the max)')

args = parser.parse_args()

# Load the logfile
with open(args.filepath) as f:
    content = f.read().splitlines()

# Filter content so we only get the actualy values
content = [val for val in content
        if re.search(r'([0-9]+)\t([0-9]+\.[0-9]+)\t([0-9]+\.[0-9]+)\t([0-9][0-9][0-9])\t[0-9]', val)]

# Print the data to get a better idea
for line in content:
    print(line)