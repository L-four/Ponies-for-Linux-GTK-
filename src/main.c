/*
*	Desktop ponies for linux/GTK+
*	By: rabbit
*	Version: 0.1a
*
* TODO:
* 1) Make ponies stop/idle when mouse hovers over them
* 2) Make command line options work
* 3) Randomize movements/actions
* 4) Have window resizer work better, or different solution?
* 5) Allow closing of 1 pony, while keeping others
*/
#include "ponies.h"

//Right click = open menu, left click = drag
void click_event(GtkWidget *widget, GdkEventButton *event, pony *data)
{
  if(data->active == 0)
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
    data->speed = Stopped;
    data->animation = Idle;
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
  g_print("Close this command received from %s\n", getPonyName(closePony->name));
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

//Returns the name of the pony as a string
char* getPonyName(int name)
{
  switch(name)
  {
    case Applejack:
      return "Applejack";
    case  Fluttershy:
      return "Fluttershy";
    case RainbowDash:
      return "Rainbow Dash";
    case Rarity:
      return "Rarity";
    case Twilight:
      return "Twilight Sparkle";
    case PinkiePie:
      return "Pinkie Pie";
    default:
      return "Unnamed Pony";
  }
}


int main (int argc, char *argv[])
{
  GError *error = NULL;

  //Initialize GTK+ and set up loggers
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

  //For command line commands later, require input on inital load
  //Normally would close, but hard coded in for easier testing
  if(argc <= 1)
    printf("Incorrect format\n");

  //Number of ponies
  argc = 1;
  pony ponyArray[argc];

  //Make pony windows
  for( int i = 0; i < argc; i++)
  {
    ponyArray[i].name = Applejack;
    ponyArray[i].direction = None;
    ponyArray[i].speed = Stopped;
    ponyArray[i].animation = Idle;
    ponyArray[i].active = 1;
    //Create animation from file
    ponyArray[i].pictureanim = gdk_pixbuf_animation_new_from_file("/home/rabbit/ponies/Applejack/trotcycle_AJ_left.gif", &error);
    ponyArray[i].image = gtk_image_new_from_animation(ponyArray[i].pictureanim);
    ponyArray[i].pictureanimit = gdk_pixbuf_animation_get_iter(ponyArray[i].pictureanim, NULL);
    ponyArray[i].win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //Name the window the name of the pony being animated
    gtk_window_set_title (GTK_WINDOW(ponyArray[i].win), getPonyName(ponyArray[i].name));
    //Size down the window before showing it
    gtk_window_set_default_size(GTK_WINDOW(ponyArray[i].win), gdk_pixbuf_animation_get_width(ponyArray[i].pictureanim), gdk_pixbuf_animation_get_height(ponyArray[i].pictureanim));
    //Remove the titlebar
    gtk_window_set_decorated(GTK_WINDOW(ponyArray[i].win), FALSE);
    //Set up the signals
    ponyArray[i].clickEventID = g_signal_connect(G_OBJECT(ponyArray[i].win), "button_press_event", G_CALLBACK(click_event), &ponyArray[i]);
    ponyArray[i].enterEventID = g_signal_connect(G_OBJECT(ponyArray[i].win), "enter-notify-event", G_CALLBACK(enter_event), &ponyArray[i]);
    ponyArray[i].leaveEventID = g_signal_connect(G_OBJECT(ponyArray[i].win), "leave-notify-event", G_CALLBACK(enter_event), &ponyArray[i]);
    gtk_widget_add_events(ponyArray[i].win, GDK_BUTTON_PRESS_MASK);
    gtk_container_add(GTK_CONTAINER(ponyArray[i].win), GTK_WIDGET(ponyArray[i].image));
    //Get rid of taskbar item
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(ponyArray[i].win), TRUE);
    //Make it so it can't be selected
    gtk_window_set_accept_focus(GTK_WINDOW(ponyArray[i].win), FALSE);
    gtk_widget_realize(ponyArray[i].win);
    //Always on top
    gtk_window_set_keep_above(GTK_WINDOW(ponyArray[i].win), TRUE);
    updateWinSize(&ponyArray[i]);
    //Resize everytime the gif updates
    g_timeout_add(gdk_pixbuf_animation_iter_get_delay_time(ponyArray[i].pictureanimit),(GSourceFunc)updateWinSize, &ponyArray[i]);
  }

  //Quit out if there were any errors and give a message
  if( error != NULL)
  {
      printf("%s\n",error->message);
      return 1;
  }
  //Seed the randomizer
  srand(time(0));


  //Make it transparent?
  /*cr = gdk_cairo_create(win->window);
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0);
  cairo_set_operator( cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(image));*/

  //Main loop
  for( int i = 0; i < argc; i++)
    gtk_widget_show_all(ponyArray[i].win);
  gtk_main();
  return 0;
}


