import sys

sanitizedName = sys.argv[1].replace('/', '_').rsplit('.', 1)[0]
outputPath = sys.argv[1].rsplit('.', 1)[0] + "_data.h"

outputData = "const char " + sanitizedName + "_data[] {\n"

with open(sys.argv[1], 'rb') as file:
    
    i = 0
    while (byte := file.read(1)):

        outputData += "0x"

        outputData += byte.hex()

        outputData += ", "
        
        i += 1

        if i % 16 == 0:
            outputData += '\n'
    
    outputData = outputData.removesuffix("\n").removesuffix(", ")

outputData += "}"

with open(outputPath, 'w', -1, 'utf-16') as outputFile:
    outputFile.write(outputData)