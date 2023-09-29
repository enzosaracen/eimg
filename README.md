lossy image compressor

run `./eimg [file] [qmod]` where `[file]` is a PNG image and `[qmod]`
is a float that optionally sets the quantization level (higher qmod = higher compression rate = lower quality)

`eimg` employs lossy DCT-based compression and works very similarly to JPEG

the program will compress the PNG image data and encode it
before decoding it immediately after and displaying the compressed image using an SDL window

`eimg` is in the process of supporting a simple file format for the DCT encoded data
using Huffman coding, but it's not there yet

inspired by: https://www.youtube.com/watch?v=0me3guauqOU
