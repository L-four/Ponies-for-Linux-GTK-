#ifndef PONIES_H_INCLUDED
#define PONIES_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <cairo.h>
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
  unsigned char active;
  int dragActive;
  int releaseEventID;
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

enum PONYNAME
{
  Applejack,
  Fluttershy,
  RainbowDash,
  Rarity,
  Twilight,
  PinkiePie
};

enum DIRECTION
{
  None,
  UpLeft,
  Up,
  UpRight,
  Left,
  Right,
  DownLeft,
  Down,
  DownRight
};

enum SPEED
{
  Stopped,
  Slow,
  Normal,
  Fast,
  Faster,
  Fastest
};

enum ANIMATION
{
  Idle,
  IdleLeft,
  IdleRight,
  FlyUp,
  FlyDown,
  FallDown,
  SlowLeft,
  SlowRight,
  NormalLeft,
  NormalRight,
  FastLeft,
  FastRight,
  FasterLeft,
  FasterRight,
  FastestLeft,
  FastestRight
};

#endif // PONIES_H_INCLUDED
