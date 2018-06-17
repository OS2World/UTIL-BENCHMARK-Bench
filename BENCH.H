/* bench.h, (c) Axel Salomon, 1990, Public Domain */

extern HAB hab;
extern HMQ hmq;
extern HPS hps;
extern RECTL rc;

extern HWND hwndMainFrame;
extern HWND hwndMainClient;

#define MID_MAIN                1000

#define MID_C_FUNC              1100 
#define MID_C_ALL               1199

#define MID_DOS_FUNC            1200 
#define MID_DOS_ALL             1299 

#define MID_DEV_FUNC            1300 
#define MID_DEV_ALL             1399

#define MID_GPI_FUNC            1400
#define MID_GPI_BITBLT          1401 
#define MID_GPI_BITBLT_ALL      1409
#define MID_GPI_CLIPPING        1411
#define MID_GPI_CLIPPING_ALL    1419 
#define MID_GPI_FONTS           1421
#define MID_GPI_FONTS_BOX       1422
#define MID_GPI_FONTS_CHARS     1423
#define MID_GPI_FONTS_DIRECT    1424
#define MID_GPI_FONTS_ANGLE     1425
#define MID_GPI_FONTS_SHEAR     1426
#define MID_GPI_FONTS_ALL       1429
#define MID_GPI_LINES           1430
#define MID_GPI_LINES_ARCS      1431     
#define MID_GPI_LINES_BOXES     1432     
#define MID_GPI_LINES_FILLETS   1433
#define MID_GPI_LINES_LINES     1434
#define MID_GPI_LINES_SPLINES   1435
#define MID_GPI_LINES_STYLES    1436
#define MID_GPI_LINES_ALL       1439
#define MID_GPI_MARKER          1441
#define MID_GPI_MARKER_BOX      1442
#define MID_GPI_MARKER_POLY     1443
#define MID_GPI_MARKER_SINGLE   1444
#define MID_GPI_MARKER_USERDEF  1445
#define MID_GPI_MARKER_ALL      1449
#define MID_GPI_PATHS           1451
#define MID_GPI_PATHS_ENDS      1452
#define MID_GPI_PATHS_FILL      1453
#define MID_GPI_PATHS_JOIN      1454
#define MID_GPI_PATHS_LINES     1455
#define MID_GPI_PATHS_PATTERN   1456
#define MID_GPI_PATHS_WIDTH     1457
#define MID_GPI_PATHS_ALL       1459
#define MID_GPI_ALL             1498
#define MID_GPI_EXTEND          1499

#define MID_PRF_FUNC            1500
#define MID_PRF_ALL             1599

#define MID_WIN_FUNC            1600
#define MID_WIN_DIALOGS         1610
#define MID_WIN_DIALOGS_BUTTONS 1611
#define MID_WIN_DIALOGS_ENTRYS  1612
#define MID_WIN_DIALOGS_LISTBOX 1613
#define MID_WIN_DIALOGS_SCROLLB 1614
#define MID_WIN_DIALOGS_STATICS 1615
#define MID_WIN_DIALOGS_ALL     1619
#define MID_WIN_MENUS           1620
#define MID_WIN_MENUS_ATTRIB    1621
#define MID_WIN_MENUS_BITMAPS   1622
#define MID_WIN_MENUS_ITEMS     1623
#define MID_WIN_MENUS_SHOW      1624
#define MID_WIN_MENUS_ALL       1629
#define MID_WIN_ALL             1699 

#define CMD_CLEAR               1700
#define CMD_EXIT                1800

#define MID_NEWITEM             1900

#define MID_BENCHMARK      WM_USER+1

#define MID_BMP1               10001
#define MID_BMP2               10002

#define DID_BENCH1_DLG          2100
#define DID_BENCH2_DLG          2200 
#define DID_BENCH3_DLG          2300
#define DID_BENCH4_DLG          2400
#define DID_BENCH5_DLG          2500

#define DID_BUTTONS_0           2101
#define DID_BUTTONS_1           2102
#define DID_BUTTONS_2           2103
#define DID_BUTTONS_3           2104
#define DID_BUTTONS_4           2105
#define DID_BUTTONS_5           2106
#define DID_BUTTONS_6           2107
#define DID_BUTTONS_7           2108
#define DID_BUTTONS_8           2109
#define DID_BUTTONS_9           2110
#define DID_BUTTONS_A           2111
#define DID_BUTTONS_B           2112

#define DID_STATIC              2201
#define DID_ENTRYFIELD          2202
#define DID_BUTTONS_OK          2203

#define DID_LISTBOX             2301

#define DID_SCROLLBAR1          2401
#define DID_SCROLLBAR2          2402

#define DID_STATIC_1            2501
#define DID_STATIC_2            2502
#define DID_STATIC_3            2503
#define DID_STATIC_4            2504
#define DID_STATIC_5            2505
#define DID_STATIC_6            2506
#define DID_STATIC_7            2507
#define DID_STATIC_8            2508
#define DID_STATIC_9            2509
#define DID_STATIC_0            2510
#define DID_STATIC_A            2511
#define DID_STATIC_B            2512
