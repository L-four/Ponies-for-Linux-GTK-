#include "ponies.h"
//Right click = open menu, left click = drag
void click_event(GtkWidget *widget, GdkEventButton *event, pony *data)
{
    if(data->active == 0 || (event->type ==
       GDK_BUTTON_RELEASE_MASK && event->button != 1))
        return;
    if(event->button == 3)
    {
        g_print("Menu Event on %s\n", getPonyName(data->name));
        view_popup_menu(&data, event, &data);
    }
    else if(event->button == 1)
    {
      g_print("Drag&Drop event on %s\n", getPonyName(data->name));
      gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(data->win)),
                                 event->button,event->x_root,
                                 event->y_root,event->time);
    }
    else
        g_print("Some other button on %s\n", getPonyName(data->name));
}

//Stop pony on cursor entering window
void enter_event(GtkWidget *widget, GdkEvent *event, pony *data)
{
    if(data->active == 0)
        return;
    if(event->type == GDK_ENTER_NOTIFY)
    {
        data->speed = SpeedStopped;
        data->animation = AnimIdle;
        g_print("Enter event on %s\n", getPonyName(data->name));
    }
    else
        g_print("Leave event on %s\n", getPonyName(data->name));

}

//Keeps the window matching the current frame of the gif file
void updateWinSize(pony *ponyWin)
{
    if(!ponyWin->active)
        return;
    GdkPixbuf *pic;
    GdkBitmap *mask;

    gdk_pixbuf_animation_iter_advance(ponyWin->pictureanimit, NULL);
    pic = gdk_pixbuf_animation_iter_get_pixbuf(ponyWin->pictureanimit);

    gdk_pixbuf_render_pixmap_and_mask( pic, NULL, &mask, 255 );
    gdk_window_shape_combine_mask(GDK_WINDOW(ponyWin->win->window), mask, 0, 0);

    if(mask)
        g_object_unref(mask);

    //Randomly moves the pony 1 pixel in a direction, to be changed later
    //Comment out this line to disable the "jitteryness" there is currently
    if(!ponyWin->dragActive)
      updateWinPos(ponyWin);
}

//Just a quick hacked up function to test random movement
void updateWinPos(pony *ponyWin)
{
    gint xx = 0;
    gint yy = 0;

    int xxx = rand() % 99;
    if( xxx < 33)
        xx = -1;
    else if( xxx < 66 )
        xx = 1;

    xxx = rand() % 99;

    if( xxx < 33)
        yy = -1;
    else if( xxx < 66 )
        yy = 1;

    gint x = 0;
    gint y = 0;
    gtk_window_get_position(GTK_WINDOW(ponyWin->win),&x,&y);
    gtk_window_move(GTK_WINDOW(ponyWin->win), x+xx, y+yy);
}

//Closes the current pony, while keeping the rest open
//If the number of ponies becomes 0, close the program
void closeThis(pony *closePony)
{
  closePony->active = 0;
  g_print("Close this command received from %s\n",getPonyName(closePony->name));
  //g_signal_handler_disconnect(closePony->win, closePony->clickEventID);
}

//Close all ponies
void closeAll()
{
    g_print("Close all command received\n");
    exit(0);
}

//Open menu on right click
void view_popup_menu(pony **ponyWin, GdkEventButton *event, gpointer userdata)
{
    GtkWidget *menu;
    GtkWidget *menuCloseThis;
    GtkWidget *menuCloseAll;

    menu = gtk_menu_new();
    menuCloseThis = gtk_menu_item_new_with_label("Close");
    menuCloseAll = gtk_menu_item_new_with_label("Close All");

    g_signal_connect(menuCloseThis, "activate", (GCallback)closeThis, &ponyWin);
    g_signal_connect(menuCloseAll, "activate", (GCallback)closeAll, &ponyWin);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuCloseThis);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuCloseAll);

    gtk_widget_show_all(menu);
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
                   (event != NULL) ? event->button : 0,
                   gdk_event_get_time((GdkEvent*)event));
}

//Retrieves the pony based on string
int ponyNameFromString(char* name)
{
  char tempName[strlen(name)];
  strcpy(tempName, name);
  for( int i = 0; i < strlen(tempName); i++)
    tempName[i] = tolower(tempName[i]);
  for( int i = 0; i <= Zecora; i++ )
    if(!strcmp(tempName, getDirectoryName(i)))
      return i;
  //No match
  printf("%s is an invalid pony name! You get Applejack instead.\n", name);
  return Applejack;
}

//Returns the name of the pony as a string
char* getPonyName(int name)
{
    switch(name)
    {
    case Applejack: return "Applejack";
    case Fluttershy: return "Fluttershy";
    case RainbowDash: return "RainbowDash";
    case Rarity: return "Rarity";
    case Twilight: return "TwilightSparkle";
    case PinkiePie: return "PinkiePie";
    case Aloe: return "Aloe";
    case Applebloom: return "Applebloom";
    case BerryPunch: return "BerryPunch";
    case BigCelestia: return "CelestiaBig";
    case BonBon: return "BonBon";
    case Cherilee: return "Cherilee";
    case Derpy: return "Derpy Hooves";
    case DJpon3: return "DJPon3";
    case DoctorWhoof: return "DoctorWhoof";
    case Elsie: return "Elsie";
    case Gilda: return "Gilda";
    case Gummy: return "Gummy";
    case Lotus: return "Lotus";
    case Lyra: return "Lyra";
    case NigtmareMoon: return "NightmareMoon";
    case Octavia: return "Octavia";
    case Parasprite: return "Parasprite";
    case Photofinish: return "Photo Finish";
    case PinkaminaDianePie: return "PinkaminaDianePie";
    case Celestia: return "PrincessCelestia";
    case Luna: return "Luna";
    case Soignefolio: return "Soignefolio";
    case Spike: return "Spike";
    case Stella: return "Stella";
    case Trixie: return "Trixie";
    case Zecora: return "Zecora";
    default: return "UnnamedPony";
    }
}

//Returns the name of the directory that the appropriate pony is in
char* getDirectoryName(int name)
{
      switch(name)
    {
    case Applejack: return "applejack";
    case Fluttershy: return "fluttershy";
    case RainbowDash: return "rainbowdash";
    case Rarity: return "rarity";
    case Twilight: return "twilight";
    case PinkiePie: return "pinkiepie";
    case Aloe: return "aloe";
    case Applebloom: return "applebloom";
    case BerryPunch: return "berrypunch";
    case BigCelestia: return "bigcelestia";
    case BonBon: return "bonbon";
    case Cherilee: return "cherilee";
    case Derpy: return "derpy";
    case DJpon3: return "djpon3";
    case DoctorWhoof: return "doctorwhoof";
    case Elsie: return "elsie";
    case Gilda: return "gilda";
    case Gummy: return "gummy";
    case Lotus: return "lotus";
    case Lyra: return "lyra";
    case NigtmareMoon: return "nightmaremoon";
    case Octavia: return "octavia";
    case Parasprite: return "parasprite";
    case Photofinish: return "photofinish";
    case PinkaminaDianePie: return "pinkaminadianepie";
    case Celestia: return "celestia";
    case Luna: return "luna";
    case Soignefolio: return "soignefolio";
    case Spike: return "spike";
    case Stella: return "stella";
    case Trixie: return "trixie";
    case Zecora: return "zecora";
    default: return "Unnamed Pony";
    }
}
