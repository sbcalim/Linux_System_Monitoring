import pymongo
import re
import time
import pygal
from pymongo import MongoClient
 
client = MongoClient();
db = client.INPUT

""" 
for x in db.CPU.find({}).sort('time', pymongo.ASCENDING).limit(1):
    y = str(x).split("\'CPU", 1)[1]
    print map(int, re.findall(r'\d+', y))
"""

def ram_stuff( ):
	data = db.RAM.find({}, {"RAM_PERCENTAGE": 1, "_id": False}).sort('time', pymongo.DESCENDING).limit(1)
	ram = re.compile('\'RAM_PERCENTAGE\': [0-9]*')
	ramstring = re.findall('\d+', str(ram.findall(str(data[0]))))
	RAM.append(int(ramstring[0]) / 100)

def cpu_stuff( ):
	data = db.CPU.find({},{"CPU":1, "CPU1":1, "CPU2":1, "CPU3":1, "CPU4": 1, "_id": False}).sort('time', pymongo.DESCENDING).limit(1)
	
	cpu = re.compile('\'CPU\': [0-9]*')
	cpustring = cpu.findall(str(data[0]))
	check_null(re.findall('\d+', str(cpustring[0])), CPU)

	cpu = re.compile('\'CPU1\': [0-9]*')
	cpustring = cpu.findall(str(data[0]))
	check_null(re.findall('1\d+', str(cpustring[0])), CPU1)

	cpu = re.compile('\'CPU2\': [0-9]*')
	cpustring = cpu.findall(str(data[0]))
	check_null(re.findall('2\d+', str(cpustring[0])), CPU2)

	cpu = re.compile('\'CPU3\': [0-9]*')
	cpustring = cpu.findall(str(data[0]))
	check_null(re.findall('3\d+', str(cpustring[0])), CPU3)

	cpu = re.compile('\'CPU4\': [0-9]*')
	cpustring = cpu.findall(str(data[0]))
	check_null(re.findall('4\d+', str(cpustring[0])), CPU4)


def check_null( data, list ):
	if not data:
		list.append(1)
	else:
		list.append(int(data[0]) / 100)

while 1 == 1:
	RAM = []
	CPU = []
	CPU1 = []
	CPU2 = []
	CPU3 = []
	CPU4 = []
	counter = 0

	while counter < 10:
		ram_stuff()
		cpu_stuff()
		counter+=1
		time.sleep(1)
		
	rc = pygal.XY()
	cc = pygal.XY()
	
	rc.title = 'RAM PERCENTAGE'
	cc.title = 'CPU PERCENTAGES'
	
	rc.add('Percentage',  [(0, 0), (0, 100)])
	rc.add('Time', [(0, 0), (9, 0)])
	
	rc.add('Memory', [(0, RAM[0]), (1, RAM[1]), (2, RAM[2]), (3, RAM[3]), (4, RAM[4]), (5, RAM[5]), (6, RAM[6]), (7, RAM[7]), (8, RAM[8]), (9, RAM[9])])
	
	cc.add('Percentage',  [(0, 0), (0, 100)])
	cc.add('Time', [(0, 0), (9, 0)])
	
	cc.add('CPU', [(0, CPU[0]), (1, CPU[1]), (2, CPU[2]), (3, CPU[3]), (4, CPU[4]), (5, CPU[5]), (6, CPU[6]), (7, CPU[7]), (8, CPU[8]), (9, CPU[9])])
	cc.add('CPU 1', [(0, CPU1[0]), (1, CPU1[1]), (2, CPU1[2]), (3, CPU1[3]), (4, CPU1[4]), (5, CPU1[5]), (6, CPU1[6]), (7, CPU1[7]), (8, CPU1[8]), (9, CPU1[9])])
	cc.add('CPU 2', [(0, CPU2[0]), (1, CPU2[1]), (2, CPU2[2]), (3, CPU2[3]), (4, CPU2[4]), (5, CPU2[5]), (6, CPU2[6]), (7, CPU2[7]), (8, CPU2[8]), (9, CPU2[9])])
	cc.add('CPU 3', [(0, CPU3[0]), (1, CPU3[1]), (2, CPU3[2]), (3, CPU3[3]), (4, CPU3[4]), (5, CPU3[5]), (6, CPU3[6]), (7, CPU3[7]), (8, CPU3[8]), (9, CPU3[9])])
	cc.add('CPU 4', [(0, CPU4[0]), (1, CPU4[1]), (2, CPU4[2]), (3, CPU4[3]), (4, CPU4[4]), (5, CPU4[5]), (6, CPU4[6]), (7, CPU4[7]), (8, CPU4[8]), (9, CPU4[9])])

	rc.render_to_file('rc.svg')
	cc.render_to_file('cc.svg')

	del CPU
	del CPU1
	del CPU2
	del CPU3
	del CPU4
	del RAM
