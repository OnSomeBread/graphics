import math
# print basic data with settings and obj defs
split = open('test.rd', 'r').read().split('WorldBegin')
rd_prog = split[0]
frameData = split[1].split('WorldEnd')[0].split("\n")

# frameData = []
# for line in open("frame.txt", 'r').read().split('\n'):
#     # used to print incase want to use directly in code instead for changes
#     print("\"" + line.replace("\"", "\\\"") + "\",")
#     #frameData.append(line)

output = rd_prog + '\n'
# range(360) gives a full spin
for i in range(360):
    output += "FrameBegin " + str(i) + "\n"
    output += "WorldBegin\n"
    output += "Rotate \"Z\" " + str(i) + '\n'

    for line in frameData:
        output += line + '\n'

    output += "WorldEnd\n"
    output += "FrameEnd\n"


# print(rd_prog)
# for i in range(1000):
#     print("FrameBegin", i)
#     print("WorldBegin")

#     for line in frameData:
#         print(line)

#     print("WorldEnd")

open("output.txt", 'w').write(output)