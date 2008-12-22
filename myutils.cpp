#include "myutils.h"

bool coord::operator==(const coord &other) const {
  if ((other.X == this->X) && (other.Y == this->Y))
    return true;
  else
    return false;
}
  
bool coord::operator!=(const coord &other) const {
  return !(*this == other);
}

coord& coord::operator=(const coord& c) {
  X = c.X;
  Y = c.Y;
  return *this;
} 

void setSdl(SDL_Surface** screen) {
  if( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
    cerr << "Couldn't initialize SDL: " << SDL_GetError() << endl;
    exit(1);
  }
  //poder fechar a janela da SDL
  //signal(SIGINT, SIG_DFL);
  //signal(SIGQUIT, SIG_DFL);
  atexit(SDL_Quit);
  *screen = SDL_SetVideoMode(WW, WW, 32, SDL_SWSURFACE|SDL_ANYFORMAT);
  if ( screen == NULL ) {
    cerr << "Couldn't set "<< WW << "x" << WW << "x8 video mode: " << SDL_GetError() << endl;
    exit(1);
  }
  SDL_WM_SetCaption(W_CAPTION, 0);
  cout << "Set "<< WW << "x" << WW << " at " << (unsigned short) (*screen)->format->BitsPerPixel << " bits-per-pixel mode" << endl;
  SDL_FillRect (*screen, 0, SDL_MapRGB((*screen)->format, 0, 0, 0));
  SDL_UpdateRect(*screen, 0, 0, 0, 0);
  if( TTF_Init() == -1 ){
    cerr << "Erro inicializando SDL_ttf" << endl;
  } else 
    cout << "SDL_ttf inicializado com sucesso" << endl;
}



Uint32 colorTable(int col_index) {
  switch (col_index) {
    case 0 : return 0xffffff;
    case 1 : return 0xff0000;
    case 2 : return 0x00ff00;
    case 3 : return 0x0000ff;
    case 4 : return 0xffff00;
    case 5 : return 0xff00ff;
    case 6 : return 0x00ffff;
    case 7 : return 0xcccccc;
    case 8 : return 0xff9900;
    case 9 : return 0x9999ff;
  }
  return 0x000000;
}



void drawOnScreen(SDL_Surface* screen, int x, int y, unsigned char R, unsigned char G, unsigned char B) {
  Uint32 color;
  color = SDL_MapRGB(screen->format, R, G, B);
  static SDL_Rect player_rect;  
  player_rect.w = player_rect.h = P_SIZE;
  player_rect.x = x;
  player_rect.y = y;    
  if ( SDL_LockSurface(screen) < 0 ) {
    cerr << "Can’t lock screen: " << SDL_GetError() << endl;
    return;
  }
  SDL_FillRect (screen, &player_rect, SDL_MapRGB(screen->format, R, G, B));
  SDL_UpdateRect (screen, 0, 0, WW, WW);
  SDL_UnlockSurface(screen);
  return;
}



void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Color linecolor) {
  Uint32 color;
  color = SDL_MapRGB(screen->format, linecolor.R, linecolor.G, linecolor.B);
  static SDL_Rect player_rect;
  player_rect.w = x2 - x1;
  if (player_rect.w == 0) player_rect.w = 1;
  player_rect.h = y2 - y1;
  if (player_rect.h == 0) player_rect.h = 1;
  player_rect.x = x1;
  player_rect.y = y1;
  /* Lock the screen for direct access to the pixels */
  if ( SDL_LockSurface(screen) < 0 ) {
    cerr << "Can’t lock screen: " << SDL_GetError() << endl;
    return;
  }
  SDL_FillRect (screen, &player_rect, SDL_MapRGB(screen->format, linecolor.R, linecolor.G, linecolor.B));
  SDL_UpdateRect (screen, 0, 0, WW, WW);
  SDL_UnlockSurface(screen);
  return;
}

void drawLineBresenham(SDL_Surface *output, int x0, int y0, int x1, int y1, Uint32 linecolor) {
    static bool steep;
    static int swapper, deltax, deltay, error, ystep, x, y;
    steep = (y1 - y0) > (x1 - x0);    
    if (steep) {
      SWAP(x0,y0);      
      SWAP(x1,y1);      
    }
    if (x0 > x1) {
      SWAP(x0,x1);      
      SWAP(y0,y1);      
    }
    deltax = x1 - x0;
    deltay = ABS(y1 - y0);    
    error = deltax / 2;
    y = y0;
    if (y0 < y1)
      ystep = 1;
    else
      ystep = -1;
    for (x = x0 ; x <= x1 ; x++) { //for x from x0 to x1
      if (steep)
        putPixel32(output, y, x, linecolor);//TODO otimizar isto!
//         then plot(y,x);
      else 
        putPixel32(output, x, y, linecolor);//TODO otimizar isto! também!
//         plot(x,y);
      error = error - deltay;
      if (error < 0) {
        y = y + ystep;
        error = error + deltax;
      }
    }
}


void putpixel(SDL_Surface *output, int x, int y, Uint32 pixel)
{
  int bpp = output->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)output->pixels + y * output->pitch + x * bpp;
  switch(bpp) {
    case 1:
      *p = pixel;
      break;
    case 2:
      *(Uint16 *)p = pixel;
      break;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      } else {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
      break;
    case 4:
      *(Uint32 *)p = pixel;
      break;
  }
}


void putPixel32(SDL_Surface *output, int x, int y, Uint32 pixelcolor)
{
  Uint8 *p = (Uint8 *)output->pixels + y * output->pitch + x * 4;
  *(Uint32 *)p = pixelcolor;
}



SDL_Surface* load_image( string filename ) 
{ 
  SDL_Surface* loadedImage = NULL; 
  SDL_Surface* optimizedImage = NULL;    
  loadedImage = SDL_LoadBMP( filename.c_str() );        
  if( loadedImage != NULL )
  {        
    optimizedImage = SDL_DisplayFormat( loadedImage );
    SDL_FreeSurface( loadedImage );
  }        
  return optimizedImage;
}



void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{    
  SDL_Rect offset;      
  offset.x = x;
  offset.y = y;       
  SDL_BlitSurface( source, NULL, destination, &offset );
}



int approx(float number) {
  float round_up_diff = ceil(number) - number;
  float round_dn_diff = number - floor(number);  
  return round_up_diff > round_dn_diff ? (int)floor(number) : (int)ceil(number);  
}

long approxLong(double number) {
  double round_up_diff = ceil(number) - number;
  double round_dn_diff = number - floor(number);  
  return round_up_diff > round_dn_diff ? (long)floor(number) : (long)ceil(number);  
}

float distance (float xo, float yo, float xf, float yf) {
  return sqrt( apow(xf-xo,2) + apow(yf-yo,2) );
}

float apow(float a, int b) { //power ... only works with non-negative exponnents (b)
  if (b == 0) return 1;
  return a * apow(a, b - 1);
}

bool belongsToVisibility (float px, float py, float dx, float dy, float ox, float oy) {
  float hipo = distance (0.0, 0.0, dx, dy);
  float cos_alpha = dx / hipo;
  float sin_alpha = dy / hipo;
//  float oy__, py__;
  float ox__, px__;
  ox__ = ox * cos_alpha + oy * sin_alpha;
  px__ = px * cos_alpha + py * sin_alpha;
//  oy__ = -ox * sin_alpha + oy * cos_alpha;
//  py__ = -px * sin_alpha + py * cos_alpha;
  if (ox__ >= px__)
    return true;
  else
    return false;
}

float simpleScale(float value, float old_max, float new_max) {
  return convertToScale(value, 0, old_max, 0, new_max);
}

float convertToScale(float value, float old_min, float old_max, float new_min, float new_max) {
  float old_max0 = old_max - old_min;
  float new_max0 = new_max - new_min;
  float old_value0 = value - old_min;
  float new_value0, new_value;  
  new_value0 = old_value0 / (old_max0/new_max0);  
  new_value = new_value0 + new_min;  
  return new_value;
}

string floatToString(float f, int precision) {
  stringstream buf;
  string _result;
  buf.precision(precision);
  buf << fixed << f;
  buf >> _result;
  return _result;
}

string longToString(long l) {
  stringstream buf;
  string _result;
  buf << l;
  buf >> _result;
  return _result;
}

long absolute(long l) {
  return l >= 0 ? l : -l;
}

long minimum(long a, long b) {
  return a > b ? a : b;
}