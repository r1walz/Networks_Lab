#!/usr/bin/env python
import pandas as pd

output = pd.read_csv('analysis.csv')
IP = '10.21.2.171'

print("Average Packet Size: "
     f"{sum(output['Length']) / len(output['Length'])} bytes")
print("Average no of Packets sent per flow: "
     f"{output[output['Source'] == IP].groupby(['Source','Destination'])['Length'].count().mean()}")
print("average no of Bytes sent per flow: "
     f"{output[output['Source'] == IP].groupby(['Source','Destination'])['Length'].sum().mean()}")

x = len(output[output['Destination'] == IP]['Length']) / len(output[output['Source'] == IP]['Length'])
print(f'Average Ratio of incoming to outgoing packets: {x}')

x = (output[output['Source'] == IP]['Time'])
ans = (x.iloc[len(x) - 1] - x.iloc[0]) / len(x)
print(f'Average Time interval between packets sent: {ans}')

n_destinations = len(set(output[output['Source'] == IP]['Destination']))
print('Average connections to number of destination IPs: '
     f'{len(output) / n_destinations}')

avg_duration = (output.groupby(['Source','Destination'])['Time'].max()
- output.groupby(['Source','Destination'])['Time'].min()).mean()
print(f'Average flow duration: {avg_duration}')
print("Average no of Packets received per flow: "
     f"{output[output['Destination'] == IP].groupby(['Source','Destination'])['Length'].count().mean()}")
print("Average no of Bytes received per flow: "
     f"{output[output['Destination'] == IP].groupby(['Source','Destination'])['Length'].sum().mean()}")

ans = sum(output[output['Destination'] == IP]['Length']) / sum(output[output['Source'] == IP]['Length'])
print(f'Average ratio of incoming to outgoing bytes: {ans}')

x = (output[output['Destination'] == IP]['Time'])
ans = (x.iloc[len(x) - 1] - x.iloc[0]) / len(x)
print(f'Average Time interval between packets received: {ans}')
