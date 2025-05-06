# 🖼️ Image Processing Application

Welcome to the **CSPB 1300 Image Processing Application**!  
This C++ command-line tool allows you to apply a variety of visual transformations to `.bmp` image files.  
With 10 distinct image processing functions, this program serves as a simple but effective way to experiment with image manipulation techniques.

---

## 📸 Features

The program supports the following 10 image processing options:

1. **Vignette** – Darkens the corners of the image to focus on the center.  
2. **Clarendon Filter** – Applies a contrast-based filter with a user-defined scaling factor.  
3. **Grayscale** – Converts the image to grayscale by averaging RGB values.  
4. **Rotate 90 Degrees Clockwise** – Rotates the image once clockwise.  
5. **Rotate Multiple Times** – Rotates the image multiple times in 90-degree increments (user-defined count).  
6. **Enlarge** – Enlarges a selected quadrant of the image to fill the whole canvas.  
7. **High Contrast** – Enhances contrast by setting pixel colors to black or white depending on brightness.  
8. **Lighten** – Increases the brightness of all pixels in the image.  
9. **Darken** – Decreases the brightness of all pixels in the image.  
10. **Color Dominance Filter** – Maps each pixel to pure red, green, blue, white, or black based on brightness and dominant color.

---

## 🛠️ How to Use

**Compile the program** using a C++ compiler that supports C++11 or later:

```bash
g++ -std=c++11 -o image_processor main.cpp
Run the program:

bash
Copy
Edit
./image_processor
Follow the prompts:

Enter the filename of the input .bmp file (must be a 24-bit BMP).

Select from the menu of 10 available filters.

Provide any additional inputs requested (e.g., rotation count or scaling factor).

Choose a unique name for the output file to save your modified image.

🖼️ Example Flow
plaintext
Copy
Edit
Welcome to my CSPB 1300 Image Processing Application

Please enter input BMP filename: sample.bmp
BMP filename saved.

*******************************
*                             *
*    IMAGE PROCESSING MENU    *
*                             *
*******************************

0) Change image (current: sample.bmp)
1) Vignette
2) Clarendon
3) Grayscale
...
10) Black, white, red, green, blue

Please enter a menu selection option (Q to quit): 3

You have selected the Grayscale filter.
Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): sample_gray.bmp

The Grayscale filter has been successfully applied to your image and has been saved as sample_gray.bmp!
🧱 Requirements
C++11 or later

24-bit BMP files only

Custom Pixel structure and image I/O functions:

read_image()

write_image()

📂 Suggested File Structure
plaintext
Copy
Edit
project_folder/
├── main.cpp              # Main program and processing functions
├── image_io.cpp/h        # Image reading and writing utilities
├── process_functions.cpp # Your 10 processing functions
└── README.md             # This documentation
📌 Notes
Ensure output filenames are unique and include the .bmp extension.

User input is validated for numeric options where necessary.

Output images are saved in the same directory unless otherwise specified.

📄 License
This project is developed as part of coursework or personal practice and is available for educational and non-commercial use.
