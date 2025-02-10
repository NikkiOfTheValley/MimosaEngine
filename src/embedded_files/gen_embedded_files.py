import os
import sys

# This Python script generates the embedded engine files.
# Add a file to the `assets/` directory to include it in the embedded files.

for subdir, dirs, files in os.walk("assets/"):
    for file in files:
        filePath = os.path.join(subdir, file)

        sanitizedName = filePath.replace('/', '_').replace('\\', '_').replace('-', '_').replace('.', '_')
        outputPath = "headers/" + sanitizedName + ".h"

        numBytes = 0

        with open(filePath, 'rb') as file:
            
            bytes = file.read()
            numBytes = len(bytes)

            bytesAsString = [format(b, '#04x') for b in bytes]

            outputData = "unsigned char " + sanitizedName + "[] = {{{}}}".format(", ".join(bytesAsString))

        with open(outputPath, 'w', -1, 'utf-8') as outputFile:
            outputFile.write("const size_t " + sanitizedName.upper() + "_SIZE = " + str(numBytes) + ";\n")
            outputFile.write(outputData)
            outputFile.write(";")