# bmp-2-ascii

bmp to ascii converter using low-level, mathematical operations on the pixels of the bmp themselves - no external libraries beyond the standard ones.

---

hadn’t yet seen an image -> ascii converter that attempted to recreate that “homemade” ASCII look rather than replacing pixels with different characters based on lightness/darkness. this script is a quick and early attempt to take the concept one step further by pixelating the image + averaging the rgb values and manually implementing the sobel operator to identify lines - then taking the strength and direction of the calculated Gx and Gy into account to pick the most appropriate two characters to print said lines. 

ex: ./bmp-2-ascii blackbuck.bmp

Input:

<img width="200" src="https://github.com/big-evil-fish/bmp-2-ascii/assets/134818415/07af9ebd-fb78-4fa0-af8b-1ee533162faa">



Output (in terminal):

<img width="200" alt="Screenshot 2023-07-30 at 8 53 04 PM" src="https://github.com/big-evil-fish/bmp-2-ascii/assets/134818415/2272b804-6983-46e3-b1aa-f2a6dcc662f2">



---
level of detail can be changed by allowing for pixels with a lower sobel value to print ( line 199 )
the previous output had a threshold value of 250, and while i prefer that look for this specific image, 175 (default) is generally better in my experience:

<img width="200" alt="Screenshot 2023-07-31 at 9 54 47 PM" src="https://github.com/big-evil-fish/bmp-2-ascii/assets/134818415/4383fdc2-2845-4dcf-9685-c815cbbedcff">

results are generally a bit abstract but i like to think that’s the nature of ascii.

currently this script can only manipulate uncompressed, 24-bit bmps due to their structure, however I’ve had luck with https://online-converting.com/image/convert2bmp/ for converting any basic image file to a 24-bit bmp. credit to david malan for the script that processes the bmp file header and reads the pixels of the the original .bmp into an array.

not very complicated but i was surprised to see it hadn’t been done before. i would love to see anyone expand on this idea ( perhaps adding support for other file types? ).

