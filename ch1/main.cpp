/*
createad by FujiwaraNaoto
2024/02/19
*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include<linux/omapfb.h>
#include <string.h>
#include <stdint.h>
#include<string>
#include<cassert>
#include<vector>
#include<cmath>
#include<ft2build.h>
#include<iostream>

#include FT_FREETYPE_H



const std::string FRAME_BUFFER_PATH="/dev/fb0";
const int FONT_PTSCALE=64;
//ラズベリーパイの場合は
/*
const string FONT_PATH="/usr/share/fonts/truetype/piboto/Piboto-Light.ttf";
*/
const std::string FONT_PATH="/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf";

//enumで処理出来たら便利
const unsigned int WHITE=0xFFFFFFFF;
const unsigned int BLACK=0xFF000000;
const unsigned int RED=0xFFFF0000;
const unsigned int YELLOW=0xFFFFFF00;
const unsigned int BLUE = 0xFF0000FF;


struct Window{

    FT_Library& library;
    FT_Face& face;
    FT_GlyphSlot& slot;
    FT_Matrix& matrix;
    FT_Vector& pen;
    int height,width;
    int fb_fd;
    int target_height,target_width;
    std::string font;
    double angle;


    struct fb_var_screeninfo vinfo;

    std::vector<std::vector<unsigned int>> bgimage;
    std::vector<std::vector<unsigned char>> image;

    Window(FT_Library &library_,FT_Face &face_, FT_GlyphSlot& slot_,FT_Matrix& matrix_,FT_Vector& pen_):
    library(library_),face(face_),slot(slot_),matrix(matrix_),pen(pen_)
    {
        if((fb_fd = open(FRAME_BUFFER_PATH.c_str(), O_RDWR))<0){
            perror("No such file");
            close(fb_fd);
        }
        assert(fb_fd!=-1);

        if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
            perror("Error reading variable information");
            close(fb_fd);
            exit(1);
        }
        height=vinfo.yres_virtual;
        width=vinfo.xres_virtual;
        target_height=height;
        target_width=width;

        bgimage=std::vector<std::vector<unsigned int>>(height,std::vector<unsigned int>(width));
        image=std::vector<vector<std::unsigned char>>(height,std::vector<unsigned char>(width));
    }

    void init(double angle_){
        font=FONT_PATH;
        angle=angle_;

        matrix.xx=(FT_Fixed)(std::cos(angle)*0x10000L);
        matrix.xy=(FT_Fixed)(-std::sin(angle)*0x10000L);
        matrix.yx=(FT_Fixed)(std::sin(angle)*0x10000L);
        matrix.yy=(FT_Fixed)(std::cos(angle)*0x10000L);
        
        FT_Init_FreeType(&library);
        FT_New_Face(library,font.c_str(),0,&face);
        FT_Set_Char_Size(face,12*64,0,100,0);
        slot=face->glyph;

    }

    void draw_bitmap(FT_Bitmap* bitmap,FT_Int x,FT_Int y){
        
        FT_Int i,j,p,q;
        FT_Int x_max = x + bitmap->width;
        FT_Int y_max = y + bitmap->rows;
        
        for(i=y,p=0;i<y_max;i++,p++){
            for(j=x,q=0;j<x_max;j++,q++){
                if(0<=i && i<this->height && 0<=j && j<this->width){
                    image[i][j]|=bitmap->buffer[p*bitmap->width+q];
                }    
            }
        }

    }

    void draw_text(const std::string& text){

        for(char c:text){
            FT_Set_Transform(face,&matrix,&pen);
            FT_Load_Char(face,c,FT_LOAD_RENDER);

            draw_bitmap(&slot->bitmap,slot->bitmap_left,target_height - slot->bitmap_top);

            pen.x+=slot->advance.x;
            pen.y+=slot->advance.y;

        }
        
    }

    void draw_box(int x,int y,int width,int height,unsigned int color)
    {
        for(int iy=y;iy<height;iy++){
            for(int ix=x;ix<width;ix++){
                if(0<=iy && iy<this->height && 0<=ix && ix<this->width){
                 bgimage[iy][ix]=color;
                }
            }    
        }
    }

    int show_image(unsigned int fcolor=0xFFFFFFFF)
    {
        //cout<<"height="<<height<<",width="<<width<<endl;
         uint32_t *frameBuffer = (uint32_t*)mmap(NULL, height*width*4, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
        if (frameBuffer == MAP_FAILED) {
            close(fb_fd);
            perror("Error mapping framebuffer device to memory");
            
            
            return 1;

        }

        
        struct flock fl;
        fl.l_type = F_WRLCK;  // 書き込みロックを取得
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;
        fl.l_pid = getpid();

        // ファイルロックを設定
        if (fcntl(fb_fd, F_SETLKW, &fl) == -1) {
            perror("Error locking framebuffer device");
            close(fb_fd);
            return 1;
        }

        
        for(int iy = 0; iy < this->height; iy++){
            for(int ix = 0; ix < this->width; ix++){
                if(image[iy][ix] == 0 ){
                    frameBuffer[ix + iy * this->width]= bgimage[iy][ix];
                }else if(image[iy][ix]<128){
                    frameBuffer[ix + iy * this->width]=fcolor + 0x00888888;
                }else{
                    frameBuffer[ix + iy * this->width]=fcolor;
                }
            }
        }

        msync(frameBuffer,this->height * this->width*4,0);
       
        munmap(frameBuffer, this->height * this->width*4);

        char c;
        std::cin>>c;//waiting for input

        // ファイルロックの解除
        fl.l_type = F_UNLCK;
        if (fcntl(fb_fd, F_SETLK, &fl) == -1) {
            perror("Error unlocking framebuffer device");
            return 1;
        }


        close(fb_fd);
        return 0;
    }

    




};


int main() {
    using std::cout;
    using std::endl;
    using std::string;
    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;
    FT_Matrix matrix;
    FT_Vector pen;

    Window window(library,face,slot,matrix,pen);
    window.init(0);
    window.draw_box(0,50,window.width,window.height,WHITE);

    window.draw_box(0, 0, 100, 25, 0xFFDDDDDD);
    window.draw_box(100, 0, window.width, 25,BLUE);
    window.draw_box(0, 25, window.width, 50, RED);
    window.draw_box(100, 30, 700, 48, WHITE);
    window.draw_box(0, 50, window.width, window.height, WHITE);


    string text="Hello world";
    pen.x=100*64;
    pen.y=(window.target_height-45)*64;
    window.draw_text(text);

    const int back_ground_color=BLACK;
    //window.show_image(back_ground_color);
    window.show_image(back_ground_color);

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    //getchar();
    return 0;
}
