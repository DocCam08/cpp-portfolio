/*


CSPB 1300 Image Processing Application

*/

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
using namespace std;

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION BELOW                                    //
//***************************************************************************************************//

// Pixel structure
struct Pixel
{
    // Red, green, blue color values
    int red;
    int green;
    int blue;
};

/**
 * Gets an integer from a binary stream.
 * Helper function for read_image()
 * @param stream the stream
 * @param offset the offset at which to read the integer
 * @param bytes  the number of bytes to read
 * @return the integer starting at the given offset
 */ 
int get_int(fstream& stream, int offset, int bytes)
{
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < bytes; i++)
    {   
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}

/**
 * Reads the BMP image specified and returns the resulting image as a vector
 * @param filename BMP image filename
 * @return the image as a vector of vector of Pixels
 */
vector<vector<Pixel>> read_image(string filename)
{
    // Open the binary file
    fstream stream;
    stream.open(filename, ios::in | ios::binary);

    // Get the image properties
    int file_size = get_int(stream, 2, 4);
    int start = get_int(stream, 10, 4);
    int width = get_int(stream, 18, 4);
    int height = get_int(stream, 22, 4);
    int bits_per_pixel = get_int(stream, 28, 2);

    // Scan lines must occupy multiples of four bytes
    int scanline_size = width * (bits_per_pixel / 8);
    int padding = 0;
    if (scanline_size % 4 != 0)
    {
        padding = 4 - scanline_size % 4;
    }

    // Return empty vector if this is not a valid image
    if (file_size != start + (scanline_size + padding) * height)
    {
        return {};
    }

    // Create a vector the size of the input image
    vector<vector<Pixel>> image(height, vector<Pixel> (width));

    int pos = start;
    // For each row, starting from the last row to the first
    // Note: BMP files store pixels from bottom to top
    for (int i = height - 1; i >= 0; i--)
    {
        // For each column
        for (int j = 0; j < width; j++)
        {
            // Go to the pixel position
            stream.seekg(pos);

            // Save the pixel values to the image vector
            // Note: BMP files store pixels in blue, green, red order
            image[i][j].blue = stream.get();
            image[i][j].green = stream.get();
            image[i][j].red = stream.get();

            // We are ignoring the alpha channel if there is one

            // Advance the position to the next pixel
            pos = pos + (bits_per_pixel / 8);
        }

        // Skip the padding at the end of each row
        stream.seekg(padding, ios::cur);
        pos = pos + padding;
    }

    // Close the stream and return the image vector
    stream.close();
    return image;
}

/**
 * Sets a value to the char array starting at the offset using the size
 * specified by the bytes.
 * This is a helper function for write_image()
 * @param arr    Array to set values for
 * @param offset Starting index offset
 * @param bytes  Number of bytes to set
 * @param value  Value to set
 * @return nothing
 */
void set_bytes(unsigned char arr[], int offset, int bytes, int value)
{
    for (int i = 0; i < bytes; i++)
    {
        arr[offset+i] = (unsigned char)(value>>(i*8));
    }
}

/**
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<Pixel>>& image)
{
    // Get the image width and height in pixels
    int width_pixels = image[0].size();
    int height_pixels = image.size();

    // Calculate the width in bytes incorporating padding (4 byte alignment)
    int width_bytes = width_pixels * 3;
    int padding_bytes = 0;
    padding_bytes = (4 - width_bytes % 4) % 4;
    width_bytes = width_bytes + padding_bytes;

    // Pixel array size in bytes, including padding
    int array_bytes = width_bytes * height_pixels;

    // Open a file stream for writing to a binary file
    fstream stream;
    stream.open(filename, ios::out | ios::binary);

    // If there was a problem opening the file, return false
    if (!stream.is_open())
    {
        return false;
    }

    // Create the BMP and DIB Headers
    const int BMP_HEADER_SIZE = 14;
    const int DIB_HEADER_SIZE = 40;
    unsigned char bmp_header[BMP_HEADER_SIZE] = {0};
    unsigned char dib_header[DIB_HEADER_SIZE] = {0};

    // BMP Header
    set_bytes(bmp_header,  0, 1, 'B');              // ID field
    set_bytes(bmp_header,  1, 1, 'M');              // ID field
    set_bytes(bmp_header,  2, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE+array_bytes); // Size of BMP file
    set_bytes(bmp_header,  6, 2, 0);                // Reserved
    set_bytes(bmp_header,  8, 2, 0);                // Reserved
    set_bytes(bmp_header, 10, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE); // Pixel array offset

    // DIB Header
    set_bytes(dib_header,  0, 4, DIB_HEADER_SIZE);  // DIB header size
    set_bytes(dib_header,  4, 4, width_pixels);     // Width of bitmap in pixels
    set_bytes(dib_header,  8, 4, height_pixels);    // Height of bitmap in pixels
    set_bytes(dib_header, 12, 2, 1);                // Number of color planes
    set_bytes(dib_header, 14, 2, 24);               // Number of bits per pixel
    set_bytes(dib_header, 16, 4, 0);                // Compression method (0=BI_RGB)
    set_bytes(dib_header, 20, 4, array_bytes);      // Size of raw bitmap data (including padding)                     
    set_bytes(dib_header, 24, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 28, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 32, 4, 0);                // Number of colors in palette
    set_bytes(dib_header, 36, 4, 0);                // Number of important colors

    // Write the BMP and DIB Headers to the file
    stream.write((char*)bmp_header, sizeof(bmp_header));
    stream.write((char*)dib_header, sizeof(dib_header));

    // Initialize pixel and padding
    unsigned char pixel[3] = {0};
    unsigned char padding[3] = {0};

    // Pixel Array (Left to right, bottom to top, with padding)
    for (int h = height_pixels - 1; h >= 0; h--)
    {
        for (int w = 0; w < width_pixels; w++)
        {
            // Write the pixel (Blue, Green, Red)
            pixel[0] = image[h][w].blue;
            pixel[1] = image[h][w].green;
            pixel[2] = image[h][w].red;
            stream.write((char*)pixel, 3);
        }
        // Write the padding bytes
        stream.write((char *)padding, padding_bytes);
    }

    // Close the stream and return true
    stream.close();
    return true;
}

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION ABOVE                                    //
//***************************************************************************************************//


//************************************
//     PROCESS 1
//************************************

// Function to apply a radial darkening effect to an image based on distance from the center

vector<vector<Pixel>> process_1(const vector<vector<Pixel>>& image)
{
    // Get the number of rows (height) of the image
    int num_rows = image.size();
    
    
    // Get the number of columns (width) of the image
    int num_columns = image[0].size();
    
    
    // Create a new 2D vector of Pixels with the same dimensions as the input image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel> (num_columns));
    
    
    // Loop through each row of the image
    for (int row = 0; row < num_rows; row++)
    {
        // Loop through each column of the image
        for (int col = 0; col < num_columns; col++)
        {
            
            // Extract the red, green, and blue values of the current pixel
            int blue_color = image[row][col].blue;
            int red_color = image[row][col].red;
            int green_color = image[row][col].green;
            
            
            // Calculate the distance from the current pixel to the center of the image
            double distance = sqrt(pow(col - num_columns/2.0,2) + pow(row - num_rows/2.0,2));
            
            // Calculate a scaling factor based on the distance (closer to center = brighter)
            double scaling_factor = (num_rows - distance)/num_rows;
            
            // Scale the original color values using the scaling factor
            int newred = red_color*scaling_factor;
            int newgreen = green_color*scaling_factor;
            int newblue = blue_color*scaling_factor;
            
            // Assign the adjusted color values to the corresponding pixel in the new image
            new_image[row][col].blue = newblue;
            new_image[row][col].red = newred;
            new_image[row][col].green = newgreen;
        }
    }

    // Return the newly processed image with the radial darkening effect applied
    return new_image;
}



//************************************
//     PROCESS 2
//************************************

// Function to adjust pixel brightness based on lightness using a scaling factor
vector<vector<Pixel>> process_2(const vector<vector<Pixel>>& image, double scaling_factor)
{
    
    // Get the number of rows (i.e. height) in the input image
    int num_rows = image.size();
    
    // Get the number of columns (i.e. width) in the input image
    int num_columns = image[0].size();
    
    //double scaling_factor = 0.3;
    
    // Create a new 2D vector with the same dimensions as the input image to store the modified pixels
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel> (num_columns));
    
    
    
    // Loop through each row in the input image
    for (int row = 0; row < num_rows; row++)
    {
        // Loop through each column in the input image
        for (int col = 0; col < num_columns; col++)
        {
            // Extract the red, green, and blue components of the current pixel
            int blue_color = image[row][col].blue;
            int red_color = image[row][col].red;
            int green_color = image[row][col].green;
            
            // Compute the average brightness of the pixel
            double average_value = (blue_color + red_color + green_color) / 3.0;
            
            // Declare variables to store the modified color values
            int newred;
            int newgreen;
            int newblue;
            
            // If the pixel is bright, increase brightness further using inverse scaling
            if (average_value >= 170)
            { 
                newred = 255 - (255 - red_color) * scaling_factor;
                newgreen = 255 - (255 - green_color) * scaling_factor;
                newblue = 255 - (255 - blue_color) * scaling_factor;
            }
            
            // If the pixel is dark, darken it further using direct scaling
            else if (average_value < 90)
            {
                newred = red_color * scaling_factor;
                newgreen = green_color * scaling_factor;
                newblue = blue_color * scaling_factor;                
            }
            
            // If the pixel is neither too dark nor too bright, keep it unchanged
            else
            {
                newred = red_color;
                newgreen = green_color;
                newblue = blue_color;
            }
            
            // Assign the new color values to the corresponding pixel in the new image
            new_image[row][col].blue = newblue;
            new_image[row][col].red = newred;
            new_image[row][col].green = newgreen;
        }
    }

    // Return the modified image with adjusted brightness
    return new_image;
}


//*****************************************
//     PROCESS 3
//*****************************************

// Function to convert a color image to grayscale by averaging RGB values
vector<vector<Pixel>> process_3(const vector<vector<Pixel>>& image)
{
    // Get the number of rows (height) in the image
    int num_rows = image.size();
    
    // Get the number of columns (width) in the image
    int num_columns = image[0].size();
    
    // Create a new 2D vector to store the grayscale image, with the same dimensions as the input
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel> (num_columns));
    
    // Loop through each row of the image
    for (int row = 0; row < num_rows; row++)
    {
        // Loop through each column of the image
        for (int col = 0; col < num_columns; col++)
        {
            
            // Retrieve the red, green, and blue values of the current pixel
            int blue_color = image[row][col].blue;
            int red_color = image[row][col].red;
            int green_color = image[row][col].green;
            
            
            // Calculate the average of the RGB components to determine the gray shade
            double gray_color = (red_color + green_color + blue_color) / 3.0;
            
            // Assign the same gray value to each RGB component to create a grayscale pixel
            int newred = gray_color;
            int newgreen = gray_color;
            int newblue = gray_color;
            
            // Set the grayscale pixel values in the corresponding position of the new image
            new_image[row][col].blue = newblue;
            new_image[row][col].red = newred;
            new_image[row][col].green = newgreen;
        }
    }

    // Return the newly created grayscale image
    return new_image;
}


//*****************************************
//     PROCESS 4
//*****************************************

// Function to rotate an image 90 degrees clockwise
vector<vector<Pixel>> process_4(const vector<vector<Pixel>>& image)
{
    
    // Get the number of rows (height) in the original image
    int num_rows = image.size();
    
    // Get the number of columns (width) in the original image
    int num_columns = image[0].size();

   
    // Create a new image with swapped dimensions (width becomes height, height becomes width)
    vector<vector<Pixel>> new_image(num_columns, vector<Pixel>(num_rows));

    
    // Loop over each pixel in the original image
    for (int row = 0; row < num_rows; row++)
    {
        
        for (int col = 0; col < num_columns; col++)
        {
            // Assign the pixel from (row, col) in the original image to (col, num_rows - 1 - row) in the new image
            // This achieves a 90-degree clockwise rotation
            new_image[col][num_rows - 1 - row] = image[row][col];
        }
    }
    
    // Return the rotated image
    return new_image;
}


//*****************************************
//     PROCESS 5
//*****************************************

// Function to rotate an image by 90-degree increments based on the input number
vector<vector<Pixel>> process_5(const vector<vector<Pixel>>& image, int number)
{
    // Calculate the rotation angle as a multiple of 90 degrees
    int angle = number * 90;

    // Check if the angle is not a multiple of 90 (invalid input)    
    if (angle % 90 != 0)
    {
        // Inform the user of invalid input
        cout << "angle must be a multiple of 90 degrees." << endl;

        // Return the original image unmodified
        return image; // Return unmodified image on error
    }

    // If angle is a full rotation (0 or 360, 720, etc.), return image unchanged
    else if (angle % 360 == 0)
    {
        return image;
    }

    // If angle is 90 degrees, rotate once
    else if (angle % 360 == 90)
    {
        return process_4(image);
    }

    // If angle is 180 degrees, rotate twice
    else if (angle % 360 == 180)
    {
        return process_4(process_4(image));
    }

    // If angle is 270 degrees, rotate three times
    else
    {
        return process_4(process_4(process_4(image)));
    }
}


//*****************************************
//     PROCESS 6
//*****************************************

// Function to scale an image by repeating pixels based on x and y scaling factors
vector<vector<Pixel>> process_6(const vector<vector<Pixel>>& image, int x_scale, int y_scale)
{
    
    // Get the number of rows (height) in the original image
    int num_rows = image.size();
    
    // Get the number of columns (width) in the original image
    int num_columns = image[0].size();
    
    // Calculate the new height of the image after vertical scaling
    int new_rows = y_scale * num_rows;
    
    // Calculate the new width of the image after horizontal scaling
    int new_columns = x_scale * num_columns;

    
    // Create a new image with the scaled dimensions, filled with default Pixel values
    vector<vector<Pixel>> new_image(new_rows, vector<Pixel>(new_columns));

    // Loop over each pixel in the new (scaled) image
    for (int row = 0; row < new_rows; row++)
    {
        for (int col = 0; col < new_columns; col++)
        {
            // Determine the corresponding row in the original image
            int source_row = (row / y_scale);
            
            // Determine the corresponding column in the original image
            int source_col = (col / x_scale);
            
            // Copy the pixel from the original image to the scaled image
            new_image[row][col] = image[source_row][source_col];
        }
    }

    // Return the scaled image
    return new_image;
}

  

//*****************************************
//     PROCESS 7
//*****************************************

// Function to apply a black-and-white threshold filter to an image
vector<vector<Pixel>> process_7(const vector<vector<Pixel>>& image) 
{
    // Get the number of rows (height) in the original image
    int num_rows = image.size();
    
    // Get the number of columns (width) in the original image
    int num_columns = image[0].size();
    
    // Create a new image of the same dimensions to store the output
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));

    // Loop through each row of the image
    for (int row = 0; row < num_rows; row++)
    {
        // Loop through each column of the image
        for (int col = 0; col < num_columns; col++)
        {
            
            // Get the red, green, and blue values of the current pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;

            
            // Calculate the average of the color values to get a grayscale value
            int gray_value = (red_value + green_value + blue_value) / 3;

            
            int new_red;
            int new_green;
            int new_blue;

            // If the grayscale value is 128 or more, set the pixel to white
            if (gray_value >= 255 / 2)
            {
                new_red = 255;
                new_green = 255;
                new_blue = 255;
            }
            
            // Otherwise, set the pixel to black
            else
            {
                new_red = 0;
                new_green = 0;
                new_blue = 0;
            }

            // Create a new Pixel object with the updated color values
            Pixel new_pixel;
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;

            // Assign the new pixel to the corresponding position in the output image
            new_image[row][col] = new_pixel;
        }
    }

    // Return the processed black-and-white image
    return new_image;
}


//*****************************************
//     PROCESS 8
//*****************************************

// Function to brighten an image using a given scaling factor
vector<vector<Pixel>> process_8(const vector<vector<Pixel>>& image, double scaling_factor) 
{
    
    
    // Get the number of rows (height) in the original image
    int num_rows = image.size();
    
    // Get the number of columns (width) in the original image
    int num_columns = image[0].size();     

    // Initialize a new image with the same dimensions as the original
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns)); 

    
    // Loop through each row in the image
    for (int row = 0; row < num_rows; row++) 
    {
        // Loop through each column in the image
        for (int col = 0; col < num_columns; col++) 
        {
            
            // Extract the red, green, and blue color components from the current pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;

            
            // Brighten each color channel using the formula: 255 - (255 - original) * factor
            int new_red = 255 - (255 - red_value) * scaling_factor;
            int new_green = 255 - (255 - green_value) * scaling_factor;
            int new_blue = 255 - (255 - blue_value) * scaling_factor;

            // Create a new Pixel object and assign the updated color values
            Pixel new_pixel;
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;

            // Store the new pixel in the corresponding position of the output image
            new_image[row][col] = new_pixel;
        }
    }

    // Return the final brightened image
    return new_image;
}


//*****************************************
//     PROCESS 9
//*****************************************

// Function to adjust the brightness or darkness of an image by scaling each pixel's color values
vector<vector<Pixel>> process_9(const vector<vector<Pixel>>& image, double scaling_factor)
{
    // Get the number of rows (height) of the original image
    int num_rows = image.size();
    
    // Get the number of columns (width) of the original image
    int num_columns = image[0].size();

    
    // Create a new image with the same dimensions as the original image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));

    // Loop through each row of the original image
    for (int row = 0; row < num_rows; row++) 
    {
        // Loop through each column of the current row
        for (int col = 0; col < num_columns; col++) 
        {
            // Retrieve the red, green, and blue values from the current pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;

            // Multiply each color value by the scaling factor
            int new_red = red_value * scaling_factor;
            int new_green = green_value * scaling_factor;
            int new_blue = blue_value * scaling_factor;

            // Clamp the red value between 0 and 255 to prevent overflow or underflow
            if (new_red > 255) new_red = 255;
            if (new_red < 0) new_red = 0;

            // Clamp the green value between 0 and 255
            if (new_green > 255) new_green = 255;
            if (new_green < 0) new_green = 0;
            
            // Clamp the blue value between 0 and 255
            if (new_blue > 255) new_blue = 255;
            if (new_blue < 0) new_blue = 0;

            // Create a new pixel and assign the adjusted color values
            Pixel new_pixel;
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
            
            // Place the new pixel in the same position in the output image
            new_image[row][col] = new_pixel;
        }
    }

    // Return the new image with brightness/darkness adjusted
    return new_image;
}


//*****************************************
//     PROCESS 10
//*****************************************

// Function that transforms the image into a high-contrast, color-dominance-based version
vector<vector<Pixel>> process_10(const vector<vector<Pixel>>& image) 
{
    // Get the number of rows (height) of the original image
    int num_rows = image.size();
    
    // Get the number of columns (width) of the original image
    int num_columns = image[0].size();

    // Create a new image with the same dimensions as the original
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));

    // Loop through each row of the image
    for (int row = 0; row < num_rows; row++) 
    {
        // Loop through each column in the current row
        for (int col = 0; col < num_columns; col++) 
        {
            // Get the red, green, and blue values of the current pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;

            // Initialize max_color with red and find the highest color value among R, G, B
            int max_color = red_value;
            if (green_value > max_color) {
                max_color = green_value;
            }
            if (blue_value > max_color) {
                max_color = blue_value;
            }

            // Variables to hold the new RGB values
            int new_red;
            int new_green;
            int new_blue;

            
            // Apply color transformation based on brightness and color dominance

            // If the pixel is very bright, turn it white
            if (red_value + green_value + blue_value >= 550) {
                new_red = 255;
                new_green = 255;
                new_blue = 255;
            }
            
            // If the pixel is very dark, turn it black
            else if (red_value + green_value + blue_value <= 150) {
                new_red = 0;
                new_green = 0;
                new_blue = 0;
            }
            
            // If red is the dominant color, turn it pure red
            else if (max_color == red_value) {
                new_red = 255;
                new_green = 0;
                new_blue = 0;
            }
            
            // If green is the dominant color, turn it pure green
            else if (max_color == green_value) {
                new_red = 0;
                new_green = 255;
                new_blue = 0;
            }
            
            // Otherwise, turn it pure blue
            else {
                new_red = 0;
                new_green = 0;
                new_blue = 255;
            }

            // Create a new pixel with the transformed color values
            Pixel new_pixel;
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
            
            // Place the new pixel into the corresponding location in the new image
            new_image[row][col] = new_pixel;
        }
    }

    // Return the transformed image
    return new_image;
}


int main()
{
    // Welcome message
    cout << endl;
    cout << endl;
    cout << "Welcome to my CSPB 1300 Image Processing Application" << endl;
    cout << endl;
    
    // Prompt user for input filename
    cout << "Please enter input BMP filename: ";
    
    
    // Variable to store filename
    string filename;
    
    // Read filename from user input
    cin >> filename;                                         

    // Confirm filename is saved
    cout << "BMP filename saved.\n";
    cout << endl;
    cout << endl;

    // Read the BMP image and store it in a 2D vector of Pixel objects
    vector<vector<Pixel>> image = read_image(filename);
    
    // Variable to store user menu selection
    string selection; 
    
 
 // While loop for image processing menu   
 while (true)
    {
        // Display image processing menu options
        cout << endl;
        cout << "******************************* \n";
        cout << "*                             * \n";
        cout << "*    IMAGE PROCESSING MENU    * \n";
        cout << "*                             * \n";
        cout << "******************************* \n";
        cout << endl;
        cout << endl;
        cout << "0) Change image (current: " << filename << ") \n";
        cout << "1) Vignette \n";
        cout << "2) Clarendon \n";
        cout << "3) Grayscale \n";
        cout << "4) Rotate 90 degrees \n";
        cout << "5) Rotate multiple 90 degrees \n";
        cout << "6) Enlarge \n";
        cout << "7) High contrast \n";
        cout << "8) Lighten \n";
        cout << "9) Darken \n";
        cout << "10) Black, white, red, green, blue" << endl;

     
        cout << endl;
        
        //prompt for user selection
        cout << "Please enter a menu selection option (Q to quit): ";
        cin >> selection;
        cout << endl;

        // Exit program if user enters 'Q' or 'q'
        if (selection == "Q" || selection == "q")
        {
            cout << "Thanks for using my program!" << endl;
            cout << "Quitting... \n";
            cout << endl;
            break;
        }
     
        // Option 0: Load a new image file
        else if (selection == "0")
        {
            cout << "Please enter your new image filename: ";
            cin >> filename;
            cout << "Your BMP filename has been saved.";
            cout << endl;
            continue;
        }
     
        // Sets condition to selection == "1", if met program applies vignette
        else if (selection == "1")
        {
            cout << "You have selected the Vignette filter. \n";
            cout << endl;
            
            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename;
            cout << endl;
            
            vector<vector<Pixel>> process1 = process_1(image);
            write_image(out_filename, process1);
            
            cout << endl;
            cout << "The Vignette filter has been successfully applied to your image and saved as " << out_filename << "!\n";
            cout << endl;
        }
     
        // Sets condition to selection == "2", if met program applies clarendon
        else if (selection == "2")
        {
           cout << "You have selected the Clarendon filter. \n";
           cout << endl; 
           double scaling_factor;

           // Prompt for valid scaling factor input
           while (true)
           {
           cout << "Please enter a scaling factor value: ";
           cin >> scaling_factor;
           cout << endl;
                // Handle invalid input
                if (cin.fail())
                {
                cin.clear();  // Clears error flag
                cin.ignore(10000, '\n');  // Has program ignore previously entered input value provided by user
                }
               
                else
                {
                     break;
                }
            }
            
            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename;
            cout << endl;
            
            vector<vector<Pixel>> process2 = process_2(image, scaling_factor);
            write_image(out_filename, process2);
            
            cout << endl;
            cout << "The Clarendon filter has been successfully applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
          } 
     
        // Sets condition to selection == "3", if met program applies grayscale
        else if (selection == "3") 
        {
            cout << "You have selected the Grayscale filter. \n";
            cout << endl;
            
            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename;
            cout << endl;
            
            vector<vector<Pixel>> process3 = process_3(image);
            write_image(out_filename, process3);
            
            cout << endl;
            cout << "The Grayscale filter has been successfully applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
        }
     
        // Sets the condition to  selection == "4", if met program applies the rotate 90 degrees clockwise function
        else if (selection == "4") 
        {
            cout << "You have selected the Rotate 90 Degrees Clockwise filter. \n";
            cout << endl;
            
            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename;
            cout << endl;
            
            vector<vector<Pixel>> process4 = process_4(image); 
            write_image(out_filename, process4);
            
            cout << endl;
            cout << "The 90 Degree Rotation Clockwise filter has been successfully applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
        }
     
        // Sets condition to selection == "5", if met program rotates image 90 degrees clockwise (user input number) of times
        else if (selection == "5") 
        {
            cout << "You have selected the Rotate multiple 90 degrees filter. \n";
            cout << endl;
            
            int number;
            
           // Validate input for number of rotations 
           while (true)
           {
            cout << "Please enter the number of 90 degree rotations you'd like applied: ";
            cin >> number;
            
            // Handle invalid input
            if (cin.fail())
            {
                cin.clear();  // Clears error flag
                cin.ignore(10000, '\n');  // Has program ignore previously entered input value provided by user
             }
               
               
             else if (number < 0)
             {
                 cout << "Please enter positive number \n";
                 cout << endl;
             }
               
               
             else
             {
                     break;
             }
           }
            cout << endl;
            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename; 
            cout << endl;
            
            vector<vector<Pixel>> process5 = process_5(image, number);
            write_image(out_filename, process5);
            cout << endl;
            cout << "The Multiple 90 Degree Rotations filter has successfully been applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
        }
     
        // Sets condition to selection == "6", if met program applies enlarge filter
        else if (selection == "6") 
        {
            cout << "You have selected the Enlarge filter. \n";
            cout << endl;
            int x_scale;
            int y_scale;
            
            // Input and validate x scaling factor
            while (true)
           {
            cout << "Please enter an X scale value: ";
            cin >> x_scale;
            cout << endl;
            // Handle invalid input
            if (cin.fail())
            {
                cin.clear();  // Clears error flag
                cin.ignore(10000, '\n');  // Has program ignore previously entered input value provided by user
             }
                
             // if user enters a value less than 0, program prompts user to enter a positive number value 
             else if (x_scale < 0) 
             {
                 cout << "Please enter a positive number. \n";
                 cout << endl;
             }
                
                
             else
             {
                break;
             }
            }
            
            // Input and validate y scaling factor
            while (true)
           {
            // Prompts user to input a value for y scale
            cout << "Please enter a Y scale value: ";
            cin >> y_scale;
            cout << endl;
            
            // Handle invalid input
            if (cin.fail())
            {
                cin.clear();  // Clears error flag
                cin.ignore(10000, '\n');  // Has program ignore previously entered input value provided by user
             }
                
             // If user inputs a value for y that is less than 0, it prompts user for a positive number value   
             else if (y_scale < 0) 
             {
                 cout << "Please enter positive number. \n";
                 cout << endl;
             }
                
                
             else
             {
                break;
             }
            } 
            
            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename;
            cout<< endl;
            
            vector<vector<Pixel>> process6 = process_6(image, x_scale, y_scale);
            write_image(out_filename, process6);
            
            cout << "The Enlarged filter has been successfully applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
        }
     
        // Sets condition to selection == "7", if met program applies high contrast filter to image
        else if (selection == "7")
        {
            cout << "You have selected the High Contrast filter. \n";
            cout << endl;

            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename;
            cout << endl;
            
            vector<vector<Pixel>> process7 = process_7(image);
            write_image(out_filename, process7);
            
            cout << "The High Contrast filter has been successfully applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
        }
     
        // Sets condition to selection == "8", if met program applies lightened filter to image
        else if (selection == "8")
        {
            cout << "You have selected the Lighten filter. \n";
            cout << endl;

            double scaling_factor;            
            
            
           while (true)
           {
            cout << "Please enter a scaling factor value: ";
            cin >> scaling_factor;
            cout << endl;
            
                // Handle invalid input
                if (cin.fail())
                {
                    cin.clear();  // Clears error flag
                    cin.ignore(10000, '\n');  // Has program ignore previously entered input value provided by user
                }
               
                else
                {
                     break;
                }
            }
            
            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename;             
            cout << endl;
            
            vector<vector<Pixel>> process8 = process_8(image, scaling_factor);
            write_image(out_filename, process8);
            
            cout << "The Lighten filter has been successfully applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
        }
     
        // Sets condition to selection == "9", if met program applies darkened filter to image
        else if (selection == "9")
        {
            cout << "You have selected the Darken filter. \n";
            cout << endl;
            double scaling_factor;

            
           while (true)
           {
            cout << "Please enter scaling factor: ";
            cin >> scaling_factor;
            cout << endl;
                // Handle invalid input
                if (cin.fail())
                {
                    cin.clear();  // Clears error flag
                    cin.ignore(10000, '\n');  // Has program ignore previously entered input value provided by user
                }
               
               
                else if (scaling_factor > 1)
                {
                 cout << "Please enter a value between 0.1 and 1.\n";
                 cout << endl;
                }
               
               
                else
                {
                     break;
                }
               
           }            
            
            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename; 
            cout << endl;
            
            vector<vector<Pixel>> process9 = process_9(image, scaling_factor);
            write_image(out_filename, process9);
            
            cout << endl;
            cout << "The Darken filter has been successfully applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
        }
     
        //Sets condition to selection == "10", if met program applies black, white, red, green, blue filter to image
        else if (selection == "10")
        {
            cout << "You have selected the Black, White, Red, Green, Blue filter. \n";
            cout << endl;

            string out_filename;
            cout << "Please enter a unique file name to save the new image (Be sure to include .bmp at the end of your new file name): ";
            cin >> out_filename;
            cout <<endl;
            
            vector<vector<Pixel>> process10 = process_10(image); 
            write_image(out_filename, process10);
            
            cout << endl;
            cout << "The Black, White, Red, Green, Blue filter has been successfully applied to your image and has been saved as " << out_filename << "! \n";
            cout << endl;
        }    
        
     
        else
        {
            cout << "Please enter valid menu selection" << endl;
            cin.clear();
            continue;
        }
     }

    return 0;
}