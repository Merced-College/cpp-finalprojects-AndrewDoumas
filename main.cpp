#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <iterator> // Added for istreambuf_iterator
#include <sstream>
#include <zlib.h>
#include <algorithm>

using namespace std;


string symbolArray[] = {" ", ".", ":", "-", "=", "+", "*", "#","P", "@", "█"};
char hexcodes[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};


array<string, 8> png_signature{"89", "50", "4e", "47", "0d", "0a", "1a", "0a"};
vector<string> IHDR_signature;
vector<string> PLTE_signature;
vector<string> IDAT_signature;
vector<string> IEND_signature;
vector<string> idat_chunk_data;


struct RGB {
    int red;
    int green;
    int blue;
};



int png_check(vector<string> hexadecimal_values){

    for(int i = 0; i<8;i++){
        if(hexadecimal_values[i] != png_signature[i]){
            return -1;
        }
    }

    return 0;
}


//check IHDR chunk data in first 4 bytes after PNG signature
void chunk_check(vector<string> hexadecimal_values, int starting_spot){
    string chunk_length ="";
    string chunk_type ="";
    //int length = 12;
    
    

    for(int i = starting_spot; i<4;i++){
        chunk_length += hexadecimal_values[i];
    }
}

// Luminance = 0.2126 * R + 0.7152 * G + 0.0722 * B

void rgb_luminance(vector<string> png_colors, vector<double>& png_luminance, vector<RGB>& rgb_values){
    string r = "";
    string g = "";
    string b = "";
    double luminance = 0.0, red_value, green_value, blue_value;

    for(int i = 0; i<png_colors.size();i++){
        r = "";
        g = "";
        b = "";
        
        r += png_colors[i][0];
        r += png_colors[i][1];
        g += png_colors[i][2];
        g += png_colors[i][3];
        b += png_colors[i][4];
        b += png_colors[i][5];

        //cout<< "R: "<< r << " G: "<< g << " B: "<< b << endl;
        
        rgb_values[i].red = stoi(r,0,16);
        rgb_values[i].green = stoi(g,0,16);
        rgb_values[i].blue = stoi(b,0,16);

        
        red_value = (stoi(r, 0, 16))/255.0;
        green_value = (stoi(g, 0, 16))/255.0;
        blue_value = (stoi(b, 0, 16))/255.0;

        //cout<< "R: "<< red_value << " G: "<< green_value << " B: "<< blue_value;
        
        luminance = (0.2126*red_value) + (0.7152 *green_value)+ (0.0722*blue_value);
        png_luminance.push_back(luminance);
    }
    
}

string rgb_to_hex(int red, int green, int blue){
    string hexcode = "";
     
    int temp = red;
    int first = temp / 16;
    int second = temp % 16;

    hexcode += hexcodes[first];
    hexcode += hexcodes[second];

    temp = green;
    first = temp / 16;
    second = temp % 16;

    hexcode += hexcodes[first];
    hexcode += hexcodes[second];

    temp = blue;
    first = temp / 16;
    second = temp % 16;

    hexcode += hexcodes[first];
    hexcode += hexcodes[second];
    
    


    return hexcode;
}

double mean_pixel_average_luminance(double lum_to_ascii[8][8]){
    double mode;
    int counter = 0;

    for(int i = 0; i<8; i++){
        for(int j = 0; j<8; j++){
            mode += lum_to_ascii[i][j];
            
        }
    }
    mode /=64;
    
    
    return mode;
}



int main() {
    string filename;
    
    cout << "Please enter the file name: " << endl;
    getline(cin, filename);

    // Open file in binary mode
    //ifstream infile(filename, ios::binary); //Using filename from user input
   ifstream infile("heart.png", ios::binary);

    if (infile.fail()) {
        cout << "Error opening file" << endl;
        return 1;
    }

    // Read file into vector
    vector<unsigned char> buffer(
        (istreambuf_iterator<char>(infile)),
        (istreambuf_iterator<char>())
    );
    infile.close();

    vector<string> hexadecimal_values;
    string hex_value = "";



    //89 50 4e 47 0d 0a 1a 0a
    //89 50 4e 47 d a 1a a  what actually gets captured
    for(int i = 0; i < buffer.size(); i++){
        std::stringstream ss;
        ss << hex << static_cast<int>(buffer[i]) ;
        hex_value = ss.str();
        

        if(hex_value.length() < 2){
            hex_value = "0" + hex_value;
        }
        hexadecimal_values.push_back(hex_value);
    }
    

    //cout<<"\n\n\nhex value test:\n" <<endl;
    // Print hexadecimal values
    int i = 1;
    for (const auto& hex_value : hexadecimal_values){
        
        //cout << hex_value << " ";
        // if(i == 8){
        //     cout << endl;
        //     cout << endl;
        // }


        /*
        
        if(i == 8){
            cout<<endl;
            cout<<endl;
        }
        if(i==33){
            cout<<endl;
            cout<<endl;
        }
        if(i==348){
            cout<<endl;
            cout<<endl;
        }
        if(i==2547){
            cout<<endl;
            cout<<endl;
        }

        */

        
        // if(i % 8 == 0){
            
        //     cout << endl;
        // }
        i++;
    }

    // Extract width and height from IHDR chunk
    int width = stoi(hexadecimal_values[16] + hexadecimal_values[17] + hexadecimal_values[18] + hexadecimal_values[19], nullptr, 16);
    int height = stoi(hexadecimal_values[20] + hexadecimal_values[21] + hexadecimal_values[22] + hexadecimal_values[23], nullptr, 16);
    
    if(png_check(hexadecimal_values) == -1){
        cout << "Not a PNG file" << endl;
        return 1;
    }
    cout << "Valid PNG file" << endl;
    cout << "Width: " << width << ", Height: " << height << endl;

    //chunk checking function
    chunk_check(hexadecimal_values, 8);
    
    // Create a 2D array based on the height and width of the PNG
    vector<vector<int>> pixelMatrix(height, vector<int>(width));

    
    
    
    // Print out the data read from the PNG file

    //
    //cout << endl;
    //cout<< "Color Pallete: " << endl;
    string color_holder = "";
    //int counter = 1;
    int j = 0;
    vector<string> png_colors;
    vector<double> png_luminance;

    
    for(int i = 1; i <=303; i++){
        color_holder += hexadecimal_values[i+40];
        if(color_holder.length()==6){
            png_colors.push_back(color_holder);
            color_holder = "";
        }
        if(i%3 == 0){
            
            //cout <<j+1<<": #"<<png_colors.at(j)<<endl;
            j++;
            
        }
    }

    //convert hex values into RGB then convert to luminence
    vector<RGB> rgb_values(png_colors.size());
    
    rgb_luminance(png_colors, png_luminance, rgb_values);
    
    // Fill the pixelMatrix with RGB values
    for(int i = 0; i < rgb_values.size(); i++) {
        int row = i / width;
        int col = i % width;
        if(row < height && col < width) {
            // Combine RGB values into a single integer
            pixelMatrix[row][col] = (rgb_values[i].red << 16) | (rgb_values[i].green << 8) | rgb_values[i].blue;
        }
    }


    
    //print out luminance values
    //cout<< "Luminance Values: " << endl;
    for(int i = 0; i<png_luminance.size();i++){
        //cout<<png_luminance.at(i)<<" ";
        //cout<<i <<":  "<< rgb_values[i].red << " " << rgb_values[i].green << " " << rgb_values[i].blue << endl;
        // if(i%10 == 0){
        //     cout<<endl;
        // }
        
    }

    for(int i = 0; i< 2187; i++){
        idat_chunk_data.push_back(hexadecimal_values[i+356]);
    }

    //cout<< "IDAT chunk data: " << endl;
    for(int i = 0; i<idat_chunk_data.size();i++){
        //cout<<idat_chunk_data.at(i)<<" ";
        if(i%10 == 0){
            //cout<<endl;
        }
    }
    //cout<<endl;


// made by AI
    
    // Decompress IDAT chunk data
    vector<unsigned char> decompressed_data;
    uLongf decompressed_size = width * height * 4; // Assuming RGBA data
    decompressed_data.resize(decompressed_size);

    // Assuming "idat_chunk_data" is already extracted from the PNG file


    vector<unsigned char> idat_raw_data;
    for (const auto& hex : idat_chunk_data) {
        int byte;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> byte;
        idat_raw_data.push_back(static_cast<unsigned char>(byte));
    }

    if (uncompress(decompressed_data.data(), &decompressed_size, reinterpret_cast<const Bytef*>(idat_raw_data.data()), idat_raw_data.size()) != Z_OK) {
        cout << "Failed to decompress IDAT chunk data." << endl;
        return 1;
    }
    cout << "IDAT chunk data successfully decompressed." << endl;

    int counter = 0;
    // Create 2D vector to store the image pixels
    vector<vector<RGB>> image(height, vector<RGB>(width));
    
    // Map decompressed data to colors
    size_t pixel_index = 0;
    for(size_t y = 0; y < height; y++) {
        // Skip the filter byte at the start of each row
        pixel_index++;
        
        for(size_t x = 0; x < width; x++) {
            if(pixel_index < decompressed_data.size()) {
                // Get palette index from decompressed data
                unsigned char palette_index = decompressed_data[pixel_index];
                
                // Map to RGB color if index is valid
                if(palette_index < rgb_values.size()) {
                    image[y][x] = rgb_values[palette_index];
                }
            }
            pixel_index++;
        }
    }

    // Print first few pixels for verification
    //cout << "The first few pixels RGB values are:" << endl;
    for(size_t y = 100; y < 103 && y < height; y++) {
        //cout << "\nRow " << y << ": "<<endl;
        //cout<<"RGB: "<<endl;
        for(size_t x = 0; x < 214 && x < width; x++) {
            //cout << "RGB(" << image[y][x].red << ", " << image[y][x].green << ", " << image[y][x].blue << ") ";
        }
        //cout << "\n";
    }

    double luminance_storage[height][width];
    string hex_color[height][width];
    
    cout << "Image dimensions: "
         << image.size() << " rows by "
         << (image.empty() ? 0 : image[0].size()) << " columns" << endl;
//AI made ends    
    double lumi;
    
    for(int j = 0; j < image.size(); j++) {
        //cout << "Row " << j << ":\n";
        for(int i = 0; i < image[0].size(); i++) {
            lumi = (image[j][i].red * 0.2126) + (image[j][i].green * 0.7152) + (image[j][i].blue * 0.0722);
            lumi = lumi / 255.0;
            luminance_storage[j][i] = lumi;
            //cout << "Pixel[" << j << "][" << i << "] Luminance: " << lumi << "\n";
            hex_color[j][i] = rgb_to_hex(image[j][i].red, image[j][i].green, image[j][i].blue); 
        }
    }

    
    int lumi_index = 0;
    for(int j = 0; j < height; j++){
        //cout<<endl;
        //cout<<"Row "<<j<<endl;
        
        for(int i = 0; i < width; i++){

            //hex_color[j][i] = rgb_to_hex(luminance_storage[j][i]);
            lumi_index = luminance_storage[j][i] * 10;
            //cout<<lumi_index<<" ";
            //cout<< luminance_storage[j][i] <<" ";
            if(i%236 == 0 && i>0){
                //cout<<i<< ": ";
                //cout<<endl;
            }
            //cout<< hex_color[j][i]<<" ";
            //cout<< luminance_storage[j][i] <<" ";
        }
    }

    //make 8X8 grid for average luminance values
    string ascii_art[height/8][width/8];
    string ascii_art2[height/8][width/8];
    double average_luminance[height/8][width/8];
    double lum_to_ascii[8][8];

    //terribly overnested loops?
    for(int j = 0; j < height/8; j++){
        //cout<<"Row "<<j<<endl;
        for(int i = 0; i < width/8; i++){
            //cout<<endl;
            for(int y = j*8; y < j*8+8; y++){
                for(int x = i*8; x < i*8+8; x++){
                    lum_to_ascii[y%8][x%8] = luminance_storage[y][x]; // do i want [y*8] instead?
                    //cout<<lum_to_ascii[y%8][x%8];
                    
                }
                //cout<<endl;
            }
            //cin>> filename;
            
            average_luminance[j][i] = mean_pixel_average_luminance(lum_to_ascii);
            //cout<<average_luminance[j][i]<<" ";
            ascii_art[j][i] = symbolArray[(int)(average_luminance[j][i]*9)];
            ascii_art2[j][i] = symbolArray[(int)(average_luminance[j][i]*10)];
        }
        //cout<<"\n\n"<<endl;
    }
    system("clear");
    cout<<endl;

    for(int j = 0; j < height/8; j++){
        for(int i = 0; i < width/8; i++){
            cout<<ascii_art[j][i];
        }
        for(int h = 0; h<width/8; h++){
            cout<<ascii_art2[j][h];
        }
        cout<<endl;
    }
    cout<<"end of ascii art";
        
            
    
   

    
    

    
    return 0;
}




// Luminance = 0.2126 * R + 0.7152 * G + 0.0722 * B
// use values 0-1 for RGB to get a number between 0-1 for luminance
