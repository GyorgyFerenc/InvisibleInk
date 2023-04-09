# Invisible Ink

A simple program which allows you to encrypt data into images.
Supported image formats:
- PNG with RGBA 8bit depth encoding.

## Usage

### Encryption

```
./invisible_ink -e -k key.png -i data.txt -o encrypted.png
```
Flag -e tells the program that we are encrypting.  
Flag -k specifies the image that will be the key of the encryption.  
Flag -i specifies the file that holds data to be encrypted.  
Flag -o specifies the name of the image to be generated.  

### Decription

```
./invisible_ink -d -k key.png -i encrypted.png -o data.txt
```
Flag -d tells the program that we are decrypting.  
Flag -k specifies the image that will be the key of the decryption.  
Flag -i specifies the image which holds the encrypted data.  
Flag -o specifies the name of file where the data will be decrypted to.  

## Compilation

### Linux

```
./compile_release.sh
```
Run the script to compile 

```
./compile_and_run_tests.sh 
```
Run the script to test 

## Dependencies

[LodePNG](https://github.com/lvandeve/lodepng) for working with PNG.

## Examples

To look at the examples, run:

```
./compile_release.sh
cd examples
../build/invisible_ink -k key.png -i example1.png -o example1.txt
../build/invisible_ink -k key.png -i example2.png -o example2.txt
../build/invisible_ink -k key.png -i example3.png -o example3.txt
```

This will decrpyt the 3 examples into txt files, using key.png as the key.

## Specification

first 4 byte is lenght of encrypted message, then lenght number of bytes
