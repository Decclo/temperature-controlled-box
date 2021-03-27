from matplotlib import pyplot as plt
from itertools import tee
from colorama import Fore, Back, Style
import numpy as np
import argparse
import re
import json

''' The JSON should be in the following format:
  {
  "ms": 1000000,
  "sensors": [
  {
    "sensor01": 00.000,
    "sensor02": 00.000,
    "sensor03": 00.000
  }
  ],
  "sensorMean": 00.000,
  "fan": 255,
  "heatingElement": 0
  }
'''


# Introducing argparse to more easily select the correct logfile.
parser = argparse.ArgumentParser(description='Reads a logfile for the Temperated Box and creates relevant graphs.')
parser.add_argument('filepath', metavar='PATH', type=str,
                    help='Path to file to be read')
parser.add_argument('-o', '--output', type=str,
                    default="",
                    help='Save the extracted JSON to a file')
parser.add_argument('-ma', '--moving-average', type=int, dest='moving_average',
                    default=None,
                    help='Add a moving average of sensorMean to sensor plot')

args = parser.parse_args()


# Defining a rolling window function, source: https://stackoverflow.com/a/6822907
def window(iterable, size):
    iters = tee(iterable, size)
    for i in range(1, size):
        for each in iters[i:]:
            next(each, None)
    return zip(*iters)  


# Load the logfile
with open(args.filepath) as f:
    content = f.read().splitlines()


# Filter content so we only get the actual values
content = [val for val in content
        if re.search(r'^\{.*\}$', val)]


# If the --extract option was specified, save the extracted JSON to the specified file
print(Fore.BLUE + '[INFO] ' + Style.RESET_ALL + 'Writing output to ' + args.output)
try:
    if(args.output != ""):
        f = open(args.output, "w")
        for line in content:
            f.write(line)
            f.write("\n")
        f.close()
except:
    print(Fore.YELLOW + '[WARNING] ' + Style.RESET_ALL + 'Cannot save extracted JSON to specified location')    


# Iterate through the json data and pick out the values we want
data_raw = [[],[],[],[],[],[],[]]
for line in content:
    data = json.loads(line)
    data_raw[0].append(data['ms']/1000)
    data_raw[1].append(data['sensors'][0]['sensor00'])
    data_raw[2].append(data['sensors'][0]['sensor01'])
    data_raw[3].append(data['sensors'][0]['sensor02'])
    data_raw[4].append(data['sensorMean'])
    data_raw[5].append(data['fan'] / 255)
    data_raw[6].append(data['heatingElement'])


if (args.moving_average != None):
    # Finding the average of the last x values of sensorMean
    window_size = args.moving_average
    moving_average = []
    for i in range(window_size - 1):
        moving_average.append(None)

    for each in window(data_raw[4], window_size):
        moving_average.append(sum(each)/len(each))


# Find the average of sensorMean and make a list for plotting
sensorMean_average = sum(data_raw[4])/len(data_raw[4])
sensorMean_average_list = []
for i in data_raw[4]:
    sensorMean_average_list.append(sensorMean_average)
print(Fore.BLUE + '[INFO] ' + Style.RESET_ALL + 'The average of sensorMean is: ', sensorMean_average)


# Plot the data
# Make a figure that consists of two plots, an upper for sensors, and lower for actuators
a1 = plt.subplot2grid((3,2),(0,0),rowspan = 2, colspan = 2)
a2 = plt.subplot2grid((3,2),(2,0),colspan = 2)


# Upper plot for sensors
a1.plot(data_raw[0], data_raw[1], label='sensor00 (lower)')
a1.plot(data_raw[0], data_raw[2], label='sensor01 (upper)')
a1.plot(data_raw[0], data_raw[3], label='sensor02 (outside)')
a1.plot(data_raw[0], data_raw[4], label='sensorMean')
if (args.moving_average != None):     # Only draw the moving average if moving_average is defined
    try:        # Try to draw the moving average, if this gives and error, then it is most likely caused by too large window
        a1.plot(data_raw[0], moving_average, label=('moving average, resolution: ' + str(window_size)))
    except:
        print(Fore.YELLOW + '[WARNING] ' + Style.RESET_ALL + 'Not enough data to create moving average with specified window')
a1.plot(data_raw[0], sensorMean_average_list, label=('average: ' + str(round(sensorMean_average, 2))))
a1.set_title('Sensors')
a1.set_ylabel("Temperature [C]")
a1.grid()
a1.legend()


# Lower plot for actuators
a2.plot(data_raw[0], data_raw[5], label='Fan')
a2.plot(data_raw[0], data_raw[6], label='Heating element')
a2.set_title('Control Output')
a2.set_xlabel("Seconds since start")
a2.set_ylabel("Value")
a2.grid()
a2.legend()


# Visualize the plot
plt.tight_layout()
plt.show()