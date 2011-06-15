#ifndef PONIES_H_INCLUDED
#define PONIES_H_INCLUDED
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>
#include <gdk/gdkscreen.h>
typedef struct s_pony
{
    GtkWidget *win;
    GtkWidget *image;
    GdkPixbufAnimation *pictureanim;
    GdkPixbufAnimationIter *pictureanimit;
    int name;
    int animation;
    int direction;
    int speed;
    int facing;
    unsigned char active;
    int dragActive;
    int clickEventID;
    int enterEventID;
    int leaveEventID;
} pony;

void click_event(GtkWidget *widget, GdkEventButton *event, pony *data);
void updateWinSize(pony *ponyWin);
void updateWinPos();
void closeAll();
void view_popup_menu(pony **ponyWin, GdkEventButton *event, gpointer userdata);
char* getPonyName(int name);
void updateWinPos(pony *ponyWin);
void enter_event(GtkWidget *widget, GdkEvent *event, pony *data);
char* getDirectoryName(int name);
int ponyNameFromString(char* name);

enum PONYNAME
{
    Applejack,
    Fluttershy,
    RainbowDash,
    Rarity,
    Twilight,
    PinkiePie,
    Aloe,
    Applebloom,
    BerryPunch,
    BigCelestia,
    BonBon,
    Cherilee,
    Derpy,
    DJpon3,
    DoctorWhoof,
    Elsie,
    Gilda,
    Gummy,
    Lotus,
    Lyra,
    NigtmareMoon,
    Octavia,
    Parasprite,
    Photofinish,
    PinkaminaDianePie,
    Celestia,
    Luna,
    Soignefolio,
    Spike,
    Stella,
    Trixie,
    Zecora
};

enum DIRECTION
{
  DirNone,
  DirUpLeft,
  DirUp,
  DirUpRight,
  DirLeft,
  DirRight,
  DirDownLeft,
  DirDown,
  DirDownRight
};

enum FACING
{
  FaceLeft,
  FaceRight,
  FaceOther
};

enum SPEED
{
  SpeedStopped,
  SpeedSlow,
  SpeedNormal,
  SpeedFast,
  SpeedFaster,
  SpeedFastest
};

enum ANIMATION
{
  AnimIdle,
  AnimIdleLeft,
  AnimIdleRight,
  AnimFlyUp,
  AnimFlyDown,
  AnimFallDown,
  AnimSlowLeft,
  AnimSlowRight,
  AnimNormalLeft,
  AnimNormalRight,
  AnimFastLeft,
  AnimFastRight,
  AnimFasterLeft,
  AnimFasterRight,
  AnimFastestLeft,
  AnimFastestRight
};

#endif // PONIES_H_INCLUDED
