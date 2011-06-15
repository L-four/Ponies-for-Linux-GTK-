/*
*	Desktop ponies for linux/GTK+
*	By: rabbit
*	Version: 0.2a
*
* TODO:
* 1) Make ponies stop/idle when mouse hovers over them
* 2) Make command line options work
* 3) Randomize movements/actions
* 4) Have window resizer work better, or different solution?
* 5) Allow closing of 1 pony, while keeping others
* 6) Dynamically load animations
*/
#include "ponies.h"
char* getPonyDirectory();
void setPonyDirectory( char* path );

int main (int argc, char *argv[])
{

  if(argc > 1 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-H")))
  {
    printf("Ponies for Linux/GTK+ by rabbit\n");
    printf("Command list:\n");
    printf("No Parameters: Run with the settings used from last run\n");
    printf("-h: Display this help\n");
    printf("file path: Set new file path to look for pony gif files\n");
    printf("ponyname: Sets to run one of the pony type listed\n");
    return 0;
  }

  for(int i = 1; i < argc; i++)
    if(*argv[i] == '/')
      setPonyDirectory(argv[i]);

  char* ponyDirectory = NULL;
  char* ponyHomeFile = NULL;

  ponyHomeFile = malloc((strlen(getenv("HOME"))+ 9) * sizeof(char));
  if(!ponyHomeFile)
  {
    printf("Could not allocate memory for ponyHomeFile\n");
    return 0;
  }
  strcpy(ponyHomeFile, getenv("HOME"));
  strcat(ponyHomeFile,"/.ponies");
  ponyDirectory = getPonyDirectory();

  if(!ponyDirectory)
  {
    free(ponyHomeFile);
    printf("Error locating pony directory\n");
    return 0;
  }

  //Initialize GTK+ and set up loggers
  GError* error = NULL;
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

  //For command line commands later, require input on inital load
  //Normally would close, but hard coded in for easier testing
  int numberOfPonies = 0;
  if(argc == 1)
  {
    //Read in from ~/.ponies file to find saved settings
    FILE* ifp = NULL;
    char buffer[256];
    ifp = fopen(ponyHomeFile,"r");
    if(!ifp)
    {
      free(ponyHomeFile);
      free(ponyDirectory);
      printf("Could not open file for reading\n");
      return 0;
    }
    //fread(buffer, sizeof(char), 256, ifp);
    while(fgets(buffer, 256, ifp))
      if(buffer[0] != '\n')
        numberOfPonies++;

    fclose(ifp);
    numberOfPonies--;//First line is directory
  }
  //Not default arguments
  else
  {
    for(int i = 1; i < argc; i++)
    {
      if(*argv[i] == '/')
        continue;
      numberOfPonies++;
    }
  }

  //Seed the randomizer
  srand(time(0));

  //Number of ponies
  pony ponyArray[numberOfPonies];

  char buffer[256];
  FILE* ifp = NULL;
  ifp = fopen(ponyHomeFile,"r");
  if(!ifp)
  {
    printf("Could not open ~/.ponies\n");
    free(ponyDirectory);
    free(ponyHomeFile);
    return 0;
  }
  fgets(buffer, 256, ifp);//throw away first line

  //0 Ponies
  if(!numberOfPonies)
  {
    free(ponyDirectory);
    free(ponyHomeFile);
    printf("No ponies in the ~/.ponies file! Add some ponies!\n");
    printf("allowable ponies are: ");
    for(int i = 0; i <= Zecora; i++)
      printf("%s ", getDirectoryName(i));
    return 0;
  }


  //Make pony windows
  for( int i = 0; i < numberOfPonies; i++)
  {
    fgets(buffer, 256, ifp);
    char* temp = NULL;
    temp = strchr(buffer, '\n');
    if(temp)
      buffer[temp - buffer] = '\0';
    ponyArray[i].name = ponyNameFromString(buffer);
    ponyArray[i].direction = DirNone;
    ponyArray[i].dragActive = 0;
    ponyArray[i].speed = SpeedStopped;
    ponyArray[i].animation = AnimIdle;
    ponyArray[i].active = 1;
    ponyArray[i].facing = FaceLeft;
    //Create animation from file, default is idle_left
    char* initialPicturePath;
    initialPicturePath = malloc((strlen(ponyDirectory) +
                                 strlen(getDirectoryName(ponyArray[i].name)) +
                                 14) * sizeof(char));
    if(!initialPicturePath)
    {
      printf("Unable to allocate memory for directory");
      continue;
    }
    strcpy(initialPicturePath, ponyDirectory);
    strcat(initialPicturePath, getDirectoryName(ponyArray[i].name));
    strcat(initialPicturePath, "/idle_left.gif");
    ponyArray[i].pictureanim = gdk_pixbuf_animation_new_from_file
                              (initialPicturePath, &error);
    free(initialPicturePath);
    ponyArray[i].image = gtk_image_new_from_animation(ponyArray[i].pictureanim);
    ponyArray[i].pictureanimit = gdk_pixbuf_animation_get_iter
                                  (ponyArray[i].pictureanim, NULL);
    ponyArray[i].win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //Resize everytime the gif updates
    g_timeout_add(gdk_pixbuf_animation_iter_get_delay_time(ponyArray[i].
                  pictureanimit),(GSourceFunc)updateWinSize, &ponyArray[i]);
    //Name the window the name of the pony being animated
    gtk_window_set_title(GTK_WINDOW(ponyArray[i].win),
                        getPonyName(ponyArray[i].name));
    //Size down the window before showing it
    gtk_window_set_default_size(GTK_WINDOW(ponyArray[i].win),
                                gdk_pixbuf_animation_get_width(ponyArray[i].
                                pictureanim), gdk_pixbuf_animation_get_height
                                (ponyArray[i].pictureanim));
    //Remove the titlebar
    gtk_window_set_decorated(GTK_WINDOW(ponyArray[i].win), FALSE);

    //Set up the signals
    ponyArray[i].clickEventID = g_signal_connect(G_OBJECT(ponyArray[i].win),
                                "button_press_event", G_CALLBACK(click_event),
                                &ponyArray[i]);
    ponyArray[i].enterEventID = g_signal_connect(G_OBJECT(ponyArray[i].win),
                                "enter-notify-event", G_CALLBACK(enter_event),
                                &ponyArray[i]);
    ponyArray[i].leaveEventID = g_signal_connect(G_OBJECT(ponyArray[i].win),
                                "leave-notify-event", G_CALLBACK(enter_event),
                                &ponyArray[i]);
    gtk_widget_add_events(ponyArray[i].win, GDK_BUTTON_PRESS_MASK);
    gtk_container_add(GTK_CONTAINER(ponyArray[i].win), GTK_WIDGET(ponyArray[i].
                                                                  image));

    //Get rid of taskbar item
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(ponyArray[i].win), TRUE);
    //Make it so it can't be selected
    gtk_window_set_accept_focus(GTK_WINDOW(ponyArray[i].win), FALSE);
    gtk_widget_realize(ponyArray[i].win);
    //Always on top
    gtk_window_set_keep_above(GTK_WINDOW(ponyArray[i].win), TRUE);
    updateWinSize(&ponyArray[i]);

    if(error != NULL)
      break;
  }
  free(ponyDirectory);
  free(ponyHomeFile);

  //Quit out if there were any errors and give a message
  if( error != NULL)
  {
      printf("%s\n",error->message);
      return 0;
  }



  //Make it transparent?
  /*cr = gdk_cairo_create(win->window);
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0);
  cairo_set_operator( cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(image));*/

  //Main loop
  for( int i = 0; i < numberOfPonies; i++)
    gtk_widget_show_all(ponyArray[i].win);
  gtk_main();
  return 0;
}

//Writes the location of the pony files to ~/.ponies
void setPonyDirectory( char* path )
{
  FILE* ofp;

  char* initFilePath = NULL;

  initFilePath = malloc((strlen(getenv("HOME"))+ 9) * sizeof(char));
  if(!initFilePath)
  {
    printf("Could not allocate memory for initFilePath");
    return;
  }
  strcpy(initFilePath, getenv("HOME"));
  strcat(initFilePath, "/.ponies");

  ofp = fopen(initFilePath, "w");

  if(ofp != NULL)
  {
    if(path != NULL)
      fprintf(ofp, "%s", path);
    else
    {
      char* defaultPath = NULL;
      defaultPath = malloc((strlen(getenv("HOME")) + 9) * sizeof(char));
      if(!defaultPath)
      {
        printf("Could not allocate memory for defaultPath\n");
        free(initFilePath);
        return;
      }
      strcpy(defaultPath,getenv("HOME"));
      strcat(defaultPath, "/ponies/");
      fprintf(ofp, "%s", defaultPath);
      free(defaultPath);
    }
    fclose(ofp);
  }
  free(initFilePath);
  return;
}

//Retrieves the directory where the pony gifs are located
//Also creates the default ~/.ponies file if nonexistant
char* getPonyDirectory()
{
  char* ponyPaths = NULL;
  char* home = NULL;
  home = malloc((strlen(getenv("HOME")) + 1) * sizeof(char));
  if(!home)
  {
    printf("Could not allocate memory for home\n");
    return NULL;
  }
  strcpy(home, getenv("HOME"));
  if(home[0] != '/')
  {
    printf("getenv(\"HOME\") did not return a file path!");
    free(home);
    return NULL;
  }

  //Find gifs
  char* initFilePath = NULL;//9=length of ./ponies, need to set this somewhere..
  initFilePath = malloc(((strlen(home))+ 9) * sizeof(char));
  if(!initFilePath)
  {
    printf("Could not allocate memory for storing initfilepath\n");
    free(home);
    return NULL;
  }
  strcpy(initFilePath, home);
  strcat(initFilePath, "/.ponies");

  FILE* ifp = NULL;
  ifp = fopen(initFilePath, "r");
  if(!ifp)
  {
    //Make a default file
    printf("Could not open %s, creating default\n", initFilePath);
    setPonyDirectory(NULL);//Creates default @ ~/ponies/
    ifp = fopen(initFilePath, "r");
    if(!ifp)
    {
      free(home);
      free(initFilePath);
      printf("Could not read file that was just written!\n");
      return NULL;
    }
  }
  char filePathBuffer[1024];
  fscanf(ifp, "%s", filePathBuffer);
  fclose(ifp);
  if(filePathBuffer[0] != '/')
  {
    free(home);
    free(initFilePath);
    printf("Invalid ~/.ponies file\n");
    return NULL;
  }
  ponyPaths = malloc((strlen(filePathBuffer) + 1) * sizeof(char));
  strcpy(ponyPaths,filePathBuffer);

  free(home);
  free(initFilePath);
  return ponyPaths;
}
