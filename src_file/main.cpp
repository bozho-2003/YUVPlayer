
#include <QApplication>

#include <QApplication>
#include "yuv_main_wnd.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    gbl_formater_man.init(".");
    gbl_fielter_nam.init(".");

    YUVPlayerMainWidget w;
    w.show();

    return a.exec();
}

