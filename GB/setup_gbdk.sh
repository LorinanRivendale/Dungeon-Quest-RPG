#!/bin/bash
# Setup script for GBDK-2020

echo "================================"
echo "GBDK-2020 Setup Script"
echo "================================"
echo ""

# Check if GBDK is already installed
if [ -d "/opt/gbdk" ]; then
    echo "GBDK already installed at /opt/gbdk"
    echo ""
    /opt/gbdk/bin/lcc --version
    exit 0
fi

echo "Downloading GBDK-2020 4.2.0..."
echo ""

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]] || [[ "$OSTYPE" == "linux" ]]; then
    URL="https://github.com/gbdk-2020/gbdk-2020/releases/download/4.2.0/gbdk-linux64.tar.gz"
    ARCHIVE="gbdk-linux64.tar.gz"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    URL="https://github.com/gbdk-2020/gbdk-2020/releases/download/4.2.0/gbdk-macos.tar.gz"
    ARCHIVE="gbdk-macos.tar.gz"
else
    echo "Unsupported OS: $OSTYPE"
    echo "Please download GBDK manually from:"
    echo "https://github.com/gbdk-2020/gbdk-2020/releases"
    exit 1
fi

# Download
wget "$URL" -O "$ARCHIVE"

if [ $? -ne 0 ]; then
    echo "Download failed! Please install manually."
    exit 1
fi

# Extract
echo "Extracting GBDK..."
tar -xzf "$ARCHIVE"

# Install to /opt/gbdk
echo "Installing to /opt/gbdk (requires sudo)..."
sudo mv gbdk /opt/gbdk

if [ $? -ne 0 ]; then
    echo "Installation failed! You may need to run with sudo."
    exit 1
fi

# Cleanup
rm "$ARCHIVE"

echo ""
echo "================================"
echo "GBDK-2020 installed successfully!"
echo "================================"
echo ""
echo "Location: /opt/gbdk"
echo "Compiler: /opt/gbdk/bin/lcc"
echo ""
echo "You can now run: make"
echo ""
