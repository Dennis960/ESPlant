import seaborn as seaborn
import matplotlib.pyplot as plot
import numpy as np

seaborn.set_style("whitegrid")
seaborn.set_context("paper")

sensorDict = {
    "full": 0,
    "standard": 1,
    "tracks": 2,
    "half": 3,
    # "floating": 4
}
moistureDict = {
    "dry": 0,
    "wet": 1
}
r1Dict = {
    "1k": 1000,
    "4.7k": 4700,
    "10k": 10000,
    "20k": 20000,
}
r2Dict = {
    "100k": 100000,
    "1M": 1000000,
    "100M": 100000000
}
c1Dict = {
    "104pF": 0.000000000104,
    "0.1uF": 0.0000001,
    "1uF": 0.000001,
    "10uF": 0.00001
}

dataDict = {}
measurementDictWet = {}
measurementDictDry = {}
stabilizationDictWet = {}
stabilizationDictDry = {}

for sensorKey in sensorDict:
    for moistureKey in moistureDict:
        for r1Key in r1Dict:
            for r2Key in r2Dict:
                for c1Key in c1Dict:
                    # Read data.csv (frequency,duty_cycle,measurement,stabilization_time)
                    file = sensorKey + '/' + moistureKey + '-' + r1Key + '-' + r2Key + '-' + c1Key + '.csv'
                    try:
                        data = np.genfromtxt(file, delimiter=',', skip_header=1)
                    except:
                        continue
                    # ----------------- clean data -----------------
                    # remove from data where measurement == 4096
                    data = data[data[:, 2] < 4096]
                    # remove from data where measurement < 100
                    data = data[data[:, 2] > 100]
                    # remove from data where duty_cycle == 255
                    data = data[data[:, 1] < 255]
                    # --------------- clean data end ---------------
                    # add to dataDict
                    for row in data:
                        key = "S: {:8s} | R1: {:4s} | R2: {:4s} | C1: {:5s} | Freq: {:7d} | Duty: {:3d}".format(str(sensorKey), str(r1Key), str(r2Key), str(c1Key), int(row[0]), int(row[1]))
                        if (moistureKey == "wet"):
                            measurementDictWet[key] = row[2]
                            stabilizationDictWet[key] = row[3]
                        else:
                            measurementDictDry[key] = row[2]
                            stabilizationDictDry[key] = row[3]
                        dataDict[key] = {
                            'sensor': sensorDict[sensorKey],
                            'moisture': moistureDict[moistureKey],
                            'r1': r1Dict[r1Key],
                            'r2': r2Dict[r2Key],
                            'c1': c1Dict[c1Key],
                            'frequency': row[0],
                            'duty_cycle': row[1],
                            'measurement': row[2],
                            'stabilization_time': row[3],
                            'difference': None,
                            'stabilization_time_avg': None
                        }


for key in measurementDictWet:
    # add to differenceDict if in both dicts
    if key in measurementDictDry:
        dataDict[key]['difference'] = measurementDictDry[key] - measurementDictWet[key]

for key in stabilizationDictWet:
    # add to stabilizationDict if in both dicts
    if key in stabilizationDictDry:
        dataDict[key]['stabilization_time_avg'] = (stabilizationDictDry[key] + stabilizationDictWet[key]) / 2

# remove all keys where difference is None or stabilization_time_avg is None
for key in list(dataDict):
    if dataDict[key]['difference'] is None or dataDict[key]['stabilization_time_avg'] is None:
        del dataDict[key]

sortedByDifference = sorted(dataDict.items(), key=lambda x: x[1]['difference'], reverse=True)[:200]
# reverse sortedByDifference
sortedByDifference.reverse()
print("best by difference")
print("   sensor         R1         R2         C1            Freq         Duty   => difference = measurementDry - measurementWet => stabilization")
print("------------------------------------------------------------------------------------------------------------------------------------------")
keys = []
for item in sortedByDifference:
    print("{:73s} =>    {:4d}    =      {:4d}      -      {:4d}      =>      {:4d}".format(item[0], int(item[1]["difference"]), int(measurementDictDry[item[0]]), int(measurementDictWet[item[0]]), int(dataDict[item[0]]['stabilization_time_avg'])))
    keys.append(item[0])

# print freq and duty_cycle min and max
maxDatas = []
for key in keys:
    data = dataDict[key]
    maxDatas.append(data)

maxDatas = sorted(maxDatas, key=lambda x: x['frequency'], reverse=True)
print("frequency : {} - {}".format(maxDatas[-1]["frequency"], maxDatas[0]["frequency"]))
maxDatas = sorted(maxDatas, key=lambda x: x['duty_cycle'], reverse=True)
print("duty_cycle: {} - {}".format(maxDatas[-1]["duty_cycle"], maxDatas[0]["duty_cycle"]))

# print most common r1, r2 and c1
r1s = {}
r2s = {}
c1s = {}
for key in keys:
    data = dataDict[key]
    if data['r1'] not in r1s:
        r1s[data['r1']] = 0
    r1s[data['r1']] += 1
    if data['r2'] not in r2s:
        r2s[data['r2']] = 0
    r2s[data['r2']] += 1
    if data['c1'] not in c1s:
        c1s[data['c1']] = 0
    c1s[data['c1']] += 1

print("r1s: {}".format(r1s))
print("r2s: {}".format(r2s))
print("c1s: {}".format(c1s))

def plot2D(keyX, keyY, ax, log=False):
    x = []
    y = []
    for key in dataDict:
        data = dataDict[key]
        x.append(data[keyX])
        y.append(data[keyY])
        
    if log:
        ax.set_xscale('log')
        ax.set_yscale('log')
    ax.scatter(x, y, c='blue')
    ax.set_xlabel(keyX)
    ax.set_ylabel(keyY)
    ax.set_title("{} - {}".format(keyX, keyY))

def plot3D(keyX, keyY, keyZ, ax, log=False, dotSize=10):
    x = []
    y = []
    z = []
    for key in dataDict:
        data = dataDict[key]
        x.append(data[keyX])
        y.append(data[keyY])
        z.append(data[keyZ])
        
    if log:
        ax.set_xscale('log')
        ax.set_yscale('log')
    ax.scatter(x, y, c=z, cmap='viridis', s=dotSize)
    ax.set_xlabel(keyX)
    ax.set_ylabel(keyY)
    ax.set_title("{} - {} - {}".format(keyX, keyY, keyZ))

def plot2DHistogram(keyX, keyFilter, filter, ax, color, alpha=1):
    # calculate buckets of size 100
    buckets = {}
    count = 0
    for key in dataDict:
        data = dataDict[key]
        if keyFilter != None and data[keyFilter] != filter:
            continue
        count += 1
        bucket = int(data[keyX] / 100) * 100
        if bucket not in buckets:
            buckets[bucket] = []
        buckets[bucket].append(data[keyX])

    # x: bucket, y: count
    x = []
    y = []
    for bucket in buckets:
        x.append(bucket)
        y.append(len(buckets[bucket]) / count)

    ax.bar(x, y, width=100, color=color, alpha=alpha)
    ax.set_xlabel(keyX)
    ax.set_ylabel('count')
    ax.set_title("{} - count".format(keyX))

# def plot2DBarChart(keyX, keyY, ax):
    

# create 2x3 subplots
fig, ((ax1, ax2, ax3), (ax4, ax5, ax6)) = plot.subplots(2, 3, figsize=(16, 9))

# keys:
#   sensor
#   moisture
#   r1
#   r2
#   c1
#   frequency
#   duty_cycle
#   measurement
#   stabilization_time
#   difference
#   stabilization_time_avg

# plot2DHistogram('difference', 'sensor', 0, ax1, 'red', 0.2)
# plot2DHistogram('difference', 'sensor', 1, ax1, 'green', 0.2)
# plot2DHistogram('difference', 'sensor', 2, ax1, 'blue', 0.2)
# plot2DHistogram('difference', 'sensor', 3, ax1, 'yellow', 0.2)
# plot2DHistogram('difference', 'sensor', 4, ax1, 'black', 0.2)

# plot2DHistogram('difference', 'sensor', 0, ax2, 'red')
# plot2DHistogram('difference', 'sensor', 1, ax3, 'green')
# plot2DHistogram('difference', 'sensor', 2, ax4, 'blue')
# plot2DHistogram('difference', 'sensor', 3, ax5, 'yellow')
# plot2DHistogram('difference', 'sensor', 4, ax6, 'black')

plot3D('frequency', 'duty_cycle', 'difference', ax1, True, 300)
plot3D('duty_cycle', 'c1', 'difference', ax2, True, 300)
plot3D('frequency', 'r1', 'difference', ax3, True, 300)
plot3D('frequency', 'c1', 'difference', ax4, True, 300)
plot2D('r1', 'difference', ax5, True)
plot2D('r1', 'stabilization_time', ax5, True)
plot3D('frequency', 'duty_cycle', 'stabilization_time', ax6, True, 300)


fig.tight_layout()
plot.show()

# best by difference
#    sensor         R1         R2         C1            Freq         Duty   => difference = measurementDry - measurementWet => stabilization
# ------------------------------------------------------------------------------------------------------------------------------------------
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     400 | Duty:   3 =>    2512    =      2943      -       431      =>       877
# S: full     | R1: 10k  | R2: 100M | C1: 104pF | Freq:   12800 | Duty:   3 =>    2513    =      3091      -       578      =>       668
# S: full     | R1: 10k  | R2: 100M | C1: 104pF | Freq:   51200 | Duty:  13 =>    2525    =      3272      -       747      =>       418
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   51200 | Duty:  34 =>    2529    =      3573      -      1044      =>       209
# S: standard | R1: 10k  | R2: 100M | C1: 104pF | Freq:   25600 | Duty:   5 =>    2540    =      3216      -       676      =>       604
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    3200 | Duty:  34 =>    2543    =      3624      -      1081      =>       209
# S: standard | R1: 10k  | R2: 100M | C1: 104pF | Freq:   51200 | Duty:   8 =>    2550    =      3176      -       626      =>       484
# S: full     | R1: 10k  | R2: 100M | C1: 104pF | Freq:   25600 | Duty:   5 =>    2553    =      3093      -       540      =>       543
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:  100000 | Duty:   3 =>    2576    =      2970      -       394      =>       376
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     200 | Duty:   3 =>    2577    =      3110      -       533      =>       753
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    1600 | Duty:  21 =>    2608    =      3563      -       955      =>       209
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   25600 | Duty:  21 =>    2624    =      3597      -       973      =>       209
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     800 | Duty:   5 =>    2657    =      3092      -       435      =>       668
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:  100000 | Duty:   8 =>    2671    =      3440      -       769      =>       293
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     100 | Duty:   2 =>    2685    =      2992      -       307      =>       879
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   51200 | Duty:   8 =>    2700    =      3075      -       375      =>       376
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     400 | Duty:  21 =>    2717    =      3525      -       808      =>       335
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    3200 | Duty:   5 =>    2719    =      3118      -       399      =>       626
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     400 | Duty:   5 =>    2733    =      3226      -       493      =>       585
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     800 | Duty:  21 =>    2738    =      3498      -       760      =>       251
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   12800 | Duty:   3 =>    2742    =      3132      -       390      =>       459
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:  100000 | Duty:   5 =>    2749    =      3259      -       510      =>       335
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     100 | Duty:   3 =>    2750    =      3180      -       430      =>       670
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   25600 | Duty:   5 =>    2758    =      3139      -       381      =>       459
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    1600 | Duty:  13 =>    2758    =      3505      -       747      =>       251
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    6400 | Duty:   8 =>    2770    =      3161      -       391      =>       543
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   51200 | Duty:  21 =>    2777    =      3478      -       701      =>       293
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     800 | Duty:   8 =>    2796    =      3293      -       497      =>       501
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    1600 | Duty:   5 =>    2797    =      3272      -       475      =>       459
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     400 | Duty:   8 =>    2810    =      3378      -       568      =>       460
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:     800 | Duty:  13 =>    2814    =      3436      -       622      =>       334
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   25600 | Duty:  13 =>    2817    =      3498      -       681      =>       251
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    1600 | Duty:   8 =>    2819    =      3391      -       572      =>       334
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    6400 | Duty:  21 =>    2824    =      3487      -       663      =>       292
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   12800 | Duty:   8 =>    2836    =      3502      -       666      =>       251
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    3200 | Duty:  13 =>    2839    =      3469      -       630      =>       293
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   51200 | Duty:  13 =>    2846    =      3326      -       480      =>       293
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    3200 | Duty:   8 =>    2860    =      3347      -       487      =>       417
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:    6400 | Duty:  13 =>    2867    =      3364      -       497      =>       334
# S: tracks   | R1: 10k  | R2: 100M | C1: 104pF | Freq:   25600 | Duty:   8 =>    2875    =      3353      -       478      =>       292
# frequency : 100.0 - 100000.0
# duty_cycle: 2.0 - 34.0
# r1s: {10000: 40}
# r2s: {100000000: 40}
# c1s: {1.04e-10: 40}