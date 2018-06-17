/*    Bench.c (c) Axel Salomon, 1990, Public Domain   */

#define INCL_DOS  /* inlude what we need */
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bench.h" /* recource definitons */


/* not that much interesting macros */

#define FLOAT float
#define MIN(a,b)     ( a<b ? a : b )
#define IRAND(a)     ( rand() % a )
#define LRAND(a)     ( (LONG)rand() % a )

#define MS_HUNDREDTHS   10L
#define MS_SECONDS    1000L  
#define MS_MINUTES   60000L
#define MS_HOUR    3600000L 

#define CALC_MSEC( dt ) dt->hours      * MS_HOUR    + \
                        dt->minutes    * MS_MINUTES + \
                        dt->seconds    * MS_SECONDS + \
                        dt->hundredths * MS_HUNDREDTHS


/* let us start with prototyping */

MRESULT EXPENTRY BenchWinProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2); /* the main client window proc */
MRESULT EXPENTRY BenchDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2); /* the standard dialog proc */

VOID cdecl main (VOID);        /* the main function, everything starts here */
VOID do_paint   (HWND hwnd);   /* execute when a WM_PAINT message occure    */
VOID do_size    (MPARAM mp2);  /* execute when a WM_SIZE message occure (someone was playing with the sizeframe or with the min/max-buttons */
VOID do_command (HWND hwnd, MPARAM mp1);       /* Execute a menu selection  */


/* here are all the benchmark functions */


VOID do_gpi_fonts_all     (HWND hwnd);   
VOID do_gpi_fonts_box     (HWND hwnd);  /* GpiSetCharBox() */
VOID do_gpi_fonts_chars   (HWND hwnd);  /* FATTR.usSelection */
VOID do_gpi_fonts_direct  (HWND hwnd);  /* GpiSetCharDirection() */ 
VOID do_gpi_fonts_angle   (HWND hwnd);  /* GpiSetCharAngle() */
VOID do_gpi_fonts_shear   (HWND hwnd);  /* GpiSetCharShear() */

VOID do_gpi_lines_all     (HWND hwnd);  
VOID do_gpi_lines_arcs    (HWND hwnd);  /* GpiFullArc() */
VOID do_gpi_lines_boxes   (HWND hwnd);  /* GpiBox() */
VOID do_gpi_lines_fillets (HWND hwnd);  /* GpiPolyFillet() */
VOID do_gpi_lines_lines   (HWND hwnd);  /* GpiLine() */
VOID do_gpi_lines_splines (HWND hwnd);  /* GpiPolySpline() */
VOID do_gpi_lines_styles  (HWND hwnd);  /* GpiSetLineType() */

VOID do_gpi_marker_all    (HWND hwnd); 
VOID do_gpi_marker_box    (HWND hwnd);  /* GpiSetMarkerBox() */
VOID do_gpi_marker_poly   (HWND hwnd);  /* GpiPolyMarker() */
VOID do_gpi_marker_single (HWND hwnd);  /* GpiMarker() */
VOID do_gpi_marker_userdef(HWND hwnd);  /* GpiSetMarkerSet() */

VOID do_gpi_paths_all     (HWND hwnd);
VOID do_gpi_paths_ends    (HWND hwnd);  /* GpiSetLineEnd() */
VOID do_gpi_paths_fill    (HWND hwnd);  /* GpiFillPath() */
VOID do_gpi_paths_join    (HWND hwnd);  /* GpiSetLineJoin() */
VOID do_gpi_paths_lines   (HWND hwnd);  /* GpiStrokePath() */
VOID do_gpi_paths_pattern (HWND hwnd);  /* GpiSetPattern() */
VOID do_gpi_paths_width   (HWND hwnd);  /* GpiSetLineWidthGeom() */

VOID do_gpi_all           (HWND hwnd);

VOID do_gpi_extend        (HWND hwnd);  /* A sample GPI-programm */

VOID do_win_dialog_all    (HWND hwnd);
VOID do_win_dialog_buttons(HWND hwnd);  /* Some pushbutton functions */
VOID do_win_dialog_entrys (HWND hwnd);  /* Play with entryfields */
VOID do_win_dialog_listbox(HWND hwnd);  /* Listbox messages */
VOID do_win_dialog_scrollb(HWND hwnd);  /* Scrollbar messages */ 
VOID do_win_dialog_statics(HWND hwnd);  /* Look at different statics */

VOID do_win_menus_all     (HWND hwnd);
VOID do_win_menus_attrib  (HWND hwnd);  /* MM_SETITEMATTR - Msg */
VOID do_win_menus_bitmap  (HWND hwnd);  /* MM_SETITEM     - Msg */
VOID do_win_menus_items   (HWND hwnd);  /* Some MM_?ITEM? - Msg */
VOID do_win_menus_show    (HWND hwnd);  /* MM_SELECTITEM  - Msg */

VOID do_win_all           (HWND hwnd);

/* global functions to help me programm all this, also helpful for you */

VOID do_clear_screen( HWND hwnd ); /* clear the window */
INT  do_init(VOID);                /* PM init function */

VOID Display_Time(HWND hwnd, PDATETIME start, PDATETIME ende);     /* Displays the Benchmark result in the window title bar */

#define TMA_INC_SUBMENU   1    /* Definition for ToggleMenuAttr() */
#define TMA_INC_SYSMENU   2

SHORT ToggleMenuAttr( HWND frame, USHORT flags, SHORT id_of_menuitem, SHORT menuitem_attribute ); /* Toggles the attribute of a minuitem */

BOOL GpiSetFont (HPS hps, LONG fontid, CHAR *fontname);
BOOL GpiSetCharPointSize(HPS hps, SHORT pointwidth, SHORT pointhight);
BOOL GpiSetCharSelection(HPS hps, LONG fontid, CHAR *fontname, SHORT select);


/* global variables */
  
HAB      hab;     /* the handle of the anchor block, main connection to PM */
HMQ      hmq;     /* the message queue handle */
HDC      hdc;     /* the handle for a device context, not yet used */
HWND     hwndMainFrame,hwndMainClient;  /* Frame and Client Window handle */

POINTL   center;  /* The center of the client window */
INT      cx,cy;   /* The x and y size of the client window */

DATETIME start_dt,ende_dt; /* Date and Time of our Benchmark Start and End */


/* Constants for use in the Benchmark prog. */

LONG colors[16] = { CLR_BLACK,        /* the standard colors */
                    CLR_DARKBLUE,
                    CLR_DARKGREEN,
                    CLR_DARKCYAN,
                    CLR_DARKPINK,
                    CLR_DARKRED,
                    CLR_BROWN,
                    CLR_PALEGRAY,
                    CLR_DARKGRAY,
                    CLR_BLUE,
                    CLR_GREEN, 
                    CLR_CYAN,
                    CLR_PINK,
                    CLR_RED,
                    CLR_YELLOW,
                    CLR_WHITE };

LONG linetype[8] = { LINETYPE_DEFAULT, /* the standard line styles */
                     LINETYPE_DOT,
                     LINETYPE_SHORTDASH,
                     LINETYPE_DASHDOT,
                     LINETYPE_DOUBLEDOT,
                     LINETYPE_LONGDASH,
                     LINETYPE_DASHDOUBLEDOT,
                     LINETYPE_SOLID };

CHAR linetext[8][15] = { "DEFAULT",  /* the text of the lintypes */
                         "DOT",
                         "SHORTDASH",
                         "DASHDOT",
                         "DOUBLEDOT",
                         "LONGDASH",
                         "DASHDOUBLEDOT",
                         "SOLID" };

LONG endstyles[4] = { LINEEND_DEFAULT, /* line ending types */
                      LINEEND_FLAT,
                      LINEEND_ROUND,
                      LINEEND_SQUARE };

LONG markertype[11] = { MARKSYM_DEFAULT, /* standard marker symbols */
                        MARKSYM_CROSS,
                        MARKSYM_PLUS,
                        MARKSYM_DIAMOND,
                        MARKSYM_SQUARE,
                        MARKSYM_SIXPOINTSTAR,
                        MARKSYM_EIGHTPOINTSTAR,
                        MARKSYM_SOLIDDIAMOND,
                        MARKSYM_SOLIDSQUARE,
                        MARKSYM_DOT,
                        MARKSYM_SMALLCIRCLE };

LONG pattern[19] = { PATSYM_BLANK,  /* standard fill pattern */
                     PATSYM_DEFAULT,
                     PATSYM_DENSE1,
                     PATSYM_DENSE2,
                     PATSYM_DENSE3,
                     PATSYM_DENSE4,
                     PATSYM_DENSE5,
                     PATSYM_DENSE6,
                     PATSYM_DENSE7,
                     PATSYM_DENSE8,
                     PATSYM_DIAG1,
                     PATSYM_DIAG2,
                     PATSYM_DIAG3,
                     PATSYM_DIAG4,
                     PATSYM_HALFTONE,
                     PATSYM_HORIZ,
                     PATSYM_NOSHADE,
                     PATSYM_SOLID,
                     PATSYM_VERT };

SHORT select[15] = { 0,                 /* all possible selections for font attributes */
                     FATTR_SEL_ITALIC,
                     FATTR_SEL_ITALIC ^ FATTR_SEL_UNDERSCORE,
                     FATTR_SEL_ITALIC ^ FATTR_SEL_STRIKEOUT,
                     FATTR_SEL_ITALIC ^ FATTR_SEL_BOLD,
                     FATTR_SEL_ITALIC ^ FATTR_SEL_UNDERSCORE ^ FATTR_SEL_STRIKEOUT,
                     FATTR_SEL_ITALIC ^ FATTR_SEL_UNDERSCORE ^ FATTR_SEL_BOLD,
                     FATTR_SEL_ITALIC ^ FATTR_SEL_UNDERSCORE ^ FATTR_SEL_STRIKEOUT ^ FATTR_SEL_BOLD,
                     FATTR_SEL_UNDERSCORE,
                     FATTR_SEL_UNDERSCORE ^ FATTR_SEL_STRIKEOUT,
                     FATTR_SEL_UNDERSCORE ^ FATTR_SEL_BOLD,
                     FATTR_SEL_UNDERSCORE ^ FATTR_SEL_STRIKEOUT ^ FATTR_SEL_BOLD,
                     FATTR_SEL_STRIKEOUT,
                     FATTR_SEL_STRIKEOUT ^ FATTR_SEL_BOLD,
                     FATTR_SEL_BOLD };

LONG direct[4] = { CHDIRN_LEFTRIGHT, /* the standard char directions */
                   CHDIRN_RIGHTLEFT,
                   CHDIRN_TOPBOTTOM,
                   CHDIRN_BOTTOMTOP };


CHAR lbsamp[30][40] = { "House of the rising sun",
                        "With a little help from your friends",
                        "The year of the cat",
                        "Bad Moon Rising",
                        "Papa was a rolling stone",
                        "Why can't we live together",
                        "Under the broadwalk",
                        "MacArther park",
                        "Venus",
                        "Aquarius",
                        "Stairways to heaven",
                        "Bat out of hell",
                        "Sounds of silence",
                        "Woman in love",
                        "Never gonna give you up",
                        "Crazy for you",
                        "In the mood",
                        "Nikita",
                        "Even the bad times are good",
                        "See you later alligator",
                        "Running for our lives",
                        "Somewhere in africa",
                        "Wonderful world",
                        "Midnight special",
                        "Hit the road jack",
                        "Babooshka",
                        "I heard it through the grapevine",
                        "Madame de la luna",
                        "If i had a hammer",
                        "C'mon everybody" };

/*-----------------------------------------------------------------*/
/*                          M A I N                                */
/*-----------------------------------------------------------------*/
void cdecl main()
{
 QMSG qmsg;    /* a message queue item */
 
 srand( (unsigned) time(NULL) ); /* initialize the random number generator */

 if ( do_init() ){               /* do the init stuff */
    while(WinGetMsg(hab, &qmsg, NULL, 0, 0)) /* make the PM workfor you */
       WinDispatchMsg(hab, &qmsg);

    WinDestroyWindow( hwndMainFrame ); /* clean up everything */
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );
 }

}

/*-----------------------------------------------------------------*/
/*                  Initialisierungs-Prozedur                      */
/*-----------------------------------------------------------------*/
INT do_init()
{
  BOOL ok;
  ULONG flFlags = FCF_STANDARD;

  hab = WinInitialize( NULL );         /* Handle Anchor Block to the PM */
  hmq = WinCreateMsgQueue( hab,0 );    /* The Message Queue ... */

  ok = GpiLoadFonts( hab, "C:\\OS2\\DLL\\TIMES.FON" ); /* Load Fonts here */

  /* Register our window proc. */
  if (!(WinRegisterClass(hab,"BenchWinProc", BenchWinProc, CS_SIZEREDRAW, 0))) {
     WinDestroyMsgQueue( hmq );
     WinTerminate( hab );
     return FALSE;
  }

  /* Open the main window */
  if (!(hwndMainFrame = WinCreateStdWindow(HWND_DESKTOP,WS_VISIBLE,&flFlags,
                       "BenchWinProc",NULL,WS_CLIPSIBLINGS,(HMODULE)NULL,MID_MAIN,&hwndMainClient))) {
     WinDestroyMsgQueue( hmq );
     WinTerminate( hab );
     return FALSE;
  }

  /* and give it a title */
  WinSetWindowText(hwndMainFrame,"Benchmark Program");
  return TRUE;
}

/*-----------------------------------------------------------------*/
/*                 M A I N C L I E N T P R O C                     */
/*-----------------------------------------------------------------*/
MRESULT EXPENTRY BenchWinProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
  switch (msg)
   {
     case WM_CREATE:       /* Do what you need on creation */
          break; 
     case WM_COMMAND:      /* Menu selection and Accelerator keys are handled here */
          do_command(hwnd,mp1);
          return 0;
     case WM_PAINT:        /* Do painting here */
          do_paint(hwnd);
          return 0;
     case WM_SIZE:         /* The size of the screen has changed */
          do_size(mp2);
          return 0;
     case WM_CLOSE:        /* We want to close the screen now */
          WinPostMsg( hwnd, WM_QUIT, 0L, 0L);
          return 0;
     default:              /* The rest is the best */
          break;
   }
    return (WinDefWindowProc(hwnd, msg, mp1, mp2 )); /* so don't worry about it */
}


/*-----------------------------------------------------------------*/
/*                          BenchDlgProc                           */
/*-----------------------------------------------------------------*/
MRESULT EXPENTRY BenchDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   USHORT dialogdata;

   switch (msg) {
     case WM_INITDLG:
          WinPostMsg( hwnd, MID_BENCHMARK, mp2, (MPARAM) NULL );
          break;
     case WM_PAINT:
          break;
     case WM_CONTROL:
          break;
     case WM_SETFOCUS:
          break;
     case WM_CHAR:
          break; 
     case WM_COMMAND:
          break;
     case MID_BENCHMARK:
          memcpy(&dialogdata, mp1, sizeof(USHORT));
          switch (dialogdata) {
            case MID_WIN_DIALOGS_BUTTONS: 
                 do_win_dialog_buttons( hwnd );
                 break;
            case MID_WIN_DIALOGS_ENTRYS:
                 do_win_dialog_entrys( hwnd );
                 break;
            case MID_WIN_DIALOGS_LISTBOX:
                 do_win_dialog_listbox( hwnd );
                 break;
            case MID_WIN_DIALOGS_SCROLLB:
                 do_win_dialog_scrollb( hwnd );
                 break;
            case MID_WIN_DIALOGS_STATICS:
                 do_win_dialog_statics( hwnd );
                 break;
            default:
                 return;
          }
          WinDismissDlg( hwnd, TRUE );
          break;
     default:
          break;
   }
  
  return ( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
}


/*-----------------------------------------------------------------*/
/*                       Command-Prozeduren                        */
/*-----------------------------------------------------------------*/
VOID do_command(HWND hwnd, MPARAM mp1)
{
   HPOINTER hptr;  /* get a handle for the mouse-pointer */
   USHORT dialogdata;

   hptr = WinQueryPointer( HWND_DESKTOP ); /* Query the actual Pointer and save it */
   WinSetPointer( HWND_DESKTOP, (HPOINTER) NULL ); /* Erase the Pointer */

   DosGetDateTime(&start_dt); /* Get the current date and time for calculation */

   switch (SHORT1FROMMP(mp1))       /* And now : the Messages */
   { 
     case MID_GPI_FONTS_BOX:        /* Draw Text in different Character sizes */ 
          do_gpi_fonts_box(hwnd);
          break;
     case MID_GPI_FONTS_CHARS:      /* Draw Text with different standard attributes */
          do_gpi_fonts_chars(hwnd);
          break;
     case MID_GPI_FONTS_DIRECT:     /* Draw Text in different directions */
          do_gpi_fonts_direct(hwnd);
          break;
     case MID_GPI_FONTS_ANGLE:      /* Draw Text with different angles */
          do_gpi_fonts_angle(hwnd);
          break;
     case MID_GPI_FONTS_SHEAR:      /* Draw Text with shears (like shadows) */
          do_gpi_fonts_shear(hwnd);
          break;
     case MID_GPI_FONTS_ALL:        /* Execute all font benchmark tests */
          do_gpi_fonts_all(hwnd);
          break;

     case MID_GPI_LINES_ARCS:       /* Draw filled arcs like spots */
          do_gpi_lines_arcs(hwnd);
          break;
     case MID_GPI_LINES_BOXES:      /* Draw random boxes, you know that ... */
          do_gpi_lines_boxes(hwnd);
          break;
     case MID_GPI_LINES_FILLETS:    /* Draw 3-Point curves, like your baby sister did */
          do_gpi_lines_fillets(hwnd);
          break;
     case MID_GPI_LINES_LINES:      /* Draw some lines around the center, and check your Monitor ! */
          do_gpi_lines_lines(hwnd); /* you'll get "flimmer" on interlaced monitors ! */
          break;
     case MID_GPI_LINES_SPLINES:    /* Draw 4-Point curves, like you do !? <g> */
          do_gpi_lines_splines(hwnd);
          break;
     case MID_GPI_LINES_STYLES:     /* Draw different line-styles */
          do_gpi_lines_styles(hwnd);
          break;
     case MID_GPI_LINES_ALL:        /* execute all line benchmark tests */
          do_gpi_lines_all(hwnd);
          break;

     case MID_GPI_MARKER_BOX :      /* draw markers with different sizes */
          do_gpi_marker_box(hwnd); 
          break;
     case MID_GPI_MARKER_POLY :     /* draw 30 Markers each call */
          do_gpi_marker_poly(hwnd);
          break;
     case MID_GPI_MARKER_SINGLE :   /* draw 1 marker each call */
          do_gpi_marker_single(hwnd);
          break;
     case MID_GPI_MARKER_USERDEF :  /* draws very small character markers */
          do_gpi_marker_userdef(hwnd);
          break;
     case MID_GPI_MARKER_ALL :      /* execute all marker benchmark tests */
          do_gpi_marker_all(hwnd);
          break;

     case MID_GPI_PATHS_ENDS:       /* draw stroked lines with different endings */
          do_gpi_paths_ends(hwnd);
          break;
     case MID_GPI_PATHS_FILL:       /* draw filled triangles using GpiFillPath() */
          do_gpi_paths_fill(hwnd);
          break;
     case MID_GPI_PATHS_JOIN:       /* draw stroked triangles with different line join attributes */
          do_gpi_paths_join(hwnd);
          break;
     case MID_GPI_PATHS_LINES:      /* draw filled partial arcs, 3-point and 4-point curves and 4-line areas */
          do_gpi_paths_lines(hwnd); /* using GpiPartialArc(), GpiPolyFillet(), GpiPolySpline(), GpiPolyLine() */
          break;
     case MID_GPI_PATHS_PATTERN:    /* same as do_gpi_paths_line, but filled with pattern */
          do_gpi_paths_pattern(hwnd);
          break;
     case MID_GPI_PATHS_WIDTH:      /* same as do_gpi_paths_line, but stroked, not filled, that means only the */
          do_gpi_paths_width(hwnd); /* bounding curve is drawn with a defined line width using GpiSetLineWidthGeom() */
          break;
     case MID_GPI_PATHS_ALL:        /* execute all paths benchmark tests */
          do_gpi_paths_all(hwnd);
          break;

     case MID_GPI_ALL:              /* execute all gpi benchmark tests */
          do_gpi_all(hwnd);
          break;

     case MID_GPI_EXTEND:           /* draw a extended demo, to see what you can dow with gpi function calls */
          do_gpi_extend(hwnd);
          break;

     case MID_WIN_DIALOGS_ALL:
          do_win_dialog_all(hwnd);
          break;
     case MID_WIN_DIALOGS_BUTTONS:
          dialogdata = SHORT1FROMMP(mp1);
          WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH1_DLG, (PVOID) &dialogdata ) );
          break;
     case MID_WIN_DIALOGS_ENTRYS:
          dialogdata = SHORT1FROMMP(mp1);
          WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH2_DLG, (PVOID) &dialogdata ) );
          break;
     case MID_WIN_DIALOGS_LISTBOX:
          dialogdata = SHORT1FROMMP(mp1);
          WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH3_DLG, (PVOID) &dialogdata ) );
          break;
     case MID_WIN_DIALOGS_SCROLLB:
          dialogdata = SHORT1FROMMP(mp1);
          WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH4_DLG, (PVOID) &dialogdata ) );
          break;
     case MID_WIN_DIALOGS_STATICS:
          dialogdata = SHORT1FROMMP(mp1);
          WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH5_DLG, (PVOID) &dialogdata ) );
          break;

     case MID_WIN_MENUS_ALL:     
          do_win_menus_all(hwndMainFrame);
          break;
     case MID_WIN_MENUS_ATTRIB:     /* Enable and Disable different menu attributs */
          do_win_menus_attrib(hwndMainFrame);
          break;
     case MID_WIN_MENUS_BITMAPS:    /* Load a bitmap and displays it as a Menuitem */
          do_win_menus_bitmap(hwndMainFrame);
          break;
     case MID_WIN_MENUS_ITEMS:      /* Work on menu items with different messages */
          do_win_menus_items(hwndMainFrame);
          break;
     case MID_WIN_MENUS_SHOW:      /* Selects different Menuitems */
          do_win_menus_show(hwndMainFrame);
          break;
     case MID_WIN_ALL:
          do_win_all(hwndMainFrame);
          break;

     case CMD_CLEAR:                /* Clear the screen */
          do_clear_screen(hwnd);
          break;

     case CMD_EXIT:                 /* Leave the benchmark test */
          WinPostMsg(hwnd,WM_CLOSE,(MPARAM) NULL,(MPARAM) NULL);
          return;

     default:
          return;
     }

   DosGetDateTime(&ende_dt);        /* Get the actual time and date */
   Display_Time(hwnd, &start_dt, &ende_dt); /* Calculate and display the time in [ms] in the title bar */

   WinSetPointer(HWND_DESKTOP, hptr); /* restore the pointer */

}


/* This function Calculates the difference between start and end of a bench-
   marktest. The result is formatted with sprintf and stored in a buffer.
   The buffer is displayed in title bar. */

VOID Display_Time(HWND hwnd, PDATETIME start, PDATETIME ende)     
{
  CHAR buffer[50];

  sprintf( buffer, "Benchmark result : %lu ms", (CALC_MSEC(ende))-(CALC_MSEC(start)));

  WinSetWindowText( hwndMainFrame, buffer );

}


/*-----------------------------------------------------------------*/
/*                       drawing-procedures                        */
/*-----------------------------------------------------------------*/

/* Fill the screen with color of a window */

VOID do_paint(HWND hwnd)
{
  RECTL rc; /* the window rectangle */
  HPS hps;  /* the presantation space of the window */

  hps =  WinBeginPaint(hwnd, (HPS)NULL, &rc);    /* let us begin */
         WinFillRect( hps, &rc, SYSCLR_WINDOW);  /* fill the space */
         WinEndPaint(hps);                       /* thatïs all */
}


/* Clear the screen, please */

VOID do_clear_screen( HWND hwnd )
{
  HPS hps;
  RECTL rc;

  rc.xLeft  = rc.yBottom = 0L;
  rc.xRight = cx;
  rc.yTop   = cy;

  hps = WinGetPS( hwnd );                             /* Get the PS */
        WinFillRect( hps, &rc, colors[ IRAND(16) ] ); /* and fill it with a random color */ 
        WinReleasePS( hwnd );                         /* release the PS again */

}


/* store the actual sizes in public varibles, when the WM_SIZE message occur
   also calculate the center of the screen */

VOID do_size( MPARAM mp2 )
{
  cx = SHORT1FROMMP(mp2);    /* get the width */
  cy = SHORT2FROMMP(mp2);    /* and hight */
  center.x = (LONG)cx/2;     /* and calculate the */
  center.y = (LONG)cy/2;     /* the center of the screen */
} 


/*-----------------------------------------------------------------*/
/*                      benchmark-procedures                       */
/*-----------------------------------------------------------------*/

/* Fonts */

VOID do_gpi_fonts_box(HWND hwnd)
{
  INT i,j;
  POINTL pkt;
  SIZEF size;
  BOOL ok;
  HPS hps;

  hps = WinGetPS( hwnd );

  ok = GpiSetFont( hps, 1L, (CHAR *)"Tms Rmn" );

  for (i=0; i<100; i++ ) {
      pkt.x = LRAND( cx/3 );
      pkt.y = LRAND( cy );
      size.cx = MAKEFIXED( IRAND(200) ,0 );
      size.cy = MAKEFIXED( IRAND(200) ,0 );
      ok = GpiSetColor( hps, colors[ IRAND(16) ] );
      ok = GpiMove( hps, &pkt );
      ok = GpiSetCharBox( hps, &size );
      ok = GpiCharString( hps, 10, "Benchmark");
  }

  ok = GpiSetCharSet( hps, 0 );
  ok = GpiDeleteSetId( hps, 1);

  WinReleasePS( hwnd );

}

VOID do_gpi_fonts_direct(HWND hwnd)
{
  INT i,j;
  POINTL pkt;
  SIZEF size;
  BOOL ok;
  HPS hps;

  size.cx = MAKEFIXED( 50,0 );
  size.cy = MAKEFIXED( 50,0 );

  hps = WinGetPS( hwnd );

  ok = GpiSetFont( hps, 1L, (CHAR *)"Tms Rmn" );
  ok = GpiSetCharBox( hps, &size );

  for (i=0; i<100; i++ ) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      ok = GpiSetColor( hps, colors[IRAND(16)] );
      ok = GpiSetCharDirection( hps, direct[IRAND(4)] );
      ok = GpiMove( hps, &pkt );
      ok = GpiCharString( hps, 10, "Benchmark");
  }

  ok = GpiSetCharSet( hps, 0 );
  ok = GpiDeleteSetId( hps, 1);

  WinReleasePS( hwnd );

}

VOID do_gpi_fonts_chars(HWND hwnd)
{
  INT i,j;
  POINTL pkt;
  SIZEF size;
  BOOL ok;
  HPS hps;

  size.cx = MAKEFIXED( 50,0 );
  size.cy = MAKEFIXED( 50,0 );

  hps = WinGetPS( hwnd );

  for (i=0; i<100; i++ ) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      ok = GpiSetCharSelection( hps, 1L, (CHAR *)"Tms Rmn", select[IRAND(15)] );
      ok = GpiSetCharBox( hps, &size );
      ok = GpiSetColor( hps, colors[IRAND(16)] );
      ok = GpiMove( hps, &pkt );
      ok = GpiCharString( hps, 10, "Benchmark");
      ok = GpiSetCharSet( hps, 0 );
      ok = GpiDeleteSetId( hps, 1);
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_fonts_angle(HWND hwnd)
{
  INT i;
  POINTL pkt;
  GRADIENTL grad;
  SIZEF size;
  BOOL ok;
  HPS hps;

  size.cx = MAKEFIXED( 50,0 );
  size.cy = MAKEFIXED( 50,0 );

  hps = WinGetPS( hwnd );

  ok = GpiSetFont( hps, 1L, (CHAR *)"Tms Rmn" );
  ok = GpiSetCharBox( hps, &size );

  for (i=0; i<100; i++ ) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      grad.x = (LONG)(25-rand() % 50 );
      grad.y = (LONG)(25-rand() % 50 );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiMove( hps, &pkt );
      ok = GpiSetCharAngle( hps, &grad );      
      ok = GpiCharString( hps, 10, "Benchmark" );
  }

  ok = GpiSetCharSet( hps, 0 );
  ok = GpiDeleteSetId( hps, 1);

  WinReleasePS( hwnd );

}

VOID do_gpi_fonts_shear(HWND hwnd)
{
  INT i;
  POINTL pkt,shear;
  SIZEF size;
  BOOL ok;
  HPS hps;

  size.cx = MAKEFIXED( 50,0 );
  size.cy = MAKEFIXED( 50,0 );

  hps = WinGetPS( hwnd );

  ok = GpiSetFont( hps, 1L, (CHAR *)"Tms Rmn" );
  ok = GpiSetCharBox( hps, &size );

  for (i=0; i<100; i++ ) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      shear.x = (LONG)(25-rand() % 50 );
      shear.y = (LONG)(25-rand() % 50 );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiMove( hps, &pkt );
      ok = GpiSetCharShear( hps, &shear );      
      ok = GpiCharString( hps, 10, "Benchmark" );
  }

  ok = GpiSetCharSet( hps, 0 );
  ok = GpiDeleteSetId( hps, 1);

  WinReleasePS( hwnd );

}

VOID do_gpi_fonts_all(HWND hwnd)
{
  do_gpi_fonts_box     (hwnd);
  do_gpi_fonts_chars   (hwnd);
  do_gpi_fonts_direct  (hwnd);
  do_gpi_fonts_angle   (hwnd);
  do_gpi_fonts_shear   (hwnd);
}

/* Lines */

VOID do_gpi_lines_arcs(HWND hwnd)
{

  INT i;
  ARCPARAMS arcp = { 1, 1, 0, 0 };
  POINTL pkt;
  BOOL ok;
  HPS hps;

  hps = WinGetPS( hwnd );

  ok = GpiSetArcParams( hps, &arcp );

  for (i=0; i<500; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiMove( hps, &pkt );
      ok = GpiFullArc( hps, DRO_OUTLINEFILL, MAKEFIXED( rand() % 64, 0 ) );
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_lines_boxes(HWND hwnd)
{

  INT i;
  BOOL ok;
  HPS hps;
  POINTL pkt;

  hps = WinGetPS( hwnd );

  ok = GpiMove( hps, &center );
  ok = GpiSetColor( hps, CLR_DARKGRAY );

  for (i=0; i<500; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiBox( hps, DRO_OUTLINEFILL, &pkt, NULL, NULL );
      ok = GpiMove( hps, &pkt );
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_lines_fillets(HWND hwnd)
{ 
  HPS hps;
  POINTL Multipkt[2];
  BOOL ok;
  INT i,j;

  hps = WinGetPS( hwnd );

  ok =  GpiMove(hps, &center);
  for (j=0; j<500; j++ ) {
      for (i=0; i<2; i++ ) {
          ok = GpiSetColor( hps, colors[rand() % 16] );
          Multipkt[i].x = LRAND( cx );
          Multipkt[i].y = LRAND( cy );
          ok =  GpiPolyFillet(hps, 2, Multipkt);
      }
  }

  WinReleasePS( hwnd );
}

VOID do_gpi_lines_lines(HWND hwnd)
{ 
  BOOL ok;
  POINTL p1;
  INT i,c;
  HPS hps;

  hps = WinGetPS( hwnd );

  p1.x = 0L;
  p1.y = 0L;

  for (c=0; c<16; c++ ) {
      ok = GpiSetColor( hps, colors[c] );
      for (i=0 ; i<360; i++ ) {
          ok = GpiMove( hps, &center );
          if (i<90) {
             p1.x =(LONG)((FLOAT)cx*(FLOAT)i/90.0);
             }
          else if ( i<180 ) {
             p1.x = cx;
             p1.y = (LONG)((FLOAT)cy*(FLOAT)(i-90)/90.0);
             }
          else if ( i<270 ) {
             p1.x = (LONG)((FLOAT)cx*(FLOAT)(270-i)/90.0);
             p1.y = cy;
             }
          else if ( i<360 ) {
             p1.x = 0L;
             p1.y = (LONG)((FLOAT)cy*(FLOAT)(360-i)/90.0);
             }
          ok = GpiLine( hps, &p1 );
      }
  } 
  WinReleasePS( hwnd );

}

VOID do_gpi_lines_splines(HWND hwnd)
{
  HPS hps;
  POINTL Multipkt[3];
  BOOL ok;
  INT i,j;

  hps = WinGetPS( hwnd );

  ok =  GpiSetColor(hps, CLR_CYAN);
  ok =  GpiMove(hps, &center);   

  for (i=0; i<500; i++ ) {
      for (j=0;j<3;j++) {
          Multipkt[j].x = LRAND( cx );
          Multipkt[j].y = LRAND( cy );
      }
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiPolySpline(hps, 3, Multipkt);
  } 


  WinReleasePS( hwnd );
}

VOID do_gpi_lines_styles(HWND hwnd)
{
  HPS hps;
  BOOL ok;
  POINTL pkt;
  INT i,j;

  hps = WinGetPS( hwnd );
  
  for ( i=0; i<8; i++) {
      pkt.y = (LONG)((FLOAT)cy/8.0*(FLOAT)i);
      ok = GpiSetLineType(hps,linetype[i]);
      for (j=0; j<(INT)((FLOAT)cy/8.0);j+=3) {
          ok = GpiSetColor( hps, colors[rand() % 16] );
          pkt.x = 0L;
          ok = GpiMove(hps,&pkt);
          pkt.x = cx;
          ok = GpiLine(hps,&pkt);
          pkt.y+=3;
      }
      ok = GpiSetColor( hps, colors[0] );
      pkt.x = 10L;
      pkt.y = (LONG)((FLOAT)cy/8.0*(FLOAT)i);
      ok = GpiMove( hps, &pkt );
      ok = GpiCharString( hps, strlen(linetext[i]), linetext[i] );
  }

  WinReleasePS( hwnd );
}

VOID do_gpi_lines_all(HWND hwnd)
{
  do_gpi_lines_arcs    (hwnd);
  do_gpi_lines_boxes   (hwnd);
  do_gpi_lines_fillets (hwnd);
  do_gpi_lines_lines   (hwnd);
  do_gpi_lines_splines (hwnd);
  do_gpi_lines_styles  (hwnd);
}


/* marker */

VOID do_gpi_marker_box(HWND hwnd)
{

  INT i,j;
  POINTL pkt;
  FATTRS fat;
  SIZEF siz;
  BOOL ok;
  HPS hps;

  fat.usRecordLength = sizeof(FATTRS);
  fat.fsSelection    = 0;
  fat.lMatch         = 0;
  fat.idRegistry     = 0;
  fat.usCodePage     = 850;
  fat.lMaxBaselineExt= 0L;
  fat.lAveCharWidth  = 0L;
  fat.fsType         = 0;
  fat.fsFontUse      = FATTR_FONTUSE_OUTLINE;
  strcpy(fat.szFacename,"Tms Rmn");

  hps = WinGetPS( hwnd );

  ok = GpiCreateLogFont( hps, (PSTR8) NULL, 1, &fat );
  ok = GpiSetMarkerSet( hps, 1 );

  for (i=0; i<500; i++) {
     pkt.x = LRAND( cx );
     pkt.y = LRAND( cy );
     siz.cx = MAKEFIXED( rand() % 200, 0 );
     siz.cy = MAKEFIXED( rand() % 200, 0 );
     ok = GpiSetColor( hps, colors[ rand() % 16 ] );
     ok = GpiSetMarker( hps, rand() % 256 );
     ok = GpiSetMarkerBox( hps, &siz );
     ok = GpiMarker( hps, &pkt );

  }

  WinReleasePS( hwnd );

}

VOID do_gpi_marker_poly(HWND hwnd)
{

  INT i,j;
  POINTL pkt,pkte[30];
  BOOL ok;
  HPS hps;

  hps = WinGetPS( hwnd );
  for (i=0; i<17; i++) {
      for (j=0; j<30; j++ ) {
          pkte[j].x = LRAND( cx );
          pkte[j].y = LRAND( cy );
      }
     ok = GpiSetColor( hps, colors[ rand() % 16 ] );
     ok = GpiSetMarker( hps, markertype[ rand() % 11 ] );
     ok = GpiPolyMarker( hps, 30, pkte );
      
  }
  WinReleasePS( hwnd );

}

VOID do_gpi_marker_single(HWND hwnd)
{

  INT i;
  POINTL pkt;
  BOOL ok;
  HPS hps;

  hps = WinGetPS( hwnd );
  for( i=0; i<500; i++ ) {
     pkt.x = LRAND( cx );
     pkt.y = LRAND( cy );
     ok = GpiSetColor( hps, colors[ rand() % 16 ] );
     ok = GpiSetMarker( hps, markertype[ rand() % 11 ] );
     ok = GpiMarker( hps, &pkt );
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_marker_userdef(HWND hwnd)
{

  INT i,j;
  POINTL pkt;
  FATTRS fat;
  BOOL ok;
  HPS hps;

  fat.usRecordLength = sizeof(FATTRS);
  fat.fsSelection    = 0;
  fat.lMatch         = 0;
  fat.idRegistry     = 0;
  fat.usCodePage     = 850;
  fat.lMaxBaselineExt= 0L;
  fat.lAveCharWidth  = 0L;
  fat.fsType         = 0;
  fat.fsFontUse      = FATTR_FONTUSE_OUTLINE;

  hps = WinGetPS( hwnd );

  ok = GpiCreateLogFont( hps, (PSTR8) NULL, 1, &fat );
  ok = GpiSetMarkerSet( hps, 1 );

  for (i=0; i<500; i++) {
     pkt.x = LRAND( cx );
     pkt.y = LRAND( cy );
     ok = GpiSetColor( hps, colors[ rand() % 16 ] );
     ok = GpiSetMarker( hps, rand() % 256 );
     ok = GpiMarker( hps, &pkt );

  }

  WinReleasePS( hwnd );

}

VOID do_gpi_marker_all(HWND hwnd)
{
  do_gpi_marker_box(hwnd);
  do_gpi_marker_poly(hwnd);
  do_gpi_marker_single(hwnd);
  do_gpi_marker_userdef(hwnd);
}


/* paths */

VOID do_gpi_paths_ends(HWND hwnd)
{
  HPS hps;
  BOOL ok;
  POINTL p1,p2;
  INT i,j;

  hps = WinGetPS( hwnd );
  
  for (i=0; i<4; i++ ) {
      for (j=0; j<125; j++) {
          p1.x = LRAND( cx );
          p1.y = LRAND( cy );
          p2.x = LRAND( cx );
          p2.y = LRAND( cy );
          ok = GpiBeginPath( hps, 1L );
          ok = GpiSetLineEnd( hps, endstyles[i] );
          ok = GpiSetLineWidthGeom( hps, 20L );
          ok = GpiSetColor( hps, colors[ rand() % 16 ] );
          ok = GpiMove( hps, &p1 );
          ok = GpiLine( hps, &p2 );
          ok = GpiEndPath( hps );
          ok = GpiStrokePath( hps, 1L, NULL );
      }
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_paths_fill(HWND hwnd)
{
  HPS hps;
  BOOL ok;
  INT i;
  POINTL p1, pkt[2];

  hps = WinGetPS( hwnd );
  for (i=0; i<500; i++) {
      p1.x = LRAND( cx );
      p1.y = LRAND( cy );
      pkt[0].x = LRAND( cx );
      pkt[0].y = LRAND( cy );
      pkt[1].x = LRAND( cx );
      pkt[1].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[ rand() % 16 ] );
      ok = GpiMove( hps, &p1 );
      ok = GpiPolyLine( hps, 2, pkt );
      ok = GpiLine( hps, &p1 );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_paths_join(HWND hwnd)
{
  static LONG joinstyle[4] = { LINEJOIN_BEVEL,
                               LINEJOIN_DEFAULT,
                               LINEJOIN_MITRE,
                               LINEJOIN_ROUND };
  HPS hps;
  BOOL ok;
  INT i;
  POINTL p1, pkt[2];

  hps = WinGetPS( hwnd );
  for (i=0; i<500; i++) {
      p1.x = LRAND( cx );
      p1.y = LRAND( cy );
      pkt[0].x = LRAND( cx );
      pkt[0].y = LRAND( cy );
      pkt[1].x = LRAND( cx );
      pkt[1].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[ rand() % 16 ] );
      ok = GpiSetLineWidthGeom( hps, 10L );
      ok = GpiSetLineJoin( hps, joinstyle[ rand() % 4 ] ); 
      ok = GpiMove( hps, &p1 );
      ok = GpiPolyLine( hps, 2, pkt );
      ok = GpiLine( hps, &p1 );
      ok = GpiEndPath( hps );
      ok = GpiStrokePath( hps, 1L, NULL );
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_paths_lines(HWND hwnd)
{
  HPS hps;
  BOOL ok;
  POINTL pkt,pa1[2],pa2[3];
  INT i,sa,ea,ra;

  hps = WinGetPS( hwnd );
  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      sa = (INT) LRAND( 360L );
      ea = (INT) LRAND( 360L );
      ra = (INT) LRAND( 200L );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiMove( hps, &pkt ); 
      ok = GpiPartialArc( hps, &pkt, MAKEFIXED( ra, 0), MAKEFIXED( sa, 0 ), MAKEFIXED( ea,0) );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa1[0].x = LRAND( cx );
      pa1[0].y = LRAND( cy );
      pa1[1].x = LRAND( cx );
      pa1[1].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolyFillet( hps, 2, pa1 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa2[0].x = LRAND( cx );
      pa2[0].y = LRAND( cy );
      pa2[1].x = LRAND( cx );
      pa2[1].y = LRAND( cy );
      pa2[2].x = LRAND( cx );
      pa2[2].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolySpline( hps, 3, pa2 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa2[0].x = LRAND( cx );
      pa2[0].y = LRAND( cy );
      pa2[1].x = LRAND( cx );
      pa2[1].y = LRAND( cy );
      pa2[2].x = LRAND( cx );
      pa2[2].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolyLine( hps, 3, pa2 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_paths_pattern(HWND hwnd)
{
  HPS hps;
  BOOL ok;
  POINTL pkt,pa1[2],pa2[3];
  INT i,sa,ea,ra;

  hps = WinGetPS( hwnd );
  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      sa = (INT) LRAND( 360L );
      ea = (INT) LRAND( 360L );
      ra = (INT) LRAND( 200L );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiSetPattern( hps, pattern[ rand() % 19 ] );
      ok = GpiMove( hps, &pkt ); 
      ok = GpiPartialArc( hps, &pkt, MAKEFIXED( ra, 0), MAKEFIXED( sa, 0 ), MAKEFIXED( ea,0) );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa1[0].x = LRAND( cx );
      pa1[0].y = LRAND( cy );
      pa1[1].x = LRAND( cx );
      pa1[1].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiSetPattern( hps, pattern[ rand() % 19 ] );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolyFillet( hps, 2, pa1 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa2[0].x = LRAND( cx );
      pa2[0].y = LRAND( cy );
      pa2[1].x = LRAND( cx );
      pa2[1].y = LRAND( cy );
      pa2[2].x = LRAND( cx );
      pa2[2].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiSetPattern( hps, pattern[ rand() % 19 ] );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolySpline( hps, 3, pa2 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa2[0].x = LRAND( cx );
      pa2[0].y = LRAND( cy );
      pa2[1].x = LRAND( cx );
      pa2[1].y = LRAND( cy );
      pa2[2].x = LRAND( cx );
      pa2[2].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiSetPattern( hps, pattern[ rand() % 19 ] );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolyLine( hps, 3, pa2 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_paths_width(HWND hwnd)
{
  HPS hps;
  BOOL ok;
  POINTL pkt,pa1[2],pa2[3];
  INT i,sa,ea,ra;

  hps = WinGetPS( hwnd );

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      sa = (INT) LRAND( 360L );
      ea = (INT) LRAND( 360L );
      ra = (INT) LRAND( 200L );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiSetLineWidthGeom( hps, LRAND( 64L ) );
      ok = GpiMove( hps, &pkt ); 
      ok = GpiPartialArc( hps, &pkt, MAKEFIXED( ra, 0), MAKEFIXED( sa, 0 ), MAKEFIXED( ea,0) );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiStrokePath( hps, 1L, NULL );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa1[0].x = LRAND( cx );
      pa1[0].y = LRAND( cy );
      pa1[1].x = LRAND( cx );
      pa1[1].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiSetLineWidthGeom( hps, LRAND( 64L ) );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolyFillet( hps, 2, pa1 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiStrokePath( hps, 1L, NULL );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa2[0].x = LRAND( cx );
      pa2[0].y = LRAND( cy );
      pa2[1].x = LRAND( cx );
      pa2[1].y = LRAND( cy );
      pa2[2].x = LRAND( cx );
      pa2[2].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiSetLineWidthGeom( hps, LRAND( 64L ) );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolySpline( hps, 3, pa2 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiStrokePath( hps, 1L, NULL );
  }

  for (i=0; i<125; i++) {
      pkt.x = LRAND( cx );
      pkt.y = LRAND( cy );
      pa2[0].x = LRAND( cx );
      pa2[0].y = LRAND( cy );
      pa2[1].x = LRAND( cx );
      pa2[1].y = LRAND( cy );
      pa2[2].x = LRAND( cx );
      pa2[2].y = LRAND( cy );
      ok = GpiBeginPath( hps, 1L );
      ok = GpiSetColor( hps, colors[rand() % 16] );
      ok = GpiSetLineWidthGeom( hps, LRAND( 64L ) );
      ok = GpiMove( hps, &pkt );
      ok = GpiPolyLine( hps, 3, pa2 );
      ok = GpiLine( hps, &pkt );
      ok = GpiEndPath( hps );
      ok = GpiStrokePath( hps, 1L, NULL );
  }

  WinReleasePS( hwnd );

}

VOID do_gpi_paths_all(HWND hwnd)
{
  do_gpi_paths_ends    (hwnd);
  do_gpi_paths_fill    (hwnd);
  do_gpi_paths_join    (hwnd);
  do_gpi_paths_lines   (hwnd);
  do_gpi_paths_pattern (hwnd);
  do_gpi_paths_width   (hwnd);
}

VOID do_gpi_all(HWND hwnd)
{ 
  do_gpi_fonts_all  (hwnd);
  do_gpi_lines_all  (hwnd);
  do_gpi_marker_all (hwnd);
  do_gpi_paths_all  (hwnd);
}

VOID do_gpi_extend(HWND hwnd)
{
  HPS hps;
  BOOL ok;
  POINTL pkt,shear;
  INT i,sa,ea,ra;
  FATTRS fat;
  SIZEF size;
  GRADIENTL grad;

  static INT  pie[5] = { 40, 60, 95, 65, 50 };
  static LONG col[5] = { CLR_PINK, CLR_DARKCYAN, CLR_YELLOW, CLR_DARKPINK, CLR_CYAN }; 

  fat.usRecordLength = sizeof(FATTRS);
  fat.fsSelection    = 0;
  fat.lMatch         = 0;
  fat.idRegistry     = 0;
  fat.usCodePage     = 850;
  fat.lMaxBaselineExt= 0L;
  fat.lAveCharWidth  = 0L;
  fat.fsType         = 0;
  fat.fsFontUse      = FATTR_FONTUSE_OUTLINE;
  strcpy(fat.szFacename,"Tms Rmn");

  size.cx = MAKEFIXED( 50,0 );
  size.cy = MAKEFIXED( 50,0 );

  shear.x = 20;
  shear.y = 20;

  pkt.x = 10;
  pkt.y = 10;

  hps = WinGetPS( hwnd );

  ok = GpiCreateLogFont( hps, (PSTR8) NULL, 1, &fat );
  ok = GpiSetCharSet( hps, 1 );
  ok = GpiSetCharBox( hps, &size );
  ok = GpiSetCharShear( hps, &shear );
  ok = GpiMove( hps, &pkt );
  ok = GpiSetColor( hps, CLR_DARKGRAY );
  ok = GpiCharString( hps, 10, "Benchmark" );

  shear.x = 0;
  shear.y = 0;

  ok = GpiMove( hps, &pkt );
  ok = GpiSetColor( hps, CLR_PALEGRAY );
  ok = GpiSetCharShear( hps, &shear );
  ok = GpiCharString( hps, 10, "Benchmark" );

  pkt.x = center.x/2;
  pkt.y = center.y;

  ra = 75; sa = 0; ea = 50;

  for (i=0; i<5; i++ ) {
     ok = GpiBeginPath( hps, 1L );
     ok = GpiSetColor( hps, col[i] );
     ok = GpiMove( hps, &pkt);
     ok = GpiPartialArc( hps, &pkt, MAKEFIXED( ra, 0 ), MAKEFIXED( sa, 0 ), MAKEFIXED( ea, 0 )  );
     ok = GpiLine( hps, &pkt);
     ok = GpiEndPath( hps );
     ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );
     sa += ea; ea = pie[i];
  }

  pkt.x += 10;
  pkt.y -= 4;

  ok = GpiBeginPath( hps, 1L );
  ok = GpiSetColor( hps, CLR_BLUE );
  ok = GpiMove( hps, &pkt);
  ok = GpiPartialArc( hps, &pkt, MAKEFIXED( ra, 0 ), MAKEFIXED( sa, 0 ), MAKEFIXED( ea, 0 )  );
  ok = GpiLine( hps, &pkt);
  ok = GpiEndPath( hps );
  ok = GpiFillPath( hps, 1L, FPATH_ALTERNATE );

  grad.x =  50;
  grad.y = -25;

  size.cx = MAKEFIXED( 20,0 );
  size.cy = MAKEFIXED( 20,0 );

  pkt.x += 80;
  pkt.y -= 38;

  ok = GpiSetColor( hps, CLR_DARKBLUE );
  ok = GpiMove( hps, &pkt );
  ok = GpiSetCharAngle( hps, &grad );
  ok = GpiSetCharBox( hps, &size );
  ok = GpiCharString( hps, 14, "Extra Segment" );

  ok = GpiBeginPath( hps, 1L );
  ok = GpiSetColor( hps, CLR_BLACK );
  pkt.x = center.x+20;
  pkt.y = center.y-5;
  ok = GpiMove( hps, &pkt );
  pkt.y = cy-5;
  ok = GpiLine( hps, &pkt );
  pkt.x = center.x+15;
  pkt.y = center.y;
  ok = GpiMove( hps, &pkt );
  pkt.x = cx-5;
  ok = GpiLine( hps, &pkt );
  ok = GpiEndPath( hps );
  ok = GpiStrokePath( hps, 1L, FPATH_ALTERNATE );

  WinReleasePS( hwnd );

}


/* win-funcs */

VOID do_win_dialog_buttons( HWND hwnd )
{
  int i,j;
  CHAR bench[13] = "MBAERNKC H  ";
  CHAR c[2] = "A";
  LONG col1,col2;

  for (i=DID_BUTTONS_0; i<=DID_BUTTONS_B; i++) {
      WinShowWindow( WinWindowFromID( hwnd, i ), FALSE );
  }
  for (i=DID_BUTTONS_0; i<=DID_BUTTONS_B; i++) {
      WinShowWindow( WinWindowFromID( hwnd, i ), TRUE );
  }

  for (i=0; i<13; i++) {
      col1=colors[i+1]; col2=colors[14-i];
      WinSetPresParam( WinWindowFromID( hwnd, DID_BUTTONS_0+i ), PP_FOREGROUNDCOLORINDEX, sizeof(col1), &col1 );
      WinSetPresParam( WinWindowFromID( hwnd, DID_BUTTONS_0+i ), PP_BACKGROUNDCOLORINDEX, sizeof(col2), &col2 );
  }

  for (i=0; i<13; i++) {
      for (j=127; j>=bench[i]; j-- ) {
          c[0] = j;
          WinSetWindowText( WinWindowFromID( hwnd, DID_BUTTONS_0+i ), c );
      }
  }
}

VOID do_win_dialog_entrys ( HWND hwnd )
{
  HWND entry;
  int i;

  entry = WinWindowFromID( hwnd, DID_ENTRYFIELD );

  WinSendMsg( entry, EM_SETTEXTLIMIT, (MPARAM) 100, (MPARAM) NULL );
  WinSetWindowText( entry, "Benchmark Benchmark Benchmark Benchmark Benchmark Benchmark ");

  /* Clipbord messages */
  
  WinSendMsg( entry, EM_SETSEL, MPFROM2SHORT((USHORT) 10, (USHORT) 19), (MPARAM) NULL );
  WinSendMsg( entry, EM_CUT, (MPARAM) NULL, (MPARAM) NULL );
  WinSendMsg( entry, EM_SETSEL, MPFROM2SHORT((USHORT) 1, (USHORT) 1), (MPARAM) NULL );
  WinSendMsg( entry, EM_COPY, (MPARAM) NULL, (MPARAM) NULL ); 
  WinSendMsg( entry, EM_SETSEL, MPFROM2SHORT((USHORT) 2, (USHORT) 11), (MPARAM) NULL );
  WinSendMsg( entry, EM_PASTE, (MPARAM) NULL, (MPARAM) NULL );
  WinSendMsg( entry, EM_SETSEL, MPFROM2SHORT((USHORT) 5, (USHORT) 5), (MPARAM) NULL );
  WinSendMsg( entry, EM_COPY, (MPARAM) NULL, (MPARAM) NULL ); 

  /* Cursormovement and selection */

  for (i=0; i<60; i++) {
      WinSendMsg( entry, EM_SETFIRSTCHAR, MPFROMSHORT((SHORT) i), (MPARAM) NULL );
  }

  for (i=58; i>=0; i--) {
      WinSendMsg( entry, EM_SETFIRSTCHAR, MPFROMSHORT((SHORT) i), (MPARAM) NULL );
  }

  for (i=0; i<40; i++) {
      WinSendMsg( entry, EM_SETSEL, MPFROM2SHORT((USHORT) 0, (USHORT) i), (MPARAM) NULL );
  } 

  for (i=39; i>=0; i--) {
      WinSendMsg( entry, EM_SETSEL, MPFROM2SHORT((USHORT) 0, (USHORT) i), (MPARAM) NULL );
  } 

  for (i=0; i<100; i++) {
      WinSendMsg( entry, EM_SETINSERTMODE, MPFROMSHORT( (SHORT)(i%2) ), (MPARAM) NULL );
  }

}

VOID do_win_dialog_listbox( HWND hwnd )
{
  HWND listbox;
  CHAR buffer[40];
  int i;

  listbox = WinWindowFromID( hwnd, DID_LISTBOX );

  for (i=0; i<30; i++) {
      WinSendMsg( listbox, LM_INSERTITEM, MPFROMSHORT((SHORT) LIT_SORTASCENDING), MPFROMP( (PSZ) lbsamp[i] ) );
  }

  WinSendMsg( listbox, LM_DELETEALL, (MPARAM) NULL, (MPARAM) NULL );

  for (i=0; i<30; i++) {
      WinSendMsg( listbox, LM_INSERTITEM, MPFROMSHORT((SHORT) LIT_SORTASCENDING), MPFROMP( (PSZ) lbsamp[i] ) );
  }

  for (i=0; i<180; i++) {
      WinSendMsg( listbox, LM_SELECTITEM, MPFROMSHORT((SHORT) (i%30)), MPFROMSHORT((SHORT) TRUE) );
  }  

  for (i=0; i<80; i++) {
      WinSendMsg( listbox, LM_QUERYITEMTEXT, MPFROM2SHORT( (SHORT) 1, (SHORT) 40 ), MPFROMP( (PSZ) buffer ) );
      WinSendMsg( listbox, LM_DELETEITEM, MPFROMSHORT((SHORT) 1), (MPARAM) NULL );
      WinSendMsg( listbox, LM_INSERTITEM, MPFROMSHORT((SHORT) LIT_END), MPFROMP( (PSZ) buffer ) );
  }  

}

VOID do_win_dialog_scrollb( HWND hwnd )
{
  HWND sb1, sb2;
  int i;

  sb1 = WinWindowFromID( hwnd, DID_SCROLLBAR1 );
  sb2 = WinWindowFromID( hwnd, DID_SCROLLBAR2 );

  WinSendMsg( sb1, SBM_SETSCROLLBAR, (MPARAM) NULL, MPFROM2SHORT( (USHORT) 0, (USHORT) 255 ) );
  WinSendMsg( sb2, SBM_SETSCROLLBAR, (MPARAM) NULL, MPFROM2SHORT( (USHORT) 0, (USHORT) 255 ) );

  for (i=0; i<256; i++) {
      WinSendMsg( sb1, SBM_SETPOS, MPFROMSHORT( (USHORT) i ), (MPARAM) NULL );
      WinSendMsg( sb2, SBM_SETPOS, MPFROMSHORT( (USHORT) i ), (MPARAM) NULL );
  }

  for (i=255; i>=0; i--) {
      WinSendMsg( sb1, SBM_SETPOS, MPFROMSHORT( (USHORT) i ), (MPARAM) NULL );
      WinSendMsg( sb2, SBM_SETPOS, MPFROMSHORT( (USHORT) i ), (MPARAM) NULL );
  }

  for (i=0; i<256; i++) {
      WinSendMsg( sb1, SBM_SETTHUMBSIZE, MPFROM2SHORT( (USHORT) i, (USHORT) 256 ), (MPARAM) NULL );
      WinSendMsg( sb2, SBM_SETTHUMBSIZE, MPFROM2SHORT( (USHORT) i, (USHORT) 256 ), (MPARAM) NULL );
  }

  for (i=255; i>=0; i--) {
      WinSendMsg( sb1, SBM_SETTHUMBSIZE, MPFROM2SHORT( (USHORT) i, (USHORT) 256 ), (MPARAM) NULL );
      WinSendMsg( sb2, SBM_SETTHUMBSIZE, MPFROM2SHORT( (USHORT) i, (USHORT) 256 ), (MPARAM) NULL );
  }

}

VOID do_win_dialog_statics( HWND hwnd )
{
  HWND hStatic;
  HBITMAP hBmp1, hBmp2;
  HPS hps;
  int i;

  hps   = WinGetPS( hwnd );
  hBmp1 = GpiLoadBitmap( hps, (HMODULE) NULL, MID_BMP1, 58L, 48L );
  hBmp2 = GpiLoadBitmap( hps, (HMODULE) NULL, MID_BMP2, 58L, 48L );
  WinReleasePS( hps );

  hStatic = WinWindowFromID( hwnd, DID_STATIC_1 ); /* Get Bitmap */

  for (i=0; i<100; i++) {
      WinSendMsg( hStatic, SM_SETHANDLE, MPFROMLONG( (ULONG) hBmp2 ), (MPARAM) NULL );
      WinSendMsg( hStatic, SM_SETHANDLE, MPFROMLONG( (ULONG) hBmp1 ), (MPARAM) NULL );
  }  

}

VOID do_win_dialog_all( HWND hwnd )
{
  SHORT dialogdata;

  dialogdata = MID_WIN_DIALOGS_BUTTONS;
  WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH1_DLG, (PVOID) &dialogdata ) );
  dialogdata = MID_WIN_DIALOGS_ENTRYS;
  WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH2_DLG, (PVOID) &dialogdata ) );
  dialogdata = MID_WIN_DIALOGS_LISTBOX;
  WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH3_DLG, (PVOID) &dialogdata ) );
  dialogdata = MID_WIN_DIALOGS_SCROLLB;
  WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH4_DLG, (PVOID) &dialogdata ) );
  dialogdata = MID_WIN_DIALOGS_STATICS;
  WinProcessDlg( WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, (PFNWP) BenchDlgProc, (HMODULE) NULL, DID_BENCH5_DLG, (PVOID) &dialogdata ) );
}


/* This functions toggles the attributes of each menuitem in the actionbar.
   Everything works real fine, but MIA_CHECKED donït work. Why ? */

VOID do_win_menus_attrib( HWND hwnd )
{
  int i,j,k;
  SHORT mia[4] = { MIA_CHECKED, MIA_DISABLED, MIA_FRAMED, MIA_HILITED };

  for (i=0; i<4; i++) {
      for (j=1100; j<1900; j+=100) {
          for (k=0; k<10; k++ ) {
              ToggleMenuAttr( hwnd, 0, j, mia[i] );
              ToggleMenuAttr( hwnd, 0, j, mia[i] );
          }
      }
  }

}

VOID do_win_menus_bitmap( HWND hwnd )
{
  MENUITEM mi;
  SHORT retvalue;
  HWND menu;
  HBITMAP hBmp1, hBmp2;
  HPS hps;
  int i;

  hps   = WinGetPS( hwnd );
  hBmp1 = GpiLoadBitmap( hps, (HMODULE) NULL, MID_BMP1, 64L, 64L );
  hBmp2 = GpiLoadBitmap( hps, (HMODULE) NULL, MID_BMP2, 64L, 64L );
  WinReleasePS( hps );

  mi.iPosition   = MIT_END; 
  mi.afStyle     = MIS_BITMAP;    /* A bitmap menuitem will be inserted */
  mi.afAttribute = 0;
  mi.id          = MID_NEWITEM;  
  mi.hwndSubMenu = (HWND) NULL;   /* Specify a SubMenu handle if the menuitem is inserted in Submenu ! */
  mi.hItem       = (ULONG) hBmp1;

  menu = WinWindowFromID( hwnd, FID_MENU ); /* You may use FID_SYSMENU to effect the System menu here */

  retvalue = (SHORT) WinSendMsg( menu, MM_INSERTITEM, (MPARAM) &mi, MPFROMP( (PSZ) "~Bitmaps" ) );

  for (i=0; i<10; i++ ) {
      WinSendMsg( menu, MM_SETITEMHANDLE, MPFROMSHORT( MID_NEWITEM ), MPFROMLONG( (ULONG) hBmp1 ) );
      WinSendMsg( menu, MM_SETITEMHANDLE, MPFROMSHORT( MID_NEWITEM ), MPFROMLONG( (ULONG) hBmp2 ) );

/* you may also use :
      mi.hItem = (ULONG) hBmp2;
      WinSendMsg( menu, MM_SETITEM, (MPARAM) NULL, (MPARAM) &mi );
      mi.hItem = (ULONG) hBmp1;
      WinSendMsg( menu, MM_SETITEM, (MPARAM) NULL, (MPARAM) &mi );
   if you like ... */

  }
 
  WinSendMsg( menu, MM_DELETEITEM, MPFROM2SHORT(MID_NEWITEM, FALSE), (MPARAM) NULL );

}

VOID do_win_menus_items( HWND hwnd )
{
  MENUITEM mi;
  SHORT retvalue;
  HWND menu;
  int i,j;

  CHAR menutext[10][3]= { "~0", "~1", "~2", "~3", "~4", "~5", "~6", "~7", "~8", "~9" };

  mi.iPosition   = MIT_END; 
  mi.afStyle     = MIS_TEXT;    /* A simple menuitem is used first */
  mi.afAttribute = 0;
  mi.id          = MID_NEWITEM;  
  mi.hwndSubMenu = (HWND) NULL; /* Specify a SubMenu handle if the menuitem is inserted in Submenu ! */
  mi.hItem       = NULL; /* We have MIS_TEXT menuitem, so no handle is used */

  menu = WinWindowFromID( hwnd, FID_MENU ); /* You may use FID_SYSMENU to effect the System menu here */

  for (j=0; j<10; j++ ) {
      for (i=0; i<10; i++ ) {
          mi.id = MID_NEWITEM + i;
          retvalue = (SHORT) WinSendMsg( menu, MM_INSERTITEM, (MPARAM) &mi, MPFROMP( (PSZ) menutext[i] ) );
      }
      for (i=9; i>=0; i-- ) {
         WinSendMsg( menu, MM_DELETEITEM, MPFROM2SHORT(MID_NEWITEM + i, FALSE), (MPARAM) NULL );
      }
  }
}

VOID do_win_menus_show( HWND hwnd )
{
  HWND menu;
  int i,j;

  menu = WinWindowFromID( hwnd, FID_MENU );

  for (i=0; i<10; i++) {
      for (j=1100; j<1900; j+=100) {
          WinSendMsg( menu, MM_SELECTITEM, MPFROM2SHORT( (USHORT) j, FALSE), MPFROMSHORT( FALSE ) );
      }
  }

}

VOID do_win_menus_all( HWND hwnd )
{
  do_win_menus_attrib( hwnd );
  do_win_menus_bitmap( hwnd );
  do_win_menus_items ( hwnd );
  do_win_menus_show  ( hwnd );
}

VOID do_win_all( HWND hwnd )
{
  do_win_menus_all( hwnd );
  do_win_dialog_all( hwnd );
}


/* The helping hand */

/* Toggle the MIA_??? Attributes of your mneuitems with this function */

ToggleMenuAttr(HWND frame, USHORT flags, SHORT id, SHORT mia_id )
{
  HWND menu,submenu;                     /* menu handle and submenu handle */
  MENUITEM submenuinfo;                  /* information of the menuitem    */
  SHORT oldvalue,newvalue;               /* old and new attribute value    */
  BOOL submenues, sysmenu, ok;

  submenues = (flags & TMA_INC_SUBMENU);
  sysmenu   = (flags & TMA_INC_SYSMENU);

  if ( sysmenu ) {
     menu = WinWindowFromID( frame, FID_SYSMENU );     /* get the system menu handle */
  }
  else {
     menu = WinWindowFromID( frame, FID_MENU );        /* get the action bar menu handle */
  }

  if ( submenues ) {                                /* include submenues ? */
     WinSendMsg( menu, MM_QUERYITEM, MPFROM2SHORT( id, submenues ), (MPARAM) &submenuinfo ); /* info of menuitem */
     submenu = submenuinfo.hwndSubMenu;          /* save handle of submenu */
  }
  else {                                                   /* no submenues */
     submenu = menu;
  }

  oldvalue = (SHORT) WinSendMsg( submenu, MM_QUERYITEMATTR, MPFROM2SHORT( id, submenues ), MPFROMSHORT( mia_id ) ); /* retrieve old value */
  newvalue = oldvalue ^ mia_id; /* toggle value */
  ok = (BOOL) WinSendMsg( submenu, MM_SETITEMATTR, MPFROM2SHORT(id, submenues ),  MPFROM2SHORT(mia_id, newvalue) );

  return oldvalue;
}

BOOL GpiSetFont (HPS hps, LONG fontid, CHAR *fontname)
{
  FATTRS fat;
  BOOL ok;

  fat.usRecordLength  = sizeof(fat);
  fat.fsSelection     = 0;
  fat.lMatch          = 0;
  fat.idRegistry      = 0;
  fat.usCodePage      = GpiQueryCp(hps); /* get the Codepage */
  fat.lMaxBaselineExt = 0;
  fat.lAveCharWidth   = 0;
  fat.fsType          = 0;
  fat.fsFontUse       = FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_TRANSFORMABLE;
  strcpy (fat.szFacename, fontname);  /* Save the fontname */

  if (ok = GpiCreateLogFont( hps, (PSTR8) NULL, fontid, &fat )) /* Create the font */
     ok = GpiSetCharSet( hps, fontid ); /* make the font the active font */
 
  return ok;
}


/* Set the character box size using point-sizes. The pointsize is a multiple
   of 10. For example:  12 Points  equals 120. For more information look at
   the VECTFONT-Sample by Charles Petzold */

BOOL GpiSetCharPointSize(HPS hps, SHORT pointwidth, SHORT pointhight)
{
  HDC    hdc;                   /* you need a device context */
  LONG   dev_width, dev_hight;  /* physical device resolution */
  POINTL pkt;                   /* Use GpiConvert() to calculate for you */
  SIZEF  size;                  /* store the size of the font here */

  hdc = GpiQueryDevice (hps) ;  /* get the device context */

  DevQueryCaps( hdc, CAPS_HORIZONTAL_RESOLUTION, 1L, &dev_width ); /* get the width */
  DevQueryCaps( hdc, CAPS_VERTICAL_RESOLUTION,   1L, &dev_hight ); /* and high of your graphics card in pels */

  pkt.x = 254L * pointwidth * dev_width / 7200000L ; /* calculate the world coordinates */
  pkt.y = 254L * pointhight * dev_hight / 7200000L ; /* of the fontsize and store it as a point */ 

  GpiConvert( hps, CVTC_DEVICE, CVTC_PAGE, 1L, &pkt ); /* convert the point now */

  size.cx = MAKEFIXED( pkt.x, 0 );  /* save the converted values */
  size.cy = MAKEFIXED( pkt.y, 0 );

  return GpiSetCharBox( hps, &size ); /* and size the character box */
}

BOOL GpiSetCharSelection(HPS hps, LONG fontid, CHAR *fontname, SHORT select)
{
  FATTRS fat;
  BOOL ok;

  fat.usRecordLength  = sizeof(fat);
  fat.fsSelection     = select;
  fat.lMatch          = 0;
  fat.idRegistry      = 0;
  fat.usCodePage      = GpiQueryCp(hps); /* get the Codepage */
  fat.lMaxBaselineExt = 0;
  fat.lAveCharWidth   = 0;
  fat.fsType          = 0;
  fat.fsFontUse       = FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_TRANSFORMABLE;
  strcpy (fat.szFacename, fontname);  /* Save the fontname */

  if (ok = GpiCreateLogFont( hps, (PSTR8) NULL, fontid, &fat )) /* Create the font */
     ok = GpiSetCharSet( hps, fontid ); /* make the font the active font */
 
  return ok;
}


