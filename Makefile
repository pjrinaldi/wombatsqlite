OBJECTS = icons.o wombatsqlite.o managetags.o aboutbox.o viewer.o common.o

ICONS = resources/folder-open.png resources/help-contents.png resources/managetags.png resources/paperairplane2.png resources/reportpreview1.png resources/wombat_32.ico resources/bookmark.png resources/bookmark-new.png resources/bookmark-rem.png resources/aboutwombat.png

# Generate header file and source file containing declarations and data arrays
icons: $(ICONS) icons.h
	reswrap --header -o icons.h $(ICONS) --source -i icons.h -o icons.cpp $(ICONS)

# Link it all together

wombatsqlite: $(OBJECTS)
	g++ -O3 -o wombatsqlite $(OBJECTS) -lX11 -lXext -lXft -lfontconfig -lfreetype -lXcursor -lXrender -lXrandr -lXfixes -lXi -lGLU -lGL -ldl -lpthread -ljpeg -lrt -lpng -ltiff -lz -lbz2 libFOX-1.7.a

wombatsqlite.o: wombatsqlite.cpp icons.cpp managetags.cpp aboutbox.cpp viewer.cpp common.cpp
